

/********************************************************************
	created:	2012/11/18
	created:	18:11:2012   1:19
	filename: 	D:\Development\mrayEngine\Plugins\mraySound\DirectSoundOutputStream.h
	file path:	D:\Development\mrayEngine\Plugins\mraySound
	file base:	DirectSoundOutputStream
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __DirectSoundOutputStream__
#define __DirectSoundOutputStream__


#include <string>
#include <vector>

namespace mray
{
namespace sound
{

	struct OutputStreamDeviceInfo
	{
		int id;
		std::string name;
		std::string GUID;
		std::string description;
	};
	struct DirectSoundOutDeviceData;
	class DirectSoundOutputStream;

	class IDirectSoundOutputListener
	{
	public:
		virtual unsigned int GetAudioSamples(DirectSoundOutputStream* stream,unsigned char* buffer,unsigned int samples)=0;
		virtual bool OnTimerTick(DirectSoundOutputStream* stream){return true;} // this call back check whether or not to stop the stream
	};

	class DirectSoundOutputStreamData;
class  DirectSoundOutputStream
{
protected:
	DirectSoundOutputStreamData* m_data;
	std::string m_name;

	IDirectSoundOutputListener* m_listener;

public:
	DirectSoundOutputStream();
	virtual~DirectSoundOutputStream();

	void SetName(const std::string& name){m_name=name;}
	const std::string& GetName()const{return m_name;}

	virtual void SetListener(IDirectSoundOutputListener* l){m_listener=l;}
	virtual IDirectSoundOutputListener* GetListener(){return m_listener;}

	virtual void ListDevices(std::vector<OutputStreamDeviceInfo> &lst);
};

}
}

#endif
