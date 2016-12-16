
#pragma once

#include "GStreamerCore.h"
#include "Unity/IUnityInterface.h"
#include "ImageInfo.h"

using namespace mray;



extern "C" UNITY_INTERFACE_EXPORT bool mray_gstreamer_initialize();

extern "C" UNITY_INTERFACE_EXPORT void mray_gstreamer_shutdown();

extern "C" UNITY_INTERFACE_EXPORT bool mray_gstreamer_isActive();


extern "C" UNITY_INTERFACE_EXPORT video::ImageInfo* mray_createImageData(int width,int height,video::EPixelFormat format);
extern "C" UNITY_INTERFACE_EXPORT void mray_resizeImageData(video::ImageInfo* ifo,int width,int height,video::EPixelFormat format);
extern "C" UNITY_INTERFACE_EXPORT void mray_getImageDataInfo(video::ImageInfo* ifo,int& width,int& height,video::EPixelFormat&  format);
extern "C" UNITY_INTERFACE_EXPORT void mray_deleteImageData(video::ImageInfo* ifo);
