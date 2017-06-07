
#include "IGStreamerPlayer.h"
#include "IVideoGrabber.h"


namespace mray
{
namespace video
{

class GstCustomDataReceiverImpl;

extern "C" class UNITY_INTERFACE_EXPORT GstCustomDataReceiver :public IGStreamerPlayer
{
protected:
	GstCustomDataReceiverImpl* m_impl;
	friend class GstCustomDataReceiverImpl;


	GstPipelineHandler* GetPipeline();
public:
	GstCustomDataReceiver();
	virtual ~GstCustomDataReceiver();

	//set ip address for the target host
	// videoport: port for the video stream, video rtcp is allocated as videoPort+1 and videoPort+2
	void SetIPAddress(const std::string& ip, uint baseVideoPort, uint count, uint clockPort, bool rtcp);
	bool CreateStream();

	//add custom elements to pipeline. Should be set before calling CreateStream
	void AddIntermidateElement(const std::string& elems);

	uint GetDataPort(int i);

	virtual bool IsStream();

	void SetVolume(float vol);

	virtual void Play();
	virtual void Pause();
	virtual void Stop();
	virtual bool IsLoaded();
	virtual bool IsPlaying();
	virtual void Close();


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

}
}
