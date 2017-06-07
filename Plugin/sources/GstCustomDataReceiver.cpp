

#include "stdafx.h"
#include "GstCustomDataReceiver.h"

#include "VideoAppSinkHandler.h"
#include "GstPipelineHandler.h"
#include "UnityHelpers.h"
#include "CMyListener.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <vector>


namespace mray
{
namespace video
{

class DataSinkHandler :public IAppSinkHandler
{
	uint m_frameID;
	bool m_IsFrameNew;
public:
	bool GrabFrame()
	{

	}

	bool 			isFrameNew(){ return m_IsFrameNew; }
	uint GetFrameID(){ return m_frameID; }

	virtual GstFlowReturn process_sample(std::shared_ptr<GstSample> sample)
	{

	}

};

class GstCustomDataReceiverImpl :public GstPipelineHandler, IPipelineListener
{
	GstCustomDataReceiver* m_owner;
	std::string m_ipAddr;
	uint m_clockPort;

	std::string m_pipeLineString;

	std::string m_intermidateElems;

	video::EPixelFormat m_targetFormat;

	bool m_rtcp;

	uint m_dataPort;
	DataSinkHandler m_dataHandler;

	GstElement* m_dataSrc;
	GstAppSink* m_dataSink;

public:
	GstCustomDataReceiverImpl(GstCustomDataReceiver* o)
	{
		m_owner = o;
		m_ipAddr = "127.0.0.1";
		m_clockPort = 5010;

		GstPipelineHandler::AddListener(this);
		m_targetFormat = EPixel_B8G8R8;
	}
	virtual ~GstCustomDataReceiverImpl()
	{

	}
	void AddIntermidateElement(const std::string& elems)
	{
		m_intermidateElems = elems;
	}


	void _BuildPipeline()
	{
		m_pipeLineString = "";
		if (m_rtcp)
		{
			m_pipeLineString = "rtpbin name=rtpbin ";
		}


		std::stringstream ss;
		ss << "udpsrc name=dataSrc port=" << m_dataPort << " !"
			//"udpsrc port=7000 buffer-size=2097152 do-timestamp=true !"
			"application/x-rtp ";

		if (m_rtcp)
		{
			ss << "! rtpbin.recv_rtp_sink rtpbin. ";
		}
		else{
			ss << " ! rtpjitterbuffer latency=500  ";
		}
		ss << " ! mylistener name=rtplistener ! rtpgstdepay ! mylistener name=postdepay ! appsink name=\"dataSink\" ";


		m_pipeLineString += ss.str() + " ";
	}

	void _UpdatePorts()
	{
		if (!GetPipeline())
			return;
		m_dataSrc = gst_bin_get_by_name(GST_BIN(GetPipeline()), "dataSrc");
		if (m_dataPort != 0)
			g_object_set(m_dataSrc, "port", m_dataPort, 0);
	}

	void SetIPAddress(const std::string& ip, uint dataPort, uint clockPort, bool rtcp)
	{
		m_ipAddr = ip;
		m_clockPort = clockPort;
		m_rtcp = rtcp;

		m_dataPort = dataPort;

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
			LogMessage("GstCustomDataReceiver: Pipeline error: " + std::string(err->message), ELL_WARNING);
		}
		if (!p)
			return false;
		SetPipeline(p);
		printf("Connecting Video stream with IP:%s\n", m_ipAddr.c_str());

		_UpdatePorts();
		{
			m_dataSink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(p), "dataSink"));
		}


		g_signal_connect(m_dataSink, "new-sample", G_CALLBACK(new_buffer), this);
		//attach videosink callbacks
		GstAppSinkCallbacks gstCallbacks;
		gstCallbacks.eos = &IAppSinkHandler::on_eos_from_source;
		gstCallbacks.new_preroll = &IAppSinkHandler::on_new_preroll_from_source;
#if GST_VERSION_MAJOR==0
		gstCallbacks.new_buffer = &IAppSinkHandler::on_new_buffer_from_source;
#else
		gstCallbacks.new_sample = &IAppSinkHandler::on_new_buffer_from_source;
#endif
		gst_app_sink_set_callbacks(GST_APP_SINK(m_dataSink), &gstCallbacks, &m_dataHandler, NULL);
		//gst_app_sink_set_emit_signals(GST_APP_SINK(m_videoSink), true);


		// 		gst_base_sink_set_async_enabled(GST_BASE_SINK(m_videoSink), TRUE);
		gst_base_sink_set_sync(GST_BASE_SINK(m_dataSink), false);
		gst_app_sink_set_drop(GST_APP_SINK(m_dataSink), TRUE);
		gst_app_sink_set_max_buffers(GST_APP_SINK(m_dataSink), 2);
		gst_base_sink_set_max_lateness(GST_BASE_SINK(m_dataSink), 0);
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


