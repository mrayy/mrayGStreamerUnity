

#ifndef __GstCustomDataPlayer__
#define __GstCustomDataPlayer__

#include "IGStreamerPlayer.h"
#include "IVideoGrabber.h"

namespace mray
{
namespace video
{

class GstCustomDataPlayerImpl;

class GstCustomDataPlayer :public IGStreamerPlayer
{
protected:
	GstCustomDataPlayerImpl* m_impl;

	GstPipelineHandler* GetPipeline();

public:
	GstCustomDataPlayer();
	virtual ~GstCustomDataPlayer();

	void SetApplicationDataType(const std::string& dataType, bool autotimestamp, int payload = 98);// application/x-"dataType"

	void SetPort(int port);

	bool CreateStream();

	virtual bool IsStream();

	void SetVolume(float vol);

	virtual void Play();
	virtual void Pause();
	virtual void Stop();
	virtual bool IsLoaded();
	virtual bool IsPlaying();
	virtual void Close();

	virtual int GetPort(int i);

	bool GrabFrame(int i=0);
	virtual bool HasNewFrame();
	virtual ulong GetBufferID();// incremented once per frame

	virtual bool SetPosition(signed long pos) { return false; }
	virtual signed long  GetPosition() { return 0; }
	virtual signed long GetDuration() { return 0; }
	virtual const ImageInfo* GetLastFrame(int i = 0) { return 0; }
	virtual unsigned long GetLastFrameTimestamp(int i = 0) { return 0; }
	virtual void* GetLastFrameRTPMeta(int i = 0) { return 0; }
	int GetDataLength();
	int GetData(void* data,int length);

};

}
}


#endif
