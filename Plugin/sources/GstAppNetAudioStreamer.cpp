

#include "stdafx.h"
#include "GstAppNetAudioStreamer.h"


#include "GstPipelineHandler.h"

#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"
#include "UnityHelpers.h"
#include <sstream>

#include <gst/app/gstappsrc.h>




namespace mray
{
namespace video
{



class GstAppNetAudioStreamerImpl :public GstPipelineHandler
{
protected:
	std::string m_ipAddr;
	uint m_audioPort;

	uint m_appSourceID;
	GstAppSrcCallbacks m_srcCB;
	std::string m_pipeLineString;
	GstAppSrc* m_audioSrc;
	GstElement* m_audioSink;
	GstMyUDPSink* m_audioRtcpSink;
	GstMyUDPSrc* m_audioRtcpSrc;
	
	IAudioGrabber* m_audioGrabber;

	bool m_rtcp;
public:
	GstAppNetAudioStreamerImpl()
	{
		m_appSourceID = 0;
		m_audioSink = 0;
		m_audioRtcpSink = 0;
		m_audioRtcpSrc = 0;
		m_ipAddr = "127.0.0.1";
		m_audioPort = 5005;
		m_rtcp = false;
		m_audioGrabber = 0;
	}
	virtual ~GstAppNetAudioStreamerImpl()
	{
	}
#define OPUS_ENC
	void BuildString()
	{
#ifdef FLAC_ENC

		std::string audioStr = "directsoundsrc! audio/x-raw,endianness=1234,signed=true,width=16,depth=16,rate=8000,channels=1 ! audioconvert ! flacenc quality=2 ! rtpgstpay ";
#elif defined OPUS_ENC
		//actual-buffer-time=0 actual-latency-time=0

		std::stringstream ss;

		ss<< "appsrc name=src  ";

		ss << " ! audio/x-raw,endianness=1234,format=F32LE,rate=" << (m_audioGrabber->GetSamplingRate()) <<
			",channels=" << (m_audioGrabber->GetChannelsCount()) << " "
			//" ! audiochebband mode=band-pass lower-frequency=1000 upper-frequency=6000 poles=4 "
			"! audioconvert  ! audioresample ! ";
		//	"audiochebband mode=band-pass lower-frequency=1000 upper-frequency=4000 type=2 ! "

		ss<< "opusenc  bitrate=96000 frame-size=5 ! rtpopuspay  ";
#elif defined VORBIS_ENC
		//actual-buffer-time=0 actual-latency-time=0
		std::string audioStr = "directsoundsrc buffer-time=200 ";

		if (m_interface.deviceGUID != "")
		{
			audioStr += "device=\"" + m_interface.deviceGUID + "\"";
		}

		audioStr += " ! audio/x-raw,endianness=1234,signed=true,width=16,depth=16,rate=32000,channels=2   ! audioconvert ! ";
		//	"audiochebband mode=band-pass lower-frequency=1000 upper-frequency=4000 type=2 ! "


		audioStr += "vorbisenc quality=1 ! rtpvorbispay config-interval=3 ";
#elif defined SPEEX_ENC
		std::string audioStr = "directsoundsrc! audio/x-raw,endianness=1234,signed=true,width=16,depth=16,rate=22000,channels=2   ! speexenc ! rtpspeexpay";
#else
		std::string audioStr = "directsoundsrc! audio/x-raw,endianness=1234,signed=true,width=16,depth=16,rate=8000,channels=1   ! amrnbenc ! rtpamrpay";
#endif
		if (m_rtcp)
		{
			m_pipeLineString = "rtpbin  name=rtpbin " + ss.str() + " ! "
				" rtpbin.send_rtp_sink_0 "

				"rtpbin.send_rtp_src_0 ! "
				"myudpsink name=audioSink  "

				"rtpbin.send_rtcp_src_0 ! "
				"myudpsink name=audioRtcpSink sync=false async=false "
				"myudpsrc name=audioRtcpSrc ! rtpbin.recv_rtcp_sink_0 ";
		}
		else
		{
			ss << +" ! "
				"udpsink name=audioSink port=" << (m_audioPort) << " host=" << m_ipAddr << " sync=false ";

		}
		m_pipeLineString = ss.str();


	}
	void _UpdatePorts()
	{
		if (!GetPipeline())
			return;
#define SET_SRC(name,p) m_##name=GST_MyUDPSrc(gst_bin_get_by_name(GST_BIN(GetPipeline()), #name)); if(m_##name){m_##name->SetPort(p);}
#define SET_SINK(name,p) m_##name=GST_MyUDPSink(gst_bin_get_by_name(GST_BIN(GetPipeline()), #name)); if(m_##name){m_##name->SetPort(m_ipAddr,p);}


		m_audioSink = gst_bin_get_by_name(GST_BIN(GetPipeline()), "audioSink");
		g_object_set(m_audioSink, "port", m_audioPort, 0);
		g_object_set(m_audioSink, "host", m_ipAddr.c_str(), 0);
		//	SET_SINK(audioSink, m_audioPort);
		SET_SRC(audioRtcpSrc, (m_audioPort + 1));
		SET_SINK(audioRtcpSink, (m_audioPort + 2));

	}


