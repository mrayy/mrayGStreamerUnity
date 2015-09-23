

#include "stdafx.h"
#include "GstNetworkAudioPlayer.h"


#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"

#include "CMySrc.h"
#include "CMySink.h"
#include "GstPipelineHandler.h"

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

	GstMyUDPSrc* m_audioSrc;
	GstMyUDPSrc* m_audioRtcpSrc;
	GstMyUDPSink* m_audioRtcpSink;
	bool m_rtcp;

public:
	GstNetworkAudioPlayerImpl()
	{
		m_ipAddr = "127.0.0.1";
		m_audioPort = 5005;
		m_audioSrc = 0;
		m_clockPort = 5010;
		m_audioRtcpSrc = 0;
		m_audioRtcpSink = 0;
	}
	virtual ~GstNetworkAudioPlayerImpl()
	{

	}

#define VORBIS_ENC
	void _BuildPipeline()
	{
#ifdef FLAC_ENC
		core::string audiocaps = " caps=application/x-rtp,media=application,clock-rate=90000,encoding-name=X-GST,payload=96 ";
		core::string audioStr = " rtpgstdepay ! audio/x-flac,rate=8000,channels=1! flacdec !  audioconvert ! audioresample  ";
#else 
#ifdef VORBIS_ENC

		std::string audiocaps = "caps=application/x-rtp,media=(string)audio,clock-rate=22000,encoding-name=VORBIS,payload=96,encoding-params=2 ";
		/*
		if (false)
		{
			OS::IStreamPtr capsFile = gFileSystem.openFile("audio.caps");
			if (capsFile)
			{
				OS::StreamReader rdr(capsFile);
				audiocaps = "caps=";

				while (!capsFile->eof())
				{
					audiocaps += rdr.readLine();
				}

				//audiocaps += "\"";
			}
		}*/

		std::string audioStr = " rtpvorbisdepay ! vorbisdec ! audioconvert ! audioresample  ";

#else
#ifdef SPEEX_ENC
		core::string audiocaps = "caps=application/x-rtp,media=(string)audio,clock-rate=(int)22000,encoding-name=(string)SPEEX ";
		core::string audioStr = " rtpspeexdepay ! speexdec ! audioconvert ! audioresample  ";
#else
		core::string audiocaps = "caps=application/x-rtp,media=(string)audio,clock-rate=(int)8000,encoding-name=(string)AMR,encoding-params=(string)1,octet-align=(string)1 ";
		core::string audioStr = " rtpamrdepay ! amrnbdec  ";
#endif
#endif
#endif
		if (m_rtcp)
		{
			m_pipeLineString =
				"rtpbin name=rtpbin "
				"myudpsrc name=audioSrc "+ audiocaps +
				//audio rtp
				"! rtpbin.recv_rtp_sink_0 "

				"rtpbin. ! " + audioStr+ " ! directsoundsink "

				"myudpsrc name=audioRtcpSrc ! rtpbin.recv_rtcp_sink_0 "

				//audio rtcp
				"rtpbin.send_rtcp_src_0 ! myudpsink name=audioRtcpSink sync=false async=false ";
		}
		else
		{
			m_pipeLineString =
				"myudpsrc name=audioSrc " + audiocaps +"!"+ audioStr  + " !directsoundsink ";

		}

	}
	void _UpdatePorts()
	{
		if (!GetPipeline())
			return;
#define SET_SRC(name,p) m_##name=GST_MyUDPSrc(gst_bin_get_by_name(GST_BIN(GetPipeline()), #name)); if(m_##name){m_##name->SetPort(p);}
#define SET_SINK(name,p) m_##name=GST_MyUDPSink(gst_bin_get_by_name(GST_BIN(GetPipeline()), #name)); if(m_##name){m_##name->SetPort(m_ipAddr,p);}


		SET_SRC(audioSrc, m_audioPort);
		SET_SINK(audioRtcpSink, (m_audioPort + 1));
		SET_SRC(audioRtcpSrc, (m_audioPort + 2));

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
			printf("GstNetworkAudioPlayer: Pipeline error: %s", err->message);
		}
		if (!p)
			return false;

		SetPipeline(p);
		_UpdatePorts();

		return CreatePipeline(false,m_ipAddr,m_clockPort);

	}

	bool IsStream()
	{
		return true;
	}

	virtual void Close()
	{
		if (m_audioSrc && m_audioSrc->m_client)
			m_audioSrc->m_client->Close();
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

}
}


