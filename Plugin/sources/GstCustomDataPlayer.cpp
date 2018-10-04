

#include "stdafx.h"
#include "GstCustomDataPlayer.h"

#include "GstPipelineHandler.h"
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <memory>
//#include "ILogManager.h"
#include "IThreadManager.h"
#include "IMutex.h"
#include "GZipCompress.h"

namespace mray
{
namespace video
{

	class GstCustomDataPlayerImpl :public GstPipelineHandler
	{

		struct DataSegment
		{
			DataSegment()
			{
				data = 0;
				length = 0;
				owner = 0;
				index = 0;
				avail = true;
			}
			~DataSegment()
			{
				if (data)
					delete[] data;
			}
			uchar* data;
			int length;
			GstCustomDataPlayerImpl* owner;
			int index;

			bool avail;
		};
		typedef std::list<DataSegment*> DataList;
		DataList m_data;
		DataList m_grave;
		std::string m_pipeLineString;
		GstAppSink* m_sink;
		GstElement* m_src;
		int m_port;

		uint m_frameID;
		bool			m_IsFrameNew;			// if we are new
		bool			m_IsAllocated;
		OS::IMutex*			m_mutex;

		DataSegment* m_dataSegment;

		Compress::GZipCompress m_compress;

		std::string m_dataType;
		int m_payload;
		bool m_autotimestamp;
	public:
		GstCustomDataPlayerImpl()
		{
			m_dataSegment = 0;
			m_IsFrameNew = false;
			m_IsAllocated = false;
			m_frameID = 0;
			m_sink = 0;
			m_dataType = "rtp";
			m_payload = 98;
			m_port = 0;
			m_mutex = OS::IThreadManager::getInstance().createMutex();
		}
		virtual ~GstCustomDataPlayerImpl()
		{
			Close();
			for (DataSegment* d : m_data)
				delete d;
			for (DataSegment* d : m_grave)
				delete d;
			m_data.clear();
			m_grave.clear();
			delete m_mutex;
		}

		static void _DestroyNotify(gpointer       data)
		{
			DataSegment* d = static_cast<DataSegment*>(data);
			if (d)
			{
				d->owner->m_mutex->lock();
				d->avail = true;
				d->owner->m_mutex->unlock();
			}
		}

		DataSegment* GetDataSegment(int size)
		{
			m_mutex->lock();
			DataSegment* d = 0;/*
			DataList::iterator it = m_grave.begin();
			for(;it!=m_grave.end();++it)
			{
				DataSegment* t = *it;
				if (t->avail && t->length >= size)
				{
					d = t;
					m_grave.erase(it);
					break;
				}
			}*/
			if (!d)
			{
				d = new DataSegment();
				d->owner = this;
				d->data = new uchar[size];
				d->length = size;
			}
			m_data.push_back(d);
			d->avail = false;
			m_mutex->unlock();
			return d;
		}
		GstClockTime getTimeStamp() {
			if (!IsPlaying()) return GST_CLOCK_TIME_NONE;
			GstClock * clock = gst_pipeline_get_clock(GST_PIPELINE(GetPipeline()));

			gst_object_ref(clock);
			GstClockTime now = gst_clock_get_time(clock) - gst_element_get_base_time(GetPipeline());
			gst_object_unref(clock);
			return now;
		}
		void AddDataFrame(uchar* data, int length)
		{
			if (!IsPlaying())
				return;

			auto d = GetDataSegment(length);
			memcpy(d->data, data, length);

		}
		void SetApplicationDataType(const std::string& dataType, bool autotimestamp, int payload = 98)
		{
			m_dataType = dataType;
			m_payload = payload;
			m_autotimestamp = autotimestamp;
		}
		void SetPort(int port)
		{
			m_port = port;
		}

		void _BuildPipeline()
		{
			std::stringstream ss;
			{
				ss << "udpsrc name=src port="<<m_port<<" ! application/x-" << m_dataType <<",media=application,payload="<< m_payload<<" ! rtpgstdepay !  appsink name=datasink sync=false";

			}
			m_pipeLineString = ss.str();
		}
		void _UpdatePorts()
		{

			if (!GetPipeline())
				return;
		}

