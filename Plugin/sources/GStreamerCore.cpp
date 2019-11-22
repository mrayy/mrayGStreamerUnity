

#include "stdafx.h"
#include "GStreamerCore.h"

#include "UnityHelpers.h"
#include <IThreadManager.h>

#include <gst/app/gstappsink.h>
#include <gst/video/video.h>

#include <glib-object.h>
#include <glib.h>
#include <algorithm>

#if false
#include "CMySrc.h"
#include "CMySink.h"
#endif

#ifdef USE_UNITY_NETWORK
#include "CMyUDPSrc.h"
#include "CMyUDPSink.h"
#include "CMyListener.h"
#endif

#ifdef __ANDROID__

void
gst_android_register_static_plugins(void);
/* Call this function to load GIO modules */
void
gst_android_load_gio_modules(void);
#endif


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
	gub_main_loop_thread = 0;
	gub_main_loop = 0;
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
    if(log_level==G_LOG_LEVEL_INFO || log_level==G_LOG_LEVEL_DEBUG)
        LogMessage(message, ELL_INFO);
    else if(log_level==G_LOG_LEVEL_WARNING)
        LogMessage(message, ELL_WARNING);
    else if(log_level==G_LOG_LEVEL_CRITICAL || log_level==G_LOG_FLAG_FATAL)
        LogMessage(message, ELL_ERROR);
    else
        LogMessage(message, ELL_INFO);
}

gpointer gst_main_loop_func(gpointer data)
{
	GStreamerCore* core = (GStreamerCore*)data;
	core->_loopFunction();

	return NULL;
}

void GStreamerCore::_Init()
{

	GError *err = 0;
	_gst_debug_enabled = false; 
	if (!gst_init_check(0,0, &err))
	{
		LogManager::Instance()->LogMessage("GStreamerCore - Failed to init GStreamer!");
	}
	else
    {
        g_log_set_handler(0,  G_LOG_LEVEL_WARNING, g_logFunction, 0);
        g_log_set_handler(0,  G_LOG_LEVEL_MESSAGE, g_logFunction, 0);
        g_log_set_handler(0,  G_LOG_LEVEL_INFO, g_logFunction, 0);
        g_log_set_handler(0,  G_LOG_LEVEL_DEBUG, g_logFunction, 0);
        g_log_set_handler(0,  G_LOG_LEVEL_CRITICAL, g_logFunction, 0);
		g_log_set_handler(0, G_LOG_FLAG_FATAL , g_logFunction, 0);
		g_log_set_default_handler(g_logFunction, 0);


#if defined (__ANDROID__)
		//gst_android_register_static_plugins();
		//gst_android_load_gio_modules();
#endif
        
        LogManager::Instance()->LogMessage("GStreamerCore - Registering Elements!");
		//fclose(stderr);
        
		//register plugin path
        
		//std::string gst_path = g_getenv("GSTREAMER_1_0_ROOT_X86_64");
		//putenv(("GST_PLUGIN_PATH_1_0=" + gst_path + "lib\\gstreamer-1.0" + ";.").c_str());
        //add our custom src/sink elements
#ifdef USE_UNITY_NETWORK
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"appsink", (char*)"Element application sink",
			appsink_plugin_init, "0.1", "LGPL", "ofVideoPlayer", "openFrameworks",
                                   "http://openframeworks.cc/");
	/*	gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"mysrc", (char*)"Element application src",
			_GstMySrcClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
			"");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"mysink", (char*)"Element application sink",
			_GstMySinkClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
                                   "");*/
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"myudpsrc", (char*)"Element udp src",
			_GstMyUDPSrcClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
			"");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"myudpsink", (char*)"Element udp sink",
			_GstMyUDPSinkClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
			"");

		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"mylistener", (char*)"Custom listener element",
			_GstMyListenerClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
			"");
#endif
		LogManager::Instance()->LogMessage("GStreamerCore - GStreamer inited");
	}

#if GLIB_MINOR_VERSION<32
	if (!g_thread_supported()){
		g_thread_init(NULL);
	}
#endif


	gub_main_loop_thread = g_thread_new("GstUnityBridge Main Thread", gst_main_loop_func, this);
	if (!gub_main_loop_thread) {
		LogManager::Instance()->LogMessage(std::string("Failed to create GLib main thread: ") + std::string(err ? err->message : "<No error message>"));
		return;
	}

	_StartLoop();
}


void GStreamerCore::_loopFunction() {
	LogManager::Instance()->LogMessage("Entering main loop");
	gub_main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(gub_main_loop);
	LogManager::Instance()->LogMessage("Quitting main loop");
}


void GStreamerCore::_StartLoop()
{
	/*m_threadFunc = new GstMainLoopThread();
	m_mainLoopThread = OS::IThreadManager::getInstance().createThread(m_threadFunc);
	m_mainLoopThread->start(0);*/
}

void GStreamerCore::_StopLoop()
{
	/*
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
	*/


	if (!gub_main_loop) {
		return;
	}
	g_main_loop_quit(gub_main_loop);
	gub_main_loop = NULL;
	g_thread_join(gub_main_loop_thread);
	gub_main_loop_thread = NULL;
}

		
void GStreamerCore::Ref()
{
	m_refCount++;
	if (m_refCount==1)
	{
		LogManager::Instance()->LogMessage("Initializing GStreamer");
		m_instance = new GStreamerCore();
		LogManager::Instance()->LogMessage("Initializing GStreamer - Done");
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


