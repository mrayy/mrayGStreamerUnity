

/********************************************************************
	created:	2012/07/27
	created:	27:7:2012   13:54
	filename: 	d:\Development\mrayEngine\Engine\mrayEngine\include\IVideoGrabber.h
	file path:	d:\Development\mrayEngine\Engine\mrayEngine\include
	file base:	IVideoGrabber
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___IVideoGrabber___
#define ___IVideoGrabber___

#include "ImageInfo.h"

namespace mray
{
namespace video
{

class IVideoGrabber
{
protected:

public:
	IVideoGrabber(){}
	virtual~IVideoGrabber(){}

	virtual void SetFrameSize(int w,int h)=0;
	virtual const Vector2d& GetFrameSize()=0;

	virtual void SetImageFormat(video::EPixelFormat fmt)=0;
	virtual video::EPixelFormat GetImageFormat()=0;

	virtual bool GrabFrame()=0;
	virtual bool HasNewFrame()=0;
	virtual ulong GetBufferID() = 0;// incremented once per frame


	virtual const ImageInfo* GetLastFrame()=0;

	virtual void Lock(){}
	virtual void Unlock(){}
	
};

}
}

#endif
