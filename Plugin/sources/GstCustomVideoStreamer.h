






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
#include <string>

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

	void SetPipeline(const std::string& pipeline);
	bool CreateStream();

	void Stream();
	bool IsStreaming();
	void Stop();
	virtual void Close();

	void SetVideoGrabber(IVideoGrabber* grabber0);
	void  SetResolution(int width, int height, int fps);

	virtual void SetPaused(bool paused);
	virtual bool IsPaused();
};

}
}

#endif // GstCustomVideoStreamer_h__

