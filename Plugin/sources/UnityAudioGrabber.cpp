

#include "stdafx.h"
#include "UnityAudioGrabber.h"

namespace mray
{
namespace video
{



UnityAudioGrabber::UnityAudioGrabber()
{
	_samplingRate = 44100;
	_channels = 1;
	_bufferLength = 1000;
	_started = false;
}
UnityAudioGrabber::~UnityAudioGrabber()
{
	for each (float* v in _graveyard)
	{
		delete[] v;
	}
	for each (float* v in _samples)
	{
		delete[] v;
	}
}

void UnityAudioGrabber::Init(int bufferLength, int channels, int samplingrate)
{
	_bufferLength = bufferLength;
	_channels= channels;
	_samplingRate = samplingrate;
	int buffers = 2*samplingrate / bufferLength;
	if (buffers < 1)
		buffers = 1;
	for (int i = 0; i < buffers; ++i)
	{
		_graveyard.push_back(new float[bufferLength]);
	}
}

void UnityAudioGrabber::AddFrame(float* data)
{
	float* ptr = 0;
	if (_graveyard.size() > 0)
	{
		ptr=_graveyard.front();
		_graveyard.pop_front();
	}
	else
	{
		ptr = new float[_bufferLength];
	}

	memcpy(ptr, data, _bufferLength*sizeof(float));
	_samples.push_back(ptr);
}



bool UnityAudioGrabber::Start()
{
	_started = true;
	return true;
}


void UnityAudioGrabber::Pause()
{
	_started = false;
}


void UnityAudioGrabber::Close()
{
	_started = false;
}


bool UnityAudioGrabber::IsStarted()
{
	return _started;
}

void UnityAudioGrabber::SetVolume(float vol)
{

}


uint UnityAudioGrabber::GetSamplingRate()
{
	return _samplingRate;
}


uint UnityAudioGrabber::GetChannelsCount()
{
	return _channels;
}


bool UnityAudioGrabber::GrabFrame()
{
	return _samples.size() > 0;
}


uchar* UnityAudioGrabber::GetAudioFrame()
{
	if (_samples.size() > 0)
	{
		float* ptr=_samples.front();
		_graveyard.push_back(ptr);
		_samples.pop_front();
		return (uchar*)ptr;
	}
	return 0;
}


uint UnityAudioGrabber::GetAudioFrameSize()
{
	return _bufferLength*sizeof(float);
}


}
}
