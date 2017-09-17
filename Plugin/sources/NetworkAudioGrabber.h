

#ifndef __NetworkAudioGrabber__
#define __NetworkAudioGrabber__

#include "CustomAudioGrabber.h"

namespace mray
{
namespace video
{

	class NetworkAudioGrabberData;
class NetworkAudioGrabber:public CustomAudioGrabber
{
protected:
	NetworkAudioGrabberData* m_data;
public:
	NetworkAudioGrabber();
	virtual ~NetworkAudioGrabber();

	void Init( uint audioPort, int channels, int samplingrate);

	void SetPort( uint audioPort);

	uint GetAudioPort();
};

}
}

#endif
