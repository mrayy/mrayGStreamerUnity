

#include "stdafx.h"
#include "GstNetworkMultipleVideoPlayer.h"

#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"

#include "CMySrc.h"
#include "CMySink.h"

#include "VideoAppSinkHandler.h"
#include "GstPipelineHandler.h"
#include "UnityHelpers.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <vector>

namespace mray
{
namespace video
{


class GstNetworkMultipleVideoPlayerImpl :public GstPipelineHandler,public IPipelineListener
{
	GstNetworkMultipleVideoPlayer* m_owner;
	std::string m_ipAddr;
	uint m_clockPort;
	uint m_baseVideoSrc;
	int m_playersCount;

	std::string m_pipeLineString;


	GstMyUDPSrc* m_videoRtcpSrc;
	GstMyUDPSink* m_videoRtcpSink;
	bool m_rtcp;

	struct VideoHandler
	{
		VideoHandler()
		{
			videoPort = 5000;
			videoSrc = 0;
			videoSink = 0;
		}
		VideoAppSinkHandler handler;
		uint videoPort;

		GstMyUDPSrc* videoSrc;
		GstAppSink* videoSink;
	};

	std::vector<VideoHandler> m_videoHandler;

public:
	GstNetworkMultipleVideoPlayerImpl(GstNetworkMultipleVideoPlayer* o)
	{
		m_owner = o;
		m_ipAddr = "127.0.0.1";
		m_clockPort = 5010;
		m_videoRtcpSrc = 0;
		m_videoRtcpSink = 0;
		m_playersCount = 0;
		m_videoHandler.resize(1);
		AddListener(this);
	}
	virtual ~GstNetworkMultipleVideoPlayerImpl()
	{

	}

	std::string _BuildPipelineH264(int i)
	{
		std::stringstream ss;
		std::string videoStr;
		ss<<"myudpsrc name=videoSrc"<<i<<" !"
			//"udpsrc port=7000 buffer-size=2097152 do-timestamp=true !"
			"application/x-rtp ";
		videoStr = ss.str();
		ss = std::stringstream();
		if (m_rtcp)
		{
			ss<<
				"rtpbin "
				"name=rtpbin "
				<< videoStr <<
				"! rtpbin.recv_rtp_sink_0 "
				"rtpbin. ! rtph264depay !  avdec_h264 ! "
				"videoconvert ! video/x-raw,format=RGB  !"
				" appsink name=videoSink" <<i<<
				//video rtcp
				"myudpsrc name=videoRtcpSrc ! rtpbin.recv_rtcp_sink_0 "
				"rtpbin.send_rtcp_src_0 !  myudpsink name=videoRtcpSink sync=false async=false ";
			videoStr = ss.str();
		}
		else
		{
			ss<<videoStr <<"! queue !"
				"rtpjitterbuffer !  "
				"rtph264depay ! h264parse !  avdec_h264 ! "
				// " videorate  ! "//"video/x-raw,framerate=60/1 ! "
				//	"videoconvert ! video/x-raw,format=RGB  !" // Very slow!!
				"videorate ! "
				" videoconvert ! video/x-raw,format=I420  !"
				//"videoconvert ! video/x-raw,format=RGB  !"
			//	" timeoverlay halignment=right text=\"Local Time =\"! "
			" appsink name=videoSink" <<i<< " sync=false  emit-signals=false ";
				//"fpsdisplaysink sync=false";

			videoStr = ss.str();

		}
		return videoStr;
	}


	void _BuildPipeline()
	{
		m_pipeLineString = "";
		for (int i = 0; i < m_playersCount; ++i)
			m_pipeLineString+= _BuildPipelineH264(i)+" ";
	}

	void _UpdatePorts()
	{
		if (!GetPipeline())
			return;
#define SET_SRC(idx,name,p) m_videoHandler[idx].videoSrc=GST_MyUDPSrc(gst_bin_get_by_name(GST_BIN(GetPipeline()), name)); if(m_videoHandler[idx].videoSrc){m_videoHandler[idx].videoSrc->SetPort(p);}
#define SET_SINK(idx,name,p) m_videoHandler[idx].videoSink=GST_MyUDPSink(gst_bin_get_by_name(GST_BIN(GetPipeline()), name)); if(m_videoHandler[idx].videoSink){m_videoHandler[idx].videoSink->SetPort(m_ipAddr,p);}

		for (int i = 0; i < m_playersCount; ++i)
		{
			std::stringstream ss;
			ss<<"videoSrc"<<i;
			SET_SRC(i, ss.str().c_str(), m_videoHandler[i].videoPort);
// 			SET_SINK(i, videoRtcpSink, (m_videoHandler[i].videoPort + 1));
// 			SET_SRC(i, videoRtcpSrc, (m_videoHandler[i].videoPort + 2));
		}
	}

