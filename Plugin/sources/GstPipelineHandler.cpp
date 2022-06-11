

#include "GstPipelineHandler.h"

#include <gst/net/gstnet.h>

#include <mutex>

#include "GStreamerCore.h"
#include "IThreadManager.h"
#include "UnityHelpers.h"
#include "stdafx.h"

namespace mray {
namespace video {

static gboolean delayed_seek_cb(GstPipelineHandlerImpl *self);

const std::string STATE_STRING[] = {" GST_STATE_VOID_PENDING",
                                    " GST_STATE_NULL", " GST_STATE_READY",
                                    " GST_STATE_PAUSED", " GST_STATE_PLAYING"};

class GstPipelineHandlerImpl {
 public:
  GstPipelineHandlerImpl(GstPipelineHandler *o, const char *name) {
    _name = name;
    _owner = o;
    gstPipeline = 0;
    closing = false;
    paused = true;
    Loaded = false;
    playing = false;
    trickSeek = false;
    optimizedSeekMode = false;
    _restartingEOS = false;
    _eosSeekDone = false;

    freeze = false;
    baseTime = 0;
    isMasterClock = false;
    clockProvider = 0;

    _video_sink = 0;
    seekingDone = true;

    clock = 0;
    clockPort = 0;
    _seekPos = _lastSeekPos = -1;
    last_seek_time = 0;
    timeout_source = 0;
    _state = GST_STATE_NULL;
  }

  GstPipelineHandler *_owner;
  guint busWatchID;
  GstElement *gstPipeline;
  GstElement *_video_sink;
  GstNetTimeProvider *clockProvider;
  GstClock *clock;

  bool paused;
  bool Loaded;
  bool playing;
  bool closing;
  bool seekingDone;
  bool trickSeek;
  bool optimizedSeekMode;
  bool _restartingEOS;
  bool _eosSeekDone;

  bool freeze;

  std::string _name;
  GstState _state;

  gint64 _seekPos, _lastSeekPos, last_seek_time;

  uint clockPort;
  std::string clockIP;
  ulong baseTime;
  bool isMasterClock;
  GSource *timeout_source;

#define SEEK_MIN_DELAY (50 * GST_MSECOND)
  void _createSeekTimeout() {
    LogMessage(_name + ": Creating seek request", ELL_INFO);
    gint64 diff = gst_util_get_timestamp() - last_seek_time;
    if (diff < 0) diff = 10 * GST_MSECOND;
    timeout_source =
        g_timeout_source_new((SEEK_MIN_DELAY - diff) / GST_MSECOND);
    g_source_set_callback(timeout_source, (GSourceFunc)delayed_seek_cb,
                          (void *)this, NULL);
    g_source_attach(timeout_source, g_main_context_get_thread_default());
    g_source_unref(timeout_source);
  }