		bool CreateStream()
		{
			if (IsLoaded())
				return true;
			_BuildPipeline();

			GError *err = 0;
			GstElement* p = gst_parse_launch(m_pipeLineString.c_str(), &err);
			if (err)
			{
				printf("GstCustomDataPlayer: Pipeline error: %s", err->message);
			}
			if (!p)
				return false;
			SetPipeline(p);

			m_sink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(p), "datasink"));
			m_src = gst_bin_get_by_name(GST_BIN(p), "src");

			//setup video sink
			gst_base_sink_set_sync(GST_BASE_SINK(m_sink), true);
			g_object_set(G_OBJECT(m_sink), "emit-signals", FALSE, "sync", false, (void*)NULL);

			//attach videosink callbacks
			GstAppSinkCallbacks gstCallbacks;
			gstCallbacks.eos = &on_eos_from_source;
			gstCallbacks.new_preroll = &on_new_preroll_from_source;
#if GST_VERSION_MAJOR==0
			gstCallbacks.new_buffer = &on_new_buffer_from_source;
#else
			gstCallbacks.new_sample = &on_new_buffer_from_source;
#endif
			gst_app_sink_set_callbacks(GST_APP_SINK(m_sink), &gstCallbacks, this, NULL);


			return CreatePipeline(false);

		}

		bool IsStream()
		{
			return true;
		}

		virtual void Close()
		{
			GstPipelineHandler::Close();
		}

		void Play()
		{
			SetPaused(false);
		}

		void Pause()
		{
			SetPaused(true);
		}
		void SetVolume(float vol)
		{
		}

		virtual int GetPort()
		{
			int port;
			g_object_get(m_src, "port", &port);
			return port;
		}

		virtual bool GrabFrame() {
			m_mutex->lock();
			if (m_dataSegment)
			{
				//m_grave.push_back(m_dataSegment);
				delete m_dataSegment;
				m_dataSegment = 0;
			}
			if (m_data.size() > 0) {
				m_dataSegment = m_data.front();
				m_data.pop_front();
				m_IsFrameNew = true;
			}
			else m_IsFrameNew = false;

			m_mutex->unlock();
			return m_IsFrameNew;
		}
		virtual bool HasNewFrame() { return m_IsFrameNew; }
		virtual ulong GetBufferID() { return m_frameID; }

		int GetDataLength()
		{
			if (!m_dataSegment)
				return 0;
			return m_dataSegment->length;
		}


		bool GetData(void* data, int length) {
			m_mutex->lock();
			if (!m_dataSegment)
				return false;

			memcpy(data, m_dataSegment->data, std::min(length, m_dataSegment->length));

			m_mutex->unlock();
			return true;
		}

		virtual GstFlowReturn process_sample(std::shared_ptr<GstSample> sample)
		{
			GstBuffer * _buffer = gst_sample_get_buffer(sample.get());
			GstMapInfo mapinfo;
			gst_buffer_map(_buffer, &mapinfo, GST_MAP_READ);
			guint size = mapinfo.size;

			m_mutex->lock();

			if (mapinfo.data[0])
			{

				Compress::GZipCompress::Chunk input(false), output(true);
				input.data = mapinfo.data + 1;
				input.length = size-1;
				m_compress.decompress(&input, &output);
				DataSegment* s = GetDataSegment(output.length);

				memcpy(s->data, output.data, output.length);
			}
			else
			{
				DataSegment* s = GetDataSegment(size-1);

				memcpy(s->data, mapinfo.data + 1, size - 1);
			}
			m_mutex->unlock();

			gst_buffer_unmap(_buffer, &mapinfo);
			return GstFlowReturn::GST_FLOW_OK;
		}

