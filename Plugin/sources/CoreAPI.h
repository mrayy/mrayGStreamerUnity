
#pragma once

#include "UnityPlugin.h"
#include "GStreamerCore.h"


extern "C" EXPORT_API bool mray_gstreamer_initialize();

extern "C" EXPORT_API void mray_gstreamer_shutdown();

extern "C" EXPORT_API bool mray_gstreamer_isActive();


