

#include "stdafx.h"
#include "DirectSoundOutputStream.h"

#include <dsound.h>

namespace mray
{
namespace sound
{

struct DirectSoundOutDeviceInfo
{
	LPGUID lpGuid;
	std::string Desciption;
	std::string module;
};


class DirectSoundOutputStreamData
{
public:

	DirectSoundOutDeviceData* dsData;
};

struct DirectSoundOutDeviceData
{
	DirectSoundOutDeviceData()
	{
		device=0;
	}
	LPDIRECTSOUND device;
	std::vector<DirectSoundOutDeviceInfo> devices;
};


BOOL CALLBACK DirectSoundEnumCallback(LPGUID lpGuid,
	LPCSTR lpcstrDescription,
	LPCSTR lpcstrModule,
	LPVOID lpContext)
{
	std::vector<DirectSoundOutDeviceInfo> &lst=*(std::vector<DirectSoundOutDeviceInfo>*)lpContext;
	DirectSoundOutDeviceInfo ifo;
	ifo.lpGuid=lpGuid;
	ifo.Desciption= lpcstrDescription;
	ifo.module=lpcstrModule;
	lst.push_back(ifo);
	return true;
}


DirectSoundOutputStream::DirectSoundOutputStream()
{
	m_data=new DirectSoundOutputStreamData;
	m_data->dsData=new DirectSoundOutDeviceData;
	m_listener=0;

}
DirectSoundOutputStream::~DirectSoundOutputStream()
{
	delete m_data;
}

void DirectSoundOutputStream::ListDevices(std::vector<OutputStreamDeviceInfo> &lst)
{
	char buffer[128];
	m_data->dsData->devices.clear();
	DirectSoundEnumerate(DirectSoundEnumCallback,&m_data->dsData->devices);
	for(int i=0;i<m_data->dsData->devices.size();++i)
	{
		OutputStreamDeviceInfo ifo;
		ifo.id=i;
		ifo.name = m_data->dsData->devices[i].module;
		OLECHAR* bstrGuid;
		if (m_data->dsData->devices[i].lpGuid)
		{
			StringFromCLSID(*m_data->dsData->devices[i].lpGuid, &bstrGuid);
			wcstombs(buffer, bstrGuid, 128);
			ifo.GUID = buffer;
			::CoTaskMemFree(bstrGuid);
		}
		ifo.description=m_data->dsData->devices[i].Desciption;
		lst.push_back(ifo);
	}
}
}
}


