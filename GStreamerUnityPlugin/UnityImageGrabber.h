

#ifndef __UNITYIMAGEGRABBER__
#define __UNITYIMAGEGRABBER__

#include "IVideoGrabber.h"

namespace mray
{
namespace video
{
	
class UnityImageGrabber:public video::IVideoGrabber
{
protected:

public:
	UnityImageGrabber();
	virtual ~UnityImageGrabber();

	virtual void SetFrameSize(int w, int h) ;
	virtual const Vector2d& GetFrameSize() ;

	virtual void SetImageFormat(video::EPixelFormat fmt) ;
	virtual video::EPixelFormat GetImageFormat() ;

	virtual bool GrabFrame() ;
	virtual bool HasNewFrame() ;
	virtual ulong GetBufferID() ;// incremented once per frame


	virtual const ImageInfo* GetLastFrame();

	void SetData(void* ptr, int w, int h);

	virtual void Lock(){}
	virtual void Unlock(){}
};

}
}


#endif