  void _seekDone(bool retry) {
    seekingDone = true;
    if (retry && _seekPos != _lastSeekPos && _seekPos != GST_CLOCK_TIME_NONE) {
      /* LogMessage(_name + ": Seek pending: " + std::to_string(_seekPos),
                  ELL_WARNING);*/
      _executeSeek(); /*
       if (!timeout_source) {
         _createSeekTimeout();
       }*/
    } else
      _owner->FIRE_LISTENR_METHOD(OnPostSeek, (_owner));
  }
  bool _executeSeek() {
    std::lock_guard<std::recursive_mutex> lock(_owner->GetMutex());
    if (freeze /* || !seekingDone*/) {
      LogMessage("Can't seek due to: " + std::string(freeze ? "Frozen" : "") +
                     "," + std::string(seekingDone ? "" : "Still seeking"),
                 ELL_WARNING);
      // _createSeekTimeout();
      return false;
    }
    if (_state != GST_STATE_PAUSED || !paused) {
      LogMessage(
          _name + ": Trying to seek while not paused: " + STATE_STRING[_state],
          ELL_WARNING);
      _seekDone(false);
      return false;
    }
    if (_seekPos == GST_CLOCK_TIME_NONE) {
      LogMessage("Can't seek to invalid time", ELL_WARNING);
      _seekDone(false);
      return false;  // || _seekPos == _lastSeekPos
    }
    /*if (_seekPos == _lastSeekPos) {
      _seekPos = GST_CLOCK_TIME_NONE;
      LogMessage("Skipping seeking to same previous frame", ELL_WARNING);
      _seekDone(false);
      return false;  // || _seekPos == _lastSeekPos
    }*/

    gint64 diff = gst_util_get_timestamp() - last_seek_time;
#if 0
    if (GST_CLOCK_TIME_IS_VALID(last_seek_time) && diff < SEEK_MIN_DELAY) {
      /* The previous seek was too close, delay this one */
      /* There was no previous seek scheduled. Setup a timer for some
       * time in the future */
      if (!timeout_source) {
        _createSeekTimeout();
      }
      return true;
    } else
#endif
    {
      timeout_source = 0;
      last_seek_time = gst_util_get_timestamp();
      GstSeekFlags seek_flags = (GstSeekFlags)0;

      if (optimizedSeekMode) {
        seek_flags =
            (GstSeekFlags)(seek_flags |
                           GST_SEEK_FLAG_TRICKMODE |  // GST_SEEK_FLAG_TRICKMODE_KEY_UNITS
                                                      // |
                           GST_SEEK_FLAG_TRICKMODE_NO_AUDIO |
                           GST_SEEK_FLAG_TRICKMODE_FORWARD_PREDICTED);
        trickSeek = true;
      } else {
        trickSeek = false;
        seek_flags = (GstSeekFlags)(seek_flags | GST_SEEK_FLAG_ACCURATE);
      }
      seekingDone = false;
#if false
      GstEvent *seek = gst_event_new_seek(
          1, GST_FORMAT_TIME, (GstSeekFlags)(seek_flags | GST_SEEK_FLAG_FLUSH),
          GST_SEEK_TYPE_SET, _seekPos * GST_MSECOND, GST_SEEK_TYPE_NONE,
          GST_CLOCK_TIME_NONE);
      if (optimizedSeekMode) {
        gst_event_set_seek_trickmode_interval(seek, 0);
      }
      if (!gst_element_send_event(GST_ELEMENT(gstPipeline), seek)) {
       /* LogMessage(_name + " - Failed to execute seeking into: " +
                       std::to_string(_seekPos),
                   ELL_WARNING);*/
        _seekDone(false);
      } else
        LogMessage(_name + ": Seeking into: " + std::to_string(_seekPos),
                   ELL_INFO);
#else
      if (!gst_element_seek(gstPipeline, 1.0, GST_FORMAT_TIME,
                            (GstSeekFlags)(seek_flags | GST_SEEK_FLAG_FLUSH),
                            GST_SEEK_TYPE_SET, _seekPos * GST_MSECOND,
                            GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
        /*LogMessage(_name + " - Failed to execute seeking into: " +
                       std::to_string(_seekPos),
                   ELL_WARNING);*/
        _seekDone(false);
        return false;
      }
#endif
      _lastSeekPos = _seekPos;
      return true;
    }
    return false;
  }

  void TriggerPlay(bool async) {
    std::lock_guard<std::recursive_mutex> lock(_owner->GetMutex());
    //    std::lock_guard<std::mutex> lock(_seekMutex);
    if (!paused) {
      if (!async || trickSeek) {
        //  if (!seekingDone) return;
        if (_seekPos != GST_CLOCK_TIME_NONE) {
          _lastSeekPos = _seekPos;
          _seekPos = GST_CLOCK_TIME_NONE;
        }
        if (_lastSeekPos != GST_CLOCK_TIME_NONE) {
          LogMessage(
              "TriggerPlay: Seeking into: " + std::to_string(_lastSeekPos),
              ELL_INFO);
          bool okay = true;
          seekingDone = false;
          if (!gst_element_seek(
                  gstPipeline, 1.0, GST_FORMAT_TIME,
                  (GstSeekFlags)(GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH),
                  GST_SEEK_TYPE_SET, _lastSeekPos * GST_MSECOND,
                  GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
            LogMessage(_name +
                           ": Failed to send seek event before "
                           "playing: " +
                           std::to_string(_lastSeekPos),
                       ELL_WARNING);
            okay = false;
          }
          if (!okay) _seekDone(true);
          trickSeek = false;
        }
      }
      if (seekingDone) {
        LogMessage(_name + ": Playing", ELL_INFO);
        gst_element_set_state(gstPipeline, GST_STATE_PLAYING);
      }
    }
  }
};

GstPipelineHandler::GstPipelineHandler(const char *name) {
  GStreamerCore::Instance()->Ref();

  m_data = new GstPipelineHandlerImpl(this, name);
}
GstPipelineHandler::~GstPipelineHandler() {
  Close();
  GStreamerCore::Instance()->Unref();

  OS::IThreadManager::getInstance().sleep(100);
  std::lock_guard<std::recursive_mutex> lock(GetMutex(1));
  delete m_data;
  m_data = 0;
}

bool GstPipelineHandler::CreatePipeline(bool isMasterClock,
                                        const std::string &clockIP,
                                        uint clockPort) {
  LogMessage(m_data->_name + ": GstPipelineHandler::CreatePipeline ", ELL_INFO);
  if (!m_data->gstPipeline) return false;
  std::lock_guard<std::recursive_mutex> lock(GetMutex());

  FIRE_LISTENR_METHOD(OnPipelineLink, (this));

  // enable logging to stdout
  // g_signal_connect(m_data->gstPipeline, "deep-notify",
  //                 G_CALLBACK(gst_object_default_deep_notify), NULL);

  // g_object_set(GST_BIN(m_data->gstPipeline), "message-forward", TRUE,
  // NULL);
  m_data->clockIP = clockIP;
  m_data->clockPort = clockPort;
  m_data->isMasterClock = isMasterClock;
  m_data->paused = true;
  m_data->Loaded = false;
  m_data->playing = false;

  GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_data->gstPipeline));

