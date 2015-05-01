
#ifndef VideoAppSinkHandler_h__
#define VideoAppSinkHandler_h__

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <memory>
#include "ImageInfo.h"
#include "ListenerContainer.h"
#include "IMutex.h"
#include "Point2d.h"

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

	video::ImageInfo m_pixels;				// 24 bit: rgb
	video::ImageInfo m_backPixels;
	bool			m_IsFrameNew;			// if we are new
	bool			m_HavePixelsChanged;
	bool			m_BackPixelsChanged;
	bool			m_IsAllocated;
	OS::IMutex*			m_mutex;
	math::vector2di m_frameSize;

	uint m_frameID;

	DECLARE_FIRE_METHOD(OnStreamPrepared, (VideoAppSinkHandler*v), (v));

	bool _Allocate(int width, int height, video::EPixelFormat fmt);
public:
	VideoAppSinkHandler();
	virtual ~VideoAppSinkHandler();

	void SetSink(GstAppSink* s){ m_sink = s; }

	void Close();
	bool 			isFrameNew(){ return m_IsFrameNew; }
	video::ImageInfo*	getPixelsRef(){ return &m_pixels; }
	bool GrabFrame();
	uint GetFrameID(){ return m_frameID; }
	virtual const math::vector2di& GetFrameSize(){ return m_frameSize; }

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

