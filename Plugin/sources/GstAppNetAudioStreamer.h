


#ifndef GstAppNetAudioStreamer_h__
#define GstAppNetAudioStreamer_h__

#include "IGStreamerStreamer.h"
#include "IAudioGrabber.h"

namespace mray
{
namespace video
{

class GstAppNetAudioStreamerImpl;


class GstAppNetAudioStreamer :public IGStreamerStreamer
{
protected:
	GstAppNetAudioStreamerImpl* m_impl;
	GstPipelineHandler* GetPipeline();

public:
	GstAppNetAudioStreamer();
	virtual ~GstAppNetAudioStreamer();

	// addr: target address to stream video to
	// audioport: port for the audio stream , audio rtcp is allocated as audioPort+1 and audioPort+2
	void BindPorts(const std::string& addr, uint *port, uint count, bool rtcp);

	void SetAudioGrabber(IAudioGrabber* g);

	bool CreateStream();
	void Stream();
	bool IsStreaming();
	void Stop();
	virtual void Close();

	virtual void SetPaused(bool paused);
	virtual bool IsPaused();
};

}
}

#endif // GstAppNetAudioStreamer_h__
