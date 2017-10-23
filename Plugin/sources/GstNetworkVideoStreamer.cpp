

#include "stdafx.h"
#include "GstNetworkVideoStreamer.h"

#include "GstPipelineHandler.h"

#include "IVideoGrabber.h"
#include "CMySrc.h"
#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"

#include "IThreadManager.h"
#include <map>

#include <gst/app/gstappsrc.h>
namespace mray
{
namespace video
{

class GstNetworkVideoStreamerImpl :public GstPipelineHandler,public IPipelineListener
{
protected:

	GstNetworkVideoStreamer* m_owner;

	std::string m_ipAddr;
	uint m_videoPort;
	uint m_clockPort;

	IVideoGrabber* m_grabber;
	int m_bitRate;
	int m_quality;
	bool m_rtcp;

	std::string m_pipeLineString;
	GstElement* m_videoSink;
	GstMyUDPSink* m_videoRtcpSink;
	GstMyUDPSrc* m_videoRtcpSrc;

	int m_fps;
	Vector2d m_frameSize;

	video::ImageInfo m_tmp;


	std::map<std::string, std::string> m_encoderParams;

	struct VideoSrcData
	{
		GstAppSrcCallbacks srcCB;
		GstAppSrc* videoSrc;
		//GstMySrc * videoSrc;
		int index;
		GstNetworkVideoStreamerImpl* o;
		int sourceID;
	};
	VideoSrcData m_videoSrc;


public:
	GstNetworkVideoStreamerImpl(GstNetworkVideoStreamer* owner)
	{
		m_owner = owner;
		m_ipAddr = "127.0.0.1";
		m_videoPort = 5000;
		m_clockPort = 5010;

		m_bitRate = 5000;
		m_grabber = 0;
		m_quality = -1;

		m_videoSink = 0;
		m_videoRtcpSink = 0;
		m_videoRtcpSrc = 0;

		m_videoSrc.sourceID = 0;

		m_frameSize = Vector2d(640,480);
		m_fps = 30;


		m_encoderParams["speed-preset"] = "superfast";
		m_encoderParams["tune"] = "zerolatency";
		m_encoderParams["pass"] = "cbr";
		m_encoderParams["sliced-threads"] = "true";
		m_encoderParams["sync-lookahead"] = "0";
		m_encoderParams["rc-lookahead"] = "0";
		m_encoderParams["quantizer"] = "15";

		AddListener(this);

	}

	virtual ~GstNetworkVideoStreamerImpl()
	{
// 		if (m_videoSink && m_videoSink->m_client)
// 		{
// 			m_videoSink->m_client->Close();
// 		}
		Stop();
	}
	void  SetResolution(int width, int height, int fps)
	{
		m_frameSize = Vector2d(width, height);
		m_fps = fps;
	}

	std::string GetFormatStr(EPixelFormat fmt)
	{

		std::string format = "RGB";
		switch (fmt)
		{
		case EPixel_R8G8B8:
			format = "RGB";
			break;
		case EPixel_R8G8B8A8:
			format = "RGBA";
			break;
		case EPixel_X8R8G8B8:
			format = "xRGB";
			break;

		case EPixel_B8G8R8:
			format = "BGR";
			break;
		case EPixel_B8G8R8A8:
			format = "BGRA";
			break;
		case EPixel_X8B8G8R8:
			format = "xBGR";
			break;
		case EPixel_I420:
			format = "Y41B";
			break;

		case EPixel_LUMINANCE8:
		case EPixel_Alpha8:
			format = "GRAY8";
			break;
		}
		return format;
	}

	std::string BuildGStr()
	{

		std::stringstream ss;
		if (m_grabber)
		{
				std::string format = GetFormatStr(m_grabber->GetImageFormat());
				//ksvideosrc
				ss<< "appsrc"
					 " name=src"  
					<< " do-timestamp=0 is-live=true format=time"
					" ! video/x-raw,format=" + format + ",width=" << m_grabber->GetFrameSize().x <<
					",height=" << m_grabber->GetFrameSize().y << ",framerate=" << m_fps << "/1 "
				<<  "!  videoconvert  ! video/x-raw,format=I420 ";// !videoflip method = 1  ";
				//videoStr += "! videorate ";//" max-rate=" + std::stringConverter::toString(m_fps) + " ";
				//	videoStr += " ! queue ";
				//	if (m_grabber->GetImageFormat()!=video::EPixel_YUYV)
		}
		else{
			ss<< "mysrc name=src" <<
				" ! video/x-raw,format=RGB  ";// !videoflip method = 1  ";
		}
		//add time stamp

		return ss.str();
	}

