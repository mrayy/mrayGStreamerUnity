
#include "stdafx.h"
#include "IAppSinkHandler.h"

namespace mray
{
namespace video
{


IAppSinkHandler::IAppSinkHandler()
{
	m_sink = 0;
}
IAppSinkHandler::~IAppSinkHandler()
{
	Close();
}
void IAppSinkHandler::Close()
{
}


GstFlowReturn IAppSinkHandler::preroll_cb(std::shared_ptr<GstSample> sample)
{
	GstFlowReturn ret = process_sample(sample);
	if (ret == GST_FLOW_OK){
		return GST_FLOW_OK;
	}
	else{
		return ret;
	}
}
GstFlowReturn IAppSinkHandler::buffer_cb(std::shared_ptr<GstSample> sample)
{


	GstFlowReturn ret = process_sample(sample);
	if (ret == GST_FLOW_OK){
		return GST_FLOW_OK;
	}
	else{
		return ret;
	}
}
void  IAppSinkHandler::eos_cb()
{
}


GstFlowReturn IAppSinkHandler::on_new_buffer_from_source(GstAppSink * elt, void * data){
#if GST_VERSION_MAJOR==0
	shared_ptr<GstBuffer> buffer(gst_app_sink_pull_buffer(GST_APP_SINK(elt)), &gst_buffer_unref);
#else
	std::shared_ptr<GstSample> buffer(gst_app_sink_pull_sample(GST_APP_SINK(elt)), &gst_sample_unref);
#endif
	return ((IAppSinkHandler*)data)->buffer_cb(buffer);
}

GstFlowReturn IAppSinkHandler::on_new_preroll_from_source(GstAppSink * elt, void * data){
#if GST_VERSION_MAJOR==0
	shared_ptr<GstBuffer> buffer(gst_app_sink_pull_preroll(GST_APP_SINK(elt)), &gst_buffer_unref);
#else
	std::shared_ptr<GstSample> buffer(gst_app_sink_pull_preroll(GST_APP_SINK(elt)), &gst_sample_unref);
#endif
	return ((IAppSinkHandler*)data)->preroll_cb(buffer);
}

void IAppSinkHandler::on_eos_from_source(GstAppSink * elt, void * data){
	((IAppSinkHandler*)data)->eos_cb();
}

}
}
