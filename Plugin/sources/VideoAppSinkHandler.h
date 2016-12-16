
#ifndef VideoAppSinkHandler_h__
#define VideoAppSinkHandler_h__

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <memory>
#include "ImageInfo.h"
#include "ListenerContainer.h"
#include "IMutex.h"

namespace mray
{
namespace video
{
	class VideoAppSinkHandler;

	class IVideoAppSinkHandlerListener
	{
	public: 
		virtual void OnStreamPrepared(VideoAppSinkHandler*){};
	};
	
class VideoAppSinkHandler:public ListenerContainer<IVideoAppSinkHandlerListener*>
{
protected:
	GstAppSink* m_sink;
	std::shared_ptr<GstSample> 	buffer, prevBuffer;
	GstMapInfo mapinfo;

	video::EPixelFormat m_pixelFormat;
	//quick hack for YUV support: maximum of 3 surfaces
	video::ImageInfo m_pixels[1];				// 24 bit: rgb
	video::ImageInfo m_backPixels[1];
	bool			m_IsFrameNew;			// if we are new
	bool			m_HavePixelsChanged;
	bool			m_BackPixelsChanged;
	bool			m_IsAllocated;
	OS::IMutex*			m_mutex;
	Vector2d m_frameSize;
	int m_surfaceCount;

	int m_captureFPS;
	int m_frameCount;
	float m_timeAcc;
	float m_lastT;

	uint m_frameID;

	DECLARE_FIRE_METHOD(OnStreamPrepared, (VideoAppSinkHandler*v), (v));

	bool _Allocate(int width, int height, video::EPixelFormat fmt);
public:
	VideoAppSinkHandler();
	virtual ~VideoAppSinkHandler();

	void SetSink(GstAppSink* s){ m_sink = s; }

	void Close();
	bool 			isFrameNew(){ return m_IsFrameNew; }
	video::ImageInfo*	getPixelsRef(int surface=0){ return &m_pixels[surface]; }
	int getSurfaceCount(){ return m_surfaceCount; }
	bool GrabFrame();
	uint GetFrameID(){ return m_frameID; }
	virtual const Vector2d& GetFrameSize(){ return m_frameSize; }

	float GetCaptureFrameRate();

	virtual GstFlowReturn process_sample(std::shared_ptr<GstSample> sample);
	virtual GstFlowReturn preroll_cb(std::shared_ptr<GstSample> buffer);
	virtual GstFlowReturn buffer_cb(std::shared_ptr<GstSample> buffer);
	virtual void 		  eos_cb();


	static GstFlowReturn on_new_buffer_from_source(GstAppSink * elt, void * data);
	static GstFlowReturn on_new_preroll_from_source(GstAppSink * elt, void * data);
	static void on_eos_from_source(GstAppSink * elt, void * data);
};

}
}

#endif // VideoAppSinkHandler_h__

