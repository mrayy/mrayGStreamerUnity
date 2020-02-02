

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

extern "C" {
void
gst_android_register_static_plugins(void);
/* Call this function to load GIO modules */
void
gst_android_load_gio_modules(void);
}
#endif


namespace mray
{
namespace video
{

GStreamerCore* GStreamerCore::m_instance=0;
uint GStreamerCore::m_refCount = 0;

static GstClockTime _priv_gst_info_start_time;

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
		LogMessage(ELL_INFO,"GST Thread shutdown");
	}
};





GStreamerCore::GStreamerCore()
{
	m_mainLoopThread = 0;
	gub_main_loop_thread = 0;
	gub_main_loop = 0;
	_Init();
}

GStreamerCore::~GStreamerCore()
{
	_StopLoop();
	gst_deinit();
}



static void gst_debug_gub(GstDebugCategory * category, GstDebugLevel level,
	const gchar * file, const gchar * function, gint line,
	GObject * object, GstDebugMessage * message, gpointer unused)
{
	GstClockTime elapsed;
	gchar *tag;
	const gchar *level_str;

	if (level > gst_debug_category_get_threshold(category))
		return;

	elapsed = GST_CLOCK_DIFF(_priv_gst_info_start_time,
		gst_util_get_timestamp());

	switch (level) {
	case GST_LEVEL_ERROR:
		level_str = "ERR";
		break;
	case GST_LEVEL_WARNING:
		level_str = "WRN";
		break;
	case GST_LEVEL_INFO:
		level_str = "NFO";
		break;
	case GST_LEVEL_DEBUG:
		level_str = "DBG";
		break;
	default:
		level_str = "LOG";
		break;
	}

	tag = g_strdup_printf("%s", gst_debug_category_get_name(category));

	if (object) {
		gchar *obj;

		if (GST_IS_PAD(object) && GST_OBJECT_NAME(object)) {
			obj = g_strdup_printf("<%s:%s>", GST_DEBUG_PAD_NAME(object));
		}
		else if (GST_IS_OBJECT(object) && GST_OBJECT_NAME(object)) {
			obj = g_strdup_printf("<%s>", GST_OBJECT_NAME(object));
		}
		else if (G_IS_OBJECT(object)) {
			obj = g_strdup_printf("<%s@%p>", G_OBJECT_TYPE_NAME(object), object);
		}
		else {
			obj = g_strdup_printf("<%p>", object);
		}

		LogMessage(ELL_INFO,
			"%" GST_TIME_FORMAT " %p %s %s %s:%d:%s:%s %s",
			GST_TIME_ARGS(elapsed), g_thread_self(), level_str, tag,
			file, line, function, obj, gst_debug_message_get(message));

		g_free(obj);
	}
	else {
		LogMessage(ELL_INFO,
			"%" GST_TIME_FORMAT " %p %s %s %s:%d:%s %s",
			GST_TIME_ARGS(elapsed), g_thread_self(), level_str, tag,
			file, line, function, gst_debug_message_get(message));
	}
	g_free(tag);
}
void g_logFunction(const gchar   *log_domain,
	GLogLevelFlags log_level,
	const gchar   *message,
	gpointer       user_data)
{
    if(log_level==G_LOG_LEVEL_INFO || log_level==G_LOG_LEVEL_DEBUG)
        LogMessage(ELL_INFO,message);
    else if(log_level==G_LOG_LEVEL_WARNING)
        LogMessage(ELL_WARNING,message);
    else if(log_level==G_LOG_LEVEL_CRITICAL || log_level==G_LOG_FLAG_FATAL)
        LogMessage(ELL_ERROR,message);
    else
        LogMessage(ELL_INFO,message);
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
		LogMessage(ELL_WARNING,"GStreamerCore - Failed to init GStreamer!");
	}
	else
	{

		_priv_gst_info_start_time = gst_util_get_timestamp();
		gst_debug_remove_log_function(gst_debug_log_default);
		gst_debug_add_log_function((GstLogFunction)gst_debug_gub, NULL, NULL);
/*
        g_log_set_handler(0,  G_LOG_LEVEL_WARNING, g_logFunction, 0);
        g_log_set_handler(0,  G_LOG_LEVEL_MESSAGE, g_logFunction, 0);
        g_log_set_handler(0,  G_LOG_LEVEL_INFO, g_logFunction, 0);
        g_log_set_handler(0,  G_LOG_LEVEL_DEBUG, g_logFunction, 0);
        g_log_set_handler(0,  G_LOG_LEVEL_CRITICAL, g_logFunction, 0);
		g_log_set_handler(0, G_LOG_FLAG_FATAL , g_logFunction, 0);
		g_log_set_default_handler(g_logFunction, 0);
		*/

		gst_debug_set_active(FALSE);
		//gst_debug_set_threshold_from_string("2,dashdemux:5", TRUE);

#if defined (__ANDROID__)
		gst_android_register_static_plugins();
		gst_android_load_gio_modules();
#endif
        
        LogMessage(ELL_INFO,"GStreamerCore - Registering Elements!");
		//fclose(stderr);
        
		//register plugin path
        
		//std::string gst_path = g_getenv("GSTREAMER_1_0_ROOT_X86_64");
		//putenv(("GST_PLUGIN_PATH_1_0=" + gst_path + "lib\\gstreamer-1.0" + ";.").c_str());
        //add our custom src/sink elements
#ifdef USE_UNITY_NETWORK
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"appsink", (char*)"Element application sink",
			appsink_plugin_init, "0.1", "LGPL", "GStreamerUnity", "mray",
                                   "http://openframeworks.cc/");
	/*	gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"mysrc", (char*)"Element application src",
			_GstMySrcClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
			"");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"mysink", (char*)"Element application sink",
			_GstMySinkClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "mray",
                                   "");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"myudpsrc", (char*)"Element udp src",
			_GstMyUDPSrcClass::plugin_init, "0.1", "LGPL", "GStreamerUnity", "mray",
			"");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"myudpsink", (char*)"Element udp sink",
			_GstMyUDPSinkClass::plugin_init, "0.1", "LGPL", "GStreamerUnity", "mray",
			"");*/

		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"mylistener", (char*)"Custom listener element",
			_GstMyListenerClass::plugin_init, "0.1", "LGPL", "GStreamerUnity", "mray",
			"http://gstreamer.net/");
