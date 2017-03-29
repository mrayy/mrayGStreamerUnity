



/********************************************************************
created:	2013/12/08
created:	8:12:2013   0:07
filename: 	C:\Development\mrayEngine\Projects\TelubeeRobotAgent\CMyUDPSrc.h
file path:	C:\Development\mrayEngine\Projects\TelubeeRobotAgent
file base:	CMyUDPSrc
file ext:	h
author:		MHD Yamen Saraiji

purpose:
*********************************************************************/

#ifndef __CMyUDPSrc__
#define __CMyUDPSrc__

#include <gst/gst.h>
#include <gst/base/gstpushSrc.h>
#include <gio/gio.h>
#include <list>

#include <stdint.h>
#include "IUDPClient.h"
#include "IMutex.h"

G_BEGIN_DECLS


#define GST_TYPE_MyUDPSrc (gst_MyUDPSrc_get_type())
#define GST_MyUDPSrc(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_MyUDPSrc, GstMyUDPSrc))
#define GST_MyUDPSrc_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_MyUDPSrc, GstMyUDPSrcClass))
#define GST_IS_MyUDPSrc(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_MyUDPSrc))
#define GST_IS_MyUDPSrc_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_MyUDPSrc))
#define GST_MyUDPSrc_CAST(obj) ((GstMyUDPSrc *)(obj))

typedef class _GstMyUDPSrc GstMyUDPSrc;
typedef struct _GstMyUDPSrcClass GstMyUDPSrcClass;

class _GstMyUDPSrc {
public:
	GstPushSrc parent;

	GstCaps   *caps;
	guint16 port;

	mray::network::IUDPClient* m_client;
	/*
	struct RTPPacketData
	{
		uint32_t timestamp;
		uint64_t presentationTime;
		unsigned short length;
		unsigned char data[255];
	};
	std::list<RTPPacketData> *timestampQueue;

	mray::OS::IMutex* m_mutex;

	GstMyUDPSrc::RTPPacketData GetLastRTPPacket(bool remove);

	void AddRTPPacket(const GstMyUDPSrc::RTPPacketData& ts);*/

	_GstMyUDPSrc()
	{
	}
	~_GstMyUDPSrc()
	{
	}

	void SetPort(guint16 port);

};

struct _GstMyUDPSrcClass {
	GstPushSrcClass parent_class;
	static gboolean plugin_init(GstPlugin * plugin);
};

GType gst_MyUDPSrc_get_type(void);

G_END_DECLS


#endif /* __GST_MyUDPSrc_H__ */




