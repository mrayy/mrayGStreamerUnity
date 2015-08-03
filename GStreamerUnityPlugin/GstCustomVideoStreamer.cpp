

#include "stdafx.h"
#include "GstCustomVideoStreamer.h"

#include "GstPipelineHandler.h"

#include "IVideoGrabber.h"
#include "CMySrc.h"
#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"

#include "StringConverter.h"
#include "ILogManager.h"
#include "IThreadManager.h"

#include <gst/app/gstappsrc.h>
namespace mray
{
namespace video
{

class GstCustomVideoStreamerImpl :public GstPipelineHandler,public IPipelineListener
{
protected:

	GstCustomVideoStreamer* m_owner;

	core::string m_ipAddr;
	uint m_videoPort;
	uint m_clockPort;

	IVideoGrabber* m_grabber[2];
	int m_bitRate;
	bool m_rtcp;

	core::string m_pipeLineString;
	GstMyUDPSink* m_videoSink;
	GstMyUDPSink* m_videoRtcpSink;
	GstMyUDPSrc* m_videoRtcpSrc;

	int m_fps;
	math::vector2di m_frameSize;

	video::ImageInfo m_tmp;

	struct VideoSrcData
	{
		GstAppSrcCallbacks srcCB;
		GstAppSrc* videoSrc;
		//GstMySrc * videoSrc;
		int index;
		GstCustomVideoStreamerImpl* o;
		int sourceID;
	};
	VideoSrcData m_videoSrc[2];


public:
	GstCustomVideoStreamerImpl(GstCustomVideoStreamer* owner)
	{
		m_owner = owner;
		m_ipAddr = "127.0.0.1";
		m_videoPort = 5000;
		m_clockPort = 5010;

		m_bitRate = 5000;
		m_grabber[0] = m_grabber[1] = 0;

		m_videoSink = 0;
		m_videoRtcpSink = 0;
		m_videoRtcpSrc = 0;

		m_videoSrc[0].sourceID = 0;
		m_videoSrc[1].sourceID = 0;

		m_frameSize.set(1280, 720);
		m_fps = 30;

		AddListener(this);

	}

	virtual ~GstCustomVideoStreamerImpl()
	{
		if (m_videoSink && m_videoSink->m_client)
		{
			m_videoSink->m_client->Close();
		}
		Stop();
	}
	void  SetResolution(int width, int height, int fps)
	{
		m_frameSize.set(width, height);
		m_fps = fps;
	}

	core::string GetFormatStr(EPixelFormat fmt)
	{

		core::string format = "RGB";
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
		case EPixel_YUYV:
			format = "Y41B";
			break;

		case EPixel_LUMINANCE8:
		case EPixel_Alpha8:
			format = "GRAY8";
			break;
		}
		return format;
	}

	core::string BuildGStr(int i)
	{

		core::string videoStr;
		if (m_grabber[i])
		{
			if (i == 0)
			{
				core::string format = GetFormatStr(m_grabber[i]->GetImageFormat());
				//ksvideosrc

				videoStr = "appsrc";
				videoStr += " name=src" + core::StringConverter::toString(i) +
					" do-timestamp=true is-live=true block=true"
					" ! video/x-raw,format=" + format + ",width=" + core::StringConverter::toString(m_grabber[i]->GetFrameSize().x) +
					",height=" + core::StringConverter::toString(m_grabber[i]->GetFrameSize().y) + ",framerate=" + core::StringConverter::toString(m_fps) + "/1 ";

				videoStr += "! videoconvert  ! video/x-raw,format=I420 ";// !videoflip method = 1  ";
				//videoStr += "! videorate ";//" max-rate=" + core::StringConverter::toString(m_fps) + " ";
				//	videoStr += " ! queue ";
				//	if (m_grabber[i]->GetImageFormat()!=video::EPixel_YUYV)
			}
			else
			{
				videoStr = "videotestsrc name=src2 ! video/x-raw,format=I420,width=640,height=480,framerate=60/1 ! videoconvert  ";

			}
		}
		else{
			videoStr = "mysrc name=src" + core::StringConverter::toString(i) +
				" ! video/x-raw,format=RGB  ";// !videoflip method = 1  ";
		}
		//add time stamp

		return videoStr;
	}

