

#include "stdafx.h"
#include "cMyListener.h"

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

GST_DEBUG_CATEGORY_STATIC(gst_my_listener_debug);
#define GST_CAT_DEFAULT gst_my_listener_debug

/* Filter signals and args */
enum
{
	/* FILL ME */
	LAST_SIGNAL
};

enum
{
	PROP_0,
	PROP_SILENT
};

/* the capabilities of the inputs and outputs.
*
* describe the real formats here.
*/
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS("ANY")
	);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE("src",
	GST_PAD_SRC,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS("ANY")
	);

#define gst_my_listener_parent_class parent_class
G_DEFINE_TYPE(GstMyListener, gst_my_listener, GST_TYPE_ELEMENT);

static void gst_my_listener_set_property(GObject * object, guint prop_id,
	const GValue * value, GParamSpec * pspec);
static void gst_my_listener_get_property(GObject * object, guint prop_id,
	GValue * value, GParamSpec * pspec);

static gboolean gst_my_listener_sink_event(GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_my_listener_chain(GstPad * pad, GstObject * parent, GstBuffer * buf);

static void gst_my_listener_finalize(GObject * object);

/* GObject vmethod implementations */

/* initialize the MyListener's class */
static void
gst_my_listener_class_init(GstMyListenerClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;

	gobject_class = (GObjectClass *)klass;
	gstelement_class = (GstElementClass *)klass;

	gobject_class->set_property = gst_my_listener_set_property;
	gobject_class->get_property = gst_my_listener_get_property;
	gobject_class->finalize = gst_my_listener_finalize;


	g_object_class_install_property(gobject_class, PROP_SILENT,
		g_param_spec_boolean("silent", "Silent", "Produce verbose output ?",
		FALSE, G_PARAM_READWRITE));

	gst_element_class_set_details_simple(gstelement_class,
		"MyListener",
		"FIXME:Generic",
		"FIXME:Generic Template Element",
		" <<user@hostname.org>>");

	gst_element_class_add_pad_template(gstelement_class,
		gst_static_pad_template_get(&src_factory));
	gst_element_class_add_pad_template(gstelement_class,
		gst_static_pad_template_get(&sink_factory));
}

/* initialize the new element
* instantiate pads and add them to element
* set pad calback functions
* initialize instance structure
*/
static void
gst_my_listener_init(GstMyListener * filter)
{
	filter->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
	gst_pad_set_event_function(filter->sinkpad,
		GST_DEBUG_FUNCPTR(gst_my_listener_sink_event));
	gst_pad_set_chain_function(filter->sinkpad,
		GST_DEBUG_FUNCPTR(gst_my_listener_chain));
	GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
	gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

	filter->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
	GST_PAD_SET_PROXY_CAPS(filter->srcpad);
	gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

	filter->silent = FALSE;

	filter->listeners = new MyListenerContainer();
}

static void
gst_my_listener_finalize(GObject * object)
{
	GstMyListener *sink;

	sink = GST_MyListener(object);

	if (sink->listeners)
	{
		delete sink->listeners;
	}
	G_OBJECT_CLASS(parent_class)->finalize(object);
}


static void
gst_my_listener_set_property(GObject * object, guint prop_id,
const GValue * value, GParamSpec * pspec)
{
	GstMyListener *filter = GST_MyListener(object);

	switch (prop_id) {
	case PROP_SILENT:
		filter->silent = g_value_get_boolean(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void
gst_my_listener_get_property(GObject * object, guint prop_id,
GValue * value, GParamSpec * pspec)
{
	GstMyListener *filter = GST_MyListener(object);

	switch (prop_id) {
	case PROP_SILENT:
		g_value_set_boolean(value, filter->silent);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_my_listener_sink_event(GstPad * pad, GstObject * parent, GstEvent * event)
{
	gboolean ret;
	GstMyListener *filter;

	filter = GST_MyListener(parent);

	switch (GST_EVENT_TYPE(event)) {
	case GST_EVENT_CAPS:
	{
						   GstCaps * caps;

						   gst_event_parse_caps(event, &caps);
						   /* do something with the caps */

						   /* and forward */
						   ret = gst_pad_event_default(pad, parent, event);
						   break;
	}
	default:
		ret = gst_pad_event_default(pad, parent, event);
		break;
	}
	return ret;
}

/* chain function
* this function does the actual processing
*/
static GstFlowReturn
gst_my_listener_chain(GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	GstMyListener *filter;

	filter = GST_MyListener(parent);

	if (filter->silent == FALSE)
	{
		filter->listeners->__FIRE_ListenerOnDataChained(filter, buf);
	}

	/* just push out the incoming buffer without touching it */
	return gst_pad_push(filter->srcpad, buf);
}


/* entry point to initialize the plug-in
* initialize the plug-in itself
* register the element factories and other features
*/
static gboolean
MyListener_init(GstPlugin * MyListener)
{
	/* debug category for fltering log messages
	*
	* exchange the string 'Template MyListener' with your description
	*/
	GST_DEBUG_CATEGORY_INIT(gst_my_listener_debug, "MyListener",
		0, "Template MyListener");

	return gst_element_register(MyListener, "MyListener", GST_RANK_NONE,
		GST_TYPE_MyListener);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
* in configure.ac and then written into and defined in config.h, but we can
* just set it ourselves here in case someone doesn't use autotools to
* compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
*/
#ifndef PACKAGE
#define PACKAGE "MyListener"
#endif

/* gstreamer looks for this structure to register MyListeners
*
* exchange the string 'Template MyListener' with your MyListener description
*/
/*
GST_PLUGIN_DEFINE(
	GST_VERSION_MAJOR,
	GST_VERSION_MINOR,
	MyListener,
	"Template MyListener",
	MyListener_init,
	"1.0",
	"LGPL",
	"GStreamer",
	"http://gstreamer.net/"
	)
	
	*/

/*** GSTURIHANDLER INTERFACE *************************************************/

static GstURIType
gst_udpsrc_uri_get_type(GType type)
{
	return GST_URI_SRC;
}
gboolean
_GstMyListenerClass::plugin_init(GstPlugin * plugin)
{
	if (!gst_element_register(plugin, "mylistener", GST_RANK_NONE,
		GST_TYPE_MyListener))
		return FALSE;

	return TRUE;
}