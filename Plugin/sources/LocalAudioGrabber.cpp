

#include "stdafx.h"
#include "LocalAudioGrabber.h"

namespace mray
{
namespace video
{



LocalAudioGrabber::LocalAudioGrabber()
{
}
LocalAudioGrabber::~LocalAudioGrabber()
{
}

void LocalAudioGrabber::Init(const std::string &deviceGUID, int channels, int samplingrate)
{
	this->deviceGUID = deviceGUID;

	std::string audioStr = "directsoundsrc buffer-time=100  ";

	if (deviceGUID != "")
	{
		audioStr += "device=\"" + deviceGUID + "\"";
	}

	CustomAudioGrabber::Init(audioStr, channels, samplingrate);
}

}
}
