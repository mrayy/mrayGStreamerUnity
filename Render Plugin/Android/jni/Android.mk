LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)


IMMERSIATV_ROOT  := $(LOCAL_PATH)/../..
PLUGIN_SOURCE_PATH      := $(IMMERSIATV_ROOT)/sources
EXT_SOURCE_PATH         := $(IMMERSIATV_ROOT)/includes

$(warning $(EXT_SOURCE_PATH))

LOCAL_MODULE            := RenderUnityPlugin
LOCAL_C_INCLUDES        := $(PLUGIN_SOURCE_PATH) $(EXT_SOURCE_PATH)
LOCAL_SRC_FILES         :=  $(EXT_SOURCE_PATH)/ImageInfo.cpp\
							$(EXT_SOURCE_PATH)/PixelUtil.cpp\
							$(EXT_SOURCE_PATH)/RenderAPI.cpp\
							$(EXT_SOURCE_PATH)/UnityGraphicsDevice.cpp\
							$(EXT_SOURCE_PATH)/RenderAPI_OpenGLCoreES.cpp\
							$(PLUGIN_SOURCE_PATH)/PlayersAPI.cpp\
							$(PLUGIN_SOURCE_PATH)/UnityHelpers.cpp


LOCAL_C_INCLUDES += $(NDK_ROOT)/sources/third_party/vulkan/src/include
LOCAL_CPPFLAGS += -DSUPPORT_VULKAN=1
LOCAL_CPPFLAGS += -DSUPPORT_OPENGL_ES=1
LOCAL_SRC_FILES += $(EXT_SOURCE_PATH)/RenderAPI_Vulkan.cpp

LOCAL_LDLIBS            := -llog -lGLESv2
include $(BUILD_SHARED_LIBRARY)