		GstFlowReturn preroll_cb(std::shared_ptr<GstSample> sample)
		{
			GstFlowReturn ret = process_sample(sample);
			if (ret == GST_FLOW_OK) {
				return GST_FLOW_OK;
			}
			else {
				return ret;
			}
		}
		GstFlowReturn buffer_cb(std::shared_ptr<GstSample> sample)
		{


			GstFlowReturn ret = process_sample(sample);
			if (ret == GST_FLOW_OK) {
				return GST_FLOW_OK;
			}
			else {
				return ret;
			}
		}
		static void  eos_cb()
		{
		}


		static GstFlowReturn on_new_buffer_from_source(GstAppSink * elt, void * data) {
#if GST_VERSION_MAJOR==0
			shared_ptr<GstBuffer> buffer(gst_app_sink_pull_buffer(GST_APP_SINK(elt)), &gst_buffer_unref);
#else
			std::shared_ptr<GstSample> buffer(gst_app_sink_pull_sample(GST_APP_SINK(elt)), &gst_sample_unref);
#endif
			return ((GstCustomDataPlayerImpl*)data)->buffer_cb(buffer);
		}

		static GstFlowReturn on_new_preroll_from_source(GstAppSink * elt, void * data) {
#if GST_VERSION_MAJOR==0
			shared_ptr<GstBuffer> buffer(gst_app_sink_pull_preroll(GST_APP_SINK(elt)), &gst_buffer_unref);
#else
			std::shared_ptr<GstSample> buffer(gst_app_sink_pull_preroll(GST_APP_SINK(elt)), &gst_sample_unref);
#endif
			return ((GstCustomDataPlayerImpl*)data)->preroll_cb(buffer);
		}

		static void on_eos_from_source(GstAppSink * elt, void * data) {
			((GstCustomDataPlayerImpl*)data)->eos_cb();
		}
	};
	GstPipelineHandler* GstCustomDataPlayer::GetPipeline()
	{
		return m_impl;
	}

	GstCustomDataPlayer::GstCustomDataPlayer()
	{
		m_impl = new GstCustomDataPlayerImpl();
	}

	GstCustomDataPlayer::~GstCustomDataPlayer()
	{
		delete m_impl;
	}


	void GstCustomDataPlayer::SetApplicationDataType(const std::string& dataType, bool autotimestamp, int payload )
	{
		m_impl->SetApplicationDataType(dataType, autotimestamp, payload);
	}

	bool GstCustomDataPlayer::CreateStream()
	{
		return m_impl->CreateStream();
	}


	void GstCustomDataPlayer::SetVolume(float vol)
	{
		m_impl->SetVolume(vol);
	}
	bool GstCustomDataPlayer::IsStream()
	{
		return m_impl->IsStream();
	}

	void GstCustomDataPlayer::Play()
	{
		m_impl->Play();
	}
	void GstCustomDataPlayer::Stop()
	{
		m_impl->Stop();
	}
	void GstCustomDataPlayer::Pause()
	{
		m_impl->Pause();
	}
	bool GstCustomDataPlayer::IsLoaded()
	{
		return m_impl->IsLoaded();
	}
	bool GstCustomDataPlayer::IsPlaying()
	{
		return m_impl->IsPlaying();
	}
	void GstCustomDataPlayer::Close()
	{
		m_impl->Close();
	}


	void GstCustomDataPlayer::SetPort(int i)
	{
		m_impl->SetPort(i);
	}
	int GstCustomDataPlayer::GetPort(int i)
	{
		return m_impl->GetPort();
	}


	bool GstCustomDataPlayer::GrabFrame(int i) {
		return m_impl->GrabFrame();
	}
	bool GstCustomDataPlayer::HasNewFrame() {
		return m_impl->HasNewFrame();

	}
	 ulong GstCustomDataPlayer::GetBufferID() {
		 return m_impl->GetBufferID();

	 }

	 int GstCustomDataPlayer::GetDataLength() {
		 return m_impl->GetDataLength();

	 }
	int GstCustomDataPlayer::GetData(void* data, int length) {
		return m_impl->GetData(data,length);

	}

}
}