	void SetIPAddress(const std::string& ip, uint videoPort, int count,uint clockPort, bool rtcp)
	{
		m_playersCount = count;
		m_ipAddr = ip;
		m_baseVideoSrc= videoPort;
		m_clockPort = clockPort;
		m_rtcp = rtcp;

		m_videoHandler.resize(m_playersCount);
		for (int i = 0; i < m_playersCount; ++i)
		{
			m_videoHandler[i].videoPort = m_baseVideoSrc + i;
		}

		//set src and sinks elements
		_UpdatePorts();
	}

	
	static GstFlowReturn new_buffer(GstElement *sink, void *data) {
		GstBuffer *buffer;
		GstMapInfo map;
		GstSample *sample;
		//gsize size;



		g_signal_emit_by_name(sink, "pull-sample", &sample, NULL);
		if (sample) {

			buffer = gst_sample_get_buffer(sample);

			gst_buffer_map(buffer, &map, GST_MAP_READ);

		//	fwrite(map.data, 1, map.size, data->audio_file);

			gst_buffer_unmap(buffer, &map);
			gst_sample_unref(sample);

		}

		return GST_FLOW_OK;
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
			LogMessage("GstNetworkMultipleVideoPlayer: Pipeline error: "+ std::string(err->message),ELL_WARNING);
		}
		if (!p)
			return false;
		SetPipeline(p);
		printf("Connecting Video stream with IP:%s\n", m_ipAddr.c_str());

		_UpdatePorts();

		for (int i = 0; i < m_playersCount; ++i)
		{
			std::stringstream ss;
			ss<<"videoSink" <<i;

			m_videoHandler[i].videoSink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(p), ss.str().c_str()));

			m_videoHandler[i].handler.SetSink(m_videoHandler[i].videoSink);
			g_signal_connect(m_videoHandler[i].videoSink, "new-sample", G_CALLBACK(new_buffer), this);
			//attach videosink callbacks
			GstAppSinkCallbacks gstCallbacks;
			gstCallbacks.eos = &VideoAppSinkHandler::on_eos_from_source;
			gstCallbacks.new_preroll = &VideoAppSinkHandler::on_new_preroll_from_source;
	#if GST_VERSION_MAJOR==0
			gstCallbacks.new_buffer = &VideoAppSinkHandler::on_new_buffer_from_source;
	#else
			gstCallbacks.new_sample = &VideoAppSinkHandler::on_new_buffer_from_source;
	#endif
			gst_app_sink_set_callbacks(GST_APP_SINK(m_videoHandler[i].videoSink), &gstCallbacks, &m_videoHandler[i].handler, NULL);
			//gst_app_sink_set_emit_signals(GST_APP_SINK(m_videoSink), true);

		
			// 		gst_base_sink_set_async_enabled(GST_BASE_SINK(m_videoSink), TRUE);
			gst_base_sink_set_sync(GST_BASE_SINK(m_videoHandler[i].videoSink), false);
			gst_app_sink_set_drop(GST_APP_SINK(m_videoHandler[i].videoSink), TRUE);
			gst_app_sink_set_max_buffers(GST_APP_SINK(m_videoHandler[i].videoSink), 8);
			gst_base_sink_set_max_lateness(GST_BASE_SINK(m_videoHandler[i].videoSink), 0);
			/*
			GstCaps* caps;
			caps = gst_caps_new_simple("video/x-raw",
				"format", G_TYPE_STRING, "RGB",
				"pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
				"width", G_TYPE_INT, 1280,
				"height", G_TYPE_INT, 720,
				NULL);

			gst_app_sink_set_caps(GST_APP_SINK(m_videoSink), caps);
			gst_caps_unref(caps);
			// Set the configured video appsink to the main pipeline
			g_object_set(m_gstPipeline, "video-sink", m_videoSink, (void*)NULL);
			// Tell the video appsink that it should not emit signals as the buffer retrieving is handled via callback methods
			g_object_set(m_videoSink, "emit-signals", false, "sync", false, "async", false, (void*)NULL);
			*/

		}

