

#include "stdafx.h"
#include "cmysrc.h"



static GstFlowReturn gst_mysrc_create(GstPushSrc * psrc, GstBuffer ** buf);

static gboolean gst_mysrc_start(GstBaseSrc * bsrc);

static gboolean gst_mysrc_stop(GstBaseSrc * bsrc);

static gboolean gst_mysrc_unlock(GstBaseSrc * bsrc);

static gboolean gst_mysrc_unlock_stop(GstBaseSrc * bsrc);

static void gst_mysrc_finalize(GObject * object);

static void gst_mysrc_set_property(GObject * object, guint prop_id,
	const GValue * value, GParamSpec * pspec);
static void gst_mysrc_get_property(GObject * object, guint prop_id,
	GValue * value, GParamSpec * pspec);

#define gst_mysrc_parent_class parent_class
G_DEFINE_TYPE(GstMySrc, gst_mysrc, GST_TYPE_PUSH_SRC);

GST_DEBUG_CATEGORY_STATIC(mysrc_debug);
#define GST_CAT_DEFAULT (mysrc_debug)
static GstStaticPadTemplate mysrc_template = GST_STATIC_PAD_TEMPLATE("src",
	GST_PAD_SRC,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS_ANY);

#if GST_VERSION_MAJOR==0
static GstCaps *
gst_mysrc_getcaps(GstBaseSrc * src)
#else
static GstCaps *
gst_mysrc_getcaps(GstBaseSrc * src, GstCaps * filter)
#endif
{
	GstMySrc *udpsrc;

	udpsrc = GST_MySRC(src);

	if (!udpsrc->caps)
	{
		udpsrc->caps=gst_caps_new_any();
	}
		
	return gst_caps_ref(udpsrc->caps);

}

enum
{
	PROP_0,

	PROP_CAPS,

	PROP_LAST
};
static void
gst_mysrc_class_init(GstMySrcClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;
	GstBaseSrcClass *gstbasesrc_class;
	GstPushSrcClass *gstpushsrc_class;

	gobject_class = (GObjectClass *)klass;
	gstelement_class = (GstElementClass *)klass;
	gstbasesrc_class = (GstBaseSrcClass *)klass;
	gstpushsrc_class = (GstPushSrcClass *)klass;


 	gobject_class->set_property = gst_mysrc_set_property;
 	gobject_class->get_property = gst_mysrc_get_property;
 	gobject_class->finalize = gst_mysrc_finalize;

	GST_DEBUG_CATEGORY_INIT(mysrc_debug, "mysrc", 0, "My src");

	gst_element_class_add_pad_template(gstelement_class,
		gst_static_pad_template_get(&mysrc_template));

	gst_element_class_set_details_simple(gstelement_class,
		"MYSrc class",
		"Source",
		"Custom App Source",
		"MHD Yamen Saraiji <mrayyamen@gmail.com>");
	 
	g_object_class_install_property(gobject_class, PROP_CAPS,
		g_param_spec_boxed("caps", "Caps",
		"The caps of the source pad", GST_TYPE_CAPS,
		GParamFlags(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

	gstbasesrc_class->start = gst_mysrc_start;
	gstbasesrc_class->stop = gst_mysrc_stop;
	gstbasesrc_class->unlock = gst_mysrc_unlock;
	gstbasesrc_class->unlock_stop = gst_mysrc_unlock_stop;
	gstbasesrc_class->get_caps = gst_mysrc_getcaps;

	gstpushsrc_class->create = gst_mysrc_create;
}

static void
gst_mysrc_init(GstMySrc * mysrc)
{
	/* configure basesrc to be a live source */
	gst_base_src_set_live(GST_BASE_SRC(mysrc), TRUE);
	/* make basesrc output a segment in time */
	gst_base_src_set_format(GST_BASE_SRC(mysrc), GST_FORMAT_TIME);
	/* make basesrc set timestamps on outgoing buffers based on the running_time
	* when they were captured */
	gst_base_src_set_do_timestamp(GST_BASE_SRC(mysrc), TRUE);
}

static void
gst_mysrc_finalize(GObject * object)
{
	GstMySrc *mysrc;

	mysrc = GST_MySRC(object);

	if (mysrc->caps)
		gst_caps_unref(mysrc->caps);
	mysrc->caps = NULL;
	G_OBJECT_CLASS(parent_class)->finalize(object);
}


static GstFlowReturn
gst_mysrc_create(GstPushSrc * psrc, GstBuffer ** buf)
{
	GstFlowReturn ret=GST_FLOW_ERROR;
	GstMySrc *mysrc;

	mysrc = GST_MySRC(psrc);
	if (mysrc->need_buffer)
	{
		ret=mysrc->need_buffer(mysrc, mysrc->data, buf);
	}
	return ret;
}


static void
gst_mysrc_set_property(GObject * object, guint prop_id, const GValue * value,
GParamSpec * pspec)
{
	GstMySrc *mysrc = GST_MySRC(object);

	switch (prop_id)
	{
		case PROP_CAPS :
		{
			const GstCaps *new_caps_val = gst_value_get_caps(value);

			GstCaps *new_caps;

			GstCaps *old_caps;

			if (new_caps_val == NULL) {
				new_caps = gst_caps_new_any();
			}
			else {
				new_caps = gst_caps_copy(new_caps_val);
			}

			old_caps = mysrc->caps;
			mysrc->caps = new_caps;
			if (old_caps)
				gst_caps_unref(old_caps);
			gst_pad_set_caps(GST_BASE_SRC(mysrc)->srcpad, new_caps);
			break;
		}
	default:
		break;
	}

}

static void
gst_mysrc_get_property(GObject * object, guint prop_id, GValue * value,
GParamSpec * pspec)
{
	GstMySrc *mysrc = GST_MySRC(object);
	switch (prop_id)
	{
	case PROP_CAPS:
		gst_value_set_caps(value, mysrc->caps);
		break;
	}
}

/* create a socket for sending to remote machine */
static gboolean
gst_mysrc_start(GstBaseSrc * bsrc)
{
	GstMySrc *src;

	src = GST_MySRC(bsrc);


	return TRUE;

}

static gboolean
gst_mysrc_unlock(GstBaseSrc * bsrc)
{
	GstMySrc *src;

	src = GST_MySRC(bsrc);

	GST_LOG_OBJECT(src, "Flushing");

	return TRUE;
}

static gboolean
gst_mysrc_unlock_stop(GstBaseSrc * bsrc)
{
	GstMySrc *src;

	src = GST_MySRC(bsrc);

	GST_LOG_OBJECT(src, "No longer flushing");

	return TRUE;
}

static gboolean
gst_mysrc_stop(GstBaseSrc * bsrc)
{
	GstMySrc *src;

	src = GST_MySRC(bsrc);

	GST_DEBUG("stopping, closing sockets");

	return TRUE;
}

gboolean
_GstMySrcClass::plugin_init(GstPlugin * plugin)
{
	if (!gst_element_register(plugin, "mysrc", GST_RANK_NONE,
		GST_TYPE_MySRC))
		return FALSE;

	return TRUE;
}