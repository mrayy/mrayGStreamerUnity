

#ifndef GstAppAudioPlayer_h__
#define GstAppAudioPlayer_h__



#include "IGStreamerPlayer.h"
#include "mTypes.h"

namespace mray
{
namespace video
{
	class IAudioGrabber;

class GstAppAudioPlayerImpl;

class GstAppAudioPlayer :public IGStreamerPlayer
{
protected:
	GstAppAudioPlayerImpl* m_impl;
	GstPipelineHandler* GetPipeline();
public:
	GstAppAudioPlayer();
	virtual ~GstAppAudioPlayer();

	
	void SetAudioGrabber(IAudioGrabber* g);
	void Init(int audioInterface,int samplingRate);
	bool CreateStream();

	virtual bool IsStream();

	void SetVolume(float vol);

	virtual void Play();
	virtual void Pause();
	virtual void Stop();
	virtual bool IsLoaded();
	virtual bool IsPlaying();
	virtual void Close();
	virtual bool GrabFrame(int i = 0) { return false; }
	virtual const ImageInfo* GetLastFrame(int i = 0){ return 0; }
	virtual unsigned long GetLastFrameTimestamp(int i = 0){ return 0; }
	virtual void* GetLastFrameRTPMeta(int i = 0){return 0;}

	virtual bool SetPosition(signed long pos) { return false; }
	virtual signed long  GetPosition(){ return -1; }
	virtual signed long GetDuration() { return -1; }


};

}
}
#endif // GstAppAudioPlayer_h__