	void BuildString()
	{
		core::string videoStr;

		bool mixer = false;
		int height = m_frameSize.y;
		int width = m_frameSize.x;
		if (m_grabber[0]->GetFrameSize().x * 2 < width)
			width = m_grabber[0]->GetFrameSize().x*2;

		if (m_grabber[0]->GetFrameSize().y < height)
			height = m_grabber[0]->GetFrameSize().y;

		int halfW = width / 2;
		if (m_grabber[0] != 0 && m_grabber[1] != 0 )
		{
			mixer = true;
			videoStr = "videomixer name=mix ";

				"  sink_0::xpos=0 sink_0::ypos=0  sink_0::zorder=0 sink_0::alpha=1  "
				"  sink_1::xpos=0 sink_1::ypos=0  sink_1::zorder=1  "
				"  sink_2::xpos=" + core::StringConverter::toString(halfW) + "   sink_2::ypos=0  sink_2::zorder=1  ";

// 			core::string fmt = "video/x-raw,format=I420,width=" + core::StringConverter::toString(width) + ",height=" + core::StringConverter::toString(height);
// 			videoStr += "videotestsrc pattern=\"black\"  ! "+fmt+ " !  mix.sink_0 ";

		}
		for (int i = 0; i < 2; ++i)
		{
			if (m_grabber[i])
			{
				videoStr += BuildGStr(i);

				if (mixer )
				{
					if (m_grabber[i] && (halfW != m_grabber[i]->GetFrameSize().x || height != m_grabber[i]->GetFrameSize().y))
					{
						videoStr += "! videoscale ! video/x-raw,width=" + core::StringConverter::toString(halfW) +
						",height=" + core::StringConverter::toString(height) ;
					}
					//videoStr += " ! autovideosink sync=false ";

					videoStr += " ! videobox left=-" + core::StringConverter::toString(i*halfW) + " ";
					videoStr += +" ! mix.sink_" + core::StringConverter::toString(i ) + " ";
				}
				else
				{
				//	videoStr += " ! timeoverlay halignment=right valignment=position ypos=0.15 text=\"Remote Time =\" ";
				}
			}
		}
		if (!mixer && (width < m_grabber[0]->GetFrameSize().x || height < m_grabber[0]->GetFrameSize().y))
		{
			videoStr += "! videoscale ! video/x-raw,width=" + core::StringConverter::toString(width) +
				",height=" + core::StringConverter::toString(height) + ",framerate=" + core::StringConverter::toString(m_fps) + "/1";
		}
		if (mixer)
		{
			videoStr += " mix. ";
		//	videoStr += " ! timeoverlay halignment=right valignment=position ypos=0.15 text=\"Remote Time =\" ";
		}
		else
		{
		}

		//videoStr = "mysrc name=src0 ! video/x-raw,format=RGB,width=640,height=480,framerate="+core::StringConverter::toString(m_fps)+"/1 ! videoconvert ";
		//encoder string


		videoStr += "! x264enc name=videoEnc bitrate=" + core::StringConverter::toString(m_bitRate) + 
			" speed-preset=superfast tune=zerolatency sync-lookahead=0  pass=qual ! rtph264pay ";
		if (m_rtcp)
		{
			m_pipeLineString = "rtpbin  name=rtpbin " +
				videoStr +
				"! rtpbin.send_rtp_sink_0 "

				"rtpbin.send_rtp_src_0 ! "
				"myudpsink name=videoSink  "

				"rtpbin.send_rtcp_src_0 ! "
				"myudpsink name=videoRtcpSink sync=false async=false "
				"myudpsrc name=videoRtcpSrc ! rtpbin.recv_rtcp_sink_0 ";
		}
		else
		{

			m_pipeLineString = videoStr + " ! "
				"myudpsink name=videoSink sync=false";
			//"udpsink host=127.0.0.1 port=7000";
		}

	}

	void SetBitRate(int bitRate)
	{
		m_bitRate = bitRate;
	}
	void SetVideoGrabber(IVideoGrabber* grabber0, IVideoGrabber* grabber1)
	{
		m_grabber[0] = grabber0;
		m_grabber[1] = grabber1;
	}


