




/********************************************************************
	created:	2013/12/05
	created:	5:12:2013   20:26
	filename: 	C:\Development\mrayEngine\Plugins\mrayGStreamer\GstNetworkVideoStreamer
	file base:	GstNetworkVideoStreamer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef GstNetworkVideoStreamer_h__
#define GstNetworkVideoStreamer_h__

#include "IGStreamerStreamer.h"

namespace mray
{
namespace video
{

class GstNetworkVideoStreamerImpl;
class IVideoGrabber;

class GstNetworkVideoStreamer :public IGStreamerStreamer
{
protected:
	GstNetworkVideoStreamerImpl* m_impl;

	friend class GstNetworkVideoStreamerImpl;
	GstPipelineHandler* GetPipeline();
public:
	GstNetworkVideoStreamer();
	virtual ~GstNetworkVideoStreamer();

	// addr: target address to stream video to
	// videoport: port for the video stream, video rtcp is allocated as videoPort+1/videoPort+2
	void BindPorts(const std::string& addr, uint videoPort,uint clockPort, bool rtcp);
	void SetBitRate(int bitRate, int quality);
	void SetResolution(int width, int height,int fps);

	void SetEncoderSettings(const std::string& param, const std::string& value);

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

#endif // GstNetworkVideoStreamer_h__
