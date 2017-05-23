


#pragma once

#include "Unity/IUnityInterface.h"
#ifdef USE_UNITY_GRABBER
#include "UnityImageGrabber.h"
#endif
#include "GstCustomVideoStreamer.h"
#include "GstNetworkAudioStreamer.h"



using namespace mray;
using namespace video;

#ifdef USE_UNITY_GRABBER
extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createUnityImageGrabber();
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_UnityImageGrabberSetData(UnityImageGrabber* g, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight,int Format);
#endif

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_StreamerDestroy(IGStreamerStreamer* p);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_StreamerStream(IGStreamerStreamer* p);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_StreamerPause(IGStreamerStreamer* p);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_StreamerStop(IGStreamerStreamer* p);
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_StreamerIsStreaming(IGStreamerStreamer* p);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_StreamerClose(IGStreamerStreamer* p);


#ifdef USE_UNITY_NETWORK
extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createNetworkStreamer();
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netStreamerSetIP(GstCustomVideoStreamer* p, const char* ip, int videoPort, bool rtcp);
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_netStreamerCreateStream(GstCustomVideoStreamer* p);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netStreamerSetGrabber(GstCustomVideoStreamer* p, UnityImageGrabber* g);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netStreamerSetBitRate(GstCustomVideoStreamer* p, int bitRate);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netStreamerSetResolution(GstCustomVideoStreamer* p, int width, int height, int fps);


extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createAudioNetworkStreamer();
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_audioStreamerAddClient(GstNetworkAudioStreamer* p, const char* ip, int port);
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_audioStreamerGetClientCount(GstNetworkAudioStreamer* p);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_audioStreamerRemoveClient(GstNetworkAudioStreamer* p, int i);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_audioStreamerClearClients(GstNetworkAudioStreamer* p);
extern "C" UNITY_INTERFACE_EXPORT const char* mray_gst_audioStreamerGetClient(GstNetworkAudioStreamer* p, int i);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_audioStreamerSetClientVolume(GstNetworkAudioStreamer* p, int i, float vol);
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_audioStreamerSetChannels(GstNetworkAudioStreamer* p, int c);

#endif
