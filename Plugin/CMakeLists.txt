cmake_minimum_required(VERSION 3.0.0)

project("GStreamerUnityPlugin")

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)


add_compile_definitions(USE_UNITY_GRABBER)

if(APPLE)
        set(ENV{PKG_CONFIG_PATH} "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/pkgconfig")
else()
      #  set(ZLIB_ROOT  "D:/Projects/zlib-1.2.11/")
     #   set(ZLIB_LIBRARY  "D:/Projects/zlib-1.2.11/build/MinSizeRel/zlibstaticd.lib")

     #   add_compile_definitions(USE_UNITY_NETWORK)
        add_compile_definitions(GSTREAMERUNITYPLUGIN_EXPORTS)
endif()
#find_package(ZLIB REQUIRED)


pkg_check_modules(GSTLIBS REQUIRED
        gstreamer-1.0
        gobject-2.0
        glib-2.0
        gstreamer-app-1.0
        gstreamer-audio-1.0
        gstreamer-video-1.0
        )


INCLUDE_DIRECTORIES("${GSTLIBS_INCLUDE_DIRS}")
INCLUDE_DIRECTORIES("./includes" )
INCLUDE_DIRECTORIES("./sources" )


set (SOURCE 
        ${SOURCE}

        "sources/AudioAppSinkHandler.cpp"
        "sources/CMyListener.cpp"
        "sources/CMySink.cpp"
        "sources/CMySrc.cpp"
        "sources/CoreAPI.cpp"
        "sources/CustomAudioGrabber.cpp"
        "sources/GStreamerCore.cpp"
        #"sources/GZipCompress.cpp"
        "sources/GstAppNetAudioStreamer.cpp"
        "sources/GstAudioSampler.cpp"
        #"sources/GstCustomDataReceiver.cpp"
        "sources/GstCustomDataStreamer.cpp"
        "sources/GstCustomDataPlayer.cpp"
        "sources/GstCustomVideoPlayer.cpp"
        "sources/GstCustomVideoStreamer.cpp"
        "sources/GstNetworkAudioPlayer.cpp"
        "sources/GstNetworkAudioStreamer.cpp"
        "sources/GstNetworkMultipleVideoPlayer.cpp"
        "sources/GstNetworkVideoPlayer.cpp"
        "sources/GstNetworkVideoStreamer.cpp"
        "sources/GstPipelineHandler.cpp"
        "sources/GstPlayerBin.cpp"
        "sources/IAppSinkHandler.cpp"
        "sources/IGStreamerPlayer.cpp"
        "sources/IGStreamerStreamer.cpp"
        "sources/NetworkAudioGrabber.cpp"
        "sources/PlayersAPI.cpp"
        "sources/StreamersAPI.cpp"
        "sources/UnityHelpers.cpp"
        "sources/UnityImageGrabber.cpp"
        "sources/VideoAppSinkHandler.cpp"
        "sources/rtp.cpp"
        
        "includes/PixelUtil.cpp"
        "includes/NetAddress.cpp"
        "includes/IThreadManager.cpp"
        "includes/ImageInfo.cpp"
        "includes/RenderAPI.cpp"
        "includes/UnityGraphicsDevice.cpp"
        "includes/RenderAPI_OpenGLCoreES.cpp"
        "includes/RenderAPI_OpenGL2.cpp"

)
if(APPLE)
set (SOURCE 
        ${SOURCE}

        "includes/RenderAPI_Metal.mm"
        "includes/OSX/OSXThread.cpp"
        "includes/OSX/OSXThreadManager.cpp"
        "includes/OSX/OSXMutex.cpp"
        "includes/GLEW/glew.c"
)

set(CMAKE_C_FLAGS "-x objective-c")
set(CMAKE_CXX_FLAGS "-x objective-c++")
set(CMAKE_EXE_LINKER_FLAGS "-framework OpenGL -framework CoreFoundation")

else()

set (SOURCE 
        ${SOURCE}

        "includes/Win32/Win32Network.cpp"
        "includes/Win32/Win32UDPClient.cpp"
        "includes/Win32/WinThread.cpp"
        "includes/Win32/WinThreadManager.cpp"
        "includes/Win32/WinMutex.cpp"
       # "sources/DirectSoundInputStream.cpp"
       # "sources/DirectSoundOutputStream.cpp"
        "sources/LocalAudioGrabber.cpp"
        "sources/GstAppAudioPlayer.cpp"
        "sources/UnityAudioGrabber.cpp"
        "includes/RenderAPI_D3D11.cpp"
)

endif()


link_directories("${GSTLIBS_LIBRARY_DIRS}")


if(APPLE)
        add_library("${PROJECT_NAME}"   MODULE  "${SOURCE}")
        set_target_properties("${PROJECT_NAME}"  PROPERTIES BUNDLE TRUE)
       # target_link_libraries("${PROJECT_NAME}" ZLIB::ZLIB)
else()
        add_library("${PROJECT_NAME}"   SHARED  "${SOURCE}")
        target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
     #   target_link_libraries("${PROJECT_NAME}" ${ZLIB_LIBRARY})
endif()
target_link_libraries("${PROJECT_NAME}" ${GSTLIBS_LIBRARIES})
target_link_libraries("${PROJECT_NAME}" ${OPENGL_LIBRARIES})