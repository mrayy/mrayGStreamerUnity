APP_ABI := armeabi-v7a 
APP_PLATFORM := android-23

APP_STL := gnustl_static
APP_CPPFLAGS += -std=c++11
NDK_TOOLCHAIN_VERSION := clang
APP_CPPFLAGS += -frtti
#APP_CPPFLAGS += -fexceptions
