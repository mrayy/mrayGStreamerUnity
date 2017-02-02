

#include "stdafx.h"
/* FIXME 0.11: suppress warnings for deprecated API such as GValueArray
* with newer GLib versions (>= 2.31.0) */
#define GLIB_DISABLE_DEPRECATION_WARNINGS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "CMySink.h"

#include <string.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifndef G_OS_WIN32
#include <netinet/in.h>
#endif

#include <gst/gstelement.h>


GST_DEBUG_CATEGORY_STATIC(mysink_debug);
#define GST_CAT_DEFAULT (mysink_debug)

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE("sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS_ANY);


static void gst_mysink_finalize(GObject * object);

static GstFlowReturn gst_mysink_render(GstBaseSink * sink,
	GstBuffer * buffer);

static gboolean gst_mysink_start(GstBaseSink * bsink);
static gboolean gst_mysink_stop(GstBaseSink * bsink);
static gboolean gst_mysink_unlock(GstBaseSink * bsink);
static gboolean gst_mysink_unlock_stop(GstBaseSink * bsink);

static void gst_mysink_set_property(GObject * object, guint prop_id,
	const GValue * value, GParamSpec * pspec);
static void gst_mysink_get_property(GObject * object, guint prop_id,
	GValue * value, GParamSpec * pspec);


//static guint gst_mysink_signals[LAST_SIGNAL] = { 0 };

#define gst_mysink_parent_class parent_class
G_DEFINE_TYPE(GstMySink, gst_mysink, GST_TYPE_BASE_SINK);

static void
gst_mysink_class_init(GstMySinkClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;
	GstBaseSinkClass *gstbasesink_class;

	gobject_class = (GObjectClass *)klass;
	gstelement_class = (GstElementClass *)klass;
	gstbasesink_class = (GstBaseSinkClass *)klass;

	gobject_class->set_property = gst_mysink_set_property;
	gobject_class->get_property = gst_mysink_get_property;
	gobject_class->finalize = gst_mysink_finalize;

	/**
	* GstMySink::add:
	* @GstMySink: the sink on which the signal is emitted
	* @host: the hostname/IP address of the client to add
	* @port: the port of the client to add
	*
	* Add a client with destination @host and @port to the list of
	* clients. When the same host/port pair is added multiple times, the
	* send-duplicates property defines if the packets are sent multiple times to
	* the same host/port pair or not.
	*
	* When a host/port pair is added multiple times, an equal amount of remove
	* calls must be performed to actually remove the host/port pair from the list
	* of destinations.
	*/
#if 0
	gst_mysink_signals[SIGNAL_ADD] =
		g_signal_new("add", G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_LAST ,
		G_STRUCT_OFFSET(GstMySinkClass, add),
		NULL, NULL, g_cclosure_marshal_generic, G_TYPE_NONE, 2,
		G_TYPE_STRING, G_TYPE_INT);
	/**
	* GstMySink::remove:
	* @GstMySink: the sink on which the signal is emitted
	* @host: the hostname/IP address of the client to remove
	* @port: the port of the client to remove
	*
	* Remove the client with destination @host and @port from the list of
	* clients.
	*/
	gst_mysink_signals[SIGNAL_REMOVE] =
		g_signal_new("remove", G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_LAST ,
		G_STRUCT_OFFSET(GstMySinkClass, remove),
		NULL, NULL, g_cclosure_marshal_generic, G_TYPE_NONE, 2,
		G_TYPE_STRING, G_TYPE_INT);
	/**
	* GstMySink::clear:
	* @GstMySink: the sink on which the signal is emitted
	*
	* Clear the list of clients.
	*/
	gst_mysink_signals[SIGNAL_CLEAR] =
		g_signal_new("clear", G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_LAST ,
		G_STRUCT_OFFSET(GstMySinkClass, clear),
		NULL, NULL, g_cclosure_marshal_generic, G_TYPE_NONE, 0);
	/**
	* GstMySink::get-stats:
	* @GstMySink: the sink on which the signal is emitted
	* @host: the hostname/IP address of the client to get stats on
	* @port: the port of the client to get stats on
	*
	* Get the statistics of the client with destination @host and @port.
	*
	* Returns: a GstStructure: bytes_sent, packets_sent,
	*           connect_time (in epoch seconds), disconnect_time (in epoch seconds)
	*/
	gst_mysink_signals[SIGNAL_GET_STATS] =
		g_signal_new("get-stats", G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_LAST ,
		G_STRUCT_OFFSET(GstMySinkClass, get_stats),
		NULL, NULL, g_cclosure_marshal_generic, GST_TYPE_STRUCTURE, 2,
		G_TYPE_STRING, G_TYPE_INT);
	/**
	* GstMySink::client-added:
	* @GstMySink: the sink emitting the signal
	* @host: the hostname/IP address of the added client
	* @port: the port of the added client
	*
	* Signal emited when a new client is added to the list of
	* clients.
	*/
	gst_mysink_signals[SIGNAL_CLIENT_ADDED] =
		g_signal_new("client-added", G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET(GstMySinkClass, client_added),
		NULL, NULL, g_cclosure_marshal_generic, G_TYPE_NONE, 2,
		G_TYPE_STRING, G_TYPE_INT);
	/**
	* GstMySink::client-removed:
	* @GstMySink: the sink emitting the signal
	* @host: the hostname/IP address of the removed client
	* @port: the port of the removed client
	*
	* Signal emited when a client is removed from the list of
	* clients.
	*/
	gst_mysink_signals[SIGNAL_CLIENT_REMOVED] =
		g_signal_new("client-removed", G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET(GstMySinkClass,
		client_removed), NULL, NULL, g_cclosure_marshal_generic,
		G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_INT);
#endif
	gst_element_class_add_pad_template(gstelement_class,
		gst_static_pad_template_get(&sink_template));

	//gst_element_class_set_details()
	gst_element_class_set_details_simple(gstelement_class, "MYSink class",
		"Sink",
		"Custom App Sink",
		"MHD Yamen Saraiji <mrayyamen@gmail.com>");
	gstbasesink_class->render = gst_mysink_render;
	gstbasesink_class->start = gst_mysink_start;
	gstbasesink_class->stop = gst_mysink_stop;
	gstbasesink_class->unlock = gst_mysink_unlock;
	gstbasesink_class->unlock_stop = gst_mysink_unlock_stop;


	GST_DEBUG_CATEGORY_INIT(mysink_debug, "mysink", 0, "My sink");
}


static void
gst_mysink_init(GstMySink * sink)
{
	sink->new_buffer = 0;
	sink->data = 0;
}

static void
gst_mysink_finalize(GObject * object)
{
	GstMySink *sink;

	sink = GST_MYSINK(object);

	G_OBJECT_CLASS(parent_class)->finalize(object);
}

static GstFlowReturn
gst_mysink_render(GstBaseSink * bsink, GstBuffer * buffer)
{
	GstMySink *sink;
	GError *err = NULL;

	sink = GST_MYSINK(bsink);
	if (sink->new_buffer)
	{
		return sink->new_buffer(sink,sink->data, buffer);
	}
		return GST_FLOW_OK;
}


static void
gst_mysink_set_property(GObject * object, guint prop_id,
const GValue * value, GParamSpec * pspec)
{
	GstMySink *sink = GST_MYSINK(object);

}

static void
gst_mysink_get_property(GObject * object, guint prop_id, GValue * value,
GParamSpec * pspec)
{
	GstMySink *sink=GST_MYSINK(object);

}


static gboolean
gst_mysink_start(GstBaseSink * bsink)
{
	GstMySink *sink;
	GError *err = NULL;

	sink = GST_MYSINK(bsink);

	return TRUE;
}

static gboolean
gst_mysink_stop(GstBaseSink * bsink)
{
	GstMySink *udpsink;

	udpsink = GST_MYSINK(bsink);


	return TRUE;
}

void
gst_mysink_clear(GstMySink * sink)
{
}

static gboolean
gst_mysink_unlock(GstBaseSink * bsink)
{
	GstMySink *sink;

	sink = GST_MYSINK(bsink);

	//g_cancellable_cancel(sink->cancellable);

	return TRUE;
}

static gboolean
gst_mysink_unlock_stop(GstBaseSink * bsink)
{
	GstMySink *sink;

	sink = GST_MYSINK(bsink);

	//g_cancellable_reset(sink->cancellable);
	return TRUE;
}

gboolean
_GstMySinkClass::plugin_init(GstPlugin * plugin)
{
	if (!gst_element_register(plugin, "mysink", GST_RANK_NONE,
		GST_TYPE_MYSINK))
		return FALSE;

	return TRUE;
}

/*#define PACKAGE ""
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
	GST_VERSION_MINOR,
	"mysink",
	"Custom app sink",
	_GstMySinkClass::plugin_init, "", "", "", "")
	*/