

#include "stdafx.h"
#include "UnityImageGrabber.h"
#include "IThreadManager.h"



namespace mray
{
namespace video
{


UnityImageGrabber::UnityImageGrabber()
{
	m_hasNewFrame = false;
	m_bufferID = 0;
    m_mutex=OS::IThreadManager::getInstance().createMutex();
}

UnityImageGrabber::~UnityImageGrabber()
{
    delete m_mutex;
}


void UnityImageGrabber::SetFrameSize(int w, int h)
{
}

const Vector2d& UnityImageGrabber::GetFrameSize()
{
	return m_imageInfo.Size;
}


void UnityImageGrabber::SetImageFormat(video::EPixelFormat fmt)
{
}

video::EPixelFormat UnityImageGrabber::GetImageFormat()
{
	return m_imageInfo.format;
}


bool UnityImageGrabber::GrabFrame()
{
	if (HasNewFrame())
	{
		Lock();
		uchar* ptr = m_imageInfo.imageData;
		m_imageInfo.imageData = m_backBuffer.imageData;
		m_backBuffer.imageData = ptr;
		++m_bufferID;
		m_hasNewFrame = false;
		Unlock();
		return true;
	}
	return false;
}

bool UnityImageGrabber::HasNewFrame()
{
	return m_hasNewFrame;
}

ulong UnityImageGrabber::GetBufferID()
{
	return m_bufferID;
}


const ImageInfo* UnityImageGrabber::GetLastFrame()
{
	return &m_imageInfo;
}


void UnityImageGrabber::SetData(void* ptr, int w, int h,int FORMAT)
{
	Lock();

	m_backBuffer.setData(ptr, Vector2d(w, h), (video::EPixelFormat)FORMAT);
	m_imageInfo.createData(Vector2d(w, h), (video::EPixelFormat)FORMAT);
	m_hasNewFrame = true;
	Unlock();
}

void UnityImageGrabber::SetImageInfo(ImageInfo* ifo)
{
	SetData(ifo->imageData, ifo->Size.x, ifo->Size.y, ifo->format);
}


}
}

