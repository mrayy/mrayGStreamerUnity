




/********************************************************************
	created:	2013/12/05
	created:	5:12:2013   20:26
	filename: 	C:\Development\mrayEngine\Plugins\mrayGStreamer\GstCustomVideoStreamer
	file base:	GstCustomVideoStreamer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef GstCustomVideoStreamer_h__
#define GstCustomVideoStreamer_h__

#include "IGStreamerStreamer.h"

namespace mray
{
namespace video
{

class GstCustomVideoStreamerImpl;
class IVideoGrabber;

class GstCustomVideoStreamer :public IGStreamerStreamer
{
protected:
	GstCustomVideoStreamerImpl* m_impl;

	friend class GstCustomVideoStreamerImpl;
	GstPipelineHandler* GetPipeline();
public:
	GstCustomVideoStreamer();
	virtual ~GstCustomVideoStreamer();

	// addr: target address to stream video to
	// videoport: port for the video stream, video rtcp is allocated as videoPort+1/videoPort+2
	void BindPorts(const std::string& addr, uint videoPort,uint clockPort, bool rtcp);
	void SetBitRate(int bitRate);
	void SetResolution(int width, int height,int fps);

	bool CreateStream();
	void Stream();
	bool IsStreaming();
	void Stop();
	virtual void Close();

	void SetVideoGrabber(IVideoGrabber* grabber0);

	virtual void SetPaused(bool paused);
	virtual bool IsPaused();
};

}
}

#endif // GstCustomVideoStreamer_h__