	bool BuildString()
	{
		if (!m_grabber)
			return false;
		std::string videoStr;
		std::stringstream ss;

		int height = m_frameSize.y;
		int width = m_frameSize.x;
		if (m_grabber->GetFrameSize().x < width)
			width = m_grabber->GetFrameSize().x;

		if (m_grabber->GetFrameSize().y < height)
			height = m_grabber->GetFrameSize().y;

		ss << BuildGStr();

		if ( (width < m_grabber->GetFrameSize().x || height < m_grabber->GetFrameSize().y))
		{
			ss << "! videoscale ! video/x-raw,width=" << width <<
				",height=" << height << ",framerate=" << m_fps << "/1";
		}
		//videoStr = "mysrc name=src0 ! video/x-raw,format=RGB,width=640,height=480,framerate="+std::stringConverter::toString(m_fps)+"/1 ! videoconvert ";
		//encoder string


		ss   << "! x264enc name=videoEnc bitrate=" << m_bitRate;
		if (m_quality > 0)
		{
			std::stringstream tmp;
			tmp << m_quality;
			m_encoderParams["pass"] = "qual";
			m_encoderParams["quantizer"] =tmp.str();
		}
		else {
			m_encoderParams["pass"] = "cbr";
		}

		std::string encoderParams = " ";
		std::map<std::string, std::string>::iterator it = m_encoderParams.begin();
		for (; it != m_encoderParams.end(); ++it)
			encoderParams += it->first + "=" + it->second + " ";

		ss << encoderParams<<" ! rtph264pay ";
		if (m_rtcp)
		{
			m_pipeLineString = "rtpbin  name=rtpbin " +
				videoStr +
				"! rtpbin.send_rtp_sink_0 "

				"rtpbin.send_rtp_src_0 ! "
				"myudpsink name=videoSink  "

				"rtpbin.send_rtcp_src_0 ! "
				"myudpsink name=videoRtcpSink sync=false  "
				"udpsrc name=videoRtcpSrc ! rtpbin.recv_rtcp_sink_0 ";
		}
		else
		{

			m_pipeLineString = ss.str() + " ! "
				"udpsink name=videoSink ";
			//"udpsink host=127.0.0.1 port=7000";
		}
		return true;

	}

	void SetEncoderSettings(const std::string& param, const std::string& value)
	{
		m_encoderParams[param] = value;
	}
	void SetBitRate(int bitRate, int quality)
	{
		m_bitRate = bitRate;
		m_quality = quality;
	}
	void SetVideoGrabber(IVideoGrabber* grabber0)
	{
		m_grabber = grabber0;
	}


	GstFlowReturn NeedBuffer(GstMySrc * sink, GstBuffer ** buffer,int index)
	{
		if (!GetPipeline() || !IsStreaming())
			return GST_FLOW_ERROR;
		if (!m_grabber)
		{
			LogMessage(std::string("No video grabber is assigned to CustomVideoStreamer"), ELL_WARNING);
			return GST_FLOW_ERROR;
		}
// 		do 
// 		{
// 			OS::IThreadManager::getInstance().sleep (1);
// 		} while (!m_grabber->GrabFrame());
 		if (!m_grabber->GrabFrame())
  		{
  			return GST_FLOW_ERROR;
  		}
		m_grabber->Lock();

		const video::ImageInfo* ifo = m_grabber->GetLastFrame();
		int len = ifo->imageDataSize;
		GstMapInfo map;
		GstBuffer* outbuf = gst_buffer_new_and_alloc(len);
		gst_buffer_map(outbuf, &map, GST_MAP_WRITE);
		memcpy(map.data, ifo->imageData, len);


		gst_buffer_unmap(outbuf, &map);
		m_grabber->Unlock();
		*buffer = outbuf;
		return GST_FLOW_OK;
	}

