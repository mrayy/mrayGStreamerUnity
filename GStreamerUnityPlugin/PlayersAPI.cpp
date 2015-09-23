
#include "PlayersAPI.h"
#include "GStreamerCore.h"
#include "GraphicsInclude.h"
#include "PixelUtil.h"

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
		p->SetIPAddress(ip, videoPort, 0,rtcp);
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
extern "C" EXPORT_API void mray_gst_netPlayerGetFrameSize(GstNetworkVideoPlayer* p, int &w, int &h, int& components)
{
	if (p != nullptr)
	{
		Vector2d sz = p->GetFrameSize();
		w = sz.x;
		h = sz.y;
		components = video::PixelUtil::getPixelDescription(p->GetImageFormat()).componentsCount;
	}
	else
		w = h = components = 0;
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
extern "C" EXPORT_API int mray_gst_netPlayerFrameCount(GstNetworkVideoPlayer* p)
{
	if (p == nullptr)
		return 0;
	return p->GetCaptureFrameRate();
}
//////////////////////////////////////////////////////////////////////////
extern "C" EXPORT_API void* mray_gst_createNetworkMultiplePlayer()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstNetworkMultipleVideoPlayer* player = new GstNetworkMultipleVideoPlayer();
		return player;
	}
	return 0;
}
extern "C" EXPORT_API void mray_gst_multiNetPlayerSetIP(GstNetworkMultipleVideoPlayer* p, const char* ip, int baseVideoPort, int count, bool rtcp)
{
	if (p != nullptr)
	{
		p->SetIPAddress(ip, baseVideoPort,count, 0, rtcp);
	}
}

extern "C" EXPORT_API bool mray_gst_multiNetPlayerCreateStream(GstNetworkMultipleVideoPlayer* p)
{
	if (p != nullptr)
	{
		return p->CreateStream();
	}
	return false;

}
extern "C" EXPORT_API void mray_gst_multiNetPlayerGetFrameSize(GstNetworkMultipleVideoPlayer* p, int &w, int &h, int& components)
{
	if (p != nullptr)
	{
		Vector2d sz = p->GetFrameSize();
		w = sz.x;
		h = sz.y;
		components = video::PixelUtil::getPixelDescription(p->GetImageFormat()).componentsCount;
	}
	else
		w = h = components = 0;
}
extern "C" EXPORT_API bool mray_gst_multiNetPlayerGrabFrame(GstNetworkMultipleVideoPlayer* p, int &w, int &h, int index)
{
	if (p != nullptr)
	{
		if (p->GrabFrame(index))
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
extern "C" EXPORT_API void mray_gst_multiNetPlayerBlitImage(GstNetworkMultipleVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight, int index)
{
	if (p == nullptr || !_TextureNativePtr)
		return;

	const video::ImageInfo* ifo = p->GetLastFrame(index);

	if (ifo)
	{
		BlitImage(ifo, _TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight);
	}
}
extern "C" EXPORT_API int mray_gst_multiNetPlayerFrameCount(GstNetworkMultipleVideoPlayer* p,int index)
{
	if (p == nullptr)
		return 0;
	return p->GetCaptureFrameRate(index);
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
extern "C" EXPORT_API void mray_gst_customPlayerGetFrameSize(GstCustomVideoPlayer* p, int &w, int &h, int& components)
{
	if (p != nullptr)
	{
		Vector2d sz = p->GetFrameSize();
		w = sz.x;
		h = sz.y;
		components = video::PixelUtil::getPixelDescription(p->GetImageFormat()).componentsCount;
	}
	else
		w = h = components=0;
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
extern "C" EXPORT_API int mray_gst_customPlayerFrameCount(GstCustomVideoPlayer* p)
{
	if (p == nullptr)
		return 0;
	return p->GetCaptureFrameRate();
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


//////////////////////////////////////////////////////////////////////////



extern "C" EXPORT_API void* mray_gst_createNetworkAudioPlayer()
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstNetworkAudioPlayer* player = new GstNetworkAudioPlayer();
		return player;
	}
	return 0;
}
extern "C" EXPORT_API void mray_gst_netAudioPlayerSetIP(GstNetworkAudioPlayer* p, const char* ip, int audioPort, bool rtcp)
{
	if (p!=nullptr)
		p->SetIPAddress(ip, audioPort, 0, rtcp);
}
extern "C" EXPORT_API bool mray_gst_netAudioPlayerCreateStream(GstNetworkAudioPlayer* p)
{
	if (p != nullptr)
		return p->CreateStream();
	return false;
}
extern "C" EXPORT_API void mray_gst_netAudioPlayerSetVolume(GstNetworkAudioPlayer* p, float v)
{
	if (p != nullptr)
		p->SetVolume(v);
}
