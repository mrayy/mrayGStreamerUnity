

#include "StreamersAPI.h"
#include "GStreamerCore.h"
#include "GraphicsInclude.h"
#include "PixelUtil.h"

#ifdef USE_UNITY_GRABBER
extern "C" EXPORT_API void* mray_gst_createUnityImageGrabber()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		UnityImageGrabber* g = new UnityImageGrabber();
		return g;
	}
	return 0;
}

extern "C" EXPORT_API void mray_gst_UnityImageGrabberSetData(UnityImageGrabber* g, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight, int Format)
{
	if (!g)
		return;
	g->SetData(_TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight, Format);
}
#endif

extern "C" EXPORT_API void mray_gst_StreamerDestroy(IGStreamerStreamer* p)
{
	if (p != NULL)
	{
		p->Close();
		delete p;
	}

}
extern "C" EXPORT_API void mray_gst_StreamerStream(IGStreamerStreamer* p)
{

	if (p != NULL)
	{
		p->Stream();
	}
}
extern "C" EXPORT_API void mray_gst_StreamerPause(IGStreamerStreamer* p)
{
	if (p != NULL)
	{
		p->SetPaused(true);
	}
}
extern "C" EXPORT_API void mray_gst_StreamerStop(IGStreamerStreamer* p)
{
	if (p != NULL)
	{
		p->Stop();
	}

}
extern "C" EXPORT_API bool mray_gst_StreamerIsStreaming(IGStreamerStreamer* p)
{
	if (p != NULL)
	{
		return p->IsStreaming();
	}
	return false;

}
extern "C" EXPORT_API void mray_gst_StreamerClose(IGStreamerStreamer* p)
{
	if (p != NULL)
	{
		return p->Close();
	}

}

#ifdef USE_UNITY_NETWORK

extern "C" EXPORT_API void* mray_gst_createNetworkStreamer()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstCustomVideoStreamer* g = new GstCustomVideoStreamer();
		return g;
	}
	return 0;
}
extern "C" EXPORT_API void mray_gst_netStreamerSetIP(GstCustomVideoStreamer* p, const char* ip, int videoPort, bool rtcp)
{
	if (p)
	{
		p->BindPorts(ip, videoPort, 0, rtcp);
	}
}
extern "C" EXPORT_API bool mray_gst_netStreamerCreateStream(GstCustomVideoStreamer* p)
{
	if (p)
	{
		return p->CreateStream();
	}
	return false;
}
extern "C" EXPORT_API void mray_gst_netStreamerSetGrabber(GstCustomVideoStreamer* p, UnityImageGrabber* g)
{
	if (p)
	{
		p->SetVideoGrabber(g);
	}

}
extern "C" EXPORT_API void mray_gst_netStreamerSetBitRate(GstCustomVideoStreamer* p, int bitRate)
{
	if (p)
	{
		p->SetBitRate(bitRate);
	}

}
extern "C" EXPORT_API void mray_gst_netStreamerSetResolution(GstCustomVideoStreamer* p, int width, int height, int fps)
{
	if (p)
	{
		p->SetResolution(width,height,fps);
	}

}
#endif