		return CreatePipeline(false, m_ipAddr, m_clockPort);

	}
	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		//if (m_videoHandler[i].videoSrc && m_videoHandler[i].videoSrc->m_client)
		//	m_videoHandler[i].videoSrc->m_client->Close();
		m_dataHandler.Close();
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


	int GetFramesCount()
	{
		return 1;
	}
	virtual bool GrabFrame()
	{
		return m_dataHandler.GrabFrame();
	}
	virtual bool HasNewFrame()
	{
		return m_dataHandler.isFrameNew();
	}
	virtual ulong GetBufferID()
	{
		return m_dataHandler.GetFrameID();
	}


	virtual const ImageInfo* GetLastFrame(int i)
	{
		return m_dataHandler.getPixelsRef();
	}
	virtual unsigned long GetLastFrameTimestamp(int i)
	{
		return m_dataHandler.getPixelFrame()->RTPPacket.timestamp;
	}
	const GstImageFrame* GetLastDataFrame(int i)
	{
		return m_dataHandler.getPixelFrame();
	}
	void* GetLastFrameRTPMeta(int i)
	{
		return &m_dataHandler.getPixelFrame()->RTPPacket;
	}

	virtual int GetPort()
	{
		if (m_dataSrc){
			//return m_videoHandler[i].videoSrc->port;
			gint port = 0;
			g_object_get(m_dataSrc, "port", &port, 0);
			return port;
		}
		else return m_dataPort;
	}


	virtual void OnPipelineReady(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerReady(m_owner); }
	virtual void OnPipelinePlaying(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerStarted(m_owner); }
	virtual void OnPipelineStopped(GstPipelineHandler* p){ m_owner->__FIRE_OnPlayerStopped(m_owner); }

	ulong GetNetworkUsage()
	{
		ulong usage = 0;
		for (int i = 0; i < m_playersCount; ++i)
		{
			//			usage+=m_videoHandler[i].videoSrc->m_client->GetReceiverNetworkUsage();
		}
		return usage;
	}
};


GstCustomDataReceiver::GstCustomDataReceiver()
{
	m_impl = new GstCustomDataReceiverImpl(this);
}

GstCustomDataReceiver::~GstCustomDataReceiver()
{
	delete m_impl;
}
void GstCustomDataReceiver::SetIPAddress(const std::string& ip, uint videoPort, uint count, uint clockPort, bool rtcp)
{
	m_impl->SetIPAddress(ip, videoPort, count, clockPort, rtcp);
}
bool GstCustomDataReceiver::CreateStream()
{
	return m_impl->CreateStream();
}

void GstCustomDataReceiver::AddIntermidateElement(const std::string& elems)
{
	m_impl->AddIntermidateElement(elems);
}

GstPipelineHandler*GstCustomDataReceiver::GetPipeline()
{
	return m_impl;
}
uint GstCustomDataReceiver::GetDataPort()
{
	return m_impl->GetPort();
}


void GstCustomDataReceiver::SetVolume(float vol)
{
	m_impl->SetVolume(vol);
}
bool GstCustomDataReceiver::IsStream()
{
	return m_impl->IsStream();
}

void GstCustomDataReceiver::Play()
{
	m_impl->Play();
}
void GstCustomDataReceiver::Stop()
{
	m_impl->Stop();
}
void GstCustomDataReceiver::Pause()
{
	m_impl->Pause();
}
bool GstCustomDataReceiver::IsLoaded()
{
	return m_impl->IsLoaded();
}
bool GstCustomDataReceiver::IsPlaying()
{
	return m_impl->IsPlaying();
}
void GstCustomDataReceiver::Close()
{
	m_impl->Close();

}


int GstCustomDataReceiver::GetFramesCount()
{
	return m_impl->GetFramesCount();
}

bool GstCustomDataReceiver::GrabFrame(int i)
{
	return m_impl->GrabFrame(i);
}

bool GstCustomDataReceiver::HasNewFrame(int i)
{
	return m_impl->HasNewFrame(i);
}

ulong GstCustomDataReceiver::GetBufferID(int i)
{
	return m_impl->GetBufferID(i);
}


const ImageInfo* GstCustomDataReceiver::GetLastFrame(int i)
{
	return m_impl->GetLastFrame(i);
}


const GstImageFrame* GstCustomDataReceiver::GetLastDataFrame(int i)
{
	return m_impl->GetLastDataFrame(i);
}
unsigned long GstCustomDataReceiver::GetLastFrameTimestamp(int i)
{
	return m_impl->GetLastFrameTimestamp(i);
}
void* GstCustomDataReceiver::GetLastFrameRTPMeta(int i)
{
	return m_impl->GetLastFrameRTPMeta(i);

}
int GstCustomDataReceiver::GetPort(int i)
{
	return m_impl->GetPort(i);
}

ulong GstCustomDataReceiver::GetNetworkUsage()
{
	return m_impl->GetNetworkUsage();

}

}
}


