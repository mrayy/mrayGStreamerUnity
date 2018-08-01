

#include "stdafx.h"
#include "NetworkAudioGrabber.h"

#include "DirectSoundInputStream.h"
#include "GstPipelineHandler.h"

#include <sstream>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

namespace mray
{
namespace video
{

	class NetworkAudioGrabberData:public IPipelineListener
	{
	public:


		uint m_audioPort;
		GstElement* m_audioSrc;

		NetworkAudioGrabberData()
		{
			m_audioSrc = 0;
			m_audioPort = 5005;
		}

		void UpdatePorts()
		{

			if (m_audioSrc)
				g_object_set(m_audioSrc, "port", m_audioPort, 0);
		}

		virtual void OnPipelineLink(GstPipelineHandler* p)
		{
			m_audioSrc = gst_bin_get_by_name(GST_BIN(p->GetPipeline()), "audioSrc");
			UpdatePorts();
		}
		virtual void OnPipelineClosed(GstPipelineHandler* p)
		{
			m_audioSrc = 0;
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

	};
NetworkAudioGrabber::NetworkAudioGrabber()
{
	m_data = new NetworkAudioGrabberData();
	GetPipelineHandler()->AddListener(m_data);
}
NetworkAudioGrabber::~NetworkAudioGrabber()
{
}

void NetworkAudioGrabber::Init( uint audioPort, int channels, int samplingrate)
{
	std::string audiocaps = "caps=application/x-rtp ";// ",media=(string)audio,clock-rate=48000,encoding-name=OPUS,payload=96,encoding-params=2 ";
	
	std::string audioStr = "  queue ! rtpopusdepay ! opusdec plc=true ! audioconvert ";

	std::stringstream pipeline;
	 pipeline << "udpsrc name=audioSrc port=" << (audioPort) << " " << audiocaps << "!" << audioStr;//
	
	m_data->m_audioPort = audioPort;
	CustomAudioGrabber::Init(pipeline.str(), channels, samplingrate);
}

void NetworkAudioGrabber::SetPort( uint audioPort)
{
	m_data->m_audioPort = audioPort;
	m_data->UpdatePorts();
}
uint NetworkAudioGrabber::GetAudioPort()
{
	return m_data->GetAudioPort();
}

}
}
