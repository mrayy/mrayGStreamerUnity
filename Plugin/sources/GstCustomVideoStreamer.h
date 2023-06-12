






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
#include "Unity/IUnityInterface.h"
#include <string>

namespace mray
{
namespace video
{

class GstCustomVideoStreamerImpl;
class IVideoGrabber;

// Uggly fix to expose the ConnectionStatusCallback signature where relevant.
extern "C"
{
	typedef void(UNITY_INTERFACE_API* ConnectionStatusCallback)(bool);
}

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
	void SetResolution(int width, int height, int fps);
	void SetConnectionStatusCallback(ConnectionStatusCallback cb);

	virtual void SetPaused(bool paused);
	virtual bool IsPaused();
};

}
}

#endif // GstCustomVideoStreamer_h__