  if (bus) {
    m_data->busWatchID = gst_bus_add_watch_full(
        bus, G_PRIORITY_DEFAULT, (GstBusFunc)busFunction, this, NULL);
  }

  gst_object_unref(bus);

  if (gst_element_set_state(GST_ELEMENT(m_data->gstPipeline),
                            GST_STATE_READY) == GST_STATE_CHANGE_FAILURE) {
    LogMessage(
        m_data->_name +
            ": GstPipelineHandler::Play(): unable to set pipeline to ready",
        ELL_WARNING);
    return false;
  }
  if (gst_element_get_state(GST_ELEMENT(m_data->gstPipeline), NULL, NULL,
                            10 * GST_SECOND) == GST_STATE_CHANGE_FAILURE) {
    LogMessage(m_data->_name +
                   ": GstPipelineHandler::Play(): unable to get pipeline "
                   "ready status",
               ELL_WARNING);
    return false;
  }
  // pause the pipeline
  if (gst_element_set_state(GST_ELEMENT(m_data->gstPipeline),
                            GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
    LogMessage(m_data->_name +
                   ": GstPipelineHandler::Play(): unable to pause pipeline",
               ELL_WARNING);
    return false;
  } /*
   if (gst_element_get_state(GST_ELEMENT(m_data->gstPipeline), NULL, NULL,
                             10 * GST_SECOND) == GST_STATE_CHANGE_FAILURE) {
       LogMessage(m_data->_name +
                      ": GstPipelineHandler::Play(): unable to get pipeline "
                      "paused status",
                  ELL_WARNING);
       return false;
   }*/
  /*
   */

  return true;
}
static gboolean delayed_seek_cb(GstPipelineHandlerImpl *self) {
  self->_executeSeek();
  return FALSE;
}

bool GstPipelineHandler::SetPosition(long pos, bool optimized) {
  {
    if (pos < 0) return false;
    if (m_data->_restartingEOS) return false;
    std::lock_guard<std::recursive_mutex> lock(GetMutex());
    // LogMessage("GstPipelineHandler::SetPosition ", ELL_INFO);
    // GstState state;
    // gst_element_get_state(m_data->gstPipeline, &state, NULL, 0);// 2 *
    // GST_SECOND);

    m_data->_seekPos = pos;
    m_data->optimizedSeekMode = optimized;
  }
  FIRE_LISTENR_METHOD(OnPreSeek, (this, m_data->_seekPos));
  if (m_data->_state == GST_STATE_PAUSED) {
    return m_data->_executeSeek();
  } else {
    LogMessage("GstPipelineHandler::SetPosition " + std::to_string(pos) +
                   ", pipeline is in state: " + std::to_string(m_data->_state),
               ELL_WARNING);
    FIRE_LISTENR_METHOD(OnPostSeek, (this));
    return false;
  }
}
long GstPipelineHandler::GetPosition() {
  std::lock_guard<std::recursive_mutex> lock(GetMutex());
  // still didn't finish EOS restart
  if (m_data->_restartingEOS) return -1;
  gint64 p;
  if (!gst_element_query_position(m_data->gstPipeline, GST_FORMAT_TIME, &p))
    return -1;
  return (p / GST_MSECOND);
}
long GstPipelineHandler::GetDuration() {
  std::lock_guard<std::recursive_mutex> lock(GetMutex());
  gint64 p;
  if (!gst_element_query_duration(m_data->gstPipeline, GST_FORMAT_TIME, &p))
    return -1;
  return (p / GST_MSECOND);
}
bool GstPipelineHandler::IsResettingEOS() { return m_data->_restartingEOS; }
void GstPipelineHandler::SetPaused(bool p) {
  std::lock_guard<std::recursive_mutex> lock(GetMutex());
  m_data->paused = p;
  if (m_data->Loaded || true) {
    if (m_data->playing) {
      GstState state;
      if (m_data->paused) {
        if (m_data->_state != GST_STATE_PAUSED) {
          LogMessage(m_data->_name + ": Pausing", ELL_INFO);
          gst_element_set_state(m_data->gstPipeline, GST_STATE_PAUSED);
        }
      } else {
        if (m_data->_state != GST_STATE_PLAYING) {
          m_data->TriggerPlay(false);
        }
      }
      /*   gst_element_get_state(m_data->gstPipeline, &state, NULL,
                        2 * GST_SECOND);*/
    } else {
      GstState state = GST_STATE_PAUSED;
      if (m_data->_state != GST_STATE_PAUSED) {
        LogMessage(m_data->_name + ": Pausing", ELL_INFO);
        gst_element_set_state(m_data->gstPipeline, state);
      }
      /* gst_element_get_state(m_data->gstPipeline, &state, NULL,
                     2 * GST_SECOND);*/

      if (!m_data->freeze) {
        if (!m_data->paused) {
          LogMessage(m_data->_name + ": Playing", ELL_INFO);
          gst_element_set_state(m_data->gstPipeline, GST_STATE_PLAYING);
        }
        m_data->playing = true;
      }
    }
  }
}
void GstPipelineHandler::Stop() {
  LogMessage(m_data->_name + ": GstPipelineHandler::Stop ", ELL_INFO);
  if (!m_data->Loaded) return;
  std::lock_guard<std::recursive_mutex> lock(GetMutex());
  GstState state;
  m_data->_seekPos = GST_CLOCK_TIME_NONE;
  // gst_element_send_event(m_data->gstPipeline, gst_event_new_eos());
  // if (!m_data->paused)
  {
    state = GST_STATE_PAUSED;
    gst_element_set_state(m_data->gstPipeline, state);
    gst_element_get_state(m_data->gstPipeline, &state, NULL, 2 * GST_SECOND);
  }

  state = GST_STATE_READY;
  gst_element_set_state(m_data->gstPipeline, state);
  gst_element_get_state(m_data->gstPipeline, &state, NULL, 2 * GST_SECOND);
  m_data->playing = false;
  m_data->paused = true;
}
bool GstPipelineHandler::IsLoaded() { return m_data->Loaded; }
bool GstPipelineHandler::IsPaused() { return m_data->paused; }
bool GstPipelineHandler::IsPlaying() { return m_data->playing; }
bool GstPipelineHandler::QueryLatency(bool &isLive, ulong &minLatency,
                                      ulong &maxLatency) {
  bool ok = false;
  GstQuery *q = gst_query_new_latency();
  if (gst_element_query(m_data->gstPipeline, q)) {
    GstClockTime minlat = 0, maxlat = 0;
    gboolean live;
    gst_query_parse_latency(q, &live, &minlat, &maxlat);
    isLive = live;
    minLatency = minlat;
    maxLatency = maxlat;
    ok = true;
  }
  gst_query_unref(q);
  return ok;
}
void GstPipelineHandler::Close() {
  LogMessage(m_data->_name + ": GstPipelineHandler::Close ", ELL_INFO);
  std::lock_guard<std::recursive_mutex> lock(GetMutex());
  Stop();

  if (true || m_data->Loaded) {
    if (m_data->busWatchID != 0) g_source_remove(m_data->busWatchID);
    m_data->busWatchID = 0;

    GstState state;
    gst_element_set_state(GST_ELEMENT(m_data->gstPipeline), GST_STATE_NULL);
    gst_element_get_state(m_data->gstPipeline, &state, NULL, 2 * GST_SECOND);

    gst_object_unref(m_data->gstPipeline);
    m_data->gstPipeline = NULL;
    m_data->Loaded = false;
  }

  FIRE_LISTENR_METHOD(OnPipelineClosed, (this));
}
bool GstPipelineHandler::HandleMessage(GstBus *bus, GstMessage *msg) {
  // 		if (m_data->closing)
  // 			return true;
  std::lock_guard<std::recursive_mutex> lock(GetMutex(1));
  if (!m_data || !m_data->gstPipeline) return false;
  if (m_data->busWatchID == 0) return true;
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS: {
      LogMessage(
          m_data->_name + std::string(": GstPipelineHandler::HandleMessage(): "
                                      "Received EOS message "),
          ELL_INFO);

      // Stop();
      // when an EOS occur, we need to reset the head to the last frame or else
      // it will be reset to zero
      if (!IsRendering()) {
        m_data->_seekPos = GetDuration() - 1;  // GetPosition();

        m_data->_eosSeekDone = false;
        m_data->_restartingEOS = true;
        m_data->paused = true;
        LogMessage(m_data->_name + std::string(": Restarting Pipeline "),
                   ELL_INFO);
        GstState state;
        gst_element_set_state(m_data->gstPipeline, GST_STATE_NULL);
        gst_element_get_state(m_data->gstPipeline, &state, NULL,
                              2 * GST_SECOND);
        gst_element_set_state(m_data->gstPipeline, GST_STATE_PAUSED);
        gst_element_get_state(m_data->gstPipeline, &state, NULL,
                              2 * GST_SECOND);
        LogMessage(m_data->_name + std::string(": Done Restarting Pipeline "),
                   ELL_INFO);
      }
      FIRE_LISTENR_METHOD(OnPipelineEOS, (this));
      // m_data->_seekDone(false);
    } break;
    case GST_MESSAGE_BUFFERING:
      gint pctBuffered;
      gst_message_parse_buffering(msg, &pctBuffered);
      LogMessage("Buffering --> " + std::to_string(pctBuffered) + "%",
                 ELL_INFO);
      FIRE_LISTENR_METHOD(OnPipelineBuffering, (this, pctBuffered));
      break;

#if GST_VERSION_MAJOR == 0
    case GST_MESSAGE_DURATION: {
      GstFormat format = GST_FORMAT_TIME;
      gst_element_query_duration(m_data->gstPipeline, &format, &durationNanos);
    } break;
#else
    case GST_MESSAGE_DURATION_CHANGED:
      // gst_element_query_duration(m_data->gstPipeline, GST_FORMAT_TIME,
      // &durationNanos);
      break;
#endif

    case GST_MESSAGE_STATE_CHANGED: {
      GstState oldstate, newstate, pendstate;
      gst_message_parse_state_changed(msg, &oldstate, &newstate, &pendstate);
      if (GST_MESSAGE_SRC(msg) ==
          GST_OBJECT(m_data->gstPipeline) /* && oldstate != newstate*/) {
        FIRE_LISTENR_METHOD(OnStatusChanged,
                            (this, oldstate, newstate, pendstate));
        m_data->_state = newstate;
        if (oldstate != newstate)
          LogMessage(m_data->_name +
                         ": Pipeline State Changes: " + STATE_STRING[oldstate] +
                         "-->" + STATE_STRING[newstate],
                     ELL_INFO);
        if (oldstate == GST_STATE_READY && newstate == GST_STATE_PAUSED) {
          if (m_data->_seekPos != -1) {
            SetPosition(m_data->_seekPos, false);
          }
        }
        // gchar* name = gst_element_get_name(GST_MESSAGE_SRC(msg));
        // printf("Source: %s\n", name);

        // g_free(name);
        if (newstate == GST_STATE_PAUSED) {
          if (m_data->_restartingEOS) {
            LogMessage(m_data->_name + ": Reseting Seek Position after EOS: " +
                           std::to_string(m_data->_seekPos),
                       ELL_INFO);
            m_data->_executeSeek();
            m_data->_eosSeekDone = true;
          } else {
            m_data->_seekDone(true);
            if (!m_data->playing) {
              //  printf("OnPipelinePlay\n");
              m_data->Loaded = true;
              if (!m_data->paused) {
                m_data->playing = true;
                SetPaused(false);
              } else {
                m_data->_executeSeek();
              }
            }
          }
        } else if (newstate == GST_STATE_READY) {
          //    printf("OnPipelineReady\n");
          FIRE_LISTENR_METHOD(OnPipelineReady, (this));
        } else if (newstate == GST_STATE_PLAYING) {
          //     printf("OnPipelinePlaying\n");
          FIRE_LISTENR_METHOD(OnPipelinePlaying, (this));

          if (m_data->paused) {
            m_data->playing = true;
            SetPaused(true);
          }
        } else if (newstate == GST_STATE_PAUSED) {
          //  printf("OnPipelineStopped\n");
          FIRE_LISTENR_METHOD(OnPipelineStopped, (this));
        }
      }
    } break;

    case GST_MESSAGE_ASYNC_DONE: {
      // LogMessage("GST_MESSAGE_ASYNC_DONE ", ELL_INFO);
      if (!m_data->seekingDone) {
        m_data->seekingDone = true;
        if (m_data->_seekPos != GST_CLOCK_TIME_NONE) {
          m_data->_executeSeek();
        } else {
          m_data->_seekDone(true);
        }
      }
      if (!m_data->paused) {
        m_data->TriggerPlay(true);
      }
      if (m_data->_eosSeekDone && m_data->_restartingEOS) {
        gint64 p;
        if (gst_element_query_position(m_data->gstPipeline, GST_FORMAT_TIME,
                                       &p) &&
            p != 0) {
          m_data->_eosSeekDone = false;
          m_data->_restartingEOS = false;
        }
      }
    } break;

    case GST_MESSAGE_PROGRESS: {
      GstProgressType type;
      gchar *code, *text;
      bool in_progress = false;
      gst_message_parse_progress(msg, &type, &code, &text);
      switch (type) {
        case GST_PROGRESS_TYPE_START:
        case GST_PROGRESS_TYPE_CONTINUE:
          in_progress = TRUE;
          break;
        case GST_PROGRESS_TYPE_COMPLETE:
        case GST_PROGRESS_TYPE_CANCELED:
        case GST_PROGRESS_TYPE_ERROR:
          in_progress = FALSE;
          break;
        default:
          break;
      }
      LogMessage("Progress: (" + std::string(code) + ") " + std::string(text),
                 ELL_INFO);
      g_free(code);
      g_free(text);

    } break;

    case GST_MESSAGE_WARNING: {
      GError *err;
      gchar *debug;
      gst_message_parse_warning(msg, &err, &debug);
      gchar *name = gst_element_get_name(GST_MESSAGE_SRC(msg));

      LogMessage(m_data->_name +
                     std::string(": GstPipelineHandler::HandleMessage(): "
                                 "warning in module ") +
                     name + std::string("  reported: ") + err->message,
                 ELL_WARNING);

      g_free(name);
      g_error_free(err);
      g_free(debug);
      if (!m_data->seekingDone) {
        m_data->_seekDone(true);
      }
      if (m_data) {
        FIRE_LISTENR_METHOD(OnPipelineWarning, (this));
        // gst_element_set_state(GST_ELEMENT(m_data->gstPipeline),
        // GST_STATE_NULL);
      }
      //	gst_element_set_state(GST_ELEMENT(m_data->gstPipeline),
      // GST_STATE_NULL);
    } break;
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;
      gst_message_parse_error(msg, &err, &debug);
      gchar *name = gst_element_get_name(GST_MESSAGE_SRC(msg));

      LogMessage(m_data->_name +
                     std::string(": GstPipelineHandler::HandleMessage(): "
                                 "error in module ") +
                     name + std::string("  reported: ") + err->message +
                     std::string(" - ") + debug,
                 ELL_WARNING);

      g_free(name);
      g_error_free(err);
      g_free(debug);

      if (m_data) {
        FIRE_LISTENR_METHOD(OnPipelineError, (this));
        // gst_element_set_state(GST_ELEMENT(m_data->gstPipeline),
        // GST_STATE_NULL);
      }
    } break;