	static GstFlowReturn need_buffer(GstMySrc * sink, gpointer data, GstBuffer ** buffer)
	{
		VideoSrcData* d = static_cast<VideoSrcData*>(data);
		if (d)
		{
			return d->o->NeedBuffer(sink, buffer,d->index);
		}
		return GST_FLOW_ERROR;
	}


	static gboolean read_data(VideoSrcData *d)
	{
		GstFlowReturn ret;

		GstBuffer *buffer;
		if (d->o->NeedBuffer(0, &buffer, d->index) == GST_FLOW_OK)
		{
			ret = gst_app_src_push_buffer(d->videoSrc, buffer);
			if (ret != GST_FLOW_OK){
				ret = gst_app_src_end_of_stream(d->videoSrc);
				return FALSE;
			}
		}
		return TRUE;

	}
	static void start_feed(GstAppSrc *source, guint size, gpointer data)
	{
		VideoSrcData* o = static_cast<VideoSrcData*>(data);
		if (o->sourceID == 0) {
			GST_DEBUG("start feeding");
			o->sourceID = g_idle_add((GSourceFunc)read_data, o);
		}
	}


	static void stop_feed(GstAppSrc *source, gpointer data)
	{
		VideoSrcData* o = static_cast<VideoSrcData*>(data);
		if (o->sourceID != 0) {
			GST_DEBUG("stop feeding");
			g_source_remove(o->sourceID);
			o->sourceID = 0;
		}
	}
	static gboolean seek_data(GstAppSrc *src, guint64 offset, gpointer user_data)
	{
		return TRUE;
	}
	void _UpdatePorts()
	{

		if (!GetPipeline())
			return;
#define SET_SRC(name,p) m_##name=GST_MyUDPSrc(gst_bin_get_by_name(GST_BIN(GetPipeline()), #name)); if(m_##name){m_##name->SetPort(p);}
#define SET_SINK(name,p) m_##name=GST_MyUDPSink(gst_bin_get_by_name(GST_BIN(GetPipeline()), #name)); if(m_##name){m_##name->SetPort(m_ipAddr,p);}

		{
			std::stringstream ss;
			ss << "src" ;
			std::string name = ss.str();
#if 1
			m_videoSrc.videoSrc = GST_APP_SRC(gst_bin_get_by_name(GST_BIN(GetPipeline()), name.c_str()));
			m_videoSrc.o = this;
			m_videoSrc.index = 0;
			if (m_videoSrc.videoSrc){

				//gst_base_src_set_blocksize(GST_BASE_SRC(m_videoSrc.videoSrc), 640 * 480 * 3);
				//gst_base_src_set_live(GST_BASE_SRC(m_videoSrc.videoSrc), true);
				//gst_base_src_set_async(GST_BASE_SRC(m_videoSrc.videoSrc), false);
				//gst_base_src_set_do_timestamp(GST_BASE_SRC(m_videoSrc.videoSrc), true);

				//gst_app_src_set_max_bytes(m_videoSrc.videoSrc, 640 * 480 * 3);
				//gst_app_src_set_emit_signals(m_videoSrc.videoSrc, false);
				//gst_base_src_set_do_timestamp(GST_BASE_SRC(m_videoSrc.videoSrc), true);

// 				g_object_set(G_OBJECT(m_videoSrc.videoSrc),
// 					"stream-type", GST_APP_STREAM_TYPE_STREAM, // GST_APP_STREAM_TYPE_STREAM
// 					"format", GST_FORMAT_TIME,
// 					"is-live", TRUE,
// 					NULL);

// 				g_signal_connect(m_videoSrc.videoSrc, "need-data", G_CALLBACK(start_feed), &m_videoSrc.srcCB);
// 				g_signal_connect(m_videoSrc.videoSrc, "enough-data", G_CALLBACK(stop_feed), &m_videoSrc.srcCB);
 				m_videoSrc.srcCB.need_data = &start_feed;
 				m_videoSrc.srcCB.enough_data = &stop_feed;
 				m_videoSrc.srcCB.seek_data = &seek_data;

				gst_app_src_set_callbacks(m_videoSrc.videoSrc, &m_videoSrc.srcCB, &m_videoSrc, NULL);
			}
#else

			m_videoSrc.videoSrc = GST_MySRC(gst_bin_get_by_name(GST_BIN(m_gstPipeline), name.c_str()));
			m_videoSrc.o = this;
			m_videoSrc.index = i;
			if (m_videoSrc.videoSrc){
				m_videoSrc.videoSrc->need_buffer = need_buffer;
				m_videoSrc.videoSrc->data = &m_videoSrc;
			}
#endif
		}
		g_object_set(m_videoSink, "port", m_videoPort,"host",m_ipAddr.c_str(),0);

		//SET_SINK(videoSink, m_videoPort);
		if (m_rtcp){
			SET_SRC(videoRtcpSrc, (m_videoPort + 1));
			SET_SINK(videoRtcpSink, (m_videoPort + 2));
		}

	}

