

#include "stdafx.h"
#include "GstNetworkVideoPlayer.h"

#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"

#include "CMySrc.h"
#include "CMySink.h"

#include "VideoAppSinkHandler.h"
#include "GstPipelineHandler.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

namespace mray
{
namespace video
{


class GstNetworkVideoPlayerImpl :public GstPipelineHandler,IPipelineListener
{
	GstNetworkVideoPlayer* m_owner;
	std::string m_ipAddr;
	uint m_videoPort;
	uint m_clockPort;

	std::string m_pipeLineString;

	GstElement* m_videoSrc;
	GstMyUDPSrc* m_videoRtcpSrc;
	GstMyUDPSink* m_videoRtcpSink;

	GstAppSink* m_videoSink;
	bool m_rtcp;

	VideoAppSinkHandler m_videoHandler;

public:
	GstNetworkVideoPlayerImpl(GstNetworkVideoPlayer* o)
	{
		m_owner = o;
		m_ipAddr = "127.0.0.1";
		m_videoPort = 5000;
		m_clockPort = 5010;
		m_videoSrc = 0;
		m_videoRtcpSrc = 0;
		m_videoRtcpSink = 0;
		m_videoSink = 0;
		AddListener(this);
	}
	virtual ~GstNetworkVideoPlayerImpl()
	{

	}

	void _BuildPipelineH264()
	{
		std::string videoStr =
			//video rtp
			"udpsrc name=videoSrc !"
			//"udpsrc port=7000 buffer-size=2097152 do-timestamp=true !"
			"application/x-rtp ";
		if (m_rtcp)
		{
			m_pipeLineString =
				"rtpbin "
				"name=rtpbin "
				+ videoStr +
				"! rtpbin.recv_rtp_sink_0 "
				"rtpbin. ! rtph264depay !  avdec_h264 ! "
				"videoconvert ! video/x-raw,format=RGB  !"
				" appsink name=videoSink "

				//video rtcp
				"myudpsrc name=videoRtcpSrc ! rtpbin.recv_rtcp_sink_0 "
				"rtpbin.send_rtcp_src_0 !  myudpsink name=videoRtcpSink sync=false async=false ";
		}
		else
		{
			m_pipeLineString = videoStr + "! queue !"
				"rtpjitterbuffer !  "
				"rtph264depay ! h264parse !  avdec_h264 ! "
				// " videorate  ! "//"video/x-raw,framerate=60/1 ! "
				//	"videoconvert ! video/x-raw,format=RGB  !" // Very slow!!
				"videoconvert ! video/x-raw,format=I420  !"
			//	" timeoverlay halignment=right text=\"Local Time =\"! "
			" appsink name=videoSink sync=false  emit-signals=false";
				//"fpsdisplaysink sync=false";

		}
	}


	void _BuildPipelineMJPEG()
	{
		std::string videoStr =
			//video rtp
			"udpsrc "
			"name=videoSrc "
			"caps=application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)JPEG ";
		if (m_rtcp)
		{
			m_pipeLineString =
				"rtpbin "
				"name=rtpbin "
				+ videoStr +
				"! rtpbin.recv_rtp_sink_0 "
				"rtpbin. ! rtpjpegdepay !  jpegdec ! "
				"videoconvert ! video/x-raw,format=RGB  !"
				" appsink name=videoSink "

				//video rtcp
				"myudpsrc name=videoRtcpSrc ! rtpbin.recv_rtcp_sink_0 "
				"rtpbin.send_rtcp_src_0 !  myudpsink name=videoRtcpSink sync=false async=false ";
		}
		else
		{
			m_pipeLineString = videoStr + "!"
				"rtpjpegdepay !  jpegdec ! "
				"videorate ! "
				"videoconvert ! video/x-raw,format=RGB  !"
				" appsink name=videoSink sync=false";

		}
	}

	void _UpdatePorts()
	{
		if (!GetPipeline())
			return;
#define SET_SRC(name,p) m_##name=GST_MyUDPSrc(gst_bin_get_by_name(GST_BIN(GetPipeline()), #name)); if(m_##name){m_##name->SetPort(p);}
#define SET_SINK(name,p) m_##name=GST_MyUDPSink(gst_bin_get_by_name(GST_BIN(GetPipeline()), #name)); if(m_##name){m_##name->SetPort(m_ipAddr,p);}

		//SET_SRC(videoSrc, m_videoPort);
		SET_SINK(videoRtcpSink, (m_videoPort + 1));
		SET_SRC(videoRtcpSrc, (m_videoPort + 2));

		g_object_set(m_videoSrc, "port", m_videoPort, "host",m_ipAddr.c_str(),0);
	}

	void SetIPAddress(const std::string& ip, uint videoPort, uint clockPort, bool rtcp)
	{
		m_ipAddr = ip;
		m_videoPort = videoPort;
		m_clockPort = clockPort;
		m_rtcp = rtcp;

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

			gst_buffer_unmap(buffer, &map);
			gst_sample_unref(sample);

		}

		return GST_FLOW_OK;
	}

	bool CreateStream()
	{
		if (IsLoaded())
			return true;
		_BuildPipelineH264();

		GError *err = 0;
		GstElement* p = gst_parse_launch(m_pipeLineString.c_str(), &err);
		if (err)
		{
			LogMessage("GstNetworkVideoPlayer: Pipeline error: " + std::string(err->message), ELL_WARNING);
		}
		if (!p)
			return false;
		SetPipeline(p);
		printf("Connecting Video stream with IP:%s\n", m_ipAddr.c_str());

		_UpdatePorts();

		m_videoSink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(p), "videoSink"));

