

#include "stdafx.h"
#include "GstAppAudioPlayer.h"

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

class GstAppAudioPlayerImpl :public GstPipelineHandler
{
	std::string m_pipeLineString;

	GstAppSrcCallbacks m_srcCB;
	uint m_appSourceID;
	GstAppSrc* m_audioSrc;
	int m_interface;
	AudioAppSinkHandler m_audioHandler;
	IAudioGrabber* m_audioGrabber;

	int m_sampleRate;

public:
	GstAppAudioPlayerImpl()
	{
		m_interface = 0;
		m_appSourceID = 0;
		m_audioGrabber = 0;
		m_audioSrc = 0;
		m_sampleRate = 32000;
	}
	virtual ~GstAppAudioPlayerImpl()
	{

	}
	void Init(int audioInterface,int samplingRate)
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
			LogMessage("GstAppAudioPlayer::NeedBuffer() -No audio grabber is assigned to GstAppAudioPlayer", ELL_WARNING);
			return GST_FLOW_ERROR;
		}
		if (!m_audioGrabber->GrabFrame())
		{
			//	LogMessage("AppSrcVideoSrc::NeedBuffer() - Failed to grab buffer " + std::StringConverter::toString(index), ELL_WARNING);
			return GST_FLOW_ERROR;
		}
		//m_grabber[index]->Lock();

		GstMapInfo map;
		float* data = m_audioGrabber->GetAudioFrame();
		uint length = m_audioGrabber->GetAudioFrameSize() * sizeof(float);
		*buffer = gst_buffer_new_and_alloc(length);
		gst_buffer_map(*buffer, &map, GST_MAP_WRITE);
		memcpy(map.data, data, length);
		gst_buffer_unmap(*buffer, &map);

		//gst_buffer_fill(*buffer, 0, data, length);
		//*buffer = gst_buffer_new_wrapped_full(GST_MEMORY_FLAG_READONLY, data, length, 0, length, 0, 0);
		return GST_FLOW_OK;
	}

	static gboolean read_data(GstAppAudioPlayerImpl *d)
	{
		GstFlowReturn ret;

		if (!d->IsPlaying())
		{
			ret = gst_app_src_end_of_stream(d->m_audioSrc);
			return FALSE;
		}
		GstBuffer *buffer;
		if (d->NeedBuffer(&buffer) == GST_FLOW_OK)
		{
			ret = gst_app_src_push_buffer(d->m_audioSrc, buffer);
			//	LogMessage("pushing data to: ", std::StringConverter::toString(d->index),ELL_INFO);
			if (ret != GST_FLOW_OK) {
				LogMessage("GstAppAudioPlayer::read_data() - Failed to push data to AppSrc ", ELL_WARNING);
				ret = gst_app_src_end_of_stream(d->m_audioSrc);
				return FALSE;
			}
		}
		return TRUE;

	}
	static void start_feed(GstAppSrc *source, guint size, gpointer data)
	{
		GstAppAudioPlayerImpl* o = static_cast<GstAppAudioPlayerImpl*>(data);
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
		GstAppAudioPlayerImpl* o = static_cast<GstAppAudioPlayerImpl*>(data);
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
			",channels=" << (m_audioGrabber->GetChannelsCount()) << "  ";
// 		ss << " ! audioparse format=F32LE width=32 depth=32 signed=true rate=" << (m_audioGrabber->GetSamplingRate()) <<
// 			" channels=" << (m_audioGrabber->GetChannelsCount()) << "   ";

		if (m_sampleRate != -1 && m_sampleRate != m_audioGrabber->GetSamplingRate())
		{
			ss << " ! audioresample ! audio/x-raw,rate=" << m_sampleRate << " ";
		}

/**/

		if (false)
		{
			m_pipeLineString += " ! fakesink ";
		}
		else
		{
			ss << " ! audiorate ! audioconvert ! autoaudiosink ";
			if (m_interface > 0)
			{
				std::string guid=LocalAudioGrabber::GetOutputInterfaceGUID(m_interface);
				if(guid!="")
					ss << " device=\""<< guid<<"\" ";
			}
			ss<<"  sync=true";//buffer-time=100000
		}
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
			LogMessage("GstAppAudioPlayer:CreateStream() - Pipeline error:" + std::string(err->message), ELL_WARNING);
		}
		if (!p)
			return false;
		SetPipeline(p);

		m_audioSrc = GST_APP_SRC(gst_bin_get_by_name(GST_BIN((GstElement*)p), "src"));


		gst_app_src_set_stream_type((GstAppSrc*)m_audioSrc, GST_APP_STREAM_TYPE_STREAM);
		g_signal_connect(m_audioSrc, "need-data", G_CALLBACK(start_feed), this);
		g_signal_connect(m_audioSrc, "enough-data", G_CALLBACK(stop_feed), this);
		/*
		m_srcCB.need_data = &start_feed;
		m_srcCB.enough_data = &stop_feed;
		m_srcCB.seek_data = &seek_data;
		gst_app_src_set_callbacks(m_audioSrc, &m_srcCB, this, NULL);
		*/
		LogMessage("GstAppAudioPlayer:CreateStream() - Pipeline created", ELL_INFO);

		return CreatePipeline(false);

	}
	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		m_audioHandler.Close();
		gst_app_src_end_of_stream(m_audioSrc);
		/*
		if (m_audioSrc && m_audioSrc->m_client)
			m_audioSrc->m_client->Close();*/

		GstPipelineHandler::Close();
		LogMessage("GstAppAudioPlayer:Close() - Connection closed", ELL_INFO);
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
};


GstAppAudioPlayer::GstAppAudioPlayer()
{
	m_impl = new GstAppAudioPlayerImpl();
}

GstAppAudioPlayer::~GstAppAudioPlayer()
{
	delete m_impl;
}
void GstAppAudioPlayer::Init(int audioInterface, int samplingRate)
{
	m_impl->Init(audioInterface,samplingRate);
}
bool GstAppAudioPlayer::CreateStream()
{
	return m_impl->CreateStream();
}
void GstAppAudioPlayer::SetVolume(float vol)
{
	m_impl->SetVolume(vol);
}
bool GstAppAudioPlayer::IsStream()
{
	return m_impl->IsStream();
}
GstPipelineHandler*GstAppAudioPlayer::GetPipeline()
{
	return m_impl;
}


void GstAppAudioPlayer::Play()
{
	m_impl->Play();
}
void GstAppAudioPlayer::Stop()
{
	m_impl->Stop();
}
void GstAppAudioPlayer::Pause()
{
	m_impl->Pause();
}
bool GstAppAudioPlayer::IsLoaded()
{
	return m_impl->IsLoaded();
}
bool GstAppAudioPlayer::IsPlaying()
{
	return m_impl->IsPlaying();
}
void GstAppAudioPlayer::Close()
{
	m_impl->Close();

}
void GstAppAudioPlayer::SetAudioGrabber(IAudioGrabber* g)
{
	m_impl->SetAudioGrabber(g);
}


}
}