	void SetAudioGrabber(IAudioGrabber* g)
	{
		m_audioGrabber=g;
	}

	// addr: target address to stream video to
	// audioport: port for the audio stream , audio rtcp is allocated as audioPort+1 and audioPort+2
	void BindPorts(const std::string& addr, uint audioPort, bool rtcp)
	{
		m_ipAddr = addr;
		m_audioPort = audioPort;
		m_rtcp = rtcp;
		_UpdatePorts();
	}


	GstFlowReturn NeedBuffer( GstBuffer ** buffer)
	{
		if (!IsPlaying())
			return GST_FLOW_ERROR;

		if (!m_audioGrabber)
		{
			LogMessage("GstAppNetAudioStreamer::NeedBuffer() -No audio grabber is assigned to GstAppNetAudioStreamer", ELL_WARNING);
			return GST_FLOW_ERROR;
		}
		if (!m_audioGrabber->GrabFrame())
		{
			//	LogMessage("AppSrcVideoSrc::NeedBuffer() - Failed to grab buffer " + std::StringConverter::toString(index), ELL_WARNING);
			return GST_FLOW_ERROR;
		}
		//m_grabber[index]->Lock();

		float* data = m_audioGrabber->GetAudioFrame();
		uint length = m_audioGrabber->GetAudioFrameSize()*sizeof(float);
		*buffer = gst_buffer_new_wrapped_full(GST_MEMORY_FLAG_READONLY, data, length, 0, length, 0, 0);
		//int len = ifo->imageDataSize;
		//GstMapInfo map;
		//GstBuffer* outbuf = gst_buffer_new_and_alloc(len);
		//gst_buffer_map(outbuf, &map, GST_MAP_WRITE);
		//memcpy(map.data, ifo->imageData, len);

		//	gst_buffer_unmap(outbuf, &map);
		//	m_grabber[index]->Unlock();
		//	*buffer = outbuf;

		//OS::IThreadManager::getInstance().sleep(5);
		return GST_FLOW_OK;
	}

