

/********************************************************************
	created:	2014/02/22
	created:	22:2:2014   17:54
	filename: 	C:\Development\mrayEngine\Plugins\mrayGStreamer\IGStreamerPlayer.h
	file path:	C:\Development\mrayEngine\Plugins\mrayGStreamer
	file base:	IGStreamerPlayer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __IGStreamerPlayer__
#define __IGStreamerPlayer__


#include <string>
#include "IStreamListener.h"
#include "ListenerContainer.h"
#include "UnityHelpers.h"
//#include "CMyUDPSrc.h"

#ifdef __ANDROID__
#include <inttypes.h>
#define  __int64 long long
#endif

namespace mray
{
namespace video
{
	class GstPipelineHandler;


	struct RTPPacketData
	{
		uint32_t timestamp;
		uint64_t presentationTime;
		unsigned short length;
		unsigned char data[255];
	};
	template<class T>
	class GstFrameData
	{
	public:
		T data;
		unsigned __int64 PTS;
		unsigned __int64 DTS;
		RTPPacketData RTPPacket;
	};

	typedef GstFrameData<ImageInfo> GstImageFrame;

class IGStreamerPlayer :public ListenerContainer<IGStreamerPlayerListener*>
{
protected:
	DECLARE_FIRE_METHOD(OnPlayerStarted, (IGStreamerPlayer* s), (s));
	DECLARE_FIRE_METHOD(OnPlayerStopped, (IGStreamerPlayer* s), (s));
	DECLARE_FIRE_METHOD(OnPlayerReady, (IGStreamerPlayer* s), (s));

	virtual GstPipelineHandler* GetPipeline() = 0;
public:
	IGStreamerPlayer(){}
	virtual~IGStreamerPlayer(){}

	virtual bool IsStream() = 0;
	virtual void SetVolume(float vol) = 0;

	virtual void Play() = 0;
	virtual void Pause() = 0;
	virtual void Stop() = 0;
	virtual bool IsLoaded() = 0;
	virtual bool IsPlaying() = 0;
    virtual void Close() = 0;
    virtual bool SetPosition(signed long pos)=0;
    virtual signed long  GetPosition()=0;
    virtual signed long GetDuration()=0;
	virtual bool GrabFrame(int i = 0) = 0;
	virtual const ImageInfo* GetLastFrame(int i = 0) = 0;
	virtual unsigned long GetLastFrameTimestamp(int i = 0) = 0;
	virtual void* GetLastFrameRTPMeta(int i = 0) = 0;
	virtual bool QueryLatency(bool &isLive, ulong& minLatency, ulong& maxLatency);

	virtual void SetClockBase(ulong c);
	virtual ulong GetClockBase();
};

}
}


#endif
