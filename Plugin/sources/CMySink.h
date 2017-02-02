

/********************************************************************
	created:	2013/12/07
	created:	7:12:2013   14:47
	filename: 	C:\Development\mrayEngine\Projects\TelubeeRobotAgent\CMySink.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeRobotAgent
	file base:	CMySink
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __CMySink__
#define __CMySink__


#include <gst/gst.h>

#include <gst/base/gstbasesink.h>
#include <gio/gio.h>


G_BEGIN_DECLS


#define GST_TYPE_MYSINK                (gst_mysink_get_type())
#define GST_MYSINK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MYSINK,GstMySink))
#define GST_MYSINK_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MYSINK,GstMySinkClass))
#define GST_IS_MYSINK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MYSINK))
#define GST_IS_MYSINK_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MYSINK))

typedef struct _GstMySink GstMySink;
typedef struct _GstMySinkClass GstMySinkClass;

struct _GstMySink {
	GstBaseSink parent;

	gpointer data;
	GstFlowReturn(*new_buffer)(GstMySink * sink, gpointer data,GstBuffer * buffer);
};

struct _GstMySinkClass {
	GstBaseSinkClass parent_class;
	/* element methods */

	static gboolean plugin_init(GstPlugin * plugin);
};

GType gst_mysink_get_type(void);


G_END_DECLS



#endif
