

#ifndef GstNetworkAudioPlayer_h__
#define GstNetworkAudioPlayer_h__



#include "IGStreamerPlayer.h"
#include "mTypes.h"

namespace mray
{
namespace video
{

class GstNetworkAudioPlayerImpl;

class GstNetworkAudioPlayer :public IGStreamerPlayer
{
protected:
	GstNetworkAudioPlayerImpl* m_impl;
	GstPipelineHandler* GetPipeline();
public:
	GstNetworkAudioPlayer();
	virtual ~GstNetworkAudioPlayer();

	//set ip address for the target host
	// audioport: port for the audio stream , audio rtcp is allocated as audioPort+1 and audioPort+2
	void SetIPAddress(const std::string& ip, uint audioPort,uint clockPort, bool rtcp);
	bool CreateStream();

	uint GetAudioPort();

	virtual bool IsStream();

	void SetVolume(float vol);

	virtual void Play();
	virtual void Pause();
	virtual void Stop();
	virtual bool IsLoaded();
	virtual bool IsPlaying();
	virtual void Close();
	virtual const ImageInfo* GetLastFrame(int i = 0){ return 0; }
	virtual unsigned long GetLastFrameTimestamp(int i = 0){ return 0; }
	virtual void* GetLastFrameRTPMeta(int i = 0){return 0;}
	//if it set to false, then the audio will be played using directsound
	virtual void UseCustomAudioInterface(bool use);
	virtual bool IsUsingCustomAudioInterface();

	virtual bool SetPosition(signed long pos) { return false; }
	virtual signed long  GetPosition(){ return -1; }
	virtual signed long GetDuration() { return -1; }

	void SetSampleRate(int Rate);
	int GetSampleRate();
	bool GrabFrame(int i=0);
	int GetFrameSize();
	bool CopyAudioFrame(float* output);
	int ChannelsCount();

	virtual int GetPort(int i);

};

}
}
#endif // GstNetworkAudioPlayer_h__
