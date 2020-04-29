

#ifndef __GSTCUSTOMDATASTREAMER__
#define __GSTCUSTOMDATASTREAMER__

#include "IGStreamerStreamer.h"
#include <string>

namespace mray
{
namespace video
{

	class GstCustomDataStreamerImpl;
class GstCustomDataStreamer :public IGStreamerStreamer
{
protected:
	GstCustomDataStreamerImpl* m_impl;
	virtual GstPipelineHandler* GetPipeline();
public:
	GstCustomDataStreamer();
	virtual ~GstCustomDataStreamer();

	virtual void BindPorts(const std::string& addr, uint *port, uint count, bool rtcp);
	void SetApplicationDataType(const std::string& dataType,bool autotimestamp,int payload=98);// application/x-"dataType"

	void AddDataFrame(uchar* data,int length);

	virtual bool CreateStream() ;
	virtual void Stream() ;
	virtual bool IsStreaming() ;
	virtual void Stop() ;
	virtual void Close() ;

	int CompressRatio();

	virtual void SetPaused(bool paused) ;
	virtual bool IsPaused() ;
};

}
}


#endif