    default:
      break;
  }

  return true;
}

void GstPipelineHandler::SetClockBaseTime(ulong baseTime) {
  m_data->baseTime = baseTime;
  if (m_data->gstPipeline) {
    GstClock *clock = gst_pipeline_get_clock(GST_PIPELINE(m_data->gstPipeline));

    gst_element_set_base_time(m_data->gstPipeline,
                              baseTime);  // -gst_clock_get_time(clock));
  }
}
ulong GstPipelineHandler::GetClockBaseTime() {
  if (m_data->clock) return gst_clock_get_time(m_data->clock);
  return m_data->baseTime;
}
bool GstPipelineHandler::busFunction(GstBus *bus, GstMessage *message,
                                     GstPipelineHandler *player) {
  return player->HandleMessage(bus, message);
}
void GstPipelineHandler::SetPipeline(GstElement *p) { m_data->gstPipeline = p; }
GstElement *GstPipelineHandler::GetPipeline() { return m_data->gstPipeline; }

void GstPipelineHandler::Freeze(bool f) {
  bool oldF = m_data->freeze;
  m_data->freeze = f;
  if (!f && oldF) {
    m_data->_executeSeek();
    if (!m_data->paused) {
      SetPaused(false);
    }
  }
}

typedef gboolean (*QueryFun)(GstElement *, GstFormat, gint64 *);

static void printOne(const GValue *item, gpointer ptr) {
  QueryFun fun = (QueryFun)ptr;
  GstElement *elem = GST_ELEMENT(g_value_get_object(item));
  gboolean res;

  GstElementFactory *f = gst_element_get_factory(elem);
  if (f != 0) LogMessage(GST_OBJECT_NAME(f), ELL_INFO);

  if (GST_IS_BIN(elem)) {
    GstIterator *it = gst_bin_iterate_recurse(GST_BIN(elem));
    gst_iterator_foreach(it, printOne, NULL);
    gst_iterator_free(it);
  }
}

void GstPipelineHandler::PrintElements() {
  // LogMessage(nm, ELL_INFO);
  GstIterator *it = gst_bin_iterate_recurse(GST_BIN(m_data->gstPipeline));
  gst_iterator_foreach(it, printOne, NULL);
  gst_iterator_free(it);
}

void GstPipelineHandler::ValidateSeek() { m_data->_executeSeek(); }

std::recursive_mutex &GstPipelineHandler::GetMutex(int i) { return _mutex[i]; }
}  // namespace video
}  // namespace mray
