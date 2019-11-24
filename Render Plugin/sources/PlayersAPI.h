


#pragma once

#include "Unity/IUnityInterface.h"
#include "ImageInfo.h"

using namespace mray;

extern "C" UNITY_INTERFACE_EXPORT UnityRenderNative mray_gst_BlitImageNativeGLCall(video::ImageInfo* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight);

