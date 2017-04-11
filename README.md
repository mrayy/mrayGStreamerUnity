# mrayGStreamerUnity
GStreamer Integration with Unity using a Native plugin 

Updated 11/April/2017:

Currently designed to run under Unity 64bit version - Windows
The current integration implements a GStreamer H264 UDP receiver texture inside Unity. 

To compile the plugin:

1- Download and install (Complete, preferebly to C:\) gstreamer 1.0 runtimes 64bit (tested on 1.11.x - x86_64)

http://gstreamer.freedesktop.org/data/pkg/windows/

2- [Important] Add gstreamer binary folder path to System Environment Variables:
System Settigns --> Advanced System Settings --> Advanced Tab --> Environment Variables... --> Path variable

GStreamer path should be similar to this (depending on installation directory):
C:\gstreamer\1.0\x86_64\bin

3- [Optional] Compile using Visual studio 2013 and put the result dll into Plugins/x86_64 Unity folder (or use the already compiled dll in the Plugins/x86_64 folder)

4- Open test scenes and check the info text 