		m_videoHandler.SetSink(m_videoSink);
		g_signal_connect(m_videoSink, "new-sample", G_CALLBACK(new_buffer), this);
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
		//gst_app_sink_set_emit_signals(GST_APP_SINK(m_videoSink), true);

		
		// 		gst_base_sink_set_async_enabled(GST_BASE_SINK(m_videoSink), TRUE);
// 		gst_base_sink_set_sync(GST_BASE_SINK(m_videoSink), false);
// 		gst_app_sink_set_drop(GST_APP_SINK(m_videoSink), TRUE);
//  		gst_app_sink_set_max_buffers(GST_APP_SINK(m_videoSink), 8);
//  		gst_base_sink_set_max_lateness(GST_BASE_SINK(m_videoSink), 0);
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

		return CreatePipeline(false,m_ipAddr,m_clockPort);

	}

	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		/*if (m_videoSrc && m_videoSrc->m_client)
			m_videoSrc->m_client->Close();*/
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
		g_object_set(G_OBJECT(GetPipeline()), "volume", (gdouble)vol, (void*)0);
	}

	virtual int GetPort(int i)
	{
		/*if (m_videoSrc && m_videoSrc->m_client)
			return m_videoSrc->m_client->Port();*/
		if (m_videoSrc)
		{
			gint port;
			g_object_get(m_videoSrc, "port", &port, 0);
			return port;
		}
		return m_videoPort;
	}


	virtual const Vector2d& GetFrameSize()
	{
		return m_videoHandler.GetFrameSize();
	}

	virtual video::EPixelFormat GetImageFormat()
	{
		return m_videoHandler.getPixelsRef()->format;
	}
	const GstImageFrame* GetLastDataFrame()
	{
		return m_videoHandler.getPixelFrame();
	}
	virtual unsigned long GetLastFrameTimestamp()
	{
		return m_videoHandler.getPixelFrame()->RTPPacket.timestamp;
	}
	void* GetLastFrameRTPMeta()
	{
		return &m_videoHandler.getPixelFrame()->RTPPacket;
	}

	virtual bool GrabFrame(){ return m_videoHandler.GrabFrame(); }
	virtual bool HasNewFrame(){ return m_videoHandler.isFrameNew(); }
	virtual ulong GetBufferID(){ return m_videoHandler.GetFrameID(); }

	virtual float GetCaptureFrameRate(){ return m_videoHandler.GetCaptureFrameRate(); }

	virtual const ImageInfo* GetLastFrame(){ return m_videoHandler.getPixelsRef(); }


	virtual void OnPipelineReady(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerReady(m_owner); }
	virtual void OnPipelinePlaying(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerStarted(m_owner); }
	virtual void OnPipelineStopped(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerStopped(m_owner); }

};


GstNetworkVideoPlayer::GstNetworkVideoPlayer()
{
	m_impl = new GstNetworkVideoPlayerImpl(this);
}

GstNetworkVideoPlayer::~GstNetworkVideoPlayer()
{
	delete m_impl;
}
void GstNetworkVideoPlayer::SetIPAddress(const std::string& ip, uint videoPort, uint clockPort, bool rtcp)
{
	m_impl->SetIPAddress(ip, videoPort, clockPort,rtcp);
}
bool GstNetworkVideoPlayer::CreateStream()
{
	return m_impl->CreateStream();
}
GstPipelineHandler*GstNetworkVideoPlayer::GetPipeline()
{
	return m_impl;
}


void GstNetworkVideoPlayer::SetVolume(float vol)
{
	m_impl->SetVolume(vol);
}
bool GstNetworkVideoPlayer::IsStream()
{
	return m_impl->IsStream();
}

void GstNetworkVideoPlayer::Play()
{
	m_impl->Play();
}
void GstNetworkVideoPlayer::Stop()
{
	m_impl->Stop();
}
void GstNetworkVideoPlayer::Pause()
{
	m_impl->Pause();
}
bool GstNetworkVideoPlayer::IsLoaded()
{
	return m_impl->IsLoaded();
}
bool GstNetworkVideoPlayer::IsPlaying()
{
	return m_impl->IsPlaying();
}
void GstNetworkVideoPlayer::Close()
{
	m_impl->Close();

}

const Vector2d& GstNetworkVideoPlayer::GetFrameSize()
{
	return m_impl->GetFrameSize();
}

video::EPixelFormat GstNetworkVideoPlayer::GetImageFormat()
{
	return m_impl->GetImageFormat();
}

bool GstNetworkVideoPlayer::GrabFrame(int i)
{
	return m_impl->GrabFrame();
}

bool GstNetworkVideoPlayer::HasNewFrame()
{
	return m_impl->HasNewFrame();
}

ulong GstNetworkVideoPlayer::GetBufferID()
{
	return m_impl->GetBufferID();
}

float GstNetworkVideoPlayer::GetCaptureFrameRate()
{
	return m_impl->GetCaptureFrameRate();
}

const ImageInfo* GstNetworkVideoPlayer::GetLastFrame(int i)
{
	return m_impl->GetLastFrame();
}

const GstImageFrame* GstNetworkVideoPlayer::GetLastDataFrame(int i)
{
	return m_impl->GetLastDataFrame();
}

void* GstNetworkVideoPlayer::GetLastFrameRTPMeta(int i)
{
	return m_impl->GetLastFrameRTPMeta();
}
unsigned long GstNetworkVideoPlayer::GetLastFrameTimestamp(int i )
{
	return m_impl->GetLastFrameTimestamp();
}
int GstNetworkVideoPlayer::GetPort(int i)
{
	return m_impl->GetPort(i);
}


}
}


