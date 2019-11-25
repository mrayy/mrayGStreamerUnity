LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

IMMERSIATV_PLAYER_ROOT  := $(abspath $(LOCAL_PATH)/../)
PLUGIN_SOURCE_PATH      := $(IMMERSIATV_PLAYER_ROOT)/sources
EXT_SOURCE_PATH         := $(IMMERSIATV_PLAYER_ROOT)/includes

ifndef GSTREAMER_ROOT_ANDROID
$(error GSTREAMER_ROOT_ANDROID is not defined!)

endif

GSTREAMER_PROJECT_PATH := $(GSTREAMER_ROOT_ANDROID)/Android


LOCAL_MODULE            := GStreamerUnityPlugin
LOCAL_C_INCLUDES        := $(PLUGIN_SOURCE_PATH) $(EXT_SOURCE_PATH)
LOCAL_SRC_FILES         := $(EXT_SOURCE_PATH)/Android/AndroidMutex.cpp \
							$(EXT_SOURCE_PATH)/Android/AndroidThreadManager.cpp\
							$(EXT_SOURCE_PATH)/Android/AndroidThread.cpp\
							$(EXT_SOURCE_PATH)/IThreadManager.cpp\
							$(EXT_SOURCE_PATH)/ImageInfo.cpp\
							$(EXT_SOURCE_PATH)/PixelUtil.cpp\
							$(EXT_SOURCE_PATH)/RenderAPI.cpp\
							$(EXT_SOURCE_PATH)/UnityGraphicsDevice.cpp\
							$(EXT_SOURCE_PATH)/RenderAPI_OpenGLCoreES.cpp\
							$(EXT_SOURCE_PATH)/NetAddress.cpp\
							$(PLUGIN_SOURCE_PATH)/GZipCompress.cpp\
							$(PLUGIN_SOURCE_PATH)/CMyListener.cpp\
							$(PLUGIN_SOURCE_PATH)/rtp.cpp\
							$(PLUGIN_SOURCE_PATH)/GstCustomDataPlayer.cpp\
							$(PLUGIN_SOURCE_PATH)/GstNetworkMultipleVideoPlayer.cpp\
							$(PLUGIN_SOURCE_PATH)/GstNetworkAudioPlayer.cpp\
							$(PLUGIN_SOURCE_PATH)/UnityImageGrabber.cpp\
							$(PLUGIN_SOURCE_PATH)/PlayersAPI.cpp\
							$(PLUGIN_SOURCE_PATH)/VideoAppSinkHandler.cpp\
							$(PLUGIN_SOURCE_PATH)/AudioAppSinkHandler.cpp\
							$(PLUGIN_SOURCE_PATH)/IGStreamerPlayer.cpp\
							$(PLUGIN_SOURCE_PATH)/GStreamerCore.cpp\
							$(PLUGIN_SOURCE_PATH)/GstCustomVideoPlayer.cpp\
							$(PLUGIN_SOURCE_PATH)/GstPipelineHandler.cpp\
							$(PLUGIN_SOURCE_PATH)/IAppSinkHandler.cpp\
							$(PLUGIN_SOURCE_PATH)/CoreAPI.cpp\
							$(PLUGIN_SOURCE_PATH)/UnityHelpers.cpp


LOCAL_SHARED_LIBRARIES  := gstreamer_android 
LOCAL_LDLIBS            := -llog -lGLESv2
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
include $(GSTREAMER_PROJECT_PATH)/Android.mk
