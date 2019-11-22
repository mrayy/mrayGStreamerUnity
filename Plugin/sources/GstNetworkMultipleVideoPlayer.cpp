

#include "stdafx.h"
#include "GstNetworkMultipleVideoPlayer.h"

//#include "CMyUDPSrc.h"
//#include "CMyUDPSink.h"

//#include "CMySrc.h"
//#include "CMySink.h"

#include "VideoAppSinkHandler.h"
#include "GstPipelineHandler.h"
#include "UnityHelpers.h"
#include "CMyListener.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <vector>
#include <algorithm>


//#include <windows.h>


namespace mray
{
namespace video
{


class GstNetworkMultipleVideoPlayerImpl :public GstPipelineHandler, IPipelineListener
{
	GstNetworkMultipleVideoPlayer* m_owner;
	std::string m_ipAddr;
	uint m_clockPort;
	uint m_baseVideoSrc;
	int m_playersCount;

	std::string m_pipeLineString;

	std::string m_intermidateElems;
	std::string m_encoderType;

	video::EPixelFormat m_targetFormat;

	bool m_rtcp;

	std::string recordFileName;

	int limitRecordFramerate;

	struct VideoHandler
	{
		VideoHandler()
		{
			videoPort = 5000;
			videoSrc = 0;
			videoSink = 0;
			videoRtcpSrc = 0;
			videoRtcpSink = 0;
			rtpListener = 0;
			postdepayListener = 0;
			preappsrcListener = 0;
			Mp4Muxer = 0;
		}
		VideoAppSinkHandler handler;
		uint videoPort;

		GstElement* videoSrc;
		GstAppSink* videoSink;

		GstMyListener* rtpListener;
		GstMyListener* postdepayListener;
		GstMyListener* preappsrcListener;

		GstElement* videoRtcpSrc;
		GstElement* videoRtcpSink;
		GstElement* Mp4Muxer;
	};

	std::vector<VideoHandler*> m_videoHandler;

public:
	GstNetworkMultipleVideoPlayerImpl(GstNetworkMultipleVideoPlayer* o)
	{
		m_owner = o;
		m_ipAddr = "127.0.0.1";
		m_clockPort = 5010;
		m_playersCount = 0;
		recordFileName = "";
		m_encoderType = "H264";
		limitRecordFramerate = -1;

		AddListener(this);
		m_targetFormat = EPixel_B8G8R8;
	}
	virtual ~GstNetworkMultipleVideoPlayerImpl()
	{

	}


	void AddIntermidateElement(const std::string& elems)
	{
		m_intermidateElems = elems;
	}
	void SetDecoderType(std::string type)
	{
		m_encoderType = type;
	}

	void SetRecordToFile(std::string filename, int framerate)
	{
		limitRecordFramerate = framerate;
		recordFileName = filename;
		std::replace(recordFileName.begin(), recordFileName.end(), '\\', '/');
	}

	bool GetRecordStarted()
	{
		if (recordFileName == "")
			return false;

		for (int i = 0; i < m_videoHandler.size(); ++i)
			if (m_videoHandler[i]->handler.GetSamplesCount() > 0)
				return true;
		return false;
	}

