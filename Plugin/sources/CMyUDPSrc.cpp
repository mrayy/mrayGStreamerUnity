

#include "stdafx.h"
#include "cMyUDPSrc.h"
#include "INetwork.h"
#include "IThreadManager.h"
#include "MutexLocks.h"
#include "rtp.h"

#include <gst/rtp/rtp.h>
#include <gst/rtp/gstrtpbuffer.h>


using namespace mray;

static GstFlowReturn gst_MyUDPSrc_create(GstPushSrc * psrc, GstBuffer ** buf);

static gboolean gst_MyUDPSrc_start(GstBaseSrc * bsrc);

static gboolean gst_MyUDPSrc_stop(GstBaseSrc * bsrc);

static gboolean gst_MyUDPSrc_unlock(GstBaseSrc * bsrc);

static gboolean gst_MyUDPSrc_unlock_stop(GstBaseSrc * bsrc);
static GstStateChangeReturn
gst_myudpsrc_change_state(GstElement * element, GstStateChange transition);

static void gst_MyUDPSrc_finalize(GObject * object);

static void gst_MyUDPSrc_set_property(GObject * object, guint prop_id,
	const GValue * value, GParamSpec * pspec);
static void gst_MyUDPSrc_get_property(GObject * object, guint prop_id,
	GValue * value, GParamSpec * pspec);

#define gst_MyUDPSrc_parent_class parent_class
G_DEFINE_TYPE(GstMyUDPSrc, gst_MyUDPSrc, GST_TYPE_PUSH_SRC);

GST_DEBUG_CATEGORY_STATIC(MyUDPSrc_debug);
#define GST_CAT_DEFAULT (MyUDPSrc_debug)
static GstStaticPadTemplate MyUDPSrc_template = GST_STATIC_PAD_TEMPLATE("src",
	GST_PAD_SRC,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS_ANY);

#define UDP_DEFAULT_PORT                5004
enum
{
	PROP_0,

	PROP_CAPS,
	PROP_PORT,

	PROP_LAST
};

#if GST_VERSION_MAJOR==0
static GstCaps *
gst_MyUDPSrc_getcaps(GstBaseSrc * src)
#else
static GstCaps *
gst_MyUDPSrc_getcaps(GstBaseSrc * src, GstCaps * filter)
#endif
{
	GstMyUDPSrc *udpsrc;
	GstCaps *caps, *result;

	udpsrc = GST_MyUDPSrc(src);
	
	GST_OBJECT_LOCK(src);
	if ((caps = udpsrc->caps))
		gst_caps_ref(caps);
	GST_OBJECT_UNLOCK(src);
	if (caps) {
		if (filter) {
			result = gst_caps_intersect_full(filter, caps, GST_CAPS_INTERSECT_FIRST);
			gst_caps_unref(caps);
		}
		else {
			result = caps;
		}
	}
	else {
		result = (filter) ? gst_caps_ref(filter) : gst_caps_new_any();
	}
	return result;
	/*
	if (udpsrc->caps) {
		return (filter) ? gst_caps_intersect_full(filter, udpsrc->caps,
			GST_CAPS_INTERSECT_FIRST) : gst_caps_ref(udpsrc->caps);
	}
	else {
		return (filter) ? gst_caps_ref(filter) : gst_caps_new_any();
	}

	return gst_caps_ref(udpsrc->caps);*/

}

