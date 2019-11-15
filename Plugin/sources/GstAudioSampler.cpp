

#include "stdafx.h"
#include "GstAudioSampler.h"

#include "GstPipelineHandler.h"
#include "UnityHelpers.h"
#include "AudioAppSinkHandler.h"
#include "IAudioGrabber.h"
#include "LocalAudioGrabber.h"

#include <gst/app/gstappsrc.h>
#include <gst/gst.h>


namespace mray
{
namespace video
{

class GstAudioSamplerImpl :public GstPipelineHandler
{
	std::string m_pipeLineString;

	GstAppSrcCallbacks m_srcCB;
	uint m_appSourceID;
	GstAppSrc* m_audioSrc;
	int m_interface;
	AudioAppSinkHandler m_audioHandler;
	IAudioGrabber* m_audioGrabber;

	GstAppSink* m_audioSink;
	int m_sampleRate;

public:
	GstAudioSamplerImpl()
	{
		m_interface = 0;
		m_appSourceID = 0;
		m_audioGrabber = 0;
		m_audioSrc = 0;
		m_sampleRate = 32000;
		m_audioSink = 0;
	}
	virtual ~GstAudioSamplerImpl()
	{

	}
	void Init(int audioInterface, int samplingRate)
	{
		m_interface = audioInterface;
		m_sampleRate = samplingRate;
	}
	void SetAudioGrabber(IAudioGrabber* g)
	{
		m_audioGrabber = g;
	}

	GstFlowReturn NeedBuffer(GstBuffer ** buffer)
	{
		if (!IsPlaying())
			return GST_FLOW_ERROR;

		if (!m_audioGrabber)
		{
			LogMessage("GstAudioSampler::NeedBuffer() -No audio grabber is assigned to GstAudioSampler", ELL_WARNING);
			return GST_FLOW_ERROR;
		}
		if (!m_audioGrabber->GrabFrame())
		{
			//	LogMessage("AppSrcVideoSrc::NeedBuffer() - Failed to grab buffer " + std::StringConverter::toString(index), ELL_WARNING);
			return GST_FLOW_ERROR;
		}
		//m_grabber[index]->Lock();

		float* data = m_audioGrabber->GetAudioFrame();
		uint length = m_audioGrabber->GetAudioFrameSize() * sizeof(float);
		*buffer = gst_buffer_new_allocate(0, length, 0);
		gst_buffer_fill(*buffer, 0, data, length);
		//*buffer = gst_buffer_new_wrapped_full(GST_MEMORY_FLAG_READONLY, data, length, 0, length, 0, 0);
		return GST_FLOW_OK;
	}

	static gboolean read_data(GstAudioSamplerImpl *d)
	{
		GstFlowReturn ret;

		GstBuffer *buffer;
		if (d->NeedBuffer(&buffer) == GST_FLOW_OK)
		{
			ret = gst_app_src_push_buffer(d->m_audioSrc, buffer);
			//	LogMessage("pushing data to: ", std::StringConverter::toString(d->index),ELL_INFO);
			if (ret != GST_FLOW_OK) {
				LogMessage("GstAudioSampler::read_data() - Failed to push data to AppSrc ", ELL_WARNING);
				ret = gst_app_src_end_of_stream(d->m_audioSrc);
				return FALSE;
			}
		}
		return TRUE;

	}
	static void start_feed(GstAppSrc *source, guint size, gpointer data)
	{
		GstAudioSamplerImpl* o = static_cast<GstAudioSamplerImpl*>(data);
		if (o->m_appSourceID != 0)
		{
			g_source_remove(o->m_appSourceID);
			o->m_appSourceID = 0;
		}
		GST_DEBUG("start feeding");
		o->m_appSourceID = g_idle_add((GSourceFunc)read_data, o);
	}


	static void stop_feed(GstAppSrc *source, gpointer data)
	{
		GstAudioSamplerImpl* o = static_cast<GstAudioSamplerImpl*>(data);
		if (o->m_appSourceID != 0) {
			GST_DEBUG("stop feeding");
			g_source_remove(o->m_appSourceID);
			o->m_appSourceID = 0;
		}
	}
	static gboolean seek_data(GstAppSrc *src, guint64 offset, gpointer user_data)
	{
		return TRUE;
	}


	void _BuildPipeline()
	{

		std::stringstream ss;

		ss << "appsrc name=src is-live=true block=true stream-type=0 format=3 do-timestamp=true ";

		ss << " ! audio/x-raw,format=F32LE,rate=" << (m_audioGrabber->GetSamplingRate()) <<
			",channels=" << (m_audioGrabber->GetChannelsCount()) << " ! audioconvert ";
		// 		ss << " ! audioparse format=F32LE width=32 depth=32 signed=true rate=" << (m_audioGrabber->GetSamplingRate()) <<
		// 			" channels=" << (m_audioGrabber->GetChannelsCount()) << "   ";

		if (m_sampleRate != -1 && m_sampleRate != m_audioGrabber->GetSamplingRate())
		{
			ss << " !  audioresample ! audio/x-raw,rate=" << m_sampleRate << " ";
		}

		m_pipeLineString += " ! appsink name=audioSink sync=false  emit-signals=false";
		m_pipeLineString = ss.str();

	}
	bool CreateStream()
	{
		if (IsLoaded())
			return true;

		_BuildPipeline();

		GError *err = 0;
		GstElement* p = gst_parse_launch(m_pipeLineString.c_str(), &err);
		if (err)
		{
			LogMessage("GstAudioSampler:CreateStream() - Pipeline error:" + std::string(err->message), ELL_WARNING);
		}
		if (!p)
			return false;

		m_audioSrc = GST_APP_SRC(gst_bin_get_by_name(GST_BIN((GstElement*)p), "src"));

		//gst_base_src_set_do_timestamp(GST_BASE_SRC(m_audioSrc), true);
		/*
		g_object_set(G_OBJECT(m_audioSrc),
		"stream-type", GST_APP_STREAM_TYPE_STREAM, // GST_APP_STREAM_TYPE_STREAM
		"format", GST_FORMAT_TIME,
		"is-live", TRUE,
		NULL);
		gst_app_src_set_emit_signals(m_audioSrc, false);*/

		m_srcCB.need_data = &start_feed;
		m_srcCB.enough_data = &stop_feed;
		m_srcCB.seek_data = &seek_data;
		gst_app_src_set_callbacks(m_audioSrc, &m_srcCB, this, NULL);


		m_audioSink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(p), "audioSink"));
		m_audioHandler.SetSink(m_audioSink);

