


#ifndef GstNetworkMultipleVideoPlayer_h__
#define GstNetworkMultipleVideoPlayer_h__


#include "IGStreamerPlayer.h"
#include "IVideoGrabber.h"

namespace mray
{
namespace video
{

class GstNetworkMultipleVideoPlayerImpl;

extern "C" class UNITY_INTERFACE_EXPORT GstNetworkMultipleVideoPlayer :public IGStreamerPlayer
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

	void SetRecordToFile(std::string filename,int framerate=-1);
	bool GetRecordStarted();

	//add custom elements to pipeline. Should be set before calling CreateStream
	void AddIntermidateElement(const std::string& elems);

	//encoder type: H264 and JPEG
	void SetDecoderType(std::string type);

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

	virtual bool SetPosition(signed long pos) { return false; }
	virtual signed long  GetPosition(){ return -1; }
	virtual signed long GetDuration() { return -1; }

	//defined by the source video stream
	virtual void SetImageFormat(video::EPixelFormat fmt);
	virtual video::EPixelFormat GetImageFormat();

	int GetFramesCount();
	virtual bool GrabFrame(int i);
	virtual bool HasNewFrame(int i);
	virtual ulong GetBufferID(int i);// incremented once per frame
	virtual int GetPort(int i);

	virtual const ImageInfo* GetLastFrame(int i);
	virtual const GstImageFrame* GetLastDataFrame(int i);
	virtual unsigned long GetLastFrameTimestamp(int i);
	virtual void* GetLastFrameRTPMeta(int i);


	virtual ulong GetNetworkUsage();

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
