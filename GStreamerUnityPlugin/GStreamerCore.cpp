

#include "stdafx.h"
#include "GStreamerCore.h"

#include "UnityHelpers.h"
#include <IThreadManager.h>

#include <gst/app/gstappsink.h>
#include <gst/video/video.h>

#include <glib-object.h>
#include <glib.h>
#include <algorithm>
#include "CMySrc.h"
#include "CMySink.h"
#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"

namespace mray
{
namespace video
{

GStreamerCore* GStreamerCore::m_instance=0;
uint GStreamerCore::m_refCount = 0;

static gboolean appsink_plugin_init(GstPlugin * plugin)
{
	gst_element_register(plugin, "appsink", GST_RANK_NONE, GST_TYPE_APP_SINK);

	return TRUE;
}


class GstMainLoopThread : public OS::IThreadFunction{
public:
	GMainLoop *main_loop;
	GstMainLoopThread()
		:main_loop(NULL)
	{

	}

	void setup(){
		//			startThread();
	}
	void execute(OS::IThread*caller, void*arg){
		main_loop = g_main_loop_new(NULL, FALSE);
		g_main_loop_run(main_loop);
		LogMessage("GST Thread shutdown",ELL_INFO);
	}
};





GStreamerCore::GStreamerCore()
{
	m_mainLoopThread = 0;
	_Init();
}

GStreamerCore::~GStreamerCore()
{
	_StopLoop();
	gst_deinit();
}


void g_logFunction(const gchar   *log_domain,
	GLogLevelFlags log_level,
	const gchar   *message,
	gpointer       user_data)
{
	LogMessage(message, ELL_INFO);
}

void GStreamerCore::_Init()
{

	GError *err = 0;
	_gst_debug_enabled = false; 
	if (!gst_init_check(0,0, &err))
	{
		LogMessage("GStreamerCore - Failed to init GStreamer!"+ std::string(err->message), ELL_ERROR);
	}
	else
	{
		g_log_set_handler(0,  G_LOG_LEVEL_CRITICAL, g_logFunction, 0);
		g_log_set_handler(0, G_LOG_FLAG_FATAL , g_logFunction, 0);
		g_log_set_default_handler(g_logFunction, 0);

		fclose(stderr);

		//register plugin path
		std::string gst_path = g_getenv("GSTREAMER_1_0_ROOT_X86");
		//putenv(("GST_PLUGIN_PATH_1_0=" + gst_path + "lib\\gstreamer-1.0" + ";.").c_str());
		//add our custom src/sink elements
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"appsink", (char*)"Element application sink",
			appsink_plugin_init, "0.1", "LGPL", "ofVideoPlayer", "openFrameworks",
			"http://openframeworks.cc/");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"mysrc", (char*)"Element application src",
			_GstMySrcClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
			"");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"mysink", (char*)"Element application sink",
			_GstMySinkClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
			"");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"myudpsrc", (char*)"Element udp src",
			_GstMyUDPSrcClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
			"");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"myudpsink", (char*)"Element udp sink",
			_GstMyUDPSinkClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
			"");
		LogMessage("GStreamerCore - GStreamer inited", ELL_INFO);
	}

#if GLIB_MINOR_VERSION<32
	if (!g_thread_supported()){
		g_thread_init(NULL);
	}
#endif
	_StartLoop();
}
void GStreamerCore::_StartLoop()
{
	m_threadFunc = new GstMainLoopThread();
	m_mainLoopThread = OS::IThreadManager::getInstance().createThread(m_threadFunc);
	m_mainLoopThread->start(0);
}

void GStreamerCore::_StopLoop()
{
	if (!m_threadFunc)
		return;
	GstMainLoopThread* mainLoop = (GstMainLoopThread*)m_threadFunc;
	g_main_loop_quit(mainLoop->main_loop);
	bool running = g_main_loop_is_running(mainLoop->main_loop);
	g_main_loop_unref(mainLoop->main_loop);
	delete m_threadFunc;
	OS::IThreadManager::getInstance().killThread(m_mainLoopThread);
	delete m_mainLoopThread;
	m_threadFunc = 0;
	m_mainLoopThread = 0;
}

		
void GStreamerCore::Ref()
{
	m_refCount++;
	if (m_refCount==1)
	{
		m_instance = new GStreamerCore();
	}

}

void GStreamerCore::Unref()
{
	if (m_refCount == 0)
	{
		LogMessage("GStreamerCore::Unref() - unreferencing GStreamer with no reference! ", ELL_ERROR);
		return;
	}
	//m_refCount--;
	if (m_refCount == 0)
	{
		delete m_instance;
		m_instance = 0;
	}
}

GStreamerCore* GStreamerCore::Instance()
{
	return m_instance;
}

}
}