#endif

		gchar* version = gst_version_string();
		LogMessage(ELL_INFO,"%s initialized", version);
		g_free(version);
		LogMessage(ELL_INFO,"GStreamerCore - GStreamer inited");
	}

#if GLIB_MINOR_VERSION<32
	if (!g_thread_supported()){
		g_thread_init(NULL);
	}
#endif


	/**/

	_StartLoop();
}


void GStreamerCore::_loopFunction() {
	LogMessage(ELL_INFO,"Entering main loop");
	gub_main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(gub_main_loop);
	LogMessage(ELL_INFO,"Quitting main loop");
}


void GStreamerCore::_StartLoop()
{
	if (true)
	{
		GError *err = 0;
		gub_main_loop_thread = g_thread_new("GStreamerUnityPlugin Main Thread", gst_main_loop_func, this);
		if (!gub_main_loop_thread) {
			LogMessage(ELL_INFO, "Failed to create GLib main thread: %s", err ? err->message : "<No error message>");
			return;
		}

	}
	else {
		m_threadFunc = new GstMainLoopThread();
		m_mainLoopThread = OS::IThreadManager::getInstance().createThread(m_threadFunc);
		m_mainLoopThread->start(0);
	}
}

void GStreamerCore::_StopLoop()
{
	if (true) {

		if (!gub_main_loop) {
			return;
		}
		g_main_loop_quit(gub_main_loop);
		gub_main_loop = NULL;
		g_thread_join(gub_main_loop_thread);
		gub_main_loop_thread = NULL;
	}else{
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

}

		
void GStreamerCore::Ref()
{
	m_refCount++;
	if (m_refCount==1)
	{
		LogMessage(ELL_INFO,"Initializing GStreamer");
		m_instance = new GStreamerCore();
		LogMessage(ELL_INFO,"Initializing GStreamer - Done");
	}

}

void GStreamerCore::Unref()
{
	if (m_refCount == 0)
	{
		LogMessage(ELL_ERROR,"GStreamerCore::Unref() - unreferencing GStreamer with no reference! ");
		return;
	}
	m_refCount--;
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


