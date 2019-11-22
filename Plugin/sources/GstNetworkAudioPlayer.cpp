

#include "stdafx.h"
#include "GstNetworkAudioPlayer.h"


//#include "CMyUDPSrc.h"
//#include "CMyUDPSink.h"

#include "GstPipelineHandler.h"
#include "UnityHelpers.h"
#include "AudioAppSinkHandler.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

namespace mray
{
namespace video
{

class GstNetworkAudioPlayerImpl :public GstPipelineHandler
{
	std::string m_ipAddr;
	uint m_audioPort;
	uint m_clockPort;

	std::string m_pipeLineString;

	GstElement* m_audioSrc;
	GstElement* m_audioRtcpSrc;
	GstElement* m_audioRtcpSink;
	bool m_rtcp;

	bool m_customAudioInterface;
	GstAppSink* m_audioSink;
	AudioAppSinkHandler m_audioHandler;

	int m_sampleRate;

public:
	GstNetworkAudioPlayerImpl()
	{
		m_ipAddr = "127.0.0.1";
		m_audioPort = 5005;
		m_audioSrc = 0;
		m_clockPort = 5010;
		m_audioRtcpSrc = 0;
		m_audioRtcpSink = 0;
		m_audioSink = 0;
		m_customAudioInterface = false;
		m_sampleRate = 32000;
	}
	virtual ~GstNetworkAudioPlayerImpl()
	{
		Close();
	}

#define OPUS_ENC
	void _BuildPipeline()
	{
#ifdef FLAC_ENC
		core::string audiocaps = " caps=application/x-rtp,media=application,clock-rate=90000,encoding-name=X-GST,payload=96 ";
		core::string audioStr = " rtpgstdepay ! audio/x-flac,rate=8000,channels=1! flacdec !  audioconvert ! audioresample  ";
#elif defined OPUS_ENC

		std::string audiocaps = "caps=application/x-rtp,media=(string)audio,clock-rate=48000,encoding-name=OPUS,payload=96,encoding-params=2 ";
		char buffer[128];
		sprintf(buffer, "%d", m_sampleRate);

		std::string audioStr = " rtpopusdepay ! opusdec ! audioconvert ";
#elif defined VORBIS_ENC

		std::string audiocaps = "caps=application/x-rtp,media=(string)audio,clock-rate=32000,encoding-name=VORBIS,payload=96,encoding-params=2 ";
		char buffer[128];
		sprintf(buffer, "%d", m_sampleRate);

		std::string audioStr = " rtpvorbisdepay ! vorbisdec ! audioconvert   ! audio/x-raw, rate=";//! audioresample quality=10
		audioStr+=buffer;
		audioStr+=" ";
#elif defined SPEEX_ENC
		core::string audiocaps = "caps=application/x-rtp,media=(string)audio,clock-rate=(int)22000,encoding-name=(string)SPEEX ";
		core::string audioStr = " rtpspeexdepay ! speexdec ! audioconvert ! audioresample  ";
#else
		core::string audiocaps = "caps=application/x-rtp,media=(string)audio,clock-rate=(int)8000,encoding-name=(string)AMR,encoding-params=(string)1,octet-align=(string)1 ";
		core::string audioStr = " rtpamrdepay ! amrnbdec  ";
#endif
		if (m_rtcp)
		{
			m_pipeLineString =
				"rtpbin name=rtpbin "
				"udpsrc name=audioSrc "+ audiocaps +
				//audio rtp
				"! rtpbin.recv_rtp_sink_0 "

				"rtpbin. ! " + audioStr+ " ! directsoundsink "

				"udpsrc name=audioRtcpSrc ! rtpbin.recv_rtcp_sink_0 "

				//audio rtcp
				"rtpbin.send_rtcp_src_0 ! myudpsink name=audioRtcpSink sync=false async=false ";
		}
		else
		{
			m_pipeLineString =
				"udpsrc name=audioSrc " + audiocaps + "!" + audioStr;//
			if (m_customAudioInterface)
			{
				m_pipeLineString += "! audioresample ! audio/x-raw,format=F32LE,rate=";//! audioresample quality=10
				m_pipeLineString += buffer;
				m_pipeLineString += " ! appsink name=audioSink sync=false  emit-signals=false";
			}
			else
			{
				m_pipeLineString += " ! directsoundsink buffer-time=40000  sync=false";
			}

		}

	}
	void _UpdatePorts()
	{
		if (!GetPipeline())
			return;
#define SET_SRC(name,p) m_##name=GST_MyUDPSrc(gst_bin_get_by_name(GST_BIN(GetPipeline()), #name)); if(m_##name){m_##name->SetPort(p);}
#define SET_SINK(name,p) m_##name=GST_MyUDPSink(gst_bin_get_by_name(GST_BIN(GetPipeline()), #name)); if(m_##name){m_##name->SetPort(m_ipAddr,p);}


		m_audioSrc = gst_bin_get_by_name(GST_BIN(GetPipeline()), "audioSrc");
		if (m_audioSrc)
			g_object_set(m_audioSrc, "port", m_audioPort, 0);
		m_audioRtcpSink = gst_bin_get_by_name(GST_BIN(GetPipeline()), "audioSrc");
		if (m_audioRtcpSink)
			g_object_set(m_audioRtcpSink, "port", m_audioPort+1, 0);
		m_audioRtcpSrc = gst_bin_get_by_name(GST_BIN(GetPipeline()), "audioSrc");
		if (m_audioRtcpSrc)
			g_object_set(m_audioRtcpSrc, "port", m_audioPort+2, 0);
//		SET_SRC(audioSrc, m_audioPort);
//		SET_SINK(audioRtcpSink, (m_audioPort + 1));
//		SET_SRC(audioRtcpSrc, (m_audioPort + 2));

	}

