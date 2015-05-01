

#include "stdafx.h"
#include "CoreAPI.h"
#include "WinThreadManager.h"
#include "Win32Network.h"

using namespace mray;

extern "C" EXPORT_API bool mray_gstreamer_initialize()
{
	if (video::GStreamerCore::RefCount() == 0)
	{
		new OS::WinThreadManager();
		new network::Win32Network();
	}
	video::GStreamerCore::Ref();
	return true;
}

extern "C" EXPORT_API void mray_gstreamer_shutdown()
{
	video::GStreamerCore::Unref();
	if (video::GStreamerCore::RefCount() == 0)
	{
		delete OS::WinThreadManager::getInstancePtr();
		delete network::Win32Network::getInstancePtr();
	}

}

extern "C" EXPORT_API bool mray_gstreamer_isActive()
{
	return video::GStreamerCore::Instance()!=0;

}