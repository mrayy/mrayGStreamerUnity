


#pragma once

#include "UnityPlugin.h"
#include "GstNetworkVideoPlayer.h"
#include "GstCustomVideoPlayer.h"


using namespace mray;
using namespace video;

extern "C" EXPORT_API void* mray_gst_createNetworkPlayer();
extern "C" EXPORT_API void mray_gst_netPlayerSetIP(GstNetworkVideoPlayer* p, const char* ip, int videoPort, bool rtcp);
extern "C" EXPORT_API bool mray_gst_netPlayerCreateStream(GstNetworkVideoPlayer* p);
extern "C" EXPORT_API void mray_gst_netPlayerGetFrameSize(GstNetworkVideoPlayer* p, int &w, int &h);
extern "C" EXPORT_API bool mray_gst_netPlayerGrabFrame(GstNetworkVideoPlayer* p, int &w, int &h);
extern "C" EXPORT_API void mray_gst_netPlayerBlitImage(GstNetworkVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight);



extern "C" EXPORT_API void* mray_gst_createCustomVideoPlayer();
extern "C" EXPORT_API void mray_gst_customPlayerSetPipeline(GstCustomVideoPlayer* p, const char* pipeline);
extern "C" EXPORT_API bool mray_gst_customPlayerCreateStream(GstCustomVideoPlayer* p);
extern "C" EXPORT_API void mray_gst_customPlayerGetFrameSize(GstCustomVideoPlayer* p, int &w, int &h);
extern "C" EXPORT_API bool mray_gst_customPlayerGrabFrame(GstCustomVideoPlayer* p, int &w, int &h);
extern "C" EXPORT_API void mray_gst_customPlayerBlitImage(GstCustomVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight);



extern "C" EXPORT_API void mray_gst_PlayerDestroy(IGStreamerPlayer* p);

extern "C" EXPORT_API void mray_gst_PlayerPlay(IGStreamerPlayer* p);
extern "C" EXPORT_API void mray_gst_PlayerPause(IGStreamerPlayer* p);
extern "C" EXPORT_API void mray_gst_PlayerStop(IGStreamerPlayer* p);
extern "C" EXPORT_API bool mray_gst_PlayerIsLoaded(IGStreamerPlayer* p);
extern "C" EXPORT_API bool mray_gst_PlayerIsPlaying(IGStreamerPlayer* p);
extern "C" EXPORT_API void mray_gst_PlayerClose(IGStreamerPlayer* p);