	static gboolean read_data(GstAppNetAudioStreamerImpl *d)
	{
		GstFlowReturn ret;

		GstBuffer *buffer;
		if (d->NeedBuffer( &buffer) == GST_FLOW_OK)
		{
			ret = gst_app_src_push_buffer(d->m_audioSrc, buffer);
			//	LogMessage("pushing data to: ", std::StringConverter::toString(d->index),ELL_INFO);
			if (ret != GST_FLOW_OK){
				LogMessage("GstAppNetAudioStreamer::read_data() - Failed to push data to AppSrc " , ELL_WARNING);
				ret = gst_app_src_end_of_stream(d->m_audioSrc);
				return FALSE;
			}
		}
		return TRUE;

	}
	static void start_feed(GstAppSrc *source, guint size, gpointer data)
	{
		GstAppNetAudioStreamerImpl* o = static_cast<GstAppNetAudioStreamerImpl*>(data);
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
		GstAppNetAudioStreamerImpl* o = static_cast<GstAppNetAudioStreamerImpl*>(data);
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



	bool CreateStream()
	{
		if (!m_audioGrabber)
			return false;
		GError *err = 0;
		BuildString();
		GstElement* p = gst_parse_launch(m_pipeLineString.c_str(), &err);
		LogMessage("GstAppNetAudioStreamer::Starting with pipeline: " + m_pipeLineString, ELL_INFO);
		if (err)
		{
			LogMessage("GstAppNetAudioStreamer: Pipeline error: " + std::string(err->message), ELL_WARNING);
			gst_object_unref(p);
			p = 0;
		}
		if (!p)
			return false;
		LogMessage("GstAppNetAudioStreamer::Finished Linking Pipeline", ELL_INFO);

		m_audioSrc = GST_APP_SRC(gst_bin_get_by_name(GST_BIN((GstElement*)p), "src"));

		gst_base_src_set_do_timestamp(GST_BASE_SRC(m_audioSrc), true);

		g_object_set(G_OBJECT(m_audioSrc),
			"stream-type", GST_APP_STREAM_TYPE_STREAM, // GST_APP_STREAM_TYPE_STREAM
			"format", GST_FORMAT_TIME,
			"is-live", TRUE,
			NULL);

		gst_app_src_set_emit_signals(m_audioSrc, false);

		m_srcCB.need_data = &start_feed;
		m_srcCB.enough_data = &stop_feed;
		m_srcCB.seek_data = &seek_data;
		gst_app_src_set_callbacks(m_audioSrc, &m_srcCB, this, NULL);
		SetPipeline(p);
		_UpdatePorts();

		return CreatePipeline(false);

	}
	void Stream()
	{
		SetPaused(false);
	}
	void Stop()
	{
		SetPaused(true);
	}
	bool IsStreaming()
	{
		return IsPlaying();
	}
	virtual void Close()
	{
		GstPipelineHandler::Close();
		if (m_appSourceID != 0)
		{
			g_source_remove(m_appSourceID);
			m_appSourceID = 0;
		}
	}

};


GstAppNetAudioStreamer::GstAppNetAudioStreamer()
{
	m_impl = new GstAppNetAudioStreamerImpl();
}

GstAppNetAudioStreamer::~GstAppNetAudioStreamer()
{
	delete m_impl;
}
void GstAppNetAudioStreamer::Stream()
{
	m_impl->Stream();
}
void GstAppNetAudioStreamer::Stop()
{
	m_impl->Stop();
}
GstPipelineHandler* GstAppNetAudioStreamer::GetPipeline()
{
	return m_impl;
}

void GstAppNetAudioStreamer::SetAudioGrabber(IAudioGrabber* g)
{
	m_impl->SetAudioGrabber(g);
}

void GstAppNetAudioStreamer::BindPorts(const std::string& addr, uint* ports, uint count, bool rtcp)
{
	m_impl->BindPorts(addr, ports[0], rtcp);
}

bool GstAppNetAudioStreamer::CreateStream()
{
	return m_impl->CreateStream();
}

void GstAppNetAudioStreamer::Close()
{
	m_impl->Close();
}
bool GstAppNetAudioStreamer::IsStreaming()
{
	return m_impl->IsStreaming();
}

void GstAppNetAudioStreamer::SetPaused(bool paused)
{
	m_impl->SetPaused(paused);
}

bool GstAppNetAudioStreamer::IsPaused()
{
	return !m_impl->IsPlaying();
}

}
}

