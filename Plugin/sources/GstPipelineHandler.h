

#ifndef GstPipelineHandler_h__
#define GstPipelineHandler_h__

#include <gst/gst.h>

#include <mutex>
#include <string>

#include "ListenerContainer.h"
#include "mTypes.h"

namespace mray {
namespace video {

class GstPipelineHandlerImpl;
class GstPipelineHandler;

class IPipelineListener {
 public:
  virtual void OnPipelineLink(GstPipelineHandler* p) {}
  virtual void OnPipelineReady(GstPipelineHandler* p) {}
  virtual void OnPipelinePlaying(GstPipelineHandler* p) {}
  virtual void OnPipelineStopped(GstPipelineHandler* p) {}
  virtual void OnPipelineClosed(GstPipelineHandler* p) {}
  virtual void OnPipelineWarning(GstPipelineHandler* p) {}
  virtual void OnPipelineError(GstPipelineHandler* p, const std::string& elementName) {}
  virtual void OnPipelineEOS(GstPipelineHandler* p) {}
  virtual void OnPipelineBuffering(GstPipelineHandler* p, int percent) {}

  virtual void OnStatusChanged(GstPipelineHandler* p, GstState prevState,
                               GstState newState, GstState pendState) {}

  virtual void OnPreSeek(GstPipelineHandler* p, long pos) {}
  virtual void OnPostSeek(GstPipelineHandler* p) {}
  virtual void OnPipelineProgress(GstPipelineHandler* p, int progressType, const std::string& code, const std::string& info) {}
};

class GstPipelineHandler : public ListenerContainer<IPipelineListener*> {
 protected:
  friend class GstPipelineHandlerImpl;
  GstPipelineHandlerImpl* m_data;
  std::recursive_mutex _mutex[2];

  DECLARE_FIRE_METHOD(OnPipelineLink, (GstPipelineHandler * p), (p));
  DECLARE_FIRE_METHOD(OnPipelineReady, (GstPipelineHandler * p), (p));
  DECLARE_FIRE_METHOD(OnPipelinePlaying, (GstPipelineHandler * p), (p));
  DECLARE_FIRE_METHOD(OnPipelineStopped, (GstPipelineHandler * p), (p));
  DECLARE_FIRE_METHOD(OnPipelineClosed, (GstPipelineHandler * p), (p));
  DECLARE_FIRE_METHOD(OnPipelineWarning, (GstPipelineHandler * p), (p));
  DECLARE_FIRE_METHOD(OnPipelineError, (GstPipelineHandler * p, const std::string& elementName), (p, elementName));
  DECLARE_FIRE_METHOD(OnPipelineEOS, (GstPipelineHandler * p), (p));
  DECLARE_FIRE_METHOD(OnPreSeek, (GstPipelineHandler * p, long pos), (p, pos));
  DECLARE_FIRE_METHOD(OnPostSeek, (GstPipelineHandler * p), (p));
  DECLARE_FIRE_METHOD(OnStatusChanged,
                      (GstPipelineHandler * p, GstState prevState,
                       GstState newState, GstState pendState),
                      (p, prevState, newState, pendState));
  DECLARE_FIRE_METHOD(OnPipelineBuffering,
                      (GstPipelineHandler * p, int percent), (p, percent));
  DECLARE_FIRE_METHOD(OnPipelineProgress, (GstPipelineHandler* p, int progressType, const std::string& code, const std::string& info), (p, progressType, code, info));

 public:
  GstPipelineHandler(const char* name = "");
  virtual ~GstPipelineHandler();

  // set isMasterClock to true if
  virtual bool CreatePipeline(bool isMasterClock,
                              const std::string& clockIP = "",
                              uint clockPort = 7010);
  virtual void SetPaused(bool p);
  virtual void Stop();
  virtual bool IsLoaded();
  virtual bool IsPaused();
  virtual bool IsPlaying();
  virtual void Close();
  virtual bool HandleMessage(GstBus* bus, GstMessage* msg);
  virtual bool SetPosition(long pos, bool optimized = false);
  virtual long GetPosition();
  virtual long GetDuration();
  virtual bool IsResettingEOS();
  virtual bool IsRendering() { return false; }

  virtual bool QueryLatency(bool& isLive, ulong& minLatency, ulong& maxLatency);

  virtual void SetClockBaseTime(ulong baseTime);
  virtual ulong GetClockBaseTime();

  void SetPipeline(GstElement* p);
  GstElement* GetPipeline();

  void Freeze(bool f);
  static bool busFunction(GstBus* bus, GstMessage* message,
                          GstPipelineHandler* player);

  void PrintElements();

  void ValidateSeek();
  std::recursive_mutex& GetMutex(int i = 0);
};

}  // namespace video
}  // namespace mray

#endif  // GstPipelineHandler_h__
