

#include "stdafx.h"
#include "GstNetworkAudioStreamer.h"

#include "GstPipelineHandler.h"

#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"
#include "UnityHelpers.h"
#include <sstream>
#include <vector>





namespace mray
{
namespace video
{



class GstNetworkAudioStreamerImpl :public GstPipelineHandler
{
protected:
	std::string m_ipAddr;

	std::string m_pipeLineString;

	int m_channels;

	struct ClientInfo
	{
		ClientInfo(){
			audioSink = 0;
			volume = 0;
		}
		std::string address;
		uint audioPort;
		GstElement* audioSink;
		GstElement* volume;
	};

	bool _isCreated;

	std::vector<ClientInfo> m_clients;

public:
	GstNetworkAudioStreamerImpl()
	{
		m_channels = 2;
		m_ipAddr = "127.0.0.1";

		_isCreated = false;
	}
	virtual ~GstNetworkAudioStreamerImpl()
	{
	}
	void BuildString()
	{
		//actual-buffer-time=0 actual-latency-time=0
		char buffer[128];
#define OPUS_ENC

		sprintf(buffer, "%d", m_channels);//,rate=32000
		std::string audioStr = "directsoundsrc buffer-time=100  ! audio/x-raw,endianness=1234,signed=true,width=16,depth=16,channels=" + std::string(buffer) + "  ! audioconvert ! audioresample";// !tee name = t ";

		for (int i = 0; i < m_clients.size() && i<1; ++i)
		{
			sprintf(buffer, "%d", i);
			//audioStr+= " t. ! queue ! volume  volume=1 name=vol"+std::string(buffer);
			audioStr += " ! volume  volume=1 name=vol" + std::string(buffer) + " ! "; 
#ifdef VORBIS_ENC
			audioStr +="vorbisenc quality=1 ! rtpvorbispay config-interval=3  " ;
#elif defined OPUS_ENC
			audioStr += "opusenc complexity=10 bitrate-type=vbr frame-size=5 ! rtpopuspay  ";
#endif
			audioStr += "! udpsink name=audioSink" + std::string(buffer) + " sync=false";

			break;
		}
		m_pipeLineString = audioStr ;

	}
	void _UpdatePorts()
	{

		if (!GetPipeline())
			return;
//#define SET_SINK(target,name,p) target=GST_MyUDPSink(gst_bin_get_by_name(GST_BIN(GetPipeline()), name)); if(target){target->SetPort(p.address,p.audioPort);}

		char buffer[128];
		for (int i = 0; i < m_clients.size(); ++i)
		{
			sprintf(buffer, "audioSink%d", i);
			m_clients[i].audioSink = gst_bin_get_by_name(GST_BIN(GetPipeline()), buffer);
			if (m_clients[i].audioSink)
				g_object_set(m_clients[i].audioSink, "port", m_clients[i].audioPort, "host", m_clients[i].address.c_str(),0);

			sprintf(buffer, "vol%d", i);
			m_clients[i].volume = gst_bin_get_by_name(GST_BIN(GetPipeline()), buffer);
		}

	}


	void AddClient(const std::string& addr, uint port)
	{
		ClientInfo cifo;
		cifo.address = addr;
		cifo.audioPort = port;

		m_clients.push_back(cifo);

		if (_isCreated)
		{
			Close();
			CreateStream();
		}
	}
	void RemoveClient(int i)
	{
		if (m_clients.size() < i)return;
		m_clients.erase(m_clients.begin() + i);

		if (_isCreated)
		{
			Close();
			CreateStream();
		}
	}
	void ClearClients()
	{
		m_clients.clear();
		if (_isCreated)
		{
			Close();
			CreateStream();
		}
	}
	int GetClientCount()
	{
		return m_clients.size();
	}
	std::string GetClientAddress(int i)
	{
		if (i >= m_clients.size())
			return "";
		return m_clients[i].address;;
	}
	void SetChannels(int channels)
	{
		m_channels = channels;
	}

	bool _CreateStream(bool isInternal)
	{
		_isCreated = !isInternal ;
		GError *err = 0;
		BuildString();
		GstElement* p = gst_parse_launch(m_pipeLineString.c_str(), &err);
		if (err)
		{
			LogMessage("GstAudioNetworkStreamer: Pipeline error: "+std::string( err->message),ELL_WARNING);
		}
		if (!p)
			return false;
		SetPipeline(p);
		_UpdatePorts();

		return CreatePipeline(true, "", 0);

	}

	bool CreateStream()
	{
		return _CreateStream(false);
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
		_isCreated = false;
	}

	void SetVolume(int i,float v)
	{
		if (i >= m_clients.size())
			return;
		if (v < 0)v = 0;
		if (v>2)v = 2;

		if (m_clients[i].volume)
		{
			g_object_set(m_clients[i].volume, "volume", (gdouble)v, (void*)0);
			
		}
	}

};


GstNetworkAudioStreamer::GstNetworkAudioStreamer()
{
	m_impl = new GstNetworkAudioStreamerImpl();
}

GstNetworkAudioStreamer::~GstNetworkAudioStreamer()
{
	delete m_impl;
}
void GstNetworkAudioStreamer::Stream()
{
	m_impl->Stream();
}
void GstNetworkAudioStreamer::Stop()
{
	m_impl->Stop();
}
GstPipelineHandler* GstNetworkAudioStreamer::GetPipeline()
{
	return m_impl;
}	

void GstNetworkAudioStreamer::AddClient(const std::string& addr, uint port, uint clockPort, bool rtcp)
{
	m_impl->AddClient(addr,port);
}
void GstNetworkAudioStreamer::RemoveClient(int i)
{
	m_impl->RemoveClient(i);
}
void GstNetworkAudioStreamer::ClearClients()
{
	 m_impl->ClearClients();

}
int GstNetworkAudioStreamer::GetClientCount()
{
	return m_impl->GetClientCount();
}
std::string GstNetworkAudioStreamer::GetClientAddress(int i)
{
	return m_impl->GetClientAddress(i);
}
bool GstNetworkAudioStreamer::CreateStream()
{
	return m_impl->CreateStream();
}

void GstNetworkAudioStreamer::Close()
{
	m_impl->Close();
}
void GstNetworkAudioStreamer::SetChannels(int channels)
{
	m_impl->SetChannels(channels);
}

bool GstNetworkAudioStreamer::IsStreaming()
{
	return m_impl->IsStreaming();
}

void GstNetworkAudioStreamer::SetPaused(bool paused)
{
	m_impl->SetPaused(paused);
}

bool GstNetworkAudioStreamer::IsPaused()
{
	return !m_impl->IsPlaying();
}

void GstNetworkAudioStreamer::SetVolume(int i, float v)
{
	return m_impl->SetVolume(i, v);
}

}
}
