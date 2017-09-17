

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

	static int GetInterfacesCount();
	static const char* GetInterfaceName(int i);
	static const char* GetInterfaceGUID(int i);
};

}
}

#endif