	std::string _BuildPipeline(int i)
	{
		std::stringstream ss;
		std::string videoStr;
		ss << "udpsrc name=videoSrc" << i << " port=" << m_videoHandler[i]->videoPort << " !"
			//"udpsrc port=7000 buffer-size=2097152 do-timestamp=true !"
			"application/x-rtp";
		if (m_encoderType == "VP9")
		{
			ss << ",encoding-name=VP9";
		}
		videoStr = ss.str();

		std::string format;
		std::string extraElem="";

		switch (m_targetFormat)
		{
		case mray::video::EPixel_LUMINANCE8:
		case EPixel_Alpha8:
			format = "GRAY8";
		//	extraElem = " ! videoconvert ";
			break;
		case mray::video::EPixel_LUMINANCE16:
			format = "YUY2";
			//extraElem = " ! videoconvert ";
			break;
		case mray::video::EPixel_R8G8B8:
			format = "RGB";
			extraElem = " ! videoconvert ";
			break;
		case mray::video::EPixel_R8G8B8A8:
			format = "RGBA";
			extraElem = " ! videoconvert ";
			break;
		case mray::video::EPixel_B8G8R8:
			format = "BGR";
			extraElem = " ! videoconvert ";
			break;
		case mray::video::EPixel_B8G8R8A8:
			format = "BGRA";
			extraElem = " ! videoconvert ";
			break;
		case mray::video::EPixel_I420:
			format = "I420";
			break;
		case mray::video::EPixel_Y42B:
			format = "Y42B";
			break;
		default:
			format = "RGB";
			extraElem = " ! videoconvert ";
			break;
		}

		ss.str("");
		ss << videoStr;

		if (m_rtcp)
		{
			ss << "! rtpbin.recv_rtp_sink_" << i <<
				" rtpbin. ";
		}
		else{
			ss << " ! rtpjitterbuffer latency=500  ";
		}


		if (m_encoderType == "H264")
		{ 
			//propably rtph264depay drops some rtp packets along the way??
			ss << " ! mylistener name=rtplistener" << i << "  ! rtph264depay ! mylistener name=postdepay" << i;
			
			if (recordFileName=="")
				ss << " !  avdec_h264 output-corrupt=false ! "; //! h264parse
			else
			{
				if (limitRecordFramerate > 0)
				{
					ss << " ! h264parse ! avdec_h264 output-corrupt=false ! tee name=t ";
					ss << " t. ! queue ! videoscale ! videorate ! video/x-raw,framerate="<<limitRecordFramerate<<"/1 ! videoconvert ! x264enc pass=qual ! mp4mux name=muxer" << i << " ! filesink name=fs" << i << " location=\"" << recordFileName << "\" sync=false ";
					ss << " t. ! queue ! ";
				}
				else {
					ss << " ! tee name=t t. ! queue ! h264parse ! mp4mux name=muxer" << i << " ! filesink name=fs" << i << " location=\"" << recordFileName << "\" sync=false ";
					ss << "  t. ! queue ! avdec_h264 output-corrupt=false ! ";
				}
			}
				;//" videoconvert !"
		//	ss << " videoflip method=5 !";
		}
		else if (m_encoderType == "JPEG")
		{
			ss << ",media=video,encoding-name=JPEG,payload=96 ! queue ! rtpjpegdepay ! jpegdec ! videoconvert !";
		}
		else if (m_encoderType == "VP8")
		{
			ss << "  ! rtpvp8depay ! vp8dec ! videoconvert !";
		}
		else if (m_encoderType == "VP9")
		{
			ss << "  ! mylistener name=rtplistener" << i << "  ! rtpvp9depay ! mylistener name=postdepay" << i << " ! vp9dec  threads=3  !";
		}
		else
			return "";
		//"rtpvp8depay ! vp8dec ! "
		//"rtptheoradepay ! theoradec ! "

			// " videorate  ! "//"video/x-raw,framerate=60/1 ! "
			//	"videoconvert ! video/x-raw,format=RGB  !" // Very slow!!

		if (m_intermidateElems != "")
		{
			ss << m_intermidateElems << " ! ";
		}
		//"videorate ! "
		ss<< " videoconvert ! video/x-raw,format=" + format + extraElem + " !";
		//"videoconvert ! video/x-raw,format=RGB  !"
		//	" timeoverlay halignment=right text=\"Local Time =\"! "

		if (m_rtcp)
		{
			ss << " appsink name=videoSink" << i <<
				//video rtcp
				"udpsrc name=videoRtcpSrc" << i << " ! rtpbin.recv_rtcp_sink_" << i << " "
				"rtpbin.send_rtcp_src_" << i << " ! udpsink name=videoRtcpSink" << i << " sync=false async=false ";
		}
		else
		{
			ss << " mylistener name=preappsrc" << i << " !  appsink name=videoSink" << i << " sync=false  emit-signals=true "; //fpsdisplaysink ";// "
		}
		//"fpsdisplaysink sync=false";

		videoStr = ss.str();
		return videoStr;
	}


