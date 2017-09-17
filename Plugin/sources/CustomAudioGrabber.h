

#ifndef __CustomAudioGrabber__
#define __CustomAudioGrabber__

#include "IAudioGrabber.h"
#include <string>


namespace mray
{
namespace video
{
	class GstPipelineHandler;
class CustomAudioGrabberImpl;
class CustomAudioGrabber :public IAudioGrabber
{
protected:
	CustomAudioGrabberImpl* m_impl;
public:
	CustomAudioGrabber();
	virtual ~CustomAudioGrabber();

	virtual void Init(const std::string &pipeline, int channels, int samplingrate);

	virtual GstPipelineHandler* GetPipelineHandler();

	virtual bool Start();
	virtual void Pause();
	virtual void Close();
	virtual void Resume() ;
	virtual void Restart() ;
	virtual bool IsStarted();

	virtual uint GetSamplingRate();
	virtual uint GetChannelsCount();

	virtual bool GrabFrame();
	virtual float* GetAudioFrame();
	virtual uint GetAudioFrameSize();

	virtual void SetVolume(float vol);
};

}
}

#endif
