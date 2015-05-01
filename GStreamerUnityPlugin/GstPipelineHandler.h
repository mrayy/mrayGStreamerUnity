

#ifndef GstPipelineHandler_h__
#define GstPipelineHandler_h__


#include <gst/gst.h>


namespace mray
{
namespace video
{
	
class GstPipelineHandler
{
protected:
	guint m_busWatchID;
	GstElement* m_gstPipeline;
	bool m_paused;
	bool m_Loaded;
	bool m_playing;
	bool m_closing;
public:
	GstPipelineHandler();
	virtual ~GstPipelineHandler();

	virtual bool CreatePipeline();
	virtual void SetPaused(bool p);
	virtual void Stop();
	virtual bool IsLoaded();
	virtual bool IsPlaying();
	virtual void Close();
	virtual bool HandleMessage(GstBus * bus, GstMessage * msg);


	static bool busFunction(GstBus * bus, GstMessage * message, GstPipelineHandler * player);


	
};

}
}

#endif // GstPipelineHandler_h__
