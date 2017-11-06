

#ifndef __LOCALAUDIOGRABBER__
#define __LOCALAUDIOGRABBER__

#include "CustomAudioGrabber.h"

namespace mray
{
namespace video
{
class LocalAudioGrabber:public CustomAudioGrabber
{
protected:
	std::string deviceGUID;
public:
	LocalAudioGrabber();
	virtual ~LocalAudioGrabber();

	void Init(const std::string &deviceGUID, int channels, int samplingrate);

	static int GetInputInterfacesCount();
	static const char* GetInputInterfaceName(int i);
	static const char* GetInputInterfaceGUID(int i);


	static int GetOutputInterfacesCount();
	static const char* GetOutputInterfaceName(int i);
	static const char* GetOutputInterfaceGUID(int i);
};

}
}

#endif
