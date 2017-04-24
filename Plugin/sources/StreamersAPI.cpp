

#include "Unity/IUnityInterface.h"

#ifdef USE_UNITY_GRABBER
#include "UnityImageGrabber.h"
#endif

#include "GstCustomVideoStreamer.h"
#include "GstNetworkVideoStreamer.h"
#include "GstNetworkAudioStreamer.h"

#include "GStreamerCore.h"
#include "GraphicsInclude.h"
#include "PixelUtil.h"

using namespace mray;
using namespace video;

#ifdef USE_UNITY_GRABBER
extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createUnityImageGrabber()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		UnityImageGrabber* g = new UnityImageGrabber();
		return g;
	}
	return 0;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_UnityImageGrabberSetData(UnityImageGrabber* g, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight, int Format)
{
	if (!g)
		return;
	g->SetData(_TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight, Format);
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
		GstCustomVideoStreamer* g = new GstCustomVideoStreamer();
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
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netStreamerSetBitRate(GstNetworkVideoStreamer* p, int bitRate)
{
	if (p)
	{
		p->SetBitRate(bitRate);
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

#endif
