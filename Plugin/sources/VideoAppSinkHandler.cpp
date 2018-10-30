

#include "stdafx.h"
#include "VideoAppSinkHandler.h"
#include "PixelUtil.h"
#include "IThreadManager.h"

#include <gst/video/video.h>
#include <gst/rtp/rtp.h>
#include "rtp.h"
#include "MutexLocks.h"

namespace mray
{
namespace video
{

	//this listener is used for pre-decoding stage, when we are still receiving the rtp packets for the frame
	class VideoRTPDataListener:public IMyListenerCallback
	{
	public:
	protected:
		mray::OS::IMutex* m_mutex;
		std::list<RTPPacketData> timestampQueue;
		_GstMyListener* _pre,*_post;
	public:

		VideoRTPDataListener()
		{
			m_mutex = OS::IThreadManager::getInstance().createMutex();
		}
		void Init(_GstMyListener* pre, _GstMyListener* post)
		{
			_pre = pre;
			_post = post;
		}

		~VideoRTPDataListener()
		{
			delete m_mutex ;
		}

		void ListenerOnDataChained(_GstMyListener* src, GstBuffer * buffer)
		{
			if (src == _pre)
				PreRTP(buffer);
			if (src == _post)
				PostRTP(buffer);
		}
		void PostRTP(GstBuffer * buffer)
		{
			//check if packet dropped
			RTPPacketData &ts = timestampQueue.front();
			while (ts.presentationTime != buffer->pts)
			{
				//packet dropped!
				GetLastRTPPacket(true);
				ts = timestampQueue.front();
			}
		}
		void PreRTP(GstBuffer * buffer)
		{
			//GstRTPBuffer rtp_buf;
			//gst_rtp_buffer_map(buffer, GST_MAP_READ, &rtp_buf);
			//gst_rtp_buffer_add_extension_onebyte_header()
			//gst_rtp_buffer_unmap(&rtp_buf);

			GstMapInfo map;
			gst_buffer_map(buffer, &map, GST_MAP_READ);

			if (true)
			{
				//check if the timestamp of this packet is different from the one on top of our timestamp queue. if so, then add the packet
				RTPPacketData packet;
				packet.timestamp = rtp_timestamp(map.data);
				
				bool okay = timestampQueue.size() == 0;
				if (!okay)
				{
					const RTPPacketData& last = timestampQueue.back();
					okay = (packet.timestamp != last.timestamp);
				}
				if (okay)
				{
					//new packet, read padding and add it to packet list
					packet.length = rtp_padding_payload((unsigned char*)map.data, map.size, packet.data);
					packet.presentationTime = buffer->pts;
					AddRTPPacket(packet);
				}

			}
			else
			{
				//inject to H264 stream --> too much noise in decoded frame
				RTPPacketData packet;
				packet.length = 8;
				memcpy(packet.data, map.data + map.size - 8, 8);
				AddRTPPacket(packet);
			}
			gst_buffer_unmap(buffer, &map);

		}

		int maxPackets = 1;

		RTPPacketData GetLastRTPPacket(bool remove)
		{
			OS::ScopedLock lock(m_mutex);
			if (timestampQueue.size() == 0)
				return RTPPacketData();
//  			while (timestampQueue.size() > maxPackets)
//  				timestampQueue.pop_front();
			RTPPacketData ts = timestampQueue.front();
			if (remove)
				timestampQueue.pop_front();
			return ts;
		}
		void AddRTPPacket(const RTPPacketData& ts)
		{
			OS::ScopedLock lock(m_mutex);
			if (timestampQueue.size() == 0)
			{
				timestampQueue.push_back(ts);

			}
			else{
				const RTPPacketData& last = timestampQueue.back();
				if (last.timestamp != ts.timestamp)
					timestampQueue.push_back(ts);
			}

		}
	};

	//this listener is used after the decoding is done
	class VideoPreappDataListener :public IMyListenerCallback
	{
	public:
	protected:
		mray::OS::IMutex* m_mutex;
		VideoRTPDataListener* m_packetsListener;

		struct PacketData
		{
			ulong bufferID;
			RTPPacketData packet;
		};
		std::list<PacketData> m_packets;
		
	public:

