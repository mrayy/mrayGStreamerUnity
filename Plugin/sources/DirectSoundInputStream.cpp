#include "stdafx.h"
#include "DirectSoundInputStream.h"
#include <dsound.h>


namespace mray
{
namespace sound
{

class DirectSoundDeviceInfo
{
public:
	LPGUID lpGuid;
	std::string Desciption;
	std::string module;
};
class  DirectSoundData
{
public:
	DirectSoundData()
	{
		capture=0;
		recording=0;
	}
	LPDIRECTSOUNDCAPTURE8 capture ;

	std::vector<DirectSoundDeviceInfo> devices;

	bool recording;

	static const uint BufferBytes=16000;
};
WAVEFORMATEX w;

BOOL CALLBACK DirectSoundOutDeviceEnumCallback(LPGUID lpGuid,
	LPCSTR lpcstrDescription,
	LPCSTR lpcstrModule,
	LPVOID lpContext)
{
	std::vector<DirectSoundDeviceInfo> &lst=*(std::vector<DirectSoundDeviceInfo>*)lpContext;
	DirectSoundDeviceInfo ifo;
	ifo.lpGuid=lpGuid;
	ifo.Desciption= lpcstrDescription;
	ifo.module=lpcstrModule;
	lst.push_back(ifo);
	return true;
}
DirectSoundInputStream::DirectSoundInputStream():
m_numChannels(0),m_samplesPerSec(0),m_avgBytesPerSec(0),m_bitsPerSample(0),m_captureBuffer(0),m_lastReadOffset(0),m_recording(false)
{
	m_dsData=new DirectSoundData();

}
DirectSoundInputStream::~DirectSoundInputStream()
{

	if(m_captureBuffer)
		((LPDIRECTSOUNDCAPTUREBUFFER)m_captureBuffer)->Release();
	m_captureBuffer = NULL;

	if(m_dsData->capture)
		m_dsData->capture->Release();

	delete m_dsData;
}

void DirectSoundInputStream::ListDevices(std::vector<InputStreamDeviceInfo> &lst)
{
	m_dsData->devices.clear();
	DirectSoundCaptureEnumerate(DirectSoundOutDeviceEnumCallback,&m_dsData->devices);

	char buffer[128];
	for(int i=0;i<m_dsData->devices.size();++i)
	{
		InputStreamDeviceInfo ifo;
		ifo.ID=i;
		ifo.name=m_dsData->devices[i].module;

		OLECHAR* bstrGuid;
		if (m_dsData->devices[i].lpGuid)
		{
			StringFromCLSID(*m_dsData->devices[i].lpGuid, &bstrGuid);
			wcstombs(buffer, bstrGuid, 128);
			ifo.deviceGUID = buffer;
			::CoTaskMemFree(bstrGuid);
		}
		//ifo.GUID = *m_dsData->devices[i].lpGuid;
		ifo.description = m_dsData->devices[i].Desciption;
		lst.push_back(ifo);
	}
}

bool DirectSoundInputStream::CreateInputStream(int device,uchar numChannels,uint samplesPerSec,uint avgBytesPerSec,uchar bitsPerSample)
{

	m_dsData->devices.clear();
	DirectSoundCaptureEnumerate(DirectSoundOutDeviceEnumCallback,&m_dsData->devices);
	if(m_dsData->capture)
	{
		m_dsData->capture->Release();
		m_dsData->capture=0;
	}
	if(device>=m_dsData->devices.size())
		return false;


	if(DirectSoundCaptureCreate8(m_dsData->devices[device].lpGuid,&m_dsData->capture,0)<0)
	{
		//gLogManager.log(mT("DirectSoundInputStream() - failed to create direct sound"),ELL_WARNING);
		return false;
	}

	if(m_captureBuffer)
	{
		((LPDIRECTSOUNDCAPTUREBUFFER)m_captureBuffer)->Release();
		m_captureBuffer=0;
	}
	HRESULT hr;
	DSCBUFFERDESC dscbd;

	// wFormatTag, nChannels, nSamplesPerSec, mAvgBytesPerSec,
	// nBlockAlign, wBitsPerSample, cbSize
	WAVEFORMATEX wfx ;
	wfx.cbSize=0;
	wfx.wFormatTag=WAVE_FORMAT_PCM;
	wfx.nSamplesPerSec=samplesPerSec;
	wfx.wBitsPerSample=bitsPerSample;
	wfx.nChannels=numChannels;
	wfx.nBlockAlign=(bitsPerSample>>3)*numChannels;
	wfx.nAvgBytesPerSec=samplesPerSec*wfx.nBlockAlign;
	//{ WAVE_FORMAT_PCM, numChannels, samplesPerSec, /*avgBytesPerSec*/ BlockSize*samplesPerSec, BlockSize, bitsPerSample, 0};

	dscbd.dwSize = sizeof(DSCBUFFERDESC);
	dscbd.dwFlags = 0;
	dscbd.dwBufferBytes = DirectSoundData::BufferBytes;
	dscbd.dwReserved = 0;
	dscbd.lpwfxFormat = &wfx;
	dscbd.dwFXCount = 0;
	dscbd.lpDSCFXDesc = NULL;

	if (FAILED(hr = m_dsData->capture->CreateCaptureBuffer(&dscbd, ((LPDIRECTSOUNDCAPTUREBUFFER*)&m_captureBuffer), NULL)))
	{
		m_captureBuffer = NULL;
		return false;
	}
	m_lastReadOffset = 0;
	return true;
}

bool DirectSoundInputStream::StartInputStream()
{
	if(m_dsData->recording )
	{
		if(m_recording)
			return true;
		else
		{
			//another object is using the Direct sound
			//gLogManager.log(mT("DirectSoundInputStream::StartRecording() - another object is already using the device"),ELL_WARNING);
			return false;
		}
	}
	if(!m_captureBuffer)
	{
//		gLogManager.log(mT("DirectSoundInputStream::StartRecording() - create capture buffer first"),ELL_WARNING);
		return false;
	}
	m_dsData->recording = true;
	m_recording = true;
	m_lastReadOffset = 0;
	((LPDIRECTSOUNDCAPTUREBUFFER)m_captureBuffer)->Start(DSCBSTART_LOOPING);
	return true;
}
void DirectSoundInputStream::CaptureSamples(std::vector<uchar>& buffer)
{
	if(!m_recording)
	{
		return;
	}
	else
	{
		DWORD capturePosition;
		DWORD readPosition;

		((LPDIRECTSOUNDCAPTUREBUFFER)m_captureBuffer)->GetCurrentPosition(&capturePosition, &readPosition);
		int byteCount = readPosition - m_lastReadOffset;
		if(byteCount < 0)
			byteCount += DirectSoundData::BufferBytes;

		void *buf1;
		void *buf2;
		DWORD count1;
		DWORD count2;

		if(!byteCount)
			return;

		((LPDIRECTSOUNDCAPTUREBUFFER)m_captureBuffer)->Lock(m_lastReadOffset, byteCount, &buf1, &count1, &buf2, &count2, 0);

		uint sizeAdd = count1 + count2;
		uint start = buffer.size();

		buffer.resize(start + sizeAdd);

		memcpy(&buffer[0] + start, buf1, count1);
		if(count2)
			memcpy(&buffer[0] + start + count1, buf2, count2);

		((LPDIRECTSOUNDCAPTUREBUFFER)m_captureBuffer)->Unlock(buf1, count1, buf2, count2);

		m_lastReadOffset += sizeAdd;
		m_lastReadOffset %= DirectSoundData::BufferBytes;
	}
}
void DirectSoundInputStream::StopInputStream()
{
	if(m_recording)
	{
		m_dsData->recording=false;
		m_recording = false;
		if(!m_captureBuffer)
			return;
		((LPDIRECTSOUNDCAPTUREBUFFER)m_captureBuffer)->Stop();
		/*
		if(captureBuffer)
			captureBuffer->Release();
			captureBuffer = NULL;
			*/
	}
}

bool DirectSoundInputStream::CanStream()
{
	return !m_dsData->recording;
}

bool DirectSoundInputStream::IsStreaming()
{
	return m_recording;
}


}
}