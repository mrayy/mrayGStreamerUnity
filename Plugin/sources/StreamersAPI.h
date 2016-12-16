


#pragma once

#include "UnityPlugin.h"
#ifdef USE_UNITY_GRABBER
#include "UnityImageGrabber.h"
#endif
#include "GstCustomVideoStreamer.h"



using namespace mray;
using namespace video;

#ifdef USE_UNITY_GRABBER
extern "C" EXPORT_API void* mray_gst_createUnityImageGrabber();
extern "C" EXPORT_API void mray_gst_UnityImageGrabberSetData(UnityImageGrabber* g, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight,int Format);
#endif

extern "C" EXPORT_API void mray_gst_StreamerDestroy(IGStreamerStreamer* p);
extern "C" EXPORT_API void mray_gst_StreamerStream(IGStreamerStreamer* p);
extern "C" EXPORT_API void mray_gst_StreamerPause(IGStreamerStreamer* p);
extern "C" EXPORT_API void mray_gst_StreamerStop(IGStreamerStreamer* p);
extern "C" EXPORT_API bool mray_gst_StreamerIsStreaming(IGStreamerStreamer* p);
extern "C" EXPORT_API void mray_gst_StreamerClose(IGStreamerStreamer* p);


#ifdef USE_UNITY_NETWORK
extern "C" EXPORT_API void* mray_gst_createNetworkStreamer();
extern "C" EXPORT_API void mray_gst_netStreamerSetIP(GstCustomVideoStreamer* p, const char* ip, int videoPort, bool rtcp);
extern "C" EXPORT_API bool mray_gst_netStreamerCreateStream(GstCustomVideoStreamer* p);
extern "C" EXPORT_API void mray_gst_netStreamerSetGrabber(GstCustomVideoStreamer* p, UnityImageGrabber* g);
extern "C" EXPORT_API void mray_gst_netStreamerSetBitRate(GstCustomVideoStreamer* p, int bitRate);
extern "C" EXPORT_API void mray_gst_netStreamerSetResolution(GstCustomVideoStreamer* p, int width, int height, int fps);
#endif