	void BindPorts(const std::string& addr, int videoPort, uint clockPort, bool rtcp)
	{
		if (m_ipAddr == addr && m_videoPort == videoPort && m_rtcp == rtcp)
			return;
		m_ipAddr = addr;
		m_videoPort = videoPort;
		m_rtcp = rtcp;
		m_clockPort = clockPort;

		_UpdatePorts();
	}
	bool CreateStream()
	{
		GError *err = 0;
		if (!BuildString())
			return false;
		GstElement* pipeline = gst_parse_launch(m_pipeLineString.c_str(), &err);
		if (err)
		{
			LogMessage(std::string("GstCustomVideoPlayer: Pipeline error:") + err->message, ELL_ERROR);
		}
		if (!pipeline)
			return false;

		m_videoSink = gst_bin_get_by_name(GST_BIN(pipeline), "videoSink");
		SetPipeline(pipeline);
		_UpdatePorts();

		return CreatePipeline(true,"",m_clockPort);

	}
	void Stream()
	{
		SetPaused(false);
	}
	bool IsStreaming()
	{
		return IsPlaying();
	}
	virtual void Close()
	{
		gst_app_src_end_of_stream(m_videoSrc.videoSrc);
	//	gst_element_send_event(GST_ELEMENT(m_videoSrc.videoSrc), gst_event_new_eos());
		GstPipelineHandler::Close();
	}


	virtual void OnPipelineReady(GstPipelineHandler* p){ m_owner->__FIRE_OnStreamerReady(m_owner); }
	virtual void OnPipelinePlaying(GstPipelineHandler* p){ m_owner->__FIRE_OnStreamerStarted(m_owner); }
	virtual void OnPipelineStopped(GstPipelineHandler* p){ m_owner->__FIRE_OnStreamerStopped(m_owner); }
};


GstNetworkVideoStreamer::GstNetworkVideoStreamer()
{
	m_impl = new GstNetworkVideoStreamerImpl(this);
}

GstNetworkVideoStreamer::~GstNetworkVideoStreamer()
{
	delete m_impl;
}
void GstNetworkVideoStreamer::Stream()
{
	m_impl->Stream();
}
void GstNetworkVideoStreamer::Stop()
{
	m_impl->Stop();
}


void GstNetworkVideoStreamer::BindPorts(const std::string& addr, uint videoPort, uint clockPort, bool rtcp)
{
	m_impl->BindPorts(addr, videoPort, clockPort, rtcp);
}

bool GstNetworkVideoStreamer::CreateStream()
{
	return m_impl->CreateStream();
}

void GstNetworkVideoStreamer::Close()
{
	m_impl->Close();
}
bool GstNetworkVideoStreamer::IsStreaming()
{
	return m_impl->IsStreaming();
}

void GstNetworkVideoStreamer::SetBitRate(int bitRate, int quality)
{
	m_impl->SetBitRate(bitRate,quality);
}

void GstNetworkVideoStreamer::SetResolution(int width, int height, int fps)
{
	m_impl->SetResolution(width, height, fps);
}

void GstNetworkVideoStreamer::SetVideoGrabber(IVideoGrabber* grabber0)
{
	m_impl->SetVideoGrabber(grabber0);
}
void GstNetworkVideoStreamer::SetPaused(bool paused)
{
	m_impl->SetPaused(paused);
}

bool GstNetworkVideoStreamer::IsPaused()
{
	return !m_impl->IsPlaying();
}
GstPipelineHandler* GstNetworkVideoStreamer::GetPipeline()
{
	return m_impl;
}

}
}

