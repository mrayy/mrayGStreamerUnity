

#ifndef IGStreamerStreamer_h__
#define IGStreamerStreamer_h__

#include "IStreamListener.h"
#include "ListenerContainer.h"
#include "mTypes.h"
#include <string>

namespace mray
{
namespace video
{
	class GstPipelineHandler;
class IGStreamerStreamer:public ListenerContainer<IGStreamerStreamerListener*>
{
protected:

	DECLARE_FIRE_METHOD(OnStreamerStarted, (IGStreamerStreamer* s),(s));
	DECLARE_FIRE_METHOD(OnStreamerStopped, (IGStreamerStreamer* s), (s));
	DECLARE_FIRE_METHOD(OnStreamerReady, (IGStreamerStreamer* s), (s));

	virtual GstPipelineHandler* GetPipeline() = 0;

public:
	IGStreamerStreamer(){}
	virtual ~IGStreamerStreamer(){}

	virtual bool CreateStream() = 0;
	virtual void Stream() = 0;
	virtual bool IsStreaming() = 0;
	virtual void Stop() = 0;
	virtual void Close()=0;

	virtual void SetPaused(bool paused) = 0;
	virtual bool IsPaused() = 0;

	virtual bool QueryLatency(bool &isLive, ulong& minLatency, ulong& maxLatency);

	virtual void SetClockBase(ulong c) ;
	virtual ulong GetClockBase() ;
};

}
}

#endif // IGStreamerStreamer_h__
