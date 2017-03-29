

/********************************************************************
	created:	2013/12/08
	created:	8:12:2013   0:07
	filename: 	C:\Development\mrayEngine\Projects\TelubeeRobotAgent\CMyListener.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeRobotAgent
	file base:	CMyListener
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __CMyListener__
#define __CMyListener__

#include <gst/gst.h>
#include "ListenerContainer.h"

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_MyListener \
	(gst_my_listener_get_type())
#define GST_MyListener(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_MyListener, GstMyListener))
#define GST_MyListener_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_MyListener, GstMyListenerClass))
#define GST_IS_MyListener(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_MyListener))
#define GST_IS_MyListener_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_MyListener))

typedef struct _GstMyListener      GstMyListener;
typedef struct _GstMyListenerClass GstMyListenerClass;

class IMyListenerCallback
{
public:
	virtual void ListenerOnDataChained(_GstMyListener* src, GstBuffer * buffer){}
};

class MyListenerContainer :public mray::ListenerContainer<IMyListenerCallback*>
{
public:
	DECLARE_FIRE_METHOD(ListenerOnDataChained, (_GstMyListener* src, GstBuffer * buffer), (src, buffer));
};
struct _GstMyListener
{
	GstElement element;

	GstPad *sinkpad, *srcpad;

	gboolean silent;

	MyListenerContainer *listeners;
};

struct _GstMyListenerClass
{
	GstElementClass parent_class;
	static gboolean plugin_init(GstPlugin * plugin);
};

GType gst_my_listener_get_type(void);


G_END_DECLS


#endif /* __GST_MyListener_H__ */



