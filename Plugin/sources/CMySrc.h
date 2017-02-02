

/********************************************************************
	created:	2013/12/08
	created:	8:12:2013   0:07
	filename: 	C:\Development\mrayEngine\Projects\TelubeeRobotAgent\CMySrc.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeRobotAgent
	file base:	CMySrc
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __CMySrc__
#define __CMySrc__

#include <gst/gst.h>
#include <gst/base/gstpushsrc.h>
#include <gio/gio.h>

G_BEGIN_DECLS


#define GST_TYPE_MySRC (gst_mysrc_get_type())
#define GST_MySRC(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_MySRC, GstMySrc))
#define GST_MySRC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_MySRC, GstMySrcClass))
#define GST_IS_MySRC(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_MySRC))
#define GST_IS_MySRC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_MySRC))
#define GST_MySRC_CAST(obj) ((GstMySrc *)(obj))

typedef struct _GstMySrc GstMySrc;
typedef struct _GstMySrcClass GstMySrcClass;

struct _GstMySrc {
	GstPushSrc parent;

	GstCaps* caps;
	gpointer data;
	GstFlowReturn(*need_buffer)(GstMySrc * sink, gpointer data, GstBuffer ** buffer);

};

struct _GstMySrcClass {
	GstPushSrcClass parent_class;
	static gboolean plugin_init(GstPlugin * plugin);
};

GType gst_mysrc_get_type(void);

G_END_DECLS


#endif /* __GST_MySRC_H__ */



