



#include "stdafx.h"
#include "GstCustomVideoStreamer.h"

#include "GstPipelineHandler.h"

#include "IVideoGrabber.h"
#include "CMySrc.h"
#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"

#include "IThreadManager.h"

#include <gst/app/gstappsrc.h>
namespace mray
{
namespace video
{

class GstCustomVideoStreamerImpl :public GstPipelineHandler, public IPipelineListener
{
protected:

	GstCustomVideoStreamer* m_owner;

	uint m_clockPort;
	IVideoGrabber* m_grabber;

	std::string m_pipeLineString;
	std::string m_srcPipeLineString;

	int m_fps;
	Vector2d m_frameSize;

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
	VideoSrcData m_videoSrc;


public:
	GstCustomVideoStreamerImpl(GstCustomVideoStreamer* owner)
	{
		m_owner = owner;
		m_grabber = 0;


		m_videoSrc.sourceID = 0;

		m_frameSize = Vector2d(640, 480);
		m_fps = 30;

		AddListener(this);

	}

	virtual ~GstCustomVideoStreamerImpl()
	{
		Stop();
	}
	void SetPipelineString(const std::string& pipeline)
	{
		m_srcPipeLineString = pipeline;
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
			ss << "appsrc"
				" name=src"
				<< " do-timestamp=true is-live=true block=true"
				" ! video/x-raw,format=" + format + ",width=" << m_grabber->GetFrameSize().x <<
				",height=" << m_grabber->GetFrameSize().y << ",framerate=" << m_fps << "/1 "
				<< "! videoconvert  ! video/x-raw,format=I420 ! ";// !videoflip method = 1  ";
			//videoStr += "! videorate ";//" max-rate=" + std::stringConverter::toString(m_fps) + " ";
			//	videoStr += " ! queue ";
			//	if (m_grabber->GetImageFormat()!=video::EPixel_YUYV)
		}
		else{
			ss << "videotestsrc name=src" <<
				" ! video/x-raw,format=I420 ! ";// !videoflip method = 1  ";
		}
		//add time stamp

		return ss.str();
	}

	void BuildString()
	{
		std::string videoStr;
		std::stringstream ss;

		ss << BuildGStr();

		int height = m_frameSize.y;
		int width = m_frameSize.x;
		/*
		if (m_grabber->GetFrameSize().x !=0 &&  < width)
			width = m_grabber->GetFrameSize().x;

		if (m_grabber->GetFrameSize().y < height)
			height = m_grabber->GetFrameSize().y;

		if ((width < m_grabber->GetFrameSize().x || height < m_grabber->GetFrameSize().y))
		{
			ss << "! videoscale ! video/x-raw,width=" << width <<
				",height=" << height << ",framerate=" << m_fps << "/1";
		}*/
		//videoStr = "mysrc name=src0 ! video/x-raw,format=RGB,width=640,height=480,framerate="+std::stringConverter::toString(m_fps)+"/1 ! videoconvert ";
		//encoder string


		ss << m_srcPipeLineString;
		m_pipeLineString = ss.str();

	}

	void SetVideoGrabber(IVideoGrabber* grabber0)
	{
		m_grabber = grabber0;
	}


	GstFlowReturn NeedBuffer(GstMySrc * sink, GstBuffer ** buffer, int index)
	{
		if (!IsStreaming() || !m_grabber)
		{
			//LogMessage(std::string("No video grabber is assigned to CustomVideoStreamer"), ELL_WARNING);
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
		memcpy(map.data+1, ifo->imageData, len-1); //offset one pixel to fix RGB -->BRG issue (2017/4/24th)


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
			return d->o->NeedBuffer(sink, buffer, d->index);
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
			ss << "src";
			std::string name = ss.str();
#if 1
			m_videoSrc.videoSrc = GST_APP_SRC(gst_bin_get_by_name(GST_BIN(GetPipeline()), name.c_str()));
			m_videoSrc.o = this;
			m_videoSrc.index = 0;
			if (m_videoSrc.videoSrc){

				gst_base_src_set_blocksize(GST_BASE_SRC(m_videoSrc.videoSrc), 640 * 480 * 3);
				gst_base_src_set_live(GST_BASE_SRC(m_videoSrc.videoSrc), false);
				gst_base_src_set_async(GST_BASE_SRC(m_videoSrc.videoSrc), false);
				gst_base_src_set_do_timestamp(GST_BASE_SRC(m_videoSrc.videoSrc), true);

				gst_app_src_set_max_bytes(m_videoSrc.videoSrc, 640 * 480 * 3);
				gst_app_src_set_emit_signals(m_videoSrc.videoSrc, false);

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
		//g_object_set(G_OBJECT(m_videoSink), "sync", false, (void*)NULL);


	}

	bool CreateStream()
	{
		GError *err = 0;
		BuildString();
		GstElement* pipeline = gst_parse_launch(m_pipeLineString.c_str(), &err);
		if (err)
		{
			LogMessage(std::string("GstCustomVideoPlayer: Pipeline error:") + err->message, ELL_ERROR);
			LogMessage(m_pipeLineString, ELL_ERROR);
		}
		if (!pipeline)
			return false;

		SetPipeline(pipeline);
		_UpdatePorts();

		return CreatePipeline(true, "", m_clockPort);

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


void GstCustomVideoStreamer::SetPipeline(const std::string& pipeline)
{
	m_impl->SetPipelineString(pipeline);
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

void GstCustomVideoStreamer::SetResolution(int width, int height, int fps)
{
	m_impl->SetResolution(width, height, fps);
}

void GstCustomVideoStreamer::SetVideoGrabber(IVideoGrabber* grabber0)
{
	m_impl->SetVideoGrabber(grabber0);
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