	void SetIPAddress(const std::string& ip, uint audioPort,uint clockPort, bool rtcp)
	{
		m_ipAddr = ip;
		m_audioPort = audioPort;
		m_clockPort = clockPort;
		m_rtcp = rtcp;

		//set src and sinks elements
		_UpdatePorts();
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
			LogMessage("NetworkAudioPlayer:CreateStream() - Pipeline error:" + std::string(err->message), ELL_WARNING);
		}
		if (!p)
			return false;

		SetPipeline(p);
		_UpdatePorts();

		if (m_customAudioInterface)
		{

			m_audioSink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(p), "audioSink"));
			m_audioHandler.SetSink(m_audioSink);

			g_signal_connect(m_audioSink, "new-sample", G_CALLBACK(new_buffer), this);
			//attach videosink callbacks
			GstAppSinkCallbacks gstCallbacks;
			gstCallbacks.eos = &AudioAppSinkHandler::on_eos_from_source;
			gstCallbacks.new_preroll = &AudioAppSinkHandler::on_new_preroll_from_source;
#if GST_VERSION_MAJOR==0
			gstCallbacks.new_buffer = &AudioAppSinkHandler::on_new_buffer_from_source;
#else
			gstCallbacks.new_sample = &AudioAppSinkHandler::on_new_buffer_from_source;
#endif
			gst_app_sink_set_callbacks(GST_APP_SINK(m_audioSink), &gstCallbacks, &m_audioHandler, NULL);
			//gst_app_sink_set_emit_signals(GST_APP_SINK(m_videoSink), true);


			// 		gst_base_sink_set_async_enabled(GST_BASE_SINK(m_videoSink), TRUE);
			gst_base_sink_set_sync(GST_BASE_SINK(m_audioSink), false);
			gst_app_sink_set_drop(GST_APP_SINK(m_audioSink), TRUE);
			gst_app_sink_set_max_buffers(GST_APP_SINK(m_audioSink), 8);
			gst_base_sink_set_max_lateness(GST_BASE_SINK(m_audioSink), 0);
		}

		LogMessage("NetworkAudioPlayer:CreateStream() - Pipeline created", ELL_INFO);

