


#ifndef GstNetworkAudioStreamer_h__
#define GstNetworkAudioStreamer_h__

#include "IGStreamerStreamer.h"

namespace mray
{
namespace video
{

	class GstNetworkAudioStreamerImpl;

	
class GstNetworkAudioStreamer:public IGStreamerStreamer
{
protected:
	GstNetworkAudioStreamerImpl* m_impl;
	GstPipelineHandler* GetPipeline();
public:
	GstNetworkAudioStreamer();
	virtual ~GstNetworkAudioStreamer();

	// addr: target address to stream video to
	// audioport: port for the audio stream , audio rtcp is allocated as audioPort+1 and audioPort+2
	void AddClient(const std::string& addr, uint port, uint clockPort, bool rtcp);
	void ClearClients();
	void RemoveClient(int i);
	int GetClientCount();
	std::string GetClientAddress(int i);

	void SetChannels(int channels);

	bool CreateStream();
	void Stream();
	bool IsStreaming();
	void Stop();
	virtual void Close();

	void SetVolume(int i,float v);

	virtual void SetPaused(bool paused) ;
	virtual bool IsPaused() ;
};

}
}

#endif // GstNetworkAudioStreamer_h__
