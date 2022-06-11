

#include "stdafx.h"
#include "LocalAudioGrabber.h"

//#include "DirectSoundInputStream.h"
//#include "DirectSoundOutputStream.h"



namespace mray
{
namespace video
{
	/*
	std::vector<sound::InputStreamDeviceInfo> _dsInputInterfacesList;
	std::vector<sound::OutputStreamDeviceInfo> _dsOutputInterfacesList;

	void LoadInputInterfaces()
	{
		sound::DirectSoundInputStream* instance;
		instance = new sound::DirectSoundInputStream();
		_dsInputInterfacesList.clear();
		instance->ListDevices(_dsInputInterfacesList);
		delete instance;
	}
	void LoadOutputInterfaces()
	{
		sound::DirectSoundOutputStream* instance;
		instance = new sound::DirectSoundOutputStream();
		_dsOutputInterfacesList.clear();
		instance->ListDevices(_dsOutputInterfacesList);
		delete instance;
	}*/


LocalAudioGrabber::LocalAudioGrabber()
{
}
LocalAudioGrabber::~LocalAudioGrabber()
{
}

void LocalAudioGrabber::Init(const std::string &deviceGUID, int channels, int samplingrate)
{
	this->deviceGUID = deviceGUID;

	std::string audioStr = "directsoundsrc blocksize=4096";

	if (deviceGUID != "")
	{
		audioStr += "device=\"" + deviceGUID + "\"";
	}

	CustomAudioGrabber::Init(audioStr, channels, samplingrate);
}

int LocalAudioGrabber::GetInputInterfacesCount()
{
//	LoadInputInterfaces();
//	return _dsInputInterfacesList.size();
return 1;
}


const char* LocalAudioGrabber::GetInputInterfaceName(int i)
{
	return "Default";
	/*
	LoadInputInterfaces();
	if (i >= _dsInputInterfacesList.size())
		return "";
	return _dsInputInterfacesList[i].description.c_str();*/
}


const char* LocalAudioGrabber::GetInputInterfaceGUID(int i)
{
	return "Default";
	/*
	LoadInputInterfaces();
	if (i >= _dsInputInterfacesList.size())
		return "";
	return _dsInputInterfacesList[i].deviceGUID.c_str();*/
}

int LocalAudioGrabber::GetOutputInterfacesCount()
{
	return 1;
	/*
	LoadOutputInterfaces();
	return _dsOutputInterfacesList.size();*/
}


const char* LocalAudioGrabber::GetOutputInterfaceName(int i)
{
	return "Default";
	/*LoadOutputInterfaces();

	if (i >= _dsOutputInterfacesList.size())
		return "";
	return _dsOutputInterfacesList[i].description.c_str();*/
}


const char* LocalAudioGrabber::GetOutputInterfaceGUID(int i)
{
	return "Default";
	/*LoadOutputInterfaces();

	if (i >= _dsOutputInterfacesList.size())
		return "";
	return _dsOutputInterfacesList[i].GUID.c_str();*/
}

}
}
