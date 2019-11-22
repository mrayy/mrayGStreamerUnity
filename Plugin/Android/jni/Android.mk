LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

IMMERSIATV_PLAYER_ROOT  := $(abspath $(LOCAL_PATH)/../../)
PLUGIN_SOURCE_PATH      := $(IMMERSIATV_PLAYER_ROOT)/sources
EXT_SOURCE_PATH         := $(IMMERSIATV_PLAYER_ROOT)/includes

ifndef GSTREAMER_ROOT_ANDROID
$(error GSTREAMER_ROOT_ANDROID is not defined!)

endif

GSTREAMER_PROJECT_PATH := $(GSTREAMER_ROOT_ANDROID)/Android


LOCAL_MODULE            := GStreamerUnityPlugin
LOCAL_C_INCLUDES        := $(PLUGIN_SOURCE_PATH) $(EXT_SOURCE_PATH)
LOCAL_SRC_FILES         := $(EXT_SOURCE_PATH)/Android/AndroidMutex.cpp


LOCAL_LDLIBS            := -llog -lGLESv2
include $(BUILD_SHARED_LIBRARY)