	void _BuildPipeline()
	{
		m_pipeLineString = "";
		if (m_rtcp)
		{
			m_pipeLineString = "rtpbin name=rtpbin ";
		}
		for (int i = 0; i < m_playersCount; ++i)
		{
				m_pipeLineString += _BuildPipeline(i) + " ";
		}
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
			ss << "videoSrc" << i;
			m_videoHandler[i]->videoSrc = gst_bin_get_by_name(GST_BIN(GetPipeline()), ss.str().c_str());
			if (m_videoHandler[i]->videoPort!=0)
				g_object_set(m_videoHandler[i]->videoSrc, "port", m_videoHandler[i]->videoPort,0);
		//	SET_SRC(i, ss.str().c_str(), m_videoHandler[i]->videoPort);
			// 			SET_SINK(i, videoRtcpSink, (m_videoHandler[i]->videoPort + 1));
			// 			SET_SRC(i, videoRtcpSrc, (m_videoHandler[i]->videoPort + 2));
		}
	}

	void SetIPAddress(const std::string& ip, uint videoPort, int count, uint clockPort, bool rtcp)
	{
		m_playersCount = count;
		m_ipAddr = ip;
		m_baseVideoSrc = videoPort;
		m_clockPort = clockPort;
		m_rtcp = rtcp;

		m_videoHandler.resize(m_playersCount);
		for (int i = 0; i < m_playersCount; ++i)
		{
			if(m_videoHandler[i]==0)
				m_videoHandler[i] = new VideoHandler();
			if (m_baseVideoSrc != 0)
				m_videoHandler[i]->videoPort = m_baseVideoSrc + i;
			else
				m_videoHandler[i]->videoPort = 0;
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
			LogMessage("GstNetworkMultipleVideoPlayer: Pipeline error: " + std::string(err->message), ELL_WARNING);
		}
		if (!p)
			return false;
		SetPipeline(p);
		printf("Connecting Video stream with IP:%s\n", m_ipAddr.c_str());

		_UpdatePorts();

		for (int i = 0; i < m_playersCount; ++i)
		{
			{
				std::stringstream ss;
				ss << "videoSink" << i;
				m_videoHandler[i]->videoSink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(p), ss.str().c_str()));
			}
			{

				std::stringstream ss;
				ss << "rtplistener" << i;
				m_videoHandler[i]->rtpListener = GST_MyListener(gst_bin_get_by_name(GST_BIN(p), ss.str().c_str()));
		}
			{

				std::stringstream ss;
				ss << "postdepay" << i;
				m_videoHandler[i]->postdepayListener = GST_MyListener(gst_bin_get_by_name(GST_BIN(p), ss.str().c_str()));
			}
			{

				std::stringstream ss;
				ss << "preappsrc" << i;
				m_videoHandler[i]->preappsrcListener = GST_MyListener(gst_bin_get_by_name(GST_BIN(p), ss.str().c_str()));
			}
			{

				std::stringstream ss;
				ss << "muxer" << i;
				m_videoHandler[i]->Mp4Muxer = GST_ELEMENT(gst_bin_get_by_name(GST_BIN(p), ss.str().c_str()));
			}

			if (m_rtcp)
			{
				std::stringstream rtcpSink, rtcpSrc;
				rtcpSink << "videoRtcpSink" << i;
				rtcpSrc << "videoRtcpSrc" << i;
				m_videoHandler[i]->videoRtcpSink = gst_bin_get_by_name(GST_BIN(p), rtcpSink.str().c_str());
				m_videoHandler[i]->videoRtcpSrc = gst_bin_get_by_name(GST_BIN(p), rtcpSrc.str().c_str());
			}

