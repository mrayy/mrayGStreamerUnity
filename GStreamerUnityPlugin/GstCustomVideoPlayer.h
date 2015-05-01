

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

	//defined by the source video stream
	virtual void SetFrameSize(int w, int h) {}
	virtual const Vector2d& GetFrameSize();

	//defined by the source video stream
	virtual void SetImageFormat(video::EPixelFormat fmt){}
	virtual video::EPixelFormat GetImageFormat();

	virtual bool GrabFrame();
	virtual bool HasNewFrame();
	virtual ulong GetBufferID();// incremented once per frame

	virtual const ImageInfo* GetLastFrame();

};

}
}


#endif
