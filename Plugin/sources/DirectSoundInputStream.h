

/********************************************************************
	created:	2011/01/15
	created:	15:1:2011   20:41
	filename: 	i:\Programing\GameEngine\mrayEngine\mraySound\DirectSoundInputStream.h
	file path:	i:\Programing\GameEngine\mrayEngine\mraySound
	file base:	DirectSoundInputStream
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef DirectSoundInputStream_h__
#define DirectSoundInputStream_h__

#include "mTypes.h"
#include <string>
#include <vector>

namespace mray
{
namespace sound
{
	struct InputStreamDeviceInfo
	{
		int ID;
		std::string name;
		std::string deviceGUID;
		std::string description;
	};
	class DirectSoundData;
class DirectSoundInputStream
{
private:
protected:

	DirectSoundData* m_dsData;
	uchar	m_numChannels;
	uint	m_samplesPerSec;
	uint	m_avgBytesPerSec;
	uchar	m_bitsPerSample;
	bool	m_recording;
	uint	m_lastReadOffset;
	void*	m_captureBuffer;
public:
	DirectSoundInputStream();
	virtual ~DirectSoundInputStream();

	virtual void ListDevices(std::vector<InputStreamDeviceInfo> &lst);
	virtual bool CreateInputStream(int device,uchar numChannels,uint samplesPerSec,uint avgBytesPerSec,uchar bitsPerSample);
	virtual bool StartInputStream();
	virtual void CaptureSamples(std::vector<uchar>& buffer);
	virtual void StopInputStream();

	virtual bool CanStream();

	virtual bool IsStreaming();
};

}
}

#endif // DirectSoundInputStream_h__