		return CreatePipeline(false,m_ipAddr,m_clockPort);

	}
	uint GetAudioPort()
	{
		if (m_audioSrc)
		{
			int port;
			g_object_get(m_audioSrc, "port", &port, 0);
			return port;
		}
		else 
			return m_audioPort;
	}

	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		m_audioHandler.Close();
		/*
		if (m_audioSrc && m_audioSrc->m_client)
			m_audioSrc->m_client->Close();*/

		GstPipelineHandler::Close();
		LogMessage("NetworkAudioPlayer:Close() - Connection closed", ELL_INFO);
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

	virtual void UseCustomAudioInterface(bool use)
	{
		m_customAudioInterface = use;
	}
	virtual bool IsUsingCustomAudioInterface()
	{
		return m_customAudioInterface;
	}
	int GetPort()
	{
		if (m_audioSrc)
		{
			int port;
			g_object_get(m_audioSrc, "port", &port, 0);
			return port;
		}
		else 
			return m_audioPort;
	}

	//////////////////////////////////////////////////////////////////////////

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

	void SetSampleRate(int Rate)
	{
		m_sampleRate = Rate;
	}
	int GetSampleRate()
	{
		return m_audioHandler.SampleRate();
	}

	bool GrabFrame()
	{
		if (!m_customAudioInterface)
			return false;
		return m_audioHandler.GrabFrame();
	}
	int GetFrameSize()
	{
		if (!m_customAudioInterface)
			return 0;
		return m_audioHandler.GetFrameSize();
	}
	bool CopyAudioFrame(float* output)
	{
		if (!m_customAudioInterface)
			return false;
		m_audioHandler.CopyAudioFrame(output);
		return true;
	}
	int ChannelsCount()
	{
		if (!m_customAudioInterface)
			return 0;
		return m_audioHandler.ChannelsCount();
	}
};


GstNetworkAudioPlayer::GstNetworkAudioPlayer()
{
	m_impl = new GstNetworkAudioPlayerImpl();
}

GstNetworkAudioPlayer::~GstNetworkAudioPlayer()
{
	delete m_impl;
}
void GstNetworkAudioPlayer::SetIPAddress(const std::string& ip, uint audioPort,uint clockPort,bool rtcp)
{
	m_impl->SetIPAddress(ip, audioPort, clockPort, rtcp);
}
bool GstNetworkAudioPlayer::CreateStream()
{
	return m_impl->CreateStream();
}
uint GstNetworkAudioPlayer::GetAudioPort()
{
	return m_impl->GetAudioPort();
}

void GstNetworkAudioPlayer::SetVolume(float vol)
{
	m_impl->SetVolume(vol);
}
bool GstNetworkAudioPlayer::IsStream()
{
	return m_impl->IsStream();
}
GstPipelineHandler*GstNetworkAudioPlayer::GetPipeline()
{
	return m_impl;
}


void GstNetworkAudioPlayer::Play()
{
	m_impl->Play();
}
void GstNetworkAudioPlayer::Stop()
{
	m_impl->Stop();
}
void GstNetworkAudioPlayer::Pause()
{
	m_impl->Pause();
}
bool GstNetworkAudioPlayer::IsLoaded()
{
	return m_impl->IsLoaded();
}
bool GstNetworkAudioPlayer::IsPlaying()
{
	return m_impl->IsPlaying();
}
void GstNetworkAudioPlayer::Close()
{
	m_impl->Close();

}

void GstNetworkAudioPlayer::SetSampleRate(int Rate)
{
	m_impl->SetSampleRate(Rate);
}
int GstNetworkAudioPlayer::GetSampleRate()
{
	return m_impl->GetSampleRate();
}
void GstNetworkAudioPlayer::UseCustomAudioInterface(bool use)
{
	m_impl->UseCustomAudioInterface(use);
}
bool GstNetworkAudioPlayer::IsUsingCustomAudioInterface()
{
	return m_impl->IsUsingCustomAudioInterface();
}

bool GstNetworkAudioPlayer::GrabFrame(int i)
{
	return m_impl->GrabFrame();
}
int GstNetworkAudioPlayer::GetFrameSize()
{
	return m_impl->GetFrameSize();
}
bool GstNetworkAudioPlayer::CopyAudioFrame(float* output)
{
	return m_impl->CopyAudioFrame(output);
}
int GstNetworkAudioPlayer::ChannelsCount()
{
	return m_impl->ChannelsCount();
}
int GstNetworkAudioPlayer::GetPort(int i){
	return m_impl->GetPort();
}


}
}


