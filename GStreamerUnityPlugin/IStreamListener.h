
#ifndef __ISTREAMLISTENER__
#define __ISTREAMLISTENER__

namespace mray
{
namespace video
{

	class IGStreamerStreamer;
	class IGStreamerPlayer;
	
class IGStreamerStreamerListener
{
public:
	virtual void OnStreamerReady(IGStreamerStreamer* s){}
	virtual void OnStreamerStarted(IGStreamerStreamer* s){}
	virtual void OnStreamerStopped(IGStreamerStreamer* s){}
};

class IGStreamerPlayerListener
{
public:
	virtual void OnPlayerReady(IGStreamerPlayer* s){}
	virtual void OnPlayerStarted(IGStreamerPlayer* s){}
	virtual void OnPlayerStopped(IGStreamerPlayer* s){}
};


}
}


#endif