		//attach videosink callbacks
		GstAppSinkCallbacks gstCallbacks;
		gstCallbacks.eos = &AudioAppSinkHandler::on_eos_from_source;
		gstCallbacks.new_preroll = &AudioAppSinkHandler::on_new_preroll_from_source;
#if GST_VERSION_MAJOR==0
		gstCallbacks.new_buffer = &AudioAppSinkHandler::on_new_buffer_from_source;
#else
		gstCallbacks.new_sample = &AudioAppSinkHandler::on_new_buffer_from_source;
#endif
		gst_app_sink_set_callbacks(GST_APP_SINK(m_audioSink), &gstCallbacks, &m_audioHandler, NULL);
		//gst_app_sink_set_emit_signals(GST_APP_SINK(m_videoSink), true);


		// 		gst_base_sink_set_async_enabled(GST_BASE_SINK(m_videoSink), TRUE);
		gst_base_sink_set_sync(GST_BASE_SINK(m_audioSink), false);
		gst_app_sink_set_drop(GST_APP_SINK(m_audioSink), TRUE);
		gst_app_sink_set_max_buffers(GST_APP_SINK(m_audioSink), 8);
		gst_base_sink_set_max_lateness(GST_BASE_SINK(m_audioSink), 0);

		SetPipeline(p);

		LogMessage("GstAudioSampler:CreateStream() - Pipeline created", ELL_INFO);

		return CreatePipeline(false);

	}
	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		m_audioHandler.Close();
		/*
		if (m_audioSrc && m_audioSrc->m_client)
		m_audioSrc->m_client->Close();*/

		GstPipelineHandler::Close();
		LogMessage("GstAudioSampler:Close() - Connection closed", ELL_INFO);
	}

	void Play()
	{
		SetPaused(false);
	}

	void Pause()
	{
		SetPaused(true);
	}
	void SetVolume(float vol)
	{
		g_object_set(G_OBJECT(GetPipeline()), "volume", (gdouble)vol, (void*)0);
	}

	void SetSampleRate(int Rate)
	{
		m_sampleRate = Rate;
	}
	int GetSampleRate()
	{
		return m_audioHandler.SampleRate();
	}

	bool GrabFrame()
	{
		return m_audioHandler.GrabFrame();
	}
	int GetFrameSize()
	{
		return m_audioHandler.GetFrameSize();
	}
	bool CopyAudioFrame(float* output)
	{
		m_audioHandler.CopyAudioFrame(output);
		return true;
	}
	int ChannelsCount()
	{
		return m_audioHandler.ChannelsCount();
	}
};


GstAudioSampler::GstAudioSampler()
{
	m_impl = new GstAudioSamplerImpl();
}

GstAudioSampler::~GstAudioSampler()
{
	delete m_impl;
}
void GstAudioSampler::Init(int audioInterface, int samplingRate)
{
	m_impl->Init(audioInterface, samplingRate);
}
bool GstAudioSampler::CreateStream()
{
	return m_impl->CreateStream();
}
void GstAudioSampler::SetVolume(float vol)
{
	m_impl->SetVolume(vol);
}
bool GstAudioSampler::IsStream()
{
	return m_impl->IsStream();
}
GstPipelineHandler*GstAudioSampler::GetPipeline()
{
	return m_impl;
}


void GstAudioSampler::Play()
{
	m_impl->Play();
}
void GstAudioSampler::Stop()
{
	m_impl->Stop();
}
void GstAudioSampler::Pause()
{
	m_impl->Pause();
}
bool GstAudioSampler::IsLoaded()
{
	return m_impl->IsLoaded();
}
bool GstAudioSampler::IsPlaying()
{
	return m_impl->IsPlaying();
}
void GstAudioSampler::Close()
{
	m_impl->Close();

}
void GstAudioSampler::SetAudioGrabber(IAudioGrabber* g)
{
	m_impl->SetAudioGrabber(g);
}

void GstAudioSampler::SetSampleRate(int Rate)
{
	m_impl->SetSampleRate(Rate);
}
int GstAudioSampler::GetSampleRate()
{
	return m_impl->GetSampleRate();
}


bool GstAudioSampler::GrabFrame()
{
	return m_impl->GrabFrame();
}
int GstAudioSampler::GetFrameSize()
{
	return m_impl->GetFrameSize();
}
bool GstAudioSampler::CopyAudioFrame(float* output)
{
	return m_impl->CopyAudioFrame(output);
}
int GstAudioSampler::ChannelsCount()
{
	return m_impl->ChannelsCount();
}



}
}