			m_videoHandler[i]->handler.SetSink(m_videoHandler[i]->videoSink);
			m_videoHandler[i]->handler.SetRTPListener(m_videoHandler[i]->rtpListener, m_videoHandler[i]->postdepayListener, m_videoHandler[i]->preappsrcListener);
			//m_videoHandler[i]->handler.SetSource(GST_MyUDPSrc(m_videoHandler[i]->videoSrc));
			g_signal_connect(m_videoHandler[i]->videoSink, "new-sample", G_CALLBACK(new_buffer), this);
			//attach videosink callbacks
			GstAppSinkCallbacks gstCallbacks;
			gstCallbacks.eos = &VideoAppSinkHandler::on_eos_from_source;
			gstCallbacks.new_preroll = &VideoAppSinkHandler::on_new_preroll_from_source;
#if GST_VERSION_MAJOR==0
			gstCallbacks.new_buffer = &VideoAppSinkHandler::on_new_buffer_from_source;
#else
			gstCallbacks.new_sample = &VideoAppSinkHandler::on_new_buffer_from_source;
#endif
			gst_app_sink_set_callbacks(GST_APP_SINK(m_videoHandler[i]->videoSink), &gstCallbacks, &m_videoHandler[i]->handler, NULL);
			//gst_app_sink_set_emit_signals(GST_APP_SINK(m_videoSink), true);


			// 		gst_base_sink_set_async_enabled(GST_BASE_SINK(m_videoSink), TRUE);
			gst_base_sink_set_sync(GST_BASE_SINK(m_videoHandler[i]->videoSink), false);
			gst_app_sink_set_drop(GST_APP_SINK(m_videoHandler[i]->videoSink), TRUE);
			gst_app_sink_set_max_buffers(GST_APP_SINK(m_videoHandler[i]->videoSink), 2);
			gst_base_sink_set_max_lateness(GST_BASE_SINK(m_videoHandler[i]->videoSink), 0);
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

		return CreatePipeline(false, m_ipAddr, m_clockPort);

	}
	uint GetVideoPort(int i)
	{
		if (i >= m_videoHandler.size())
			return 0;
		if (m_videoHandler[i]->videoSrc){
			//return m_videoHandler[i]->videoSrc->port;
			gint port=0;
			g_object_get(m_videoHandler[i]->videoSrc, "port", &port, 0);
			return port;
		}
		else return m_videoHandler[i]->videoPort;
	}
	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		GstFlowReturn ret;
		
		for (int i = 0; i < m_playersCount; ++i)
		{
// 			if (m_videoHandler[i]->videoSrc != 0)
// 				gst_element_send_event(m_videoHandler[i]->videoSrc, gst_event_new_eos());
 			if(m_videoHandler[i]->Mp4Muxer !=0)
				gst_element_send_event(m_videoHandler[i]->Mp4Muxer, gst_event_new_eos());
			//if (m_videoHandler[i]->videoSrc && m_videoHandler[i]->videoSrc->m_client)
			//	m_videoHandler[i]->videoSrc->m_client->Close();
		}
		GstPipelineHandler::Close();
		//Sleep(50);

		for (int i = 0; i < m_playersCount; ++i)
		{
			m_videoHandler[i]->handler.Close();
		}
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
		return m_videoHandler[i]->handler.GetFrameSize();
	}
	void SetImageFormat(video::EPixelFormat fmt)
	{
		m_targetFormat = fmt;
	}
	virtual video::EPixelFormat GetImageFormat(int i)
	{
		return m_videoHandler[i]->handler.getPixelsRef()->format;
	}
	int GetFramesCount()
	{
		return m_playersCount;
	}
	virtual bool GrabFrame(int i)
	{
		if (i >= m_playersCount)
			return false;
		return m_videoHandler[i]->handler.GrabFrame();
	}
	virtual bool HasNewFrame(int i)
	{
		if (i >= m_playersCount)
			return false;
		return m_videoHandler[i]->handler.isFrameNew();
	}
	virtual ulong GetBufferID(int i)
	{
		if (i >= m_playersCount)
			return 0;
		return m_videoHandler[i]->handler.GetFrameID();
	}

	virtual float GetCaptureFrameRate(int i){
		if (i>m_videoHandler.size())
			return 0;
		return m_videoHandler[i]->handler.GetCaptureFrameRate();
	}

	virtual const ImageInfo* GetLastFrame(int i)
	{
		if (i > m_playersCount)
			return 0;
		else
			return m_videoHandler[i]->handler.getPixelsRef();
	}
	virtual unsigned long GetLastFrameTimestamp(int i)
	{
		if (i > m_playersCount)
			return 0;
		else
			return m_videoHandler[i]->handler.getPixelFrame()->RTPPacket.timestamp;
	}
	const GstImageFrame* GetLastDataFrame(int i)
	{
		if (i > m_playersCount)
			return 0;
		else
			return m_videoHandler[i]->handler.getPixelFrame();
	}
	void* GetLastFrameRTPMeta(int i)
	{
		if (i > m_playersCount)
			return 0;
		else
			return &m_videoHandler[i]->handler.getPixelFrame()->RTPPacket;
	}

	virtual int GetPort(int i)
	{
		return GetVideoPort(i);
	}


	virtual void OnPipelineReady(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerReady(m_owner); }
	virtual void OnPipelinePlaying(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerStarted(m_owner); }
	virtual void OnPipelineStopped(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerStopped(m_owner); }

	ulong GetNetworkUsage()
	{
		ulong usage = 0;
		for (int i = 0; i < m_playersCount; ++i)
		{
//			usage+=m_videoHandler[i]->videoSrc->m_client->GetReceiverNetworkUsage();
		}
		return usage;
	}
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
	m_impl->SetIPAddress(ip, videoPort, count, clockPort, rtcp);
}
bool GstNetworkMultipleVideoPlayer::CreateStream()
{
	return m_impl->CreateStream();
}