		return CreatePipeline(false,m_ipAddr,m_clockPort);

	}

	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		for (int i = 0; i < m_playersCount; ++i)
		{
			if (m_videoHandler[i].videoSrc && m_videoHandler[i].videoSrc->m_client)
				m_videoHandler[i].videoSrc->m_client->Close();
			m_videoHandler[i].handler.Close();
		}
		GstPipelineHandler::Close();
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


	virtual const Vector2d& GetFrameSize(int i)
	{
		return m_videoHandler[i].handler.GetFrameSize();
	}

	virtual video::EPixelFormat GetImageFormat(int i)
	{
		return m_videoHandler[i].handler.getPixelsRef()->format;
	}
	int GetFramesCount()
	{
		return m_playersCount;
	}
	virtual bool GrabFrame()
	{ 
		bool ret = false;
		for (int i = 0; i < m_videoHandler.size();++i) 
			ret |= m_videoHandler[i].handler.GrabFrame();
		return ret;
	}
	virtual bool HasNewFrame()
	{
		bool ret = false;
		for (int i = 0; i < m_videoHandler.size(); ++i)
			ret |= m_videoHandler[i].handler.isFrameNew();
		return ret;
	}
	virtual ulong GetBufferID()
	{
		return m_videoHandler[0].handler.GetFrameID();
	}

	virtual float GetCaptureFrameRate(){ return m_videoHandler[0].handler.GetCaptureFrameRate(); }

	virtual const ImageInfo* GetLastFrame(int i)
	{ 
		if (i > m_playersCount)
			return 0;
		else
			return m_videoHandler[i].handler.getPixelsRef(); 
	}


	virtual void OnPipelineReady(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerReady(m_owner); }
	virtual void OnPipelinePlaying(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerStarted(m_owner); }
	virtual void OnPipelineStopped(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerStopped(m_owner); }

};


GstNetworkMultipleVideoPlayer::GstNetworkMultipleVideoPlayer()
{
	m_impl = new GstNetworkMultipleVideoPlayerImpl(this);
}

GstNetworkMultipleVideoPlayer::~GstNetworkMultipleVideoPlayer()
{
	delete m_impl;
}
void GstNetworkMultipleVideoPlayer::SetIPAddress(const std::string& ip, uint videoPort, uint count, uint clockPort, bool rtcp)
{
	m_impl->SetIPAddress(ip, videoPort,count, clockPort,rtcp);
}
bool GstNetworkMultipleVideoPlayer::CreateStream()
{
	return m_impl->CreateStream();
}
GstPipelineHandler*GstNetworkMultipleVideoPlayer::GetPipeline()
{
	return m_impl;
}


void GstNetworkMultipleVideoPlayer::SetVolume(float vol)
{
	m_impl->SetVolume(vol);
}
bool GstNetworkMultipleVideoPlayer::IsStream()
{
	return m_impl->IsStream();
}

void GstNetworkMultipleVideoPlayer::Play()
{
	m_impl->Play();
}
void GstNetworkMultipleVideoPlayer::Stop()
{
	m_impl->Stop();
}
void GstNetworkMultipleVideoPlayer::Pause()
{
	m_impl->Pause();
}
bool GstNetworkMultipleVideoPlayer::IsLoaded()
{
	return m_impl->IsLoaded();
}
bool GstNetworkMultipleVideoPlayer::IsPlaying()
{
	return m_impl->IsPlaying();
}
void GstNetworkMultipleVideoPlayer::Close()
{
	m_impl->Close();

}

const Vector2d& GstNetworkMultipleVideoPlayer::GetFrameSize()
{
	return m_impl->GetFrameSize(0);
}

video::EPixelFormat GstNetworkMultipleVideoPlayer::GetImageFormat()
{
	return m_impl->GetImageFormat(0);
}

int GstNetworkMultipleVideoPlayer::GetFramesCount()
{
	return m_impl->GetFramesCount();
}

bool GstNetworkMultipleVideoPlayer::GrabFrame()
{
	return m_impl->GrabFrame();
}

bool GstNetworkMultipleVideoPlayer::HasNewFrame()
{
	return m_impl->HasNewFrame();
}

ulong GstNetworkMultipleVideoPlayer::GetBufferID()
{
	return m_impl->GetBufferID();
}

float GstNetworkMultipleVideoPlayer::GetCaptureFrameRate()
{
	return m_impl->GetCaptureFrameRate();
}

const ImageInfo* GstNetworkMultipleVideoPlayer::GetLastFrame(int i)
{
	return m_impl->GetLastFrame(i);
}


}
}


