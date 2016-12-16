
#pragma once

#include "GStreamerCore.h"
#include "Unity/IUnityInterface.h"


extern "C" UNITY_INTERFACE_EXPORT bool mray_gstreamer_initialize();

extern "C" UNITY_INTERFACE_EXPORT void mray_gstreamer_shutdown();

extern "C" UNITY_INTERFACE_EXPORT bool mray_gstreamer_isActive();


