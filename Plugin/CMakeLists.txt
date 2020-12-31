cmake_minimum_required(VERSION 3.0.0)

project("GStreamerUnityPlugin")

find_package(PkgConfig REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)


set(ENV{PKG_CONFIG_PATH} "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/pkgconfig")

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
        "sources/CMyUDPSink.cpp"
        "sources/CMyUDPSrc.cpp"
        "sources/CoreAPI.cpp"
        "sources/CustomAudioGrabber.cpp"
        "sources/GStreamerCore.cpp"
        "sources/GZipCompress.cpp"
        #"sources/GstAppAudioPlayer.cpp"
        "sources/GstAppNetAudioStreamer.cpp"
        "sources/GstAudioSampler.cpp"
        "sources/GstCustomDataPlayer.cpp"
        #"sources/GstCustomDataReceiver.cpp"
        "sources/GstCustomDataStreamer.cpp"
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
        #"sources/LocalAudioGrabber.cpp"
        "sources/NetworkAudioGrabber.cpp"
        "sources/PlayersAPI.cpp"
        "sources/StreamersAPI.cpp"
        #"sources/UnityAudioGrabber.cpp"
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

        "includes/OSX/OSXThread.cpp"
        "includes/OSX/OSXThreadManager.cpp"
        "includes/OSX/OSXMutex.cpp"
        "includes/GLEW/glew.c"

        #"sources/DirectSoundInputStream.cpp"
        #"sources/DirectSoundOutputStream.cpp"
)


link_directories("${GSTLIBS_LIBRARY_DIRS}")

add_library("${PROJECT_NAME}"   MODULE  "${SOURCE}")
set_target_properties("${PROJECT_NAME}"  PROPERTIES BUNDLE TRUE)

target_link_libraries("${PROJECT_NAME}" ${GSTLIBS_LIBRARIES})
target_link_libraries("${PROJECT_NAME}" ZLIB::ZLIB)
target_link_libraries("${PROJECT_NAME}" ${OPENGL_LIBRARIES})