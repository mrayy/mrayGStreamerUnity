
#include "stdafx.h"
#include "GstCustomVideoPlayer.h"

#include "UnityHelpers.h"
#include "VideoAppSinkHandler.h"
#include "GstPipelineHandler.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

namespace mray
{
namespace video
{

class GstCustomVideoPlayerImpl :public GstPipelineHandler
{
	std::string m_inputPipeline;
	std::string m_pipeLineString;
	
	GstAppSink* m_videoSink;

	VideoAppSinkHandler m_videoHandler;

public:
	GstCustomVideoPlayerImpl()
	{
		m_gstPipeline = 0;
		m_videoSink = 0;

	}
	virtual ~GstCustomVideoPlayerImpl()
	{

	}

	void _BuildPipeline()
	{
		m_pipeLineString = m_inputPipeline + 
			" ! appsink name=videoSink ";

	}


	void SetPipelineString(const std::string& pipeline)
	{
		m_inputPipeline = pipeline;
	}
	bool CreateStream()
	{
		if (m_Loaded)
			return true;
		_BuildPipeline();

		GError *err = 0;
		m_gstPipeline = gst_parse_launch(m_pipeLineString.c_str(), &err);
		if (err)
		{
			LogMessage(std::string("GstCustomVideoPlayer: Pipeline error:") + err->message, ELL_ERROR);
		}
		if (!m_gstPipeline)
			return false;

		m_videoSink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(m_gstPipeline), "videoSink"));

		m_videoHandler.SetSink(m_videoSink);

		//setup video sink
		gst_base_sink_set_sync(GST_BASE_SINK(m_videoSink), true);
		g_object_set(G_OBJECT(m_videoSink), "emit-signals", FALSE, "sync", false, (void*)NULL);

		//attach videosink callbacks
		GstAppSinkCallbacks gstCallbacks;
		gstCallbacks.eos = &VideoAppSinkHandler::on_eos_from_source;
		gstCallbacks.new_preroll = &VideoAppSinkHandler::on_new_preroll_from_source;
#if GST_VERSION_MAJOR==0
		gstCallbacks.new_buffer = &VideoAppSinkHandler::on_new_buffer_from_source;
#else
		gstCallbacks.new_sample = &VideoAppSinkHandler::on_new_buffer_from_source;
#endif
		gst_app_sink_set_callbacks(GST_APP_SINK(m_videoSink), &gstCallbacks, &m_videoHandler, NULL);


		return CreatePipeline();

	}

	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		GstPipelineHandler::Close();
		m_videoHandler.Close();
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
		g_object_set(G_OBJECT(m_gstPipeline), "volume", (gdouble)vol, (void*)0);
	}


	virtual const Vector2d& GetFrameSize()
	{
		return m_videoHandler.GetFrameSize();
	}

	virtual video::EPixelFormat GetImageFormat()
	{
		return m_videoHandler.getPixelsRef()->format;
	}

	virtual bool GrabFrame(){ return m_videoHandler.GrabFrame(); }
	virtual bool HasNewFrame(){ return m_videoHandler.isFrameNew(); }
	virtual ulong GetBufferID(){ return m_videoHandler.GetFrameID(); }

	virtual const ImageInfo* GetLastFrame(){ return m_videoHandler.getPixelsRef(); }
};


GstCustomVideoPlayer::GstCustomVideoPlayer()
{
	m_impl = new GstCustomVideoPlayerImpl();
}

GstCustomVideoPlayer::~GstCustomVideoPlayer()
{
	delete m_impl;
}
void GstCustomVideoPlayer::SetPipelineString(const std::string& pipeline)
{
	m_impl->SetPipelineString(pipeline);
}
bool GstCustomVideoPlayer::CreateStream()
{
	return m_impl->CreateStream();
}

void GstCustomVideoPlayer::SetVolume(float vol)
{
	m_impl->SetVolume(vol);
}
bool GstCustomVideoPlayer::IsStream()
{
	return m_impl->IsStream();
}

void GstCustomVideoPlayer::Play()
{
	m_impl->Play();
}
void GstCustomVideoPlayer::Stop()
{
	m_impl->Stop();
}
void GstCustomVideoPlayer::Pause()
{
	m_impl->Pause();
}
bool GstCustomVideoPlayer::IsLoaded()
{
	return m_impl->IsLoaded();
}
bool GstCustomVideoPlayer::IsPlaying()
{
	return m_impl->IsPlaying();
}
void GstCustomVideoPlayer::Close()
{
	m_impl->Close();

}

const Vector2d& GstCustomVideoPlayer::GetFrameSize()
{
	return m_impl->GetFrameSize();
}

video::EPixelFormat GstCustomVideoPlayer::GetImageFormat()
{
	return m_impl->GetImageFormat();
}

bool GstCustomVideoPlayer::GrabFrame()
{
	return m_impl->GrabFrame();
}

bool GstCustomVideoPlayer::HasNewFrame()
{
	return m_impl->HasNewFrame();
}

ulong GstCustomVideoPlayer::GetBufferID()
{
	return m_impl->GetBufferID();
}


const ImageInfo* GstCustomVideoPlayer::GetLastFrame()
{
	return m_impl->GetLastFrame();
}


}
}



