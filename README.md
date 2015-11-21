# mrayGStreamerUnity
GStreamer Integration with Unity using a Native plugin 

>>Updated 22/Nov/2015:
Currently designed to run under Unity 64bit version - Windows
The current integration implements a GStreamer H264 UDP receiver texture inside Unity. 

To compile the plugin:

1- Download gstreamer 1.0 sdk 64bit (tested on 1.5.x, 1.6.x x86_64)

http://gstreamer.freedesktop.org/data/pkg/windows/

2- Compile using Visual studio 2013 and put the result dll into Plugins/x86_64 Unity folder (or use the already compiled dll in the Plugins/x86_64 folder)

3- Open the test scene

4- To test the streaming if working or not, run the following pipeline (on the same machine) using command line:

gst-launch-1.0 ksvideosrc device-index=0 ! video/x-raw,format=I420,width=1280,height=720,framerate=30/1 ! videoconvert ! x264enc name=videoEnc bitrate=2000 tune=zerolatency pass=qual ! rtph264pay ! udpsink host=127.0.0.1 port=7000 sync=false -v


You should see the cube textured with the streamed data.

Note: Still the current implementation is buggy, every time you have to restart the editor to be able to run texture streaming

