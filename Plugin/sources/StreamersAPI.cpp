

#include "Unity/IUnityInterface.h"

#ifdef USE_UNITY_GRABBER
#include "UnityImageGrabber.h"
#endif

#include "GstCustomVideoStreamer.h"
#include "GstNetworkVideoStreamer.h"
#include "GstNetworkAudioStreamer.h"
#include "GstAppNetAudioStreamer.h"

#include "LocalAudioGrabber.h"
#include "NetworkAudioGrabber.h"
#include "UnityAudioGrabber.h"

#include "GStreamerCore.h"
#include "GraphicsInclude.h"
#include "PixelUtil.h"

using namespace mray;
using namespace video;

#ifdef USE_UNITY_GRABBER
extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createUnityImageGrabber()
{
	GStreamerCore::Ref();
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		UnityImageGrabber* g = new UnityImageGrabber();
		return g;
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_destroyUnityImageGrabber(UnityImageGrabber* g)
{
	if (g)
	{
		delete g;
	}
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_UnityImageGrabberSetData(UnityImageGrabber* g, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight, int Format)
{
	if (!g)
		return;
	g->SetData(_TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight, Format);
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_UnityImageGrabberSetImageInfo(UnityImageGrabber* g, ImageInfo* ifo)
{
	if (!g)
		return;
	g->SetImageInfo(ifo);
}


extern "C" UNITY_INTERFACE_EXPORT void mray_gst_UnityImageGrabberDestroy(UnityImageGrabber* g)
{
	if (!g)
		return;
	delete g;
}

#endif

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_StreamerDestroy(IGStreamerStreamer* p)
{
	if (p != NULL)
	{
		p->Close();
		delete p;
	}

}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_StreamerCreateStream(IGStreamerStreamer* p)
{

	if (p != NULL)
	{
		return p->CreateStream();
	}
	return false;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_StreamerStream(IGStreamerStreamer* p)
{

	if (p != NULL)
	{
		p->Stream();
	}
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_StreamerPause(IGStreamerStreamer* p)
{
	if (p != NULL)
	{
		p->SetPaused(true);
	}
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_StreamerStop(IGStreamerStreamer* p)
{
	if (p != NULL)
	{
		p->Stop();
	}

}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_StreamerIsStreaming(IGStreamerStreamer* p)
{
	if (p != NULL)
	{
		return p->IsStreaming();
	}
	return false;

}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_StreamerClose(IGStreamerStreamer* p)
{
	if (p != NULL)
	{
		return p->Close();
	}

}

extern "C" UNITY_INTERFACE_EXPORT IGStreamerStreamer* mray_gst_createCustomImageStreamer()
{
	return new GstCustomVideoStreamer();

}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_customImageStreamerSetPipeline(GstCustomVideoStreamer* p, const char* pipeline)
{
	if (p != NULL)
	{
		p->SetPipeline(pipeline);
	}

}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_customImageStreamerSetGrabber(GstCustomVideoStreamer* p, UnityImageGrabber* g)
{
	if (p)
	{
		p->SetVideoGrabber(g);
	}

}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_customImageStreamerSetResolution(GstCustomVideoStreamer* p, int width, int height, int fps)
{
	if (p)
	{
		p->SetResolution(width, height, fps);
	}

}


#ifdef USE_UNITY_NETWORK

extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createNetworkStreamer()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstNetworkVideoStreamer* g = new GstNetworkVideoStreamer();
		return g;
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netStreamerSetIP(GstNetworkVideoStreamer* p, const char* ip, int videoPort, bool rtcp)
{
	if (p)
	{
		p->BindPorts(ip, videoPort, 0, rtcp);
	}
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netStreamerSetGrabber(GstNetworkVideoStreamer* p, UnityImageGrabber* g)
{
	if (p)
	{
		p->SetVideoGrabber(g);
	}

}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netStreamerSetBitRate(GstNetworkVideoStreamer* p, int bitRate, int quality)
{
	if (p)
	{
		p->SetBitRate(bitRate,quality);
	}

}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netStreamerSetResolution(GstNetworkVideoStreamer* p, int width, int height, int fps)
{
	if (p)
	{
		p->SetResolution(width,height,fps);
	}

}

extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createAudioNetworkStreamer()
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstNetworkAudioStreamer* g = new GstNetworkAudioStreamer();
		return g;
	}
	return 0;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_audioStreamerAddClient(GstNetworkAudioStreamer* p, const char* ip, int port)
{
	if (p)
	{
		p->AddClient(ip, port, 0, 0);
	}

}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_audioStreamerGetClientCount(GstNetworkAudioStreamer* p)
{
	if (p)
	{
		return p->GetClientCount();
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_audioStreamerRemoveClient(GstNetworkAudioStreamer* p, int i)
{
	if (p)
	{
		return p->RemoveClient(i);
	}
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_audioStreamerClearClients(GstNetworkAudioStreamer* p)
{
	if (p)
	{
		return p->ClearClients();
	}
}
extern "C" UNITY_INTERFACE_EXPORT const char* mray_gst_audioStreamerGetClient(GstNetworkAudioStreamer* p, int i)
{
	if (p)
	{
		return p->GetClientAddress(i).c_str();
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_audioStreamerSetClientVolume(GstNetworkAudioStreamer* p, int i, float vol)
{
	if (p)
	{
		return p->SetVolume(i, vol);
	}
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_audioStreamerSetChannels(GstNetworkAudioStreamer* p, int c)
{
	if (p)
	{
		p->SetChannels(c);
	}

}



extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createAppNetAudioStreamer()
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstAppNetAudioStreamer* g = new GstAppNetAudioStreamer();
		return g;
	}
	return 0;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_AppNetAudioStreamerSetIP(GstAppNetAudioStreamer* p, const char* ip, uint port, bool rtcp)
{
	if (p)
	{
		p->BindPorts(ip, &port,1, rtcp);
	}
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_AppNetAudioStreamerAttachGrabber(GstAppNetAudioStreamer* a,IAudioGrabber* g)
{
	a->SetAudioGrabber(g);
}


//////////////////////////////////////////////////////////////////////////

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_AudioGrabberDestroy(IAudioGrabber* g)
{
	delete g;
}


extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_AudioGrabberStart(IAudioGrabber* g)
{
	return g->Start();
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_AudioGrabberPause(IAudioGrabber* g)
{
	g->Pause();
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_AudioGrabberClose(IAudioGrabber* g)
{
	g->Close();
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_AudioGrabberResume(IAudioGrabber* g)
{
	g->Resume();
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_AudioGrabberRestart(IAudioGrabber* g)
{
	g->Restart();
}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_AudioGrabberIsStarted(IAudioGrabber* g)
{
	return g->IsStarted();
}
extern "C" UNITY_INTERFACE_EXPORT uint mray_gst_AudioGrabberGetSamplingRate(IAudioGrabber* g)
{
	return g->GetSamplingRate();
}
extern "C" UNITY_INTERFACE_EXPORT uint mray_gst_AudioGrabberGetChannelsCount(IAudioGrabber* g)
{
	return g->GetChannelsCount();
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_AudioGrabberSetVolume(IAudioGrabber* g,float vol)
{
	g->SetVolume(vol);
}

extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_AudioGrabberGrabFrame(IAudioGrabber* g)
{
	return g->GrabFrame();
}

extern "C" UNITY_INTERFACE_EXPORT uint mray_gst_AudioGrabberGetAudioFrameSize(IAudioGrabber* g)
{
	return g->GetAudioFrameSize();
}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_AudioGrabberCopyAudioFrame(IAudioGrabber* p, float*data)
{
	float* ptr= p->GetAudioFrame();
	int size = p->GetAudioFrameSize();
	if (!ptr || !data)
		return false;
	memcpy(data, ptr, size*sizeof(float));
	return true;
}


extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createLocalAudioGrabber()
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		LocalAudioGrabber* g = new LocalAudioGrabber();
		return g;
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_LocalAudioGrabberInit(LocalAudioGrabber* g,const char* guid, int channels, int samplingrate)
{

	g->Init(guid, channels, samplingrate);
}

extern "C" UNITY_INTERFACE_EXPORT int mray_gst_GetAudioInputInterfacesCount()
{
	return LocalAudioGrabber::GetInputInterfacesCount();
}

extern "C" UNITY_INTERFACE_EXPORT const char* mray_gst_GetAudioInputInterfaceName(int index)
{
	return LocalAudioGrabber::GetInputInterfaceName(index);
}

extern "C" UNITY_INTERFACE_EXPORT const char* mray_gst_GetAudioInputInterfaceGUID(int index)
{
	return LocalAudioGrabber::GetInputInterfaceGUID(index);
}

extern "C" UNITY_INTERFACE_EXPORT int mray_gst_GetAudioOutputInterfacesCount()
{
	return LocalAudioGrabber::GetOutputInterfacesCount();
}

extern "C" UNITY_INTERFACE_EXPORT const char* mray_gst_GetAudioOutputInterfaceName(int index)
{
	return LocalAudioGrabber::GetOutputInterfaceName(index);
}

extern "C" UNITY_INTERFACE_EXPORT const char* mray_gst_GetAudioOutputInterfaceGUID(int index)
{
	return LocalAudioGrabber::GetOutputInterfaceGUID(index);
}
extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createUnityAudioGrabber()
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		UnityAudioGrabber* g = new UnityAudioGrabber();
		return g;
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_UnityAudioGrabberInit(UnityAudioGrabber* g, int bufferLength, int channels, int samplingrate)
{

	g->Init(bufferLength, channels, samplingrate);
}


extern "C" UNITY_INTERFACE_EXPORT void mray_gst_UnityAudioGrabberAddFrame(UnityAudioGrabber* g, float* data)
{
	g->AddFrame(data);
}


extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createCustomAudioGrabber()
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		CustomAudioGrabber* g = new CustomAudioGrabber();
		return g;
	}
	return 0;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_CustomAudioGrabberInit(CustomAudioGrabber* g, const char* pipeline, int channels, int samplingrate)
{

	g->Init(pipeline, channels, samplingrate);
}


extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createNetworkAudioGrabber()
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		NetworkAudioGrabber* g = new NetworkAudioGrabber();
		return g;
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_NetworkAudioGrabberInit(NetworkAudioGrabber* g, uint port, int channels, int samplingrate)
{

	g->Init(port, channels, samplingrate);
}

extern "C" UNITY_INTERFACE_EXPORT uint mray_gst_NetworkAudioGrabberGetPort(NetworkAudioGrabber* g)
{
	return g->GetAudioPort();
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_NetworkAudioGrabberSetPort(NetworkAudioGrabber* g,uint port)
{
	 g->SetPort(port);
}

#endif