void GstNetworkMultipleVideoPlayer::SetRecordToFile(std::string filename, int framerate)
{
	m_impl->SetRecordToFile(filename,framerate);
}
bool GstNetworkMultipleVideoPlayer::GetRecordStarted()
{
	return m_impl->GetRecordStarted();
}
void GstNetworkMultipleVideoPlayer::AddIntermidateElement(const std::string& elems)
{
	m_impl->AddIntermidateElement(elems);
}

GstPipelineHandler*GstNetworkMultipleVideoPlayer::GetPipeline()
{
	return m_impl;
}
uint GstNetworkMultipleVideoPlayer::GetVideoPort(int i)
{
	return m_impl->GetVideoPort(i);
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
void GstNetworkMultipleVideoPlayer::SetDecoderType(std::string type)
{
	m_impl->SetDecoderType(type);
}

const Vector2d& GstNetworkMultipleVideoPlayer::GetFrameSize()
{
	return m_impl->GetFrameSize(0);
}

void GstNetworkMultipleVideoPlayer::SetImageFormat(video::EPixelFormat fmt)
{
	m_impl->SetImageFormat(fmt);
}
video::EPixelFormat GstNetworkMultipleVideoPlayer::GetImageFormat()
{
	return m_impl->GetImageFormat(0);
}

int GstNetworkMultipleVideoPlayer::GetFramesCount()
{
	return m_impl->GetFramesCount();
}

bool GstNetworkMultipleVideoPlayer::GrabFrame(int i)
{
	return m_impl->GrabFrame(i);
}

bool GstNetworkMultipleVideoPlayer::HasNewFrame(int i)
{
	return m_impl->HasNewFrame(i);
}

ulong GstNetworkMultipleVideoPlayer::GetBufferID(int i)
{
	return m_impl->GetBufferID(i);
}

float GstNetworkMultipleVideoPlayer::GetCaptureFrameRate(int i)
{
	return m_impl->GetCaptureFrameRate(i);
}

const ImageInfo* GstNetworkMultipleVideoPlayer::GetLastFrame(int i)
{
	return m_impl->GetLastFrame(i);
}


const GstImageFrame* GstNetworkMultipleVideoPlayer::GetLastDataFrame(int i)
{
	return m_impl->GetLastDataFrame(i);
}
unsigned long GstNetworkMultipleVideoPlayer::GetLastFrameTimestamp(int i)
{
	return m_impl->GetLastFrameTimestamp(i);
}
void* GstNetworkMultipleVideoPlayer::GetLastFrameRTPMeta(int i)
{
	return m_impl->GetLastFrameRTPMeta(i);

}
int GstNetworkMultipleVideoPlayer::GetPort(int i)
{
	return m_impl->GetPort(i);
}

ulong GstNetworkMultipleVideoPlayer::GetNetworkUsage()
{
	return m_impl->GetNetworkUsage();

}

}
}


