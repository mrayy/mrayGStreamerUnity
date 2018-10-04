


#pragma once

#include "Unity/IUnityInterface.h"
#include "GstNetworkVideoPlayer.h"
#include "GstCustomVideoPlayer.h"
#include "GstNetworkMultipleVideoPlayer.h"
#include "GstNetworkAudioPlayer.h"


using namespace mray;
using namespace video;

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_playerBlitImage(IGStreamerPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight, int index);
extern "C" UNITY_INTERFACE_EXPORT UnityRenderNative mray_gst_customPlayerBlitImageNativeGLCall(GstCustomVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_customPlayerBlitImage(GstCustomVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight);

