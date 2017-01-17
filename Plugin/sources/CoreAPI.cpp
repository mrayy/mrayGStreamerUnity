

#include "stdafx.h"
#include "CoreAPI.h"
#include "UnityHelpers.h"

#ifdef WIN32
#include "Win32/WinThreadManager.h"
#include "Win32/Win32Network.h"
#else
#include "OSX/OSXThreadManager.h"

#endif
#include <vector>

extern "C" UNITY_INTERFACE_EXPORT bool mray_gstreamer_initialize()
{
	LogManager::Instance()->LogMessage("mray_gstreamer_initialize");
	if (video::GStreamerCore::RefCount() == 0)
	{
		LogManager::Instance()->LogMessage("Initializing GStreamer Engine");
#ifdef WIN32
		new OS::WinThreadManager();
		new network::Win32Network();
#else
        new OS::OSXThreadManager();
#endif
		LogManager::Instance()->LogMessage("Initializing GStreamer Engine - Done");
	}
	video::GStreamerCore::Ref();
	return true;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gstreamer_shutdown()
{
	video::GStreamerCore::Unref();
	if (video::GStreamerCore::RefCount() == 0)
    {
#ifdef WIN32

		delete OS::WinThreadManager::getInstancePtr();
		delete network::Win32Network::getInstancePtr();
#else
        delete OS::OSXThreadManager::getInstancePtr();
#endif
	}

}

extern "C" UNITY_INTERFACE_EXPORT bool mray_gstreamer_isActive()
{
	LogMessage("Checking is active", ELL_INFO);
	return video::GStreamerCore::Instance()!=0;

}

/////////////////

extern "C" UNITY_INTERFACE_EXPORT video::ImageInfo* mray_createImageData(int width,int height,video::EPixelFormat format)
{
    video::ImageInfo* ifo=new video::ImageInfo();
    ifo->createData(Vector2d(width,height), format);
    return ifo;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_resizeImageData(video::ImageInfo* ifo,int width,int height,video::EPixelFormat format)
{
    ifo->createData(Vector2d(width,height), format);
}
extern "C" UNITY_INTERFACE_EXPORT void mray_getImageDataInfo(video::ImageInfo* ifo,int& width,int& height,video::EPixelFormat&  format)
{
    width=ifo->Size.x;
    height=ifo->Size.y;
    format=ifo->format;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_cloneImageData(video::ImageInfo* ifo,video::ImageInfo* dst)
{
    dst->copyFrom(ifo);
}
extern "C" UNITY_INTERFACE_EXPORT void mray_deleteImageData(video::ImageInfo* ifo)
{
    if(ifo!=NULL)
        delete ifo;
    ifo=NULL;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_BlitImageDataInfo(video::ImageInfo* ifo,void* _TextureNativePtr)
{
    
    if (ifo == NULL || !_TextureNativePtr)
        return;
    
    if (ifo)
    {
        BlitImage(ifo, _TextureNativePtr, ifo->Size.x,ifo->Size.y);
    }

}


extern "C" UNITY_INTERFACE_EXPORT void mray_FlipImageData(video::ImageInfo* ifo,bool horizontal,bool vertical)
{
    if (ifo == NULL)
        return;
    ifo->FlipImage(horizontal, vertical);
    
}

struct ImageBlitData
{
    video::ImageInfo* ifo;
    void* _TextureNativePtr;
};

std::vector<ImageBlitData> _data;

static void  __stdcall mray_gst_customPlayerBlitImageNativeEvent(int eventID)
{
    if (_data.size() == 0)
        return;
    for(int i=0;i<_data.size();++i)
    {
        ImageBlitData& r=_data[i];
        mray_BlitImageDataInfo(r.ifo, r._TextureNativePtr);
    }
    _data.clear();
}
extern "C" UNITY_INTERFACE_EXPORT UnityRenderNative mray_BlitImageNativeGLCall(video::ImageInfo* ifo,void* _TextureNativePtr)
{
    ImageBlitData r;
    r.ifo = ifo;
    r._TextureNativePtr = _TextureNativePtr;
    _data.push_back(r);
    return mray_gst_customPlayerBlitImageNativeEvent;
}