static void
gst_MyUDPSrc_class_init(GstMyUDPSrcClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;
	GstBaseSrcClass *gstbasesrc_class;
	GstPushSrcClass *gstpushsrc_class;

	gobject_class = (GObjectClass *)klass;
	gstelement_class = (GstElementClass *)klass;
	gstbasesrc_class = (GstBaseSrcClass *)klass;
	gstpushsrc_class = (GstPushSrcClass *)klass;

	GST_DEBUG_CATEGORY_INIT(MyUDPSrc_debug, "MyUDPSrc", 0, "My src");

	gobject_class->set_property = gst_MyUDPSrc_set_property;
	gobject_class->get_property = gst_MyUDPSrc_get_property;
	gobject_class->finalize = gst_MyUDPSrc_finalize;



	gst_element_class_add_pad_template(gstelement_class,
		gst_static_pad_template_get(&MyUDPSrc_template));

	gst_element_class_set_details_simple(gstelement_class,
		"MyUDPSrc class",
		"Source",
		"Custom App Source",
		"MHD Yamen Saraiji <mrayyamen@gmail.com>");

	g_object_class_install_property(G_OBJECT_CLASS(klass), PROP_PORT,
		g_param_spec_int("port", "Port",
		"The port to receive the packets from, 0=allocate", 0, G_MAXUINT16,
		UDP_DEFAULT_PORT, GParamFlags(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));
	g_object_class_install_property(gobject_class, PROP_CAPS,
		g_param_spec_boxed("caps", "Caps",
		"The caps of the source pad", GST_TYPE_CAPS,
		GParamFlags(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

	gstelement_class->change_state = gst_myudpsrc_change_state;

	gstbasesrc_class->start = gst_MyUDPSrc_start;
	gstbasesrc_class->stop = gst_MyUDPSrc_stop;
	gstbasesrc_class->unlock = gst_MyUDPSrc_unlock;
	gstbasesrc_class->unlock_stop = gst_MyUDPSrc_unlock_stop;
	gstbasesrc_class->get_caps = gst_MyUDPSrc_getcaps;

	gstpushsrc_class->create = gst_MyUDPSrc_create;
}

static void
gst_MyUDPSrc_init(GstMyUDPSrc * MyUDPSrc)
{
	MyUDPSrc->port = UDP_DEFAULT_PORT;
	MyUDPSrc->m_client = network::INetwork::getInstance().createUDPClient();
//	MyUDPSrc->m_mutex = OS::IThreadManager::getInstance().createMutex();
//	MyUDPSrc->timestampQueue = new std::list<GstMyUDPSrc::RTPPacketData>();
	/* configure basesrc to be a live source */
	gst_base_src_set_live(GST_BASE_SRC(MyUDPSrc), TRUE);
	/* make basesrc output a segment in time */
	gst_base_src_set_format(GST_BASE_SRC(MyUDPSrc), GST_FORMAT_TIME);
	/* make basesrc set timestamps on outgoing buffers based on the running_time
	* when they were captured */
	gst_base_src_set_do_timestamp(GST_BASE_SRC(MyUDPSrc), TRUE);
}

/* create a socket for sending to remote machine */
static gboolean
gst_MyUDPSrc_start(GstBaseSrc * bsrc)
{
	GstMyUDPSrc *src;

	src = GST_MyUDPSrc(bsrc);

	return TRUE;

}

static gboolean 
gst_MyUDPSrc_Open(GstMyUDPSrc * src)
{

	if (src->m_client)
	{
		GST_DEBUG_OBJECT(src, "binding socket");
		if (src->m_client->Open(src->port) == network::UDP_SOCKET_ERROR_NONE)
		{
			src->port = src->m_client->Port();
			GST_DEBUG_OBJECT(src, "UDP Socket bound to %d", src->port);
		}
		else
		{
			GST_DEBUG_OBJECT(src, "Failed to bind UDP Socket to (%s:%d)", src->port);
			return FALSE;
		}
	}
	return true;
}
static gboolean
gst_MyUDPSrc_Close(GstMyUDPSrc * src)
{

	if (src->m_client)
	{
		GST_DEBUG_OBJECT(src, "closing socket");
		src->m_client->Close();
		delete src->m_client;
		src->m_client = 0;
	}/*
	delete src->m_mutex;
	src->m_mutex = 0;
	delete src->timestampQueue;*/
	return true;
}

static void
gst_MyUDPSrc_finalize(GObject * object)
{
	GstMyUDPSrc *MyUDPSrc;

	MyUDPSrc = GST_MyUDPSrc(object);

	if (MyUDPSrc->caps)
		gst_caps_unref(MyUDPSrc->caps);
	MyUDPSrc->caps = NULL;

	if (MyUDPSrc->m_client)
	{
		MyUDPSrc->m_client->Close();
		delete MyUDPSrc->m_client;
		MyUDPSrc->m_client = 0;
	}

	G_OBJECT_CLASS(parent_class)->finalize(object);
}


static GstFlowReturn
gst_MyUDPSrc_create(GstPushSrc * psrc, GstBuffer ** buf)
{
	GstFlowReturn ret = GST_FLOW_OK;
	GstMyUDPSrc *src;
	*buf = 0;
	src = GST_MyUDPSrc(psrc);
	if (src->m_client && src->m_client->IsOpen())
	{
		uint sz;
		const int bufferSize = 8192;
		char Buffer[bufferSize];
		uint len = bufferSize;
		if (src->m_client->RecvFrom(Buffer, &len, 0, 0) == network::UDP_SOCKET_ERROR_NONE)
		{
			GstMapInfo map;
			GstBuffer* outbuf =  gst_buffer_new_and_alloc(len);
			gst_buffer_map(outbuf, &map, GST_MAP_WRITE);
			memcpy(map.data, Buffer, len);
			gst_buffer_unmap(outbuf, &map);

			/*
			GstRTPBuffer rtp_buf;
			gst_rtp_buffer_map(outbuf, GST_MAP_READ, &rtp_buf);
			//gst_rtp_buffer_map()
			//gst_rtp_buffer_add_extension_onebyte_header()
						gst_rtp_buffer_unmap(&rtp_buf);*/
			/*
			GstMyUDPSrc::RTPPacketData packet;
			packet .timestamp= rtp_timestamp(Buffer);
			packet.length = rtp_padding_payload((unsigned char*)Buffer, len, packet.data);
			src->AddRTPPacket(packet);*/

			//printf("%d  ", len);
			/*
			gst_buffer_append_memory(outbuf,
				gst_memory_new_wrapped(GST_MEMORY_FLAG_READONLY, Buffer, len, 0, len, 0, g_free));
				*/

			GST_LOG_OBJECT(src, "read %d bytes", (int)len);
			*buf = GST_BUFFER_CAST(outbuf);
		}
		else
			ret = GST_FLOW_ERROR;
		/*
		src->m_client->WaitForData(-1,-1);
		if (src->m_client->GetAvailableBytes(&sz) != network::UDP_SOCKET_ERROR_NONE)
			ret = GST_FLOW_ERROR;
		else
		{
			if (sz != 0)
			{
				char* buffer = new char[sz];
				uint len = sz;
				network::NetAddress addr;
				if (src->m_client->RecvFrom(buffer, &sz, &addr, 0) == network::UDP_SOCKET_ERROR_NONE)
				{
					GstBuffer* outbuf = gst_buffer_new();
					gst_buffer_append_memory(outbuf,
						gst_memory_new_wrapped(GST_MEMORY_FLAG_READONLY, buffer, len, 0, len, 0, g_free));

					GST_LOG_OBJECT(src, "read %d bytes", (int)len);
					*buf = GST_BUFFER_CAST(outbuf);
				}
			}
		}*/
	}
	else
		ret = GST_FLOW_ERROR;
	return ret;
}

void _GstMyUDPSrc::SetPort(guint16 p)
{
	if ( port == p)
		return;
	port = p;
	if (m_client && m_client->IsOpen())
	{
		m_client->Close();
		// restart socket
		gst_MyUDPSrc_Open(this);
	}

}
/*
GstMyUDPSrc::RTPPacketData _GstMyUDPSrc::GetLastRTPPacket(bool remove)
{
	OS::ScopedLock lock(m_mutex);
	if (timestampQueue->size() == 0)
		return GstMyUDPSrc::RTPPacketData();
	GstMyUDPSrc::RTPPacketData ts = *timestampQueue->begin();
	if (remove)
		timestampQueue->erase(timestampQueue->begin());
	return ts;
}
void _GstMyUDPSrc::AddRTPPacket(const GstMyUDPSrc::RTPPacketData& ts)
{
	OS::ScopedLock lock(m_mutex);
	if (timestampQueue->size() == 0)
	{
		timestampQueue->push_back(ts);
		
	}
	else{
		const GstMyUDPSrc::RTPPacketData& last = timestampQueue->back();
		if (last.timestamp != ts.timestamp)
			timestampQueue->push_back(ts);
	}

}*/

static void
gst_MyUDPSrc_set_property(GObject * object, guint prop_id, const GValue * value,
GParamSpec * pspec)
{
	GstMyUDPSrc *MyUDPSrc = GST_MyUDPSrc(object);

	switch (prop_id)
	{
	case PROP_CAPS:
	{
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

			old_caps = MyUDPSrc->caps;
			MyUDPSrc->caps = new_caps;
			if (old_caps)
				gst_caps_unref(old_caps);
			gst_pad_set_caps(GST_BASE_SRC(MyUDPSrc)->srcpad, new_caps);
		}
		break;
	case PROP_PORT:
		if (MyUDPSrc->port == g_value_get_int(value))
			break;
		MyUDPSrc->port = g_value_get_int(value);
		if (MyUDPSrc->m_client && MyUDPSrc->m_client->IsConnected())
		{
			MyUDPSrc->m_client->Disconnect();
			// restart socket
			gst_MyUDPSrc_start(GST_BASE_SRC(MyUDPSrc));
		}
		break;
	}
	default:
		break;
	}

}

static void
gst_MyUDPSrc_get_property(GObject * object, guint prop_id, GValue * value,
GParamSpec * pspec)
{
	GstMyUDPSrc *MyUDPSrc = GST_MyUDPSrc(object);
	switch (prop_id)
	{
	case PROP_CAPS:
		gst_value_set_caps(value, MyUDPSrc->caps);
		break;
	case PROP_PORT:
		g_value_set_int(value, MyUDPSrc->port);
		break;
	}
}

static gboolean
gst_MyUDPSrc_unlock(GstBaseSrc * bsrc)
{
	GstMyUDPSrc *src;

	src = GST_MyUDPSrc(bsrc);

	GST_LOG_OBJECT(src, "Flushing");

	return TRUE;
}

static gboolean
gst_MyUDPSrc_unlock_stop(GstBaseSrc * bsrc)
{
	GstMyUDPSrc *src;

	src = GST_MyUDPSrc(bsrc);

	GST_LOG_OBJECT(src, "No longer flushing");

	return TRUE;
}

static gboolean
gst_MyUDPSrc_stop(GstBaseSrc * bsrc)
{
	GstMyUDPSrc *src;

	src = GST_MyUDPSrc(bsrc);

	GST_DEBUG("stopping, closing sockets");

	return TRUE;
}

static GstStateChangeReturn 
gst_myudpsrc_change_state (GstElement * element, GstStateChange transition)
{
	GstMyUDPSrc *src;
  GstStateChangeReturn result;

  src = GST_MyUDPSrc (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      if (!gst_MyUDPSrc_Open (src))
        goto open_failed;
      break;
    default:
      break;
  }
  if ((result =
          GST_ELEMENT_CLASS (parent_class)->change_state (element,
              transition)) == GST_STATE_CHANGE_FAILURE)
    goto failure;

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_NULL:
      gst_MyUDPSrc_Close (src);
      break;
    default:
      break;
  }
  return result;
  /* ERRORS */
open_failed:
  {
    GST_DEBUG_OBJECT (src, "failed to open socket");
    return GST_STATE_CHANGE_FAILURE;
  }
failure:
  {
    GST_DEBUG_OBJECT (src, "parent failed state change");
    return result;
  }
}




/*** GSTURIHANDLER INTERFACE *************************************************/

static GstURIType
gst_udpsrc_uri_get_type (GType type)
{
  return GST_URI_SRC;
}
gboolean
_GstMyUDPSrcClass::plugin_init(GstPlugin * plugin)
{
	if (!gst_element_register(plugin, "myudpsrc", GST_RANK_NONE,
		GST_TYPE_MyUDPSrc))
		return FALSE;

	return TRUE;
}