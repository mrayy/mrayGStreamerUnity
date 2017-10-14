

#include "stdafx.h"
#include "LocalAudioGrabber.h"

#include "DirectSoundInputStream.h"



namespace mray
{
namespace video
{
	std::vector<sound::InputStreamDeviceInfo> _dsInterfacesList;

	void LoadInterfaces()
	{
		sound::DirectSoundInputStream* instance;
		instance = new sound::DirectSoundInputStream();
		_dsInterfacesList.clear();
		instance->ListDevices(_dsInterfacesList);
		delete instance;
	}

LocalAudioGrabber::LocalAudioGrabber()
{
}
LocalAudioGrabber::~LocalAudioGrabber()
{
}

void LocalAudioGrabber::Init(const std::string &deviceGUID, int channels, int samplingrate)
{
	this->deviceGUID = deviceGUID;

	std::string audioStr = "directsoundsrc ";

	if (deviceGUID != "")
	{
		audioStr += "device=\"" + deviceGUID + "\"";
	}

	CustomAudioGrabber::Init(audioStr, channels, samplingrate);
}

int LocalAudioGrabber::GetInterfacesCount()
{
	LoadInterfaces();
	return _dsInterfacesList.size();
}


const char* LocalAudioGrabber::GetInterfaceName(int i)
{
	LoadInterfaces();
	if (i >= _dsInterfacesList.size())
		return "";
	return _dsInterfacesList[i].description.c_str();
}


const char* LocalAudioGrabber::GetInterfaceGUID(int i)
{
	LoadInterfaces();
	if (i >= _dsInterfacesList.size())
		return "";
	return _dsInterfacesList[i].deviceGUID.c_str();
}


}
}
