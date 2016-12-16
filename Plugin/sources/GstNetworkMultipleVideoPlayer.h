


#ifndef GstNetworkMultipleVideoPlayer_h__
#define GstNetworkMultipleVideoPlayer_h__


#include "IGStreamerPlayer.h"
#include "IVideoGrabber.h"

namespace mray
{
namespace video
{

class GstNetworkMultipleVideoPlayerImpl;

class GstNetworkMultipleVideoPlayer :public IGStreamerPlayer
{
protected:
	GstNetworkMultipleVideoPlayerImpl* m_impl;
	friend class GstNetworkMultipleVideoPlayerImpl;


	GstPipelineHandler* GetPipeline();
public:
	GstNetworkMultipleVideoPlayer();
	virtual ~GstNetworkMultipleVideoPlayer();

	//set ip address for the target host
	// videoport: port for the video stream, video rtcp is allocated as videoPort+1 and videoPort+2
	void SetIPAddress(const std::string& ip, uint baseVideoPort, uint count, uint clockPort, bool rtcp);
	bool CreateStream();

	uint GetVideoPort(int i);

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

	virtual float GetCaptureFrameRate(int i);


	//defined by the source video stream
	virtual void SetImageFormat(video::EPixelFormat fmt){}
	virtual video::EPixelFormat GetImageFormat();

	int GetFramesCount();
	virtual bool GrabFrame(int i);
	virtual bool HasNewFrame(int i);
	virtual ulong GetBufferID(int i);// incremented once per frame

	virtual const ImageInfo* GetLastFrame(int i);

};

class GstNetworkMultipleVideoPlayerGrabber :public IVideoGrabber
{
	GstNetworkMultipleVideoPlayer* m_player;
public:
	GstNetworkMultipleVideoPlayerGrabber(GstNetworkMultipleVideoPlayer * p)
	{
		m_player = p;
	}
	virtual void SetFrameSize(int w, int h) { m_player->SetFrameSize(w, h); }
	virtual const Vector2d& GetFrameSize() { return m_player->GetFrameSize(); }

	virtual void SetImageFormat(video::EPixelFormat fmt)  { m_player->SetImageFormat(fmt); }
	virtual video::EPixelFormat GetImageFormat() { return m_player->GetImageFormat(); }

	virtual int GetFramesCount(){ return m_player->GetFramesCount(); }
	virtual bool GrabFrame(int i) { return m_player->GrabFrame(i); }
	virtual bool HasNewFrame(int i) { return m_player->HasNewFrame(i); }
	virtual ulong GetBufferID(int i) { return m_player->GetBufferID(i); }
	virtual float GetCaptureFrameRate(int i) { return m_player->GetCaptureFrameRate(i); }

	virtual const ImageInfo* GetLastFrame(int i) { return m_player->GetLastFrame(i); }
};

}
}

#endif // GstNetworkMultipleVideoPlayer_h__
