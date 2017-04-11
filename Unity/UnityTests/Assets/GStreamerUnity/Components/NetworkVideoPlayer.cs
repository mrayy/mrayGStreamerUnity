using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(GstCustomTexture))]
public class NetworkVideoPlayer : BaseVideoPlayer {

	/* To test video streaming, use this from command line:
	./gst-launch-1.0 autovideosrc ! videoconvert ! \
	x264enc speed-preset=superfast tune=zerolatency pass=qual ! rtph264pay ! udpsink host=127.0.0.1 port=5050 sync=false

	//Stream from mp4 file
	./gst-launch-1.0 filesrc location=VideoFile.mp4 ! qtdemux ! queue  \
		! h264parse ! rtph264pay config-interval=10 ! udpsink port=5050 host=127.0.0.1 sync=true -v 
		
	// for Mac OS, the following element can allow to select camera source
	./gst-launch-1.0 avfvideosrc device-index=1 ! videoconvert ! \
	x264enc speed-preset=superfast tune=zerolatency pass=qual ! rtph264pay ! udpsink host=127.0.0.1 port=5050 sync=false
	*/


	public int Port=5050;

	public bool Audio=true;


	protected override string _GetPipeline ()
	{
		string pipeline="udpsrc port=" + Port.ToString () + " ! application/x-rtp ! rtpjitterbuffer ! queue !  rtph264depay ! h264parse !  avdec_h264 " +
			"! videoconvert ! video/x-raw,format=I420  ! videoflip method=5 ! appsink name=videoSink sync=false ";
		if (Audio)
			pipeline += "udpsrc port=5052 ! application/x-rtp,media=audio,clock-rate=48000,encoding-name=OPUS,payload=96,encoding-params=2  ! rtpopusdepay ! opusdec ! audioconvert ! autoaudiosink sync=false -v";
		//" autoaudiosrc ! audioconvert ! audioresample ! opusenc complexity=5 bitrate-type=vbr frame-size=5 ! rtpopuspay ! udpsink host=172.28.18.87 port=5052 sync=false"

		return pipeline;
	}

}
