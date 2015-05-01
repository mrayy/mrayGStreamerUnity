
#include "PlayersAPI.h"
#include "GStreamerCore.h"
#include "GraphicsInclude.h"

extern "C" EXPORT_API void* mray_gst_createNetworkPlayer()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstNetworkVideoPlayer* player = new GstNetworkVideoPlayer();
		return player;
	}
	return 0;
}
extern "C" EXPORT_API void mray_gst_netPlayerSetIP(GstNetworkVideoPlayer* p, const char* ip, int videoPort, bool rtcp)
{
	if (p != nullptr)
	{
		p->SetIPAddress(ip, videoPort, rtcp);
	}
}

extern "C" EXPORT_API void mray_gst_PlayerDestroy(IGStreamerPlayer* p)
{

	if (p != nullptr)
	{
		p->Close();
		delete p;
	}
}

extern "C" EXPORT_API bool mray_gst_netPlayerCreateStream(GstNetworkVideoPlayer* p)
{
	if (p != nullptr)
	{
		return p->CreateStream();
	}
	return false;

}
extern "C" EXPORT_API bool mray_gst_netPlayerGrabFrame(GstNetworkVideoPlayer* p, int &w, int &h)
{
	if (p != nullptr)
	{
		if (p->GrabFrame())
		{
			Vector2d sz = p->GetFrameSize();
			w = sz.x;
			h = sz.y;
			return true;
		}
		return false;
	}
	return false;

}
extern "C" EXPORT_API void mray_gst_netPlayerGetFrameSize(GstNetworkVideoPlayer* p, int &w, int &h)
{
	if (p != nullptr)
	{
		Vector2d sz = p->GetFrameSize();
		w = sz.x;
		h = sz.y;
	}else
		w = h = 0;
}
extern "C" EXPORT_API void mray_gst_netPlayerBlitImage(GstNetworkVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight)
{
	if (p == nullptr || !_TextureNativePtr)
		return;

	const video::ImageInfo* ifo= p->GetLastFrame();

	if (ifo)
	{
		BlitImage(ifo, _TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight);
	}
}
//////////////////////////////////////////////////////////////////////////


extern "C" EXPORT_API void* mray_gst_createCustomVideoPlayer()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstCustomVideoPlayer* player = new GstCustomVideoPlayer();
		return player;
	}
	return 0;
}
extern "C" EXPORT_API void mray_gst_customPlayerSetPipeline(GstCustomVideoPlayer* p, const char* pipeline)
{
	if (p != nullptr)
	{
		p->SetPipelineString(pipeline);
	}
}


extern "C" EXPORT_API bool mray_gst_customPlayerCreateStream(GstCustomVideoPlayer* p)
{
	if (p != nullptr)
	{
		return p->CreateStream();
	}
	return false;

}
extern "C" EXPORT_API bool mray_gst_customPlayerGrabFrame(GstCustomVideoPlayer* p, int &w, int &h)
{
	if (p != nullptr)
	{
		if (p->GrabFrame())
		{
			Vector2d sz = p->GetFrameSize();
			w = sz.x;
			h = sz.y;
			return true;
		}
		return false;
	}
	return false;

}
extern "C" EXPORT_API void mray_gst_customPlayerGetFrameSize(GstCustomVideoPlayer* p, int &w, int &h)
{
	if (p != nullptr)
	{
		Vector2d sz = p->GetFrameSize();
		w = sz.x;
		h = sz.y;
	}
	else
		w = h = 0;
}
extern "C" EXPORT_API void mray_gst_customPlayerBlitImage(GstCustomVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight)
{
	if (p == nullptr || !_TextureNativePtr)
		return;

	const video::ImageInfo* ifo = p->GetLastFrame();

	if (ifo)
	{
		BlitImage(ifo, _TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight);
	}
}

//////////////////////////////////////////////////////////////////////////


extern "C" EXPORT_API void mray_gst_PlayerPlay(IGStreamerPlayer* p)
{
	if (p != nullptr)
	{
		 p->Play();
	}

}
extern "C" EXPORT_API void mray_gst_PlayerPause(IGStreamerPlayer* p)
{
	if (p != nullptr)
	{
		p->Pause();
	}


}
extern "C" EXPORT_API void mray_gst_PlayerStop(IGStreamerPlayer* p)
{

	if (p != nullptr)
	{
		p->Stop();
	}
}
extern "C" EXPORT_API bool mray_gst_PlayerIsLoaded(IGStreamerPlayer* p)
{
	if (p != nullptr)
	{
		return p->IsLoaded();
	}
	return false;

}
extern "C" EXPORT_API bool mray_gst_PlayerIsPlaying(IGStreamerPlayer* p)
{
	if (p != nullptr)
	{
		return p->IsPlaying();
	}
	return false;
}
extern "C" EXPORT_API void mray_gst_PlayerClose(IGStreamerPlayer* p)
{
	if (p != nullptr)
	{
		p->Close();
	}

}


