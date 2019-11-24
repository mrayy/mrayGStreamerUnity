
#pragma once

#include "GStreamerCore.h"
#include "Unity/IUnityInterface.h"
#include "ImageInfo.h"

using namespace mray;



EXPORT_CDECL UNITY_INTERFACE_EXPORT bool mray_gstreamer_initialize();

EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_gstreamer_shutdown();

EXPORT_CDECL UNITY_INTERFACE_EXPORT bool mray_gstreamer_isActive();


EXPORT_CDECL UNITY_INTERFACE_EXPORT video::ImageInfo* mray_createImageData(int width,int height,video::EPixelFormat format);
EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_resizeImageData(video::ImageInfo* ifo,int width,int height,video::EPixelFormat format);
EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_getImageDataInfo(video::ImageInfo* ifo,int& width,int& height,video::EPixelFormat&  format);
EXPORT_CDECL UNITY_INTERFACE_EXPORT void* mray_getImageDataPtr(video::ImageInfo* ifo);

EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_cloneImageData(video::ImageInfo* ifo, video::ImageInfo* dst);
EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_copyCroppedImageData(video::ImageInfo* ifo, video::ImageInfo* dst, int x, int y, int width, int height, bool clamp);
EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_deleteImageData(video::ImageInfo* ifo);
EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_BlitImageDataInfo(video::ImageInfo* ifo,void* _TextureNativePtr);
EXPORT_CDECL UNITY_INTERFACE_EXPORT void mray_FlipImageData(video::ImageInfo* ifo,bool horizontal,bool vertical);
