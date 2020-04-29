
#include "PlayersAPI.h"
#include "GStreamerCore.h"
#include "GraphicsInclude.h"
#include "PixelUtil.h"

#include "INetwork.h"
#include "IMutex.h"
#include "IThreadManager.h"
#include "MutexLocks.h"

#include "GstCustomDataPlayer.h"
#include "GstAppAudioPlayer.h"
#include "IUDPClient.h"
#include <vector>

using namespace mray;
using namespace video;

struct MultiNetRenderRequest
{
	void* p;
	void* _TextureNativePtr;
	int _UnityTextureWidth;
	int _UnityTextureHeight;
	int index;
};
std::vector<MultiNetRenderRequest> __multiNetRequests;

//for GL.IssuePluginEvent
static void __stdcall mray_gst_customPlayerBlitImageNativeEvent(int eventID)
{
	for (int i = 0; i < __multiNetRequests.size(); ++i)
	{
		MultiNetRenderRequest r = __multiNetRequests[i];
		mray_gst_customPlayerBlitImage((GstCustomVideoPlayer*)r.p, r._TextureNativePtr, r._UnityTextureWidth, r._UnityTextureHeight);
	}
	__multiNetRequests.clear();
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_multiNetSetRecordToFile(GstNetworkMultipleVideoPlayer* p, const char* filename, int framerate)
{
	if (p == nullptr)
		return;

	if (filename == 0)
		p->SetRecordToFile("");
	p->SetRecordToFile(filename,framerate);

}

extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_multiNetGetRecordStarted(GstNetworkMultipleVideoPlayer* p)
{

	if (p == nullptr)
		return false;
	return p->GetRecordStarted();
}
extern "C" UNITY_INTERFACE_EXPORT UnityRenderNative mray_gst_customPlayerBlitImageNativeGLCall(GstCustomVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight)
{
	MultiNetRenderRequest r;
	r.p = p;
	r._TextureNativePtr = _TextureNativePtr;
	r._UnityTextureWidth = _UnityTextureWidth;
	r._UnityTextureHeight = _UnityTextureHeight;
	r.index = 0;
	__multiNetRequests.push_back(r);
	return mray_gst_customPlayerBlitImageNativeEvent;
}
#ifdef USE_UNITY_NETWORK
extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createNetworkPlayer()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstNetworkVideoPlayer* player = new GstNetworkVideoPlayer();
		return player;
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netPlayerSetIP(GstNetworkVideoPlayer* p, const char* ip, int videoPort, bool rtcp)
{
	if (p != NULL)
	{
		p->SetIPAddress(ip, videoPort, 0,rtcp);
	}
}


extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_netPlayerCreateStream(GstNetworkVideoPlayer* p)
{
	if (p != NULL)
	{
		return p->CreateStream();
	}
	return false;

}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netPlayerGetFrameSize(GstNetworkVideoPlayer* p, int &w, int &h, int& components)
{
	if (p != NULL)
	{
		Vector2d sz = p->GetFrameSize();
		w = sz.x;
		h = sz.y;
		components = video::PixelUtil::getPixelDescription(p->GetImageFormat()).elemSizeB;
	}
	else
		w = h = components = 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netPlayerBlitImage(GstNetworkVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight)
{
	if (p == NULL || !_TextureNativePtr)
		return;

	const video::ImageInfo* ifo= p->GetLastFrame();

	if (ifo)
	{
		BlitImage(ifo, _TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight);
	}

}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_netPlayerFrameCount(GstNetworkVideoPlayer* p)
{
	if (p == NULL)
		return 0;
	return p->GetCaptureFrameRate();
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_netPlayerGetPort(GstNetworkVideoPlayer* p, int i)
{
	if (p != nullptr)
	{
		return p->GetPort(i);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createNetworkMultiplePlayer()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstNetworkMultipleVideoPlayer* player = new GstNetworkMultipleVideoPlayer();
		return player;
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_multiNetPlayerSetIP(GstNetworkMultipleVideoPlayer* p, const char* ip, int baseVideoPort, int count, bool rtcp)
{
	if (p != NULL)
	{
		p->SetIPAddress(ip, baseVideoPort,count, 0, rtcp);
	}
}

extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_multiNetPlayerCreateStream(GstNetworkMultipleVideoPlayer* p)
{
	if (p != NULL)
	{
		return p->CreateStream();
	}
	return false;

}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_multiNetPlayerGetFrameSize(GstNetworkMultipleVideoPlayer* p, int &w, int &h, int& components)
{
	if (p != NULL)
	{
		Vector2d sz = p->GetFrameSize();
		w = sz.x;
		h = sz.y;
		components = video::PixelUtil::getPixelDescription(p->GetImageFormat()).elemSizeB;
	}
	else
		w = h = components = 0;
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_multiNetPlayerFrameCount(GstNetworkMultipleVideoPlayer* p,int index)
{
	if (p == NULL)
		return 0;
	return p->GetCaptureFrameRate(index);
}
extern "C" UNITY_INTERFACE_EXPORT uint  mray_gst_multiNetPlayerGetVideoPort(GstNetworkMultipleVideoPlayer* p, int index)
{
	if (!p)
		return 0;
	return p->GetVideoPort(index);
}

extern "C" UNITY_INTERFACE_EXPORT void  mray_gst_multiNetPlayerSetFormat(GstNetworkMultipleVideoPlayer* p, int format)
{
	if (!p)
		return;
	p->SetImageFormat((video::EPixelFormat)format);

}
extern "C" UNITY_INTERFACE_EXPORT int  mray_gst_multiNetPlayerGetFormat(GstNetworkMultipleVideoPlayer* p)
{
	if (!p)
		return 0;
	return p->GetImageFormat();

}
extern "C" UNITY_INTERFACE_EXPORT ulong  mray_gst_multiNetPlayerGetNetworkUsage(GstNetworkMultipleVideoPlayer* p)
{
	if (!p)
		return 0;
	return p->GetNetworkUsage();
}
extern "C" UNITY_INTERFACE_EXPORT void  mray_gst_multiNetPlayerSetDecoderType(GstNetworkMultipleVideoPlayer* p, const char* decoder)
{

	if (!p)
		return;
	p->SetDecoderType(decoder);
}
#endif

//////////////////////////////////////////////////////////////////////////

//for GL.IssuePluginEvent
static void __stdcall mray_gst_playerBlitImageNativeEvent(int eventID)
{
	if (__multiNetRequests.size() == 0)
		return;
	MultiNetRenderRequest r = __multiNetRequests[0];
	__multiNetRequests.erase(__multiNetRequests.begin());
	mray_gst_playerBlitImage((IGStreamerPlayer*)r.p, r._TextureNativePtr, r._UnityTextureWidth, r._UnityTextureHeight, r.index);
}
extern "C" UNITY_INTERFACE_EXPORT UnityRenderNative mray_gst_playerBlitImageNativeGLCall(IGStreamerPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight, int index)
{
	MultiNetRenderRequest r;
	r.p = p;
	r.index = index;
	r._TextureNativePtr = _TextureNativePtr;
	r._UnityTextureWidth = _UnityTextureWidth;
	r._UnityTextureHeight = _UnityTextureHeight;
	__multiNetRequests.push_back(r);
	return mray_gst_playerBlitImageNativeEvent;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_playerBlitImage(IGStreamerPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight, int index)
{
	if (p == NULL || !_TextureNativePtr)
		return;

	const video::ImageInfo* ifo = p->GetLastFrame(index);

	if (ifo)
	{
		BlitImage(ifo, _TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight);
	}
}

extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_playerGetData(IGStreamerPlayer* p, int index)
{
	if (p == nullptr)
		return 0;

	const video::ImageInfo* ifo = p->GetLastFrame(index);

	if (ifo)
	{
		return ifo->imageData;
	}
	return 0;

}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_playerCopyData(IGStreamerPlayer* p, uchar* _data, int index)
{
	if (p == nullptr || !_data)
		return;

	const video::ImageInfo* ifo = p->GetLastFrame(index);

	if (ifo)
	{
		memcpy(_data, ifo->imageData, ifo->imageDataSize);
	}

}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_playerGrabFrame(IGStreamerPlayer* p, int &w, int &h, int& c, int index)
{
	if (p != NULL)
	{
		if (p->GrabFrame(index))
		{
			const video::ImageInfo* frame = p->GetLastFrame(index);
			if (frame) {
				w = frame->Size.x;
				h = frame->Size.y;
				c = video::PixelUtil::getPixelDescription(frame->format).elemSizeB;
				
			}
			return true;
		}
		return false;
	}
	return false;

}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_PlayerDestroy(IGStreamerPlayer* p)
{
    
    if (p != NULL)
    {
        p->Close();
        delete p;
    }
}

extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createCustomVideoPlayer()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstCustomVideoPlayer* player = new GstCustomVideoPlayer();
		return player;
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_customPlayerSetPipeline(GstCustomVideoPlayer* p, const char* pipeline)
{
	if (p != NULL)
	{
		p->SetPipelineString(pipeline);
	}
}


extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_customPlayerCreateStream(GstCustomVideoPlayer* p)
{
	if (p != NULL)
	{
		return p->CreateStream();
	}
	return false;

}



extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_customPlayerCopyFrame(GstCustomVideoPlayer* p, video::ImageInfo* target)
{
    if(p!=NULL && target!=NULL)
    {
        target->copyFrom(p->GetLastFrame());
        return true;
    }
    return false;
}

extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_customPlayerCropFrame(GstCustomVideoPlayer* p, video::ImageInfo* target,int x,int y,int width,int height)
{
    if(p!=NULL && target!=NULL)
    {
        target->copyCroppedFrom(p->GetLastFrame(),Vector2d(x,y),Vector2d(width,height),false,target->format);
        return true;
    }

	return false;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_customPlayerGetFrameSize(GstCustomVideoPlayer* p, int &w, int &h, int& components)
{
	if (p != NULL)
	{
		Vector2d sz = p->GetFrameSize();
		w = sz.x;
		h = sz.y;
		components = video::PixelUtil::getPixelDescription(p->GetImageFormat()).elemSizeB;
	}
	else
		w = h = components=0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_customPlayerBlitImage(GstCustomVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight)
{
	if (p == NULL || !_TextureNativePtr)
		return;

	const video::ImageInfo* ifo = p->GetLastFrame();

	if (ifo)
	{
		BlitImage(ifo, _TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight);
	}
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_customPlayerFrameCount(GstCustomVideoPlayer* p)
{
	if (p == NULL)
		return 0;
	return p->GetCaptureFrameRate();
}

extern "C" UNITY_INTERFACE_EXPORT int  mray_gst_customPlayerGrabAudioFrame(GstCustomVideoPlayer* p)
{
	if (p == nullptr)
		return 0;
	return p->GrabAudioFrame();

}
extern "C" UNITY_INTERFACE_EXPORT int  mray_gst_customPlayerGetAudioFrameSize(GstCustomVideoPlayer* p)
{

	if (p == nullptr)
		return 0;
	return p->GetAudioFrameSize();
}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_customPlayerCopyAudioFrame(GstCustomVideoPlayer* p, float*data)
{
	if (p == nullptr)
		return 0;
	return p->CopyAudioFrame(data);

}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_customPlayerChannelsCount(GstCustomVideoPlayer* p)
{
	if (p == nullptr)
		return 0;
	return p->GetAudioChannelsCount();

}

//////////////////////////////////////////////////////////////////////////


extern "C" UNITY_INTERFACE_EXPORT void mray_gst_PlayerPlay(IGStreamerPlayer* p)
{
	if (p != NULL)
	{
		 p->Play();
	}

}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_PlayerPause(IGStreamerPlayer* p)
{
	if (p != NULL)
	{
		p->Pause();
	}


}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_PlayerStop(IGStreamerPlayer* p)
{

	if (p != NULL)
	{
		p->Stop();
	}
}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_PlayerIsLoaded(IGStreamerPlayer* p)
{
	if (p != NULL)
	{
		return p->IsLoaded();
	}
	return false;

}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_PlayerIsPlaying(IGStreamerPlayer* p)
{
	if (p != NULL)
	{
		return p->IsPlaying();
	}
	return false;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_PlayerClose(IGStreamerPlayer* p)
{
	if (p != NULL)
	{
		p->Close();
	}

}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_PlayerSetPosition(IGStreamerPlayer* p,unsigned int pos)
{
    if (p != NULL)
    {
        return p->SetPosition(pos);
    }
    return false;
}

extern "C" UNITY_INTERFACE_EXPORT unsigned int mray_gst_PlayerGetPosition(IGStreamerPlayer* p)
{
    
    if (p != NULL)
    {
        return p->GetPosition();
    }
    return -1;
}
extern "C" UNITY_INTERFACE_EXPORT unsigned int mray_gst_PlayerGetDuration(IGStreamerPlayer* p)
{
    if (p != NULL)
    {
        return p->GetDuration();
    }
    return -1;
}
extern "C" UNITY_INTERFACE_EXPORT const ImageInfo* mray_gst_PlayerGetLastImage(IGStreamerPlayer* p, int index)
{
	if (p != NULL)
	{
		return p->GetLastFrame(index);
	}
	return 0;
}

extern "C" UNITY_INTERFACE_EXPORT unsigned long mray_gst_PlayerGetLastImageTimestamp(IGStreamerPlayer* p, int index)
{
	if (p != NULL)
	{
		return p->GetLastFrameTimestamp(index);
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_PlayerSendRTPMetaToHost(IGStreamerPlayer* p, int index, const char* host, int port)
{
	static OS::IMutex* mutex=0;
	static network::IUDPClient* client=0;
	if (client == 0)
	{
		client=network::INetwork::getInstance().createUDPClient();
		client->Open();
		mutex = OS::IThreadManager::getInstance().createMutex();
	}
	OS::ScopedLock lock(mutex);
	if (p != NULL)
	{
		RTPPacketData* data = (RTPPacketData*) p->GetLastFrameRTPMeta(index);
		if (data == NULL)
			return;

		struct CPacket
		{
			uint32_t timestamp;
			uint64_t presentationTime;
		};

		CPacket packet;
		packet.presentationTime = data->presentationTime;
		packet.timestamp = data->timestamp;

		network::NetAddress address(host, port);
		client->SendTo(&address, (const char*)&packet, sizeof(packet));
	}
	return;
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_PlayerRTPGetEyeGazeCount(IGStreamerPlayer* p,  int index)
{

	RTPPacketData* data = (RTPPacketData*)p->GetLastFrameRTPMeta(index);
	if (data == NULL)
		return 0;

	int header = 0;
	int count = 0;
	memcpy(&header, data->data, sizeof(header));
	if (header != 0x1010)
		return 0;

	memcpy(&count, data->data + sizeof(header), sizeof(count));
	return count;
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_PlayerRTPGetEyeGazeLevels(IGStreamerPlayer* p, int index, int eye)
{

	RTPPacketData* data = (RTPPacketData*)p->GetLastFrameRTPMeta(index);
	if (data == NULL)
		return 0;

	int header = 0;
	int levels = 0;
	memcpy(&header, data->data, sizeof(header));
	if (header != 0x1010)
		return 0;

	memcpy(&levels, data->data + sizeof(header)+sizeof(int), sizeof(levels));
	return levels;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_PlayerRTPGetEyeGazeData(IGStreamerPlayer* p, int index, int eye, int level, int& x, int& y, int& w, int& h)
{
	RTPPacketData* data = (RTPPacketData*)p->GetLastFrameRTPMeta(index);
	if (data == NULL)
		return;

	struct Vector4di
	{
		int x, y, w, h;
	}vec;

	int header = 0;
	memcpy(&header, data->data, sizeof(header));
	if (header != 0x1010)
		return;

	int levels = 0;
	memcpy(&levels, data->data + sizeof(header)+sizeof(int), sizeof(levels));

	int offset = sizeof(int)* 3 + levels*sizeof(vec)*eye;
	memcpy(&vec, data->data + offset + sizeof(vec)*level, sizeof(vec));
	x = vec.x;
	y = vec.y;
	w = vec.w;
	h = vec.h;

}

extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_customDataPlayerCreate()
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		video::GstCustomDataPlayer* player = new video::GstCustomDataPlayer();
		return player;
	}
	return 0;
}

extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_customDataPlayerCreateStream(video::GstCustomDataPlayer* player)
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		return player->CreateStream();
	}
	return false;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_customDataPlayerSetPort(video::GstCustomDataPlayer* player,int port)
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		player->SetPort(port);
	}
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_customDataPlayerGetPort(video::GstCustomDataPlayer* player)
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		return player->GetPort(0);
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_customDataPlayerGetDataLength(video::GstCustomDataPlayer* player)
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		return player->GetDataLength();
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_customDataPlayerGetData(video::GstCustomDataPlayer* player, unsigned char* data, int length)
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		return player->GetData(data,length);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////


#ifdef USE_UNITY_NETWORK

extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createNetworkAudioPlayer()
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstNetworkAudioPlayer* player = new GstNetworkAudioPlayer();
		return player;
	}
	return 0;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netAudioPlayerSetIP(GstNetworkAudioPlayer* p, const char* ip, int audioPort, bool rtcp)
{
	if (p!=NULL)
		p->SetIPAddress(ip, audioPort, 0, rtcp);
}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_netAudioPlayerCreateStream(GstNetworkAudioPlayer* p)
{
	if (p != NULL)
		return p->CreateStream();
	return false;
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netAudioPlayerSetVolume(GstNetworkAudioPlayer* p, float v)
{
	if (p != NULL)
		p->SetVolume(v);
}

extern "C" UNITY_INTERFACE_EXPORT uint  mray_gst_netAudioPlayerGetAudioPort(GstNetworkAudioPlayer* p)
{
	if (!p)
		return 0;
	return p->GetAudioPort();
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netAudioPlayerUseCustomOutput(GstNetworkAudioPlayer* p, bool use)
{
	if (!p)
		return;
	return p->UseCustomAudioInterface(use);
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_netAudioPlayerChannelsCount(GstNetworkAudioPlayer* p)
{
	if (!p)
		return 0;
	return p->ChannelsCount();
}
extern "C" UNITY_INTERFACE_EXPORT void mray_gst_netAudioPlayerSetSampleRate(GstNetworkAudioPlayer* p, int rate)
{
	if (!p)
		return;
	p->SetSampleRate(rate);
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_netAudioPlayerSampleRate(GstNetworkAudioPlayer* p)
{
	if (!p)
		return 0;
	return p->GetSampleRate();
}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_netAudioPlayerIsUsingCustomOutput(GstNetworkAudioPlayer* p)
{
	if (!p)
		return false;
	return p->IsUsingCustomAudioInterface();
}
extern "C" UNITY_INTERFACE_EXPORT int mray_gst_netAudioPlayerGetFrameSize(GstNetworkAudioPlayer* p)
{
	if (!p)
		return 0;
	return p->GetFrameSize();
}
extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_netAudioPlayerCopyAudioFrame(GstNetworkAudioPlayer* p, float* data)
{
	if (!p)
		return false;
	return p->CopyAudioFrame(data);
}

extern "C" UNITY_INTERFACE_EXPORT float mray_gst_ProcessAudioPackets(float* srcData, int startIndex, int channelIndex, int count, int stride, int srcChannels, float* data, int length, int channels
,bool modulate)
{
	float Volume = 1;
	float average = 0;
	if (channels == srcChannels) {
		for (int i = 0, j = 0; i < count; i += stride, ++j) {
			if(modulate)
				data[j + length] *= srcData[startIndex + i + channelIndex] * Volume;
			else data[j + length] = srcData[startIndex + i + channelIndex] * Volume;
			average += srcData[startIndex + i + channelIndex] * srcData[startIndex + i + channelIndex];
		}
	}
	else if (channels == 2 && srcChannels == 1) {
		for (int i = 0, j = 0; i < count; i += stride, ++j) {
			if (modulate) {
				data[2 * j + length] *= srcData[startIndex + i + channelIndex] * Volume;
				data[2 * j + length + 1] *= srcData[startIndex + i + channelIndex] * Volume;
			}
			else {
				data[2 * j + length] = srcData[startIndex + i + channelIndex] * Volume;
				data[2 * j + length + 1] = srcData[startIndex + i + channelIndex] * Volume;
			}
			average += srcData[startIndex + i + channelIndex] * srcData[startIndex + i + channelIndex];
		}
	}
	else if (channels == 1 && srcChannels == 2) {
		for (int i = 0; i < count; i++) {
			if(modulate)
				data[i + length] *= srcData[startIndex + 2 * i] * Volume;
			else 
				data[i + length] = srcData[startIndex + 2 * i] * Volume;
			average += srcData[startIndex + 2 * i] * srcData[startIndex + 2 * i];
		}
	}
	return average;
}

extern "C" UNITY_INTERFACE_EXPORT void* mray_gst_createAppAudioPlayer()
{

	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstAppAudioPlayer* g = new GstAppAudioPlayer();
		return g;
	}
	return 0;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_AppAudioPlayerInit(GstAppAudioPlayer* p, int ifaceID,int samplingRate)
{
	if (p)
	{
		p->Init(ifaceID,samplingRate);
	}
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gst_AppAudioPlayerAttachGrabber(GstAppAudioPlayer* a, IAudioGrabber* g)
{
	a->SetAudioGrabber(g);
}

extern "C" UNITY_INTERFACE_EXPORT bool mray_gst_AppAudioPlayerCreateStream(GstAppAudioPlayer* p)
{
	if (p != NULL)
	{
		return p->CreateStream();
	}
	return false;

}

#endif
