
#include "stdafx.h"
#include "AudioAppSinkHandler.h"

#include <gst/audio/audio.h>
#include "UnityHelpers.h"
#include "IThreadManager.h"
#include "MutexLocks.h"

namespace mray
{
namespace video
{


AudioAppSinkHandler::AudioAppSinkHandler()
{
	m_grabbedFrame = 0;
	m_channelsCount = 1;
	m_sampleRate = 0;
	m_closed = false;

	m_mutex = OS::IThreadManager::getInstance().createMutex();

}
AudioAppSinkHandler::~AudioAppSinkHandler()
{
	Close();
	delete m_mutex;
}

void AudioAppSinkHandler::Close()
{
	m_closed = true;
	m_mutex->lock();
	//clear all
	std::list<AudioInfo*>::iterator it = m_frames.begin();
	for (; it != m_frames.end(); ++it)
	{
		delete *it;
	}
	m_frames.clear();
	it = m_graveyard.begin();
	for (; it != m_graveyard.end(); ++it)
	{
		delete *it;
	}
	m_graveyard.clear();
	if (m_grabbedFrame)
	{
		delete m_grabbedFrame;
		m_grabbedFrame = 0;
	}
	m_mutex->unlock();
}

static GstAudioInfo getAudioInfo(GstSample * sample){
	GstCaps *caps = gst_sample_get_caps(sample);
	GstAudioInfo vinfo;
	gst_audio_info_init(&vinfo);
	if (caps){
		gst_audio_info_from_caps(&vinfo, caps);
	}
	return vinfo;
}

AudioInfo* AudioAppSinkHandler::_CreateFrame()
{
	if (m_graveyard.size() > 0)
	{
		AudioInfo* ifo=* m_graveyard.begin();
		m_graveyard.erase(m_graveyard.begin());
		return ifo;
	}
	else
	{
		return new AudioInfo();
	}
}
void AudioAppSinkHandler::_RemoveFrame(AudioInfo* frame)
{
	m_graveyard.push_back(frame);
}
GstFlowReturn AudioAppSinkHandler::process_sample(std::shared_ptr<GstSample> sample)
{
	if (m_closed)
		return GST_FLOW_ERROR;
	GstBuffer * _buffer = gst_sample_get_buffer(sample.get());
	GstAudioInfo vinfo = getAudioInfo(sample.get());

	if (vinfo.finfo->format != GST_AUDIO_FORMAT_F32LE)
	{
		return GST_FLOW_ERROR;
	}
	gst_buffer_map(_buffer, &mapinfo, GST_MAP_READ);
	guint size = mapinfo.size;
	int length = size / (vinfo.channels * sizeof(float));;
	m_sampleRate = vinfo.rate;
	m_channelsCount = vinfo.channels;

	{

		OS::ScopedLock m(m_mutex);

		AudioInfo* frame = _CreateFrame();
		frame->CreateData(length, vinfo.channels);

		frame->SetData(mapinfo.data, length, vinfo.channels);
		m_frames.push_back(frame);
		if (m_frames.size() > 20) //limit cache to 20 packets
		{
			AudioInfo *f = *m_frames.begin();
			m_frames.erase(m_frames.begin());
			_RemoveFrame(f);
		}
	}

	gst_buffer_unmap(_buffer, &mapinfo);
	return GST_FLOW_OK;
}

bool AudioAppSinkHandler::GrabFrame(){
	if (m_closed)return false;
	bool newFrame = false;
	OS::ScopedLock m(m_mutex);
	if (m_frames.size() > 0){
		m_grabbedFrame = *m_frames.begin();
		m_frames.erase(m_frames.begin());
		newFrame = true;
	}
	else 
		m_grabbedFrame = 0;
	return newFrame;
}
int AudioAppSinkHandler::GetFrameSize()
{
	OS::ScopedLock m(m_mutex);
	if (!m_grabbedFrame)
		return 0;
	return m_grabbedFrame->dataLength;
}

void AudioAppSinkHandler::CopyAudioFrame(float* output)
{
	OS::ScopedLock m(m_mutex);
	if (!m_grabbedFrame || !output)
		return ;
	memcpy(output, m_grabbedFrame->data, m_grabbedFrame->dataLength*sizeof(float));
	_RemoveFrame(m_grabbedFrame);
	m_grabbedFrame = 0;

}

}
}
