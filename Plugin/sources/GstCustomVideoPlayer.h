

#ifndef __GSTCUSTOMVIDEOPLAYER__
#define __GSTCUSTOMVIDEOPLAYER__

#include "IGStreamerPlayer.h"
#include "IVideoGrabber.h"
#include "UnityHelpers.h"
#include <string>
namespace mray
{

namespace video
{

class GstCustomVideoPlayerImpl;

class GstCustomVideoPlayer :public IGStreamerPlayer
{
protected:
	GstCustomVideoPlayerImpl* m_impl;
	GstPipelineHandler* GetPipeline();

public:
	GstCustomVideoPlayer();
	virtual ~GstCustomVideoPlayer();

	void SetPipelineString(const std::string& pipeline);
	bool CreateStream();

	virtual bool IsStream();

	void SetVolume(float vol);

	virtual void Play();
	virtual void Pause();
	virtual void Stop();
	virtual bool IsLoaded();
	virtual bool IsPlaying();
    virtual void Close();
    virtual bool SetPosition(signed long pos);
    virtual signed long  GetPosition();
    virtual signed long GetDuration();

	//defined by the source video stream
	virtual void SetFrameSize(int w, int h) {}
	virtual const Vector2d& GetFrameSize();

	//defined by the source video stream
	virtual void SetImageFormat(video::EPixelFormat fmt){}
	virtual video::EPixelFormat GetImageFormat();

	virtual bool GrabFrame(int i=0);
	virtual bool GrabAudioFrame();
	virtual bool HasNewFrame();
	virtual ulong GetBufferID();// incremented once per frame

	virtual const ImageInfo* GetLastFrame(int i = 0);
	virtual unsigned long GetLastFrameTimestamp(int i = 0);
	virtual void* GetLastFrameRTPMeta(int i = 0);
	virtual float GetCaptureFrameRate();

	int GetAudioFrameSize();
	bool CopyAudioFrame(float* output);
	int GetAudioChannelsCount();

	virtual int GetPort(int i){ return 0; }

};

}
}


#endif