		VideoPreappDataListener(VideoRTPDataListener* listener)
		{
			m_packetsListener = listener;
			m_mutex = OS::IThreadManager::getInstance().createMutex();
		}

		~VideoPreappDataListener()
		{
			delete m_mutex;
		}

		void ListenerOnDataChained(_GstMyListener* src, GstBuffer * buffer)
		{
			PacketData packet;
			packet.bufferID = (ulong)buffer;
			packet.packet = m_packetsListener->GetLastRTPPacket(true);
			m_packets.push_back(packet);
		}
		RTPPacketData GetLastRTPPacket(ulong bufferID,bool remove)
		{
			OS::ScopedLock lock(m_mutex);
			if (m_packets.size() == 0)
				return RTPPacketData();
			PacketData packet;
			do 
			{
				packet = m_packets.front();
				m_packets.pop_front();
			} while (m_packets.size()>0 && packet.bufferID!=bufferID);
			return packet.packet;
		}
	};

	class VideoAppSinkHandlerData
	{
	public:
		VideoRTPDataListener* rtplistener;
		VideoRTPDataListener* postRtplistener;
		VideoPreappDataListener* preapplistener;

	public:
		VideoAppSinkHandlerData()
		{
			rtplistener = new VideoRTPDataListener();
			preapplistener = new VideoPreappDataListener(rtplistener);
		}
		~VideoAppSinkHandlerData()
		{
			delete rtplistener;
			delete preapplistener;
		}
	};

VideoAppSinkHandler::VideoAppSinkHandler()
{
	m_sink = 0;
	m_samplesCount = 0;
	m_IsFrameNew = false;
	m_HavePixelsChanged = false;
	m_BackPixelsChanged = false;
	m_IsAllocated = false;
	m_frameID = 0;
	m_surfaceCount = 1;
	m_captureFPS = 0;
	m_frameCount = 0;
	m_rtpDataListener = 0;

	m_data = new VideoAppSinkHandlerData();

	m_mutex = OS::IThreadManager::getInstance().createMutex();

}
VideoAppSinkHandler::~VideoAppSinkHandler()
{
	if (m_rtpDataListener)
	{
		m_rtpDataListener->listeners->RemoveListener(m_data->rtplistener);
		m_preappsrcListener->listeners->RemoveListener(m_data->preapplistener);
	}
	delete m_data;
	m_data = 0;
}
void VideoAppSinkHandler::Close()
{
	m_frameID = 0;
	m_samplesCount = 0;
	m_IsAllocated = false;
	for (int i = 0; i < m_surfaceCount; ++i)
	{
		m_pixels[i].data.clear();
		m_backPixels[i].data.clear();
	}
//	m_eventPixels.clear();
}


static GstVideoInfo getVideoInfo(GstSample * sample){
	GstCaps *caps = gst_sample_get_caps(sample);
	GstVideoInfo vinfo;
	gst_video_info_init(&vinfo);
	if (caps){
		gst_video_info_from_caps(&vinfo, caps);
	}
	return vinfo;
}

video::EPixelFormat getVideoFormat(GstVideoFormat format){
	switch (format){
	case GST_VIDEO_FORMAT_GRAY8:
		return EPixel_LUMINANCE8;
	case GST_VIDEO_FORMAT_GRAY16_BE:
	case GST_VIDEO_FORMAT_GRAY16_LE:
	//case GST_VIDEO_FORMAT_VYUY:
	case GST_VIDEO_FORMAT_YVYU:
	case GST_VIDEO_FORMAT_YUY2:
		return EPixel_LUMINANCE16;

	case GST_VIDEO_FORMAT_RGB:
		return EPixel_R8G8B8;

	case GST_VIDEO_FORMAT_BGR:
		return EPixel_B8G8R8;

	case GST_VIDEO_FORMAT_RGBA:
		return EPixel_R8G8B8A8;

	case GST_VIDEO_FORMAT_BGRA:
		return EPixel_B8G8R8A8;

	case GST_VIDEO_FORMAT_RGB16:
		return EPixel_R5G6B5;

	case GST_VIDEO_FORMAT_I420:
		return EPixel_I420;
	case GST_VIDEO_FORMAT_Y42B:
		return EPixel_Y42B;
    case GST_VIDEO_FORMAT_NV12:
            return EPixel_NV12;

	default:
		return EPixel_Unkown;
	}
}


void VideoAppSinkHandler::SetRTPListener(GstMyListener* preRTP, GstMyListener* postRTP, GstMyListener* preapp)
{ 
	m_rtpDataListener = preRTP; 
	m_rtpDataListener->listeners->AddListener(m_data->rtplistener);
	postRTP->listeners->AddListener(m_data->rtplistener);
	m_data->rtplistener->Init(preRTP, postRTP);

	m_preappsrcListener = preapp;
	m_preappsrcListener->listeners->AddListener(m_data->preapplistener);
}


GstFlowReturn VideoAppSinkHandler::process_sample(std::shared_ptr<GstSample> sample){
	GstBuffer * _buffer = gst_sample_get_buffer(sample.get());

	if (false)
	{
		GstMeta* meta=gst_buffer_iterate_meta(_buffer, 0);
		if (meta)
		{
			
			printf("%d\n", meta->info->size);
		}
	}

	GstVideoInfo vinfo = getVideoInfo(sample.get());
	video::EPixelFormat fmt = getVideoFormat(vinfo.finfo->format);
	m_pixelFormat = fmt;
	if (fmt == video::EPixel_Unkown)
	{
		return GST_FLOW_ERROR;
	}

    bool RGBFormat=true;
	int height = vinfo.height;
	if (fmt == video::EPixel_I420)
	{
        RGBFormat=false;
		//fmt = video::EPixel_LUMINANCE8;
		height = (height*3)/2;
    }else if (fmt == video::EPixel_NV12)
    {
        RGBFormat=false;
        //fmt = video::EPixel_LUMINANCE8;
        height *= 2;
	}
	else if (fmt == video::EPixel_Y42B)
	{
		RGBFormat = false;
		//fmt = video::EPixel_LUMINANCE8;
		height *= 2;
	}

	if (m_pixels[0].data.imageData && (m_pixels[0].data.Size.x != vinfo.width || m_pixels[0].data.Size.y != height || m_pixels[0].data.format != fmt))
	{
		m_IsAllocated = false;
		m_pixels[0].data.clear();
		m_backPixels[0].data.clear();
	}

	gst_buffer_map(_buffer, &mapinfo, GST_MAP_READ);
	guint size = mapinfo.size;
	int pxSize = video::PixelUtil::getPixelDescription(fmt).elemSizeB;

	int dataSize = m_pixels[0].data.Size.x*m_pixels[0].data.Size.y;
	if (RGBFormat)
        dataSize *= pxSize;

	if (m_pixels[0].data.imageData && dataSize != (int)size){
		if (vinfo.stride[0] < (m_pixels[0].data.Size.x * pxSize)) {
			gst_buffer_unmap(_buffer, &mapinfo);
			std::stringstream ss;
			ss << "VideoAppSinkHandler::process_sample(): error on new buffer, buffer size: " << size << "!= init size: " << dataSize;
			LogMessage(ss.str(), ELL_WARNING);
			return GST_FLOW_ERROR;
		}
	}
	m_mutex->lock();
	buffer = sample;

	if (m_pixels[0].data.imageData){
		++m_frameID;
		//if (stride > 0) {
	/*	if (vinfo.stride[0] == m_pixels[0].data.Size.x*pxSize)
		{
			m_backPixels[0].data.setData(mapinfo.data, m_pixels[0].data.Size, m_pixels[0].data.format);
		}
		else*/
		{
			uchar* ptr = m_backPixels[0].data.imageData;

			uchar* srcptr = mapinfo.data + vinfo.offset[0];
			if (vinfo.stride[0] == m_pixels[0].data.Size.x*pxSize)
			{
				memcpy(ptr, srcptr, vinfo.width*pxSize*vinfo.height);
				ptr += vinfo.width*pxSize*vinfo.height;
				srcptr += vinfo.width*pxSize*vinfo.height;
			}
			else{
				//copy row by row..
				for (int i = 0; i < vinfo.height; ++i)
				{
					memcpy(ptr, srcptr, vinfo.width*pxSize);
					ptr += vinfo.width*pxSize;
					srcptr += vinfo.stride[0];
				}
			}

			if (!RGBFormat)
			{
				if (vinfo.stride[1] + vinfo.stride[2] == vinfo.width)
				{
					int len = height - vinfo.height;
					memcpy(ptr, srcptr, vinfo.width*len);
				}
				else
				{
					//copy other planes
					int len = height - vinfo.height;
					for (int n = 0; n < 2; ++n)
					{
						srcptr = mapinfo.data + vinfo.offset[n + 1];
						for (int i = 0; i < len; ++i)
						{
							memcpy(ptr, srcptr, vinfo.width / 2);
							ptr += vinfo.width / 2;
							srcptr += vinfo.stride[n + 1];
						}
					}
				}
			}
		}
		m_backPixels[0].PTS = _buffer->pts;
		m_backPixels[0].DTS = _buffer->dts;

		if (m_rtpDataListener)
		{
			//read rtp data
			m_backPixels[0].RTPPacket = m_data->preapplistener->GetLastRTPPacket((ulong)_buffer, true);
			m_backPixels[0].RTPPacket.presentationTime = _buffer->pts;
		}
// 		}
// 		else {
// 			m_backPixels[0].setData(mapinfo.data, m_pixels[0].Size, m_pixels[0].format);
// 			m_eventPixels.setData(mapinfo.data, m_pixels[0].Size, m_pixels[0].format);
// 		}

			m_BackPixelsChanged = true;
		m_mutex->unlock();
		if (vinfo.stride[0] == 0) {
		//	ofNotifyEvent(prerollEvent, eventPixels);
		}
	}
	else{
	//	m_data->rtplistener->GetLastRTPPacket(true);//remove one packet
	//	m_backPixels[0].RTPPacket = m_data->preapplistener->GetLastRTPPacket((ulong)_buffer, true);
		if (RGBFormat)
			_Allocate(vinfo.width, vinfo.height, fmt);
		else 
			_Allocate(vinfo.width, height, fmt);

		m_mutex->unlock();
		FIRE_LISTENR_METHOD(OnStreamPrepared, (this));

	}
	gst_buffer_unmap(_buffer, &mapinfo);

	++m_samplesCount;
	return GST_FLOW_OK;
}
bool VideoAppSinkHandler::_Allocate(int width, int height, video::EPixelFormat fmt)
{

	if (m_IsAllocated) return true;

	m_frameSize.x = width;
	m_frameSize.y = height;

	m_pixels[0].data.createData(Vector2d(width, height), fmt);
	m_backPixels[0].data.createData(Vector2d(width, height), fmt);

	m_HavePixelsChanged = false;
	m_BackPixelsChanged = true;
	m_IsAllocated = true;

	m_frameCount = 0;
	m_timeAcc = 0;
	m_lastT = 0;
	m_captureFPS = 0;

	return m_IsAllocated;
}




bool VideoAppSinkHandler::GrabFrame(){
    m_mutex->lock();
	m_HavePixelsChanged = m_BackPixelsChanged;
	if (m_HavePixelsChanged){
		m_BackPixelsChanged = false;
		Swap(m_pixels[0].data.imageData, m_backPixels[0].data.imageData);
		m_pixels[0].DTS= m_backPixels[0].DTS;
		m_pixels[0].PTS= m_backPixels[0].PTS;
		m_pixels[0].RTPPacket= m_backPixels[0].RTPPacket;

		prevBuffer = buffer;


		++m_frameCount;
		/*
 		float t = GetEngineTime();
 		m_timeAcc += (t - m_lastT);// *0.001f;

		if (m_timeAcc > 1)
		{
			m_captureFPS = m_frameCount;
			m_timeAcc = m_timeAcc - (int)m_timeAcc;
			m_frameCount = 0;
			//	printf("Capture FPS: %d\n", m_captureFPS);
		}

		m_lastT = t;*/
	}

	m_mutex->unlock();
	m_IsFrameNew = m_HavePixelsChanged;
	m_HavePixelsChanged = false;
	return m_IsFrameNew;
}



float VideoAppSinkHandler::GetCaptureFrameRate()
{
	return m_frameCount;
}

}
}
