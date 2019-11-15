LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

IMMERSIATV_PLAYER_ROOT  := $(abspath $(LOCAL_PATH)/../../../)
PLUGIN_SOURCE_PATH      := $(IMMERSIATV_PLAYER_ROOT)/sources
EXT_SOURCE_PATH         := $(IMMERSIATV_PLAYER_ROOT)/includes

GSTREAMER_PROJECT_PATH := $(IMMERSIATV_PLAYER_ROOT)/Externals/gstreamer/Project/Android

LOCAL_MODULE            := mrayGStreamerUnity
LOCAL_C_INCLUDES        := $(PLUGIN_SOURCE_PATH) $(EXT_SOURCE_PATH)
LOCAL_SRC_FILES         := $(EXT_SOURCE_PATH)/ImageInfo.cpp \
							$(EXT_SOURCE_PATH)/PixelUtil.cpp 

LOCAL_LDLIBS := -llog

APP_STL := gnustl_static
APP_CPPFLAGS += -std=c++11
NDK_TOOLCHAIN_VERSION := clang

LOCAL_SHARED_LIBRARIES  := gstreamer_android
LOCAL_LDLIBS            := -llog -lGLESv2
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
include $(GSTREAMER_PROJECT_PATH)/jni/Android.mk
