

#ifndef CMyUDPSink_h__
#define CMyUDPSink_h__


#include <gst/gst.h>

#include <gst/base/gstbasesink.h>
#include <gio/gio.h>
#include "IUDPClient.h"

G_BEGIN_DECLS


#define GST_TYPE_MyUDPSink                (gst_MyUDPSink_get_type())
#define GST_MyUDPSink(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MyUDPSink,GstMyUDPSink))
#define GST_MyUDPSink_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MyUDPSink,GstMyUDPSinkClass))
#define GST_IS_MyUDPSink(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MyUDPSink))
#define GST_IS_MyUDPSink_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MyUDPSink))

typedef struct _GstMyUDPSink GstMyUDPSink;
typedef struct _GstMyUDPSinkClass GstMyUDPSinkClass;

struct _GstMyUDPSink {
	GstBaseSink parent;

	std::string host;
	guint16 port;

	mray::network::IUDPClient* m_client;
	void SetPort(const std::string &host,guint16 port);

	//void CreateClient(const core::string& host, guint16 port);
};

struct _GstMyUDPSinkClass {
	GstBaseSinkClass parent_class;
	/* element methods */
	static gboolean plugin_init(GstPlugin * plugin);

};

GType gst_MyUDPSink_get_type(void);


G_END_DECLS



#endif // CMyUDPSink_h__

