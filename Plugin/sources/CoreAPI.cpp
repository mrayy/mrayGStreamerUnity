

#include "stdafx.h"
#include "CoreAPI.h"
#include "UnityHelpers.h"

#ifdef WIN32
#include "Win32/WinThreadManager.h"
#include "Win32/Win32Network.h"
#else
#include "OSX/OSXThreadManager.h"

#endif

using namespace mray;

extern "C" UNITY_INTERFACE_EXPORT bool mray_gstreamer_initialize()
{
	LogManager::Instance()->LogMessage("mray_gstreamer_initialize");
	if (video::GStreamerCore::RefCount() == 0)
	{
		LogManager::Instance()->LogMessage("Initializing GStreamer Engine");
#ifdef WIN32
		new OS::WinThreadManager();
		new network::Win32Network();
#else
        new OS::OSXThreadManager();
#endif
		LogManager::Instance()->LogMessage("Initializing GStreamer Engine - Done");
	}
	video::GStreamerCore::Ref();
	return true;
}

extern "C" UNITY_INTERFACE_EXPORT void mray_gstreamer_shutdown()
{
	video::GStreamerCore::Unref();
	if (video::GStreamerCore::RefCount() == 0)
    {
#ifdef WIN32

		delete OS::WinThreadManager::getInstancePtr();
		delete network::Win32Network::getInstancePtr();
#else
        delete OS::OSXThreadManager::getInstancePtr();
#endif
	}

}

extern "C" UNITY_INTERFACE_EXPORT bool mray_gstreamer_isActive()
{
	LogMessage("Checking is active", ELL_INFO);
	return video::GStreamerCore::Instance()!=0;

}
