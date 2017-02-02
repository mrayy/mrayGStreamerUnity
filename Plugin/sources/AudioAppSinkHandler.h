
#ifndef __AUDIOAPPSINKHANDLER__
#define __AUDIOAPPSINKHANDLER__

#include "IAppSinkHandler.h"
#include "IMutex.h"

namespace mray
{
namespace video
{


	struct AudioInfo
	{
	public:
		AudioInfo(){ data = 0; channels = 0; length = 0; dataLength = 0; }
		~AudioInfo(){ Clear(); }

		void Clear()
		{
			if (data)
				delete[] data;
			data = 0; channels = 0; length = 0;
			dataLength = 0;
		}
		void CreateData(int length, int channels)
		{
			if (this->length != length || this->channels != this->channels)
			{
				Clear();
				this->length = length;
				this->channels = channels;
				this->dataLength = length*channels;
				if (dataLength > 0)
					this->data = new float[dataLength];
				else
					return;

			}

		}
		void SetData(void* data, int length, int channels)
		{
			CreateData(length, channels);
			memcpy(this->data, data, dataLength*sizeof(float));
		}
		int channels;
		int length;
		int dataLength;
		float* data;

	};
class AudioAppSinkHandler :public IAppSinkHandler
{
protected:
	GstMapInfo mapinfo;

	OS::IMutex*			m_mutex;
	int m_channelsCount;
	int m_sampleRate;

	std::list<AudioInfo*> m_frames;
	std::list<AudioInfo*> m_graveyard;
	AudioInfo* m_grabbedFrame;
	bool m_closed;

	AudioInfo* _CreateFrame();
	void _RemoveFrame(AudioInfo* frame);

public:
	AudioAppSinkHandler();
	virtual ~AudioAppSinkHandler();

	virtual void SetSink(GstAppSink* s){ IAppSinkHandler::SetSink(s); m_closed = false; }
	virtual void Close();

	virtual GstFlowReturn process_sample(std::shared_ptr<GstSample> sample);

	bool GrabFrame();
	int GetFrameSize();
	void CopyAudioFrame(float* output);

	int ChannelsCount(){ return m_channelsCount; }
	int SampleRate(){ return m_sampleRate; }
};

}
}


#endif
