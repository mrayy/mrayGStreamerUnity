

#include "stdafx.h"
#include "IGStreamerPlayer.h"
#include "GstPipelineHandler.h"



namespace mray
{
namespace video
{

bool IGStreamerPlayer::QueryLatency(bool &isLive, ulong& minLatency, ulong& maxLatency)
{
	return GetPipeline()->QueryLatency(isLive, minLatency, maxLatency);
}


void IGStreamerPlayer::SetClockBase(ulong c)
{

	GetPipeline()->SetClockBaseTime(c);
}
ulong IGStreamerPlayer::GetClockBase()
{
	return GetPipeline()->GetClockBaseTime();
}


}
}