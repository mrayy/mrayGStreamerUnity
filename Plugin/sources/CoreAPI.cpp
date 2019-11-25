

#include "stdafx.h"
#include "CoreAPI.h"
#include "UnityHelpers.h"

#ifdef WIN32
#include "Win32/WinThreadManager.h"
#include "Win32/Win32Network.h"
#else

#ifdef __APPLE__
#include "OSX/OSXThreadManager.h"
#else


#ifdef __ANDROID__
#include "Android/AndroidThreadManager.h"
#endif

#endif
#endif
#include <vector>

#include "UnityGraphicsDevice.h"



EXPORT_CDECL UNITY_INTERFACE_EXPORT bool mray_gstreamer_initialize()
{
	LogMessage(ELL_INFO,"mray_gstreamer_initialize");
	if (video::GStreamerCore::RefCount() == 0)
	{
		LogMessage(ELL_INFO, "Initializing GStreamer Engine");
#ifdef WIN32
		new OS::WinThreadManager();
		new network::Win32Network();
#else 
#ifdef __APPLE__
        new OS::OSXThreadManager();
#else
#ifdef __ANDROID__
		new OS::AndroidThreadManager();


#endif

#endif
#endif
		LogMessage(ELL_INFO,"Initializing GStreamer Engine - Done");
	}
	video::GStreamerCore::Ref();
	return true;
}

EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_gstreamer_shutdown()
{
	video::GStreamerCore::Unref();
	if (video::GStreamerCore::RefCount() == 0)
    {
#ifdef WIN32

		delete OS::WinThreadManager::getInstancePtr();
		delete network::Win32Network::getInstancePtr();
#else
#ifdef __APPLE__
		delete OS::OSXThreadManager::getInstancePtr();
#else
#ifdef __ANDROID__
		delete OS::AndroidThreadManager::getInstancePtr();


#endif
#endif
#endif
	}

}

EXPORT_CDECL UNITY_INTERFACE_EXPORT bool mray_gstreamer_isActive()
{
	LogMessage(ELL_INFO, "Checking is active");
	return video::GStreamerCore::Instance()!=0;

}

/////////////////

EXPORT_CDECL UNITY_INTERFACE_EXPORT video::ImageInfo* mray_createImageData(int width,int height,video::EPixelFormat format)
{
    video::ImageInfo* ifo=new video::ImageInfo();
    ifo->createData(Vector2d(width,height), format);
    return ifo;
}
EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_resizeImageData(video::ImageInfo* ifo,int width,int height,video::EPixelFormat format)
{
    ifo->createData(Vector2d(width,height), format);
}
EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_getImageDataInfo(video::ImageInfo* ifo,int& width,int& height,video::EPixelFormat&  format)
{
    width=ifo->Size.x;
    height=ifo->Size.y;
    format=ifo->format;
}

EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_cloneImageData(video::ImageInfo* ifo,video::ImageInfo* dst)
{
    dst->copyFrom(ifo);
}
EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_copyCroppedImageData(video::ImageInfo* ifo, video::ImageInfo* dst, int x, int y, int width, int height, bool clamp)
{
	dst->copyCroppedFrom(ifo, Vector2d(x, y), Vector2d(width, height), clamp, dst->format);
}
EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_deleteImageData(video::ImageInfo* ifo)
{
    if(ifo!=NULL)
        delete ifo;
    ifo=NULL;
}


EXPORT_CDECL UNITY_INTERFACE_EXPORT void* mray_getImageDataPtr(video::ImageInfo* ifo)
{
    
    if (ifo == NULL)
        return 0;
    return ifo->imageData;
}

EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_FlipImageData(video::ImageInfo* ifo,bool horizontal,bool vertical)
{
    if (ifo == NULL)
        return;
    ifo->FlipImage(horizontal, vertical);
    
}
