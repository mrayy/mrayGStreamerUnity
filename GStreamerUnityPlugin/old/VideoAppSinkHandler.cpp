

#include "stdafx.h"
#include "VideoAppSinkHandler.h"
#include "PixelUtil.h"
#include "UnityHelpers.h"
#include "IThreadManager.h"

#include <gst/video/video.h>
#include <sstream>

namespace mray
{
namespace video
{


VideoAppSinkHandler::VideoAppSinkHandler()
{
	m_sink = 0;
	m_IsFrameNew = false;
	m_HavePixelsChanged = false;
	m_BackPixelsChanged = false;
	m_IsAllocated = false;
	m_frameID = 0;

	m_mutex = OS::IThreadManager::getInstance().createMutex();
}
VideoAppSinkHandler::~VideoAppSinkHandler()
{
}
void VideoAppSinkHandler::Close()
{
	m_frameID = 0;
	m_IsAllocated = false;
	m_pixels.clear();
	m_backPixels.clear();
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

	default:
		return EPixel_Unkown;
	}
}
GstFlowReturn VideoAppSinkHandler::process_sample(std::shared_ptr<GstSample> sample){
	GstBuffer * _buffer = gst_sample_get_buffer(sample.get());



	GstVideoInfo vinfo = getVideoInfo(sample.get());
	video::EPixelFormat fmt = getVideoFormat(vinfo.finfo->format);
	if (fmt == video::EPixel_Unkown)
	{
		return GST_FLOW_ERROR;
	}
	if (m_pixels.imageData && (m_pixels.Size.x != vinfo.width || m_pixels.Size.y != vinfo.height || m_pixels.format != fmt))
	{
		m_IsAllocated = false;
		m_pixels.clear();
		m_backPixels.clear();
	}

	gst_buffer_map(_buffer, &mapinfo, GST_MAP_READ);
	guint size = mapinfo.size;
	int pxSize = video::PixelUtil::getPixelDescription(m_pixels.format).elemSizeB;
	int stride = 0;
	int dataSize = m_pixels.Size.x*m_pixels.Size.y * pxSize;

	if (m_pixels.imageData && dataSize != (int)size){
		GstVideoInfo vinfo = getVideoInfo(sample.get());
		stride = vinfo.stride[0];

		if (stride == (m_pixels.Size.x * pxSize)) {
			gst_buffer_unmap(_buffer, &mapinfo);

			std::stringstream ss;
			ss << "VideoAppSinkHandler::process_sample(): error on new buffer, buffer size: " << size << "!= init size: " << dataSize;
			LogMessage(ss.str(), ELL_WARNING);
			return GST_FLOW_ERROR;
		}
	}
	m_mutex->lock();
	buffer = sample;

	if (m_pixels.imageData){
		++m_frameID;
		//if (stride > 0) {
			m_backPixels.setData(mapinfo.data, m_pixels.Size, m_pixels.format);
// 		}
// 		else {
// 			m_backPixels.setData(mapinfo.data, m_pixels.Size, m_pixels.format);
// 			m_eventPixels.setData(mapinfo.data, m_pixels.Size, m_pixels.format);
// 		}

		m_BackPixelsChanged = true;
		m_mutex->unlock();
		if (stride == 0) {
		//	ofNotifyEvent(prerollEvent, eventPixels);
		}
	}
	else{

		_Allocate(vinfo.width, vinfo.height, fmt);

		m_mutex->unlock();
		FIRE_LISTENR_METHOD(OnStreamPrepared, (this));

	}
	gst_buffer_unmap(_buffer, &mapinfo);
	return GST_FLOW_OK;
}
bool VideoAppSinkHandler::_Allocate(int width, int height, video::EPixelFormat fmt)
{

	if (m_IsAllocated) return true;

	m_frameSize.x = width;
	m_frameSize.y = height;

	m_pixels.createData(Vector2d(width, height), fmt);
	m_backPixels.createData(Vector2d(width, height), fmt);

	m_HavePixelsChanged = false;
	m_BackPixelsChanged = true;
	m_IsAllocated = true;

	return m_IsAllocated;
}




bool VideoAppSinkHandler::GrabFrame(){
	m_mutex->lock();
	m_HavePixelsChanged = m_BackPixelsChanged;
	if (m_HavePixelsChanged){
		m_BackPixelsChanged = false;
		Swap(m_pixels.imageData, m_backPixels.imageData);

		prevBuffer = buffer;
	}

	m_mutex->unlock();
	m_IsFrameNew = m_HavePixelsChanged;
	m_HavePixelsChanged = false;
	return m_IsFrameNew;
}






GstFlowReturn VideoAppSinkHandler::preroll_cb(std::shared_ptr<GstSample> sample)
{
	GstFlowReturn ret = process_sample(sample);
	if (ret == GST_FLOW_OK){
		return GST_FLOW_OK;
	}
	else{
		return ret;
	}
}
GstFlowReturn VideoAppSinkHandler::buffer_cb(std::shared_ptr<GstSample> sample)
{
	GstFlowReturn ret = process_sample(sample);
	if (ret == GST_FLOW_OK){
		return GST_FLOW_OK;
	}
	else{
		return ret;
	}
}
void  VideoAppSinkHandler::eos_cb()
{
}
GstFlowReturn VideoAppSinkHandler::on_new_buffer_from_source(GstAppSink * elt, void * data){
#if GST_VERSION_MAJOR==0
	shared_ptr<GstBuffer> buffer(gst_app_sink_pull_buffer(GST_APP_SINK(elt)), &gst_buffer_unref);
#else
	std::shared_ptr<GstSample> buffer(gst_app_sink_pull_sample(GST_APP_SINK(elt)), &gst_sample_unref);
#endif
	return ((VideoAppSinkHandler*)data)->buffer_cb(buffer);
}

GstFlowReturn VideoAppSinkHandler::on_new_preroll_from_source(GstAppSink * elt, void * data){
#if GST_VERSION_MAJOR==0
	shared_ptr<GstBuffer> buffer(gst_app_sink_pull_preroll(GST_APP_SINK(elt)), &gst_buffer_unref);
#else
	std::shared_ptr<GstSample> buffer(gst_app_sink_pull_preroll(GST_APP_SINK(elt)), &gst_sample_unref);
#endif
	return ((VideoAppSinkHandler*)data)->preroll_cb(buffer);
}

void VideoAppSinkHandler::on_eos_from_source(GstAppSink * elt, void * data){
	((VideoAppSinkHandler*)data)->eos_cb();
}

}
}
