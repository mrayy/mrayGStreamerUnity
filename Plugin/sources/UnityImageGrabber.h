

#ifndef __UNITYIMAGEGRABBER__
#define __UNITYIMAGEGRABBER__

#include "IVideoGrabber.h"
#include "IMutex.h"

namespace mray
{
namespace video
{
	
class UnityImageGrabber:public video::IVideoGrabber
{
protected:
	OS::IMutex* m_mutex;
	ImageInfo m_backBuffer;
	ImageInfo m_imageInfo;
	bool m_hasNewFrame;
	ulong m_bufferID;
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

	void SetData(void* ptr, int w, int h, int FORMAT);
	void SetImageInfo(ImageInfo* ifo);

	virtual void Lock(){ m_mutex->lock(); }
	virtual void Unlock(){ m_mutex->unlock(); }
};

}
}


#endif
