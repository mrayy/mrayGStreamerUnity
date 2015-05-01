

#include "stdafx.h"
#include "GstPipelineHandler.h"
#include "UnityHelpers.h"
#include "GStreamerCore.h"


namespace mray
{
namespace video
{


	GstPipelineHandler::GstPipelineHandler()
	{
		GStreamerCore::Instance()->Ref();

		m_gstPipeline = 0;
		m_closing = false;
		m_paused = true;
		m_Loaded = false;
		m_playing = false;
	}
	GstPipelineHandler::~GstPipelineHandler()
	{
		Close();
		GStreamerCore::Instance()->Unref();
	}

	bool GstPipelineHandler::CreatePipeline()
	{
		if (!m_gstPipeline)
			return false;

		//enable logging to stdout
		g_signal_connect(m_gstPipeline, "deep-notify", G_CALLBACK(gst_object_default_deep_notify), NULL);


		m_paused = true;
		m_Loaded = false;
		m_playing = false;

		GstBus * bus = gst_pipeline_get_bus(GST_PIPELINE(m_gstPipeline));

		if (bus){
			m_busWatchID = gst_bus_add_watch(bus, (GstBusFunc)busFunction, this);
		}

		gst_object_unref(bus);

		if (gst_element_set_state(GST_ELEMENT(m_gstPipeline), GST_STATE_READY) == GST_STATE_CHANGE_FAILURE) {
			LogMessage("GStreamerNetworkPlayer::Play(): unable to set pipeline to ready", ELL_WARNING);
			return false;
		}
		if (gst_element_get_state(GST_ELEMENT(m_gstPipeline), NULL, NULL, 10 * GST_SECOND) == GST_STATE_CHANGE_FAILURE){
			LogMessage("GStreamerNetworkPlayer::Play(): unable to get pipeline ready status", ELL_WARNING);
			return false;
		}

		// pause the pipeline
		if (gst_element_set_state(GST_ELEMENT(m_gstPipeline), GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
			LogMessage("GStreamerNetworkPlayer::Play(): unable to pause pipeline", ELL_WARNING);
			return false;
		}
		return true;
	}

	void GstPipelineHandler::SetPaused(bool p)
	{
		m_paused = p;
		if (m_Loaded)
		{
			if (m_playing)
			{
				if (m_paused)
					gst_element_set_state(m_gstPipeline, GST_STATE_PAUSED);
				else
					gst_element_set_state(m_gstPipeline, GST_STATE_PLAYING);
			}
			else
			{
				GstState state = GST_STATE_PAUSED;
				gst_element_set_state(m_gstPipeline, state);
				gst_element_get_state(m_gstPipeline, &state, NULL, 2 * GST_SECOND);
				if (!m_paused)
					gst_element_set_state(m_gstPipeline, GST_STATE_PLAYING);
				m_playing = true;
			}
		}
	}
	void GstPipelineHandler::Stop()
	{
		if (!m_Loaded)return;
		GstState state;
		if (!m_paused){
			state = GST_STATE_PAUSED;
			gst_element_set_state(m_gstPipeline, state);
			gst_element_get_state(m_gstPipeline, &state, NULL, 2 * GST_SECOND);
		}

		state = GST_STATE_READY;
		gst_element_set_state(m_gstPipeline, state);
		gst_element_get_state(m_gstPipeline, &state, NULL, 2 * GST_SECOND);
		m_playing = false;
		m_paused = true;
	}
	bool GstPipelineHandler::IsLoaded()
	{
		return m_Loaded;
	}
	bool GstPipelineHandler::IsPlaying()
	{
		return m_playing;
	}
	void GstPipelineHandler::Close()
	{

		Stop();

		if (m_Loaded){
			gst_element_set_state(GST_ELEMENT(m_gstPipeline), GST_STATE_NULL);
			gst_element_get_state(m_gstPipeline, NULL, NULL, 2 * GST_SECOND);

			if (m_busWatchID != 0) g_source_remove(m_busWatchID);

			gst_object_unref(m_gstPipeline);
			m_gstPipeline = NULL;
		}

		m_Loaded = false;
	}
	bool GstPipelineHandler::HandleMessage(GstBus * bus, GstMessage * msg)
	{
		switch (GST_MESSAGE_TYPE(msg)) {

		case GST_MESSAGE_BUFFERING:
			gint pctBuffered;
			gst_message_parse_buffering(msg, &pctBuffered);
			break;

#if GST_VERSION_MAJOR==0
		case GST_MESSAGE_DURATION:{
			GstFormat format = GST_FORMAT_TIME;
			gst_element_query_duration(m_gstPipeline, &format, &durationNanos);
		}break;
#else
		case GST_MESSAGE_DURATION_CHANGED:
			//gst_element_query_duration(m_gstPipeline, GST_FORMAT_TIME, &durationNanos);
			break;

#endif

		case GST_MESSAGE_STATE_CHANGED:{
			GstState oldstate, newstate, pendstate;
			gst_message_parse_state_changed(msg, &oldstate, &newstate, &pendstate);
			if (newstate == GST_STATE_PAUSED && !m_playing){
				m_Loaded = true;
				m_playing = true;
				if (!m_paused){
					SetPaused(false);
				}
			}

		}break;

		case GST_MESSAGE_ASYNC_DONE:
			break;

		case GST_MESSAGE_WARNING:
		{
			GError *err;
			gchar *debug;
			gst_message_parse_warning(msg, &err, &debug);
			gchar * name = gst_element_get_name(GST_MESSAGE_SRC(msg));

			std::stringstream ss;
			ss<< "GStreamerNetworkPlayer::HandleMessage(): warning in module "
				<< name << "  reported: " << err->message;
			LogMessage(ss.str(), ELL_WARNING);

			g_free(name);
			g_error_free(err);
			g_free(debug);

		//	gst_element_set_state(GST_ELEMENT(m_gstPipeline), GST_STATE_NULL);

		}break;
		case GST_MESSAGE_ERROR:
		{
			GError *err;
			gchar *debug;
			gst_message_parse_error(msg, &err, &debug);
			gchar * name = gst_element_get_name(GST_MESSAGE_SRC(msg));

			std::stringstream ss;
			ss << "GStreamerNetworkPlayer::HandleMessage(): error in module "
				<< name << "  reported: " << err->message;
			LogMessage(ss.str(), ELL_WARNING);

			g_free(name);
			g_error_free(err);
			g_free(debug);

			gst_element_set_state(GST_ELEMENT(m_gstPipeline), GST_STATE_NULL);

		}break;


		default:
			break;
		}

		return true;
	}


	bool GstPipelineHandler::busFunction(GstBus * bus, GstMessage * message, GstPipelineHandler * player)
	{
		return player->HandleMessage(bus, message);

	}




}
}

