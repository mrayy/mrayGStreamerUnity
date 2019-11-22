
#ifndef VideoAppSinkHandler_h__
#define VideoAppSinkHandler_h__

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <memory>
#include "ImageInfo.h"
#include "ListenerContainer.h"
#include "IMutex.h"
#include "IAppSinkHandler.h"
#include "IGStreamerPlayer.h"


#include "CMyListener.h"


namespace mray
{
namespace video
{
	class VideoAppSinkHandlerData;

class VideoAppSinkHandler :public IAppSinkHandler
{
protected:
	VideoAppSinkHandlerData* m_data;
	GstAppSink* m_sink;
	std::shared_ptr<GstSample> 	buffer, prevBuffer;
	GstMapInfo mapinfo;


	video::EPixelFormat m_pixelFormat;
	//quick hack for YUV support: maximum of 3 surfaces
	GstImageFrame m_pixels[1];				// 24 bit: rgb
	GstImageFrame m_backPixels[1];
	bool			m_IsFrameNew;			// if we are new
	bool			m_HavePixelsChanged;
	bool			m_BackPixelsChanged;
	bool			m_IsAllocated;
	OS::IMutex*			m_mutex;
	Vector2d m_frameSize;
	int m_surfaceCount;

	int m_captureFPS;
	int m_frameCount;
	int m_samplesCount;
	float m_timeAcc;
	float m_lastT;

	uint m_frameID;
	GstMyListener* m_rtpDataListener;
	GstMyListener* m_preappsrcListener;

	//DECLARE_FIRE_METHOD(OnStreamPrepared, (VideoAppSinkHandler*v), (v));

	bool _Allocate(int width, int height, video::EPixelFormat fmt);
public:
	VideoAppSinkHandler();
	virtual ~VideoAppSinkHandler();

	void SetSink(GstAppSink* s) {
		m_sink = s;
		m_samplesCount= 0;
	}
	void SetRTPListener(GstMyListener* preRTP, GstMyListener* postRTP, GstMyListener* preapp);

	void Close();
	bool 			isFrameNew(){ return m_IsFrameNew; }
	video::ImageInfo*	getPixelsRef(int surface = 0){ return &m_pixels[surface].data; }
	GstImageFrame*	getPixelFrame(int surface = 0){ return &m_pixels[surface]; }
	int getSurfaceCount(){ return m_surfaceCount; }
	bool GrabFrame();
	uint GetFrameID(){ return m_frameID; }
	virtual const Vector2d& GetFrameSize(){ return m_frameSize; }

	float GetCaptureFrameRate();

	int GetSamplesCount() { return m_samplesCount; }

	virtual GstFlowReturn process_sample(std::shared_ptr<GstSample> sample);
};

}
}

#endif // VideoAppSinkHandler_h__