	GstFlowReturn NeedBuffer(GstMySrc * sink, GstBuffer ** buffer,int index)
	{
		if (!m_grabber[index])
		{
			gLogManager.log("No video grabber is assigned to CustomVideoStreamer", ELL_WARNING);
			return GST_FLOW_ERROR;
		}
// 		do 
// 		{
// 			OS::IThreadManager::getInstance().sleep (1);
// 		} while (!m_grabber[index]->GrabFrame());
 		if (!m_grabber[index]->GrabFrame())
  		{
  			return GST_FLOW_ERROR;
  		}
		m_grabber[index]->Lock();

		const video::ImageInfo* ifo = m_grabber[index]->GetLastFrame();
		int len = ifo->imageDataSize;
		GstMapInfo map;
		GstBuffer* outbuf = gst_buffer_new_and_alloc(len);
		gst_buffer_map(outbuf, &map, GST_MAP_WRITE);
		memcpy(map.data, ifo->imageData, len);


		gst_buffer_unmap(outbuf, &map);
		m_grabber[index]->Unlock();
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

		for (int i = 0; i < 2; ++i)
		{
			core::string name = "src" + core::StringConverter::toString(i);
#if 1
			m_videoSrc[i].videoSrc = GST_APP_SRC(gst_bin_get_by_name(GST_BIN(GetPipeline()), name.c_str()));
			m_videoSrc[i].o = this;
			m_videoSrc[i].index = i;
			if (m_videoSrc[i].videoSrc){

				gst_base_src_set_blocksize(GST_BASE_SRC(m_videoSrc[i].videoSrc), 640 * 480 * 3);
				gst_base_src_set_live(GST_BASE_SRC(m_videoSrc[i].videoSrc), false);
				gst_base_src_set_async(GST_BASE_SRC(m_videoSrc[i].videoSrc), false);
				gst_base_src_set_do_timestamp(GST_BASE_SRC(m_videoSrc[i].videoSrc), true);

				gst_app_src_set_max_bytes(m_videoSrc[i].videoSrc, 640 * 480 * 3);
				gst_app_src_set_emit_signals(m_videoSrc[i].videoSrc, false);

				m_videoSrc[i].srcCB.need_data = &start_feed;
				m_videoSrc[i].srcCB.enough_data = &stop_feed;
				m_videoSrc[i].srcCB.seek_data = &seek_data;
				gst_app_src_set_callbacks(m_videoSrc[i].videoSrc, &m_videoSrc[i].srcCB, &m_videoSrc[i], NULL);
			}
#else

			m_videoSrc[i].videoSrc = GST_MySRC(gst_bin_get_by_name(GST_BIN(m_gstPipeline), name.c_str()));
			m_videoSrc[i].o = this;
			m_videoSrc[i].index = i;
			if (m_videoSrc[i].videoSrc){
				m_videoSrc[i].videoSrc->need_buffer = need_buffer;
				m_videoSrc[i].videoSrc->data = &m_videoSrc[i];
			}
#endif
		}
		//g_object_set(G_OBJECT(m_videoSink), "sync", false, (void*)NULL);

		SET_SINK(videoSink, m_videoPort);
		if (m_rtcp){
			SET_SRC(videoRtcpSrc, (m_videoPort + 1));
			SET_SINK(videoRtcpSink, (m_videoPort + 2));
		}

	}

	void BindPorts(const core::string& addr, int videoPort, uint clockPort, bool rtcp)
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
		BuildString();
		GstElement* pipeline = gst_parse_launch(m_pipeLineString.c_str(), &err);
		if (err)
		{
			gLogManager.log("GstCustomVideoStreamer: Pipeline error: " + core::string(err->message),ELL_WARNING);
		}
		if (!pipeline)
			return false;

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
		GstPipelineHandler::Close();
	}


	virtual void OnPipelineReady(GstPipelineHandler* p){ m_owner->__FIRE_OnStreamerReady(m_owner); }
	virtual void OnPipelinePlaying(GstPipelineHandler* p){ m_owner->__FIRE_OnStreamerStarted(m_owner); }
	virtual void OnPipelineStopped(GstPipelineHandler* p){ m_owner->__FIRE_OnStreamerStopped(m_owner); }
};


GstCustomVideoStreamer::GstCustomVideoStreamer()
{
	m_impl = new GstCustomVideoStreamerImpl(this);
}

GstCustomVideoStreamer::~GstCustomVideoStreamer()
{
	delete m_impl;
}
void GstCustomVideoStreamer::Stream()
{
	m_impl->Stream();
}
void GstCustomVideoStreamer::Stop()
{
	m_impl->Stop();
}


void GstCustomVideoStreamer::BindPorts(const core::string& addr, uint videoPort, uint clockPort, bool rtcp)
{
	m_impl->BindPorts(addr, videoPort, clockPort, rtcp);
}

bool GstCustomVideoStreamer::CreateStream()
{
	return m_impl->CreateStream();
}

void GstCustomVideoStreamer::Close()
{
	m_impl->Close();
}
bool GstCustomVideoStreamer::IsStreaming()
{
	return m_impl->IsStreaming();
}

void GstCustomVideoStreamer::SetBitRate(int bitRate)
{
	m_impl->SetBitRate(bitRate);
}

void GstCustomVideoStreamer::SetResolution(int width, int height, int fps)
{
	m_impl->SetResolution(width, height, fps);
}

void GstCustomVideoStreamer::SetVideoGrabber(IVideoGrabber* grabber0, IVideoGrabber* grabber1)
{
	m_impl->SetVideoGrabber(grabber0,grabber1);
}
void GstCustomVideoStreamer::SetPaused(bool paused)
{
	m_impl->SetPaused(paused);
}

bool GstCustomVideoStreamer::IsPaused()
{
	return !m_impl->IsPlaying();
}
GstPipelineHandler* GstCustomVideoStreamer::GetPipeline()
{
	return m_impl;
}

}
}

