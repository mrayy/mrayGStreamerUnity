
#include "stdafx.h"
#include "IGStreamerStreamer.h"
#include "GstPipelineHandler.h"



namespace mray
{
namespace video
{

bool IGStreamerStreamer::QueryLatency(bool &isLive, ulong& minLatency, ulong& maxLatency)
{
	GstPipelineHandler* p = GetPipeline();
	return p->QueryLatency(isLive, minLatency, maxLatency);
}


void IGStreamerStreamer::SetClockBase(ulong c)
{
	GstPipelineHandler* p = GetPipeline();
	p->SetClockBaseTime(c);

}

ulong IGStreamerStreamer::GetClockBase()
{
	GstPipelineHandler* p = GetPipeline();
	return p->GetClockBaseTime();

}

}
}
