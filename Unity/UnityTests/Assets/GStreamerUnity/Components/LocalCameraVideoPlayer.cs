using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(GstCustomTexture))]
public class LocalCameraVideoPlayer : BaseVideoPlayer {
	
	public bool AudioSupport = false;
	public int CameraIndex=0;

	// Use this for initialization
	protected override string _GetPipeline ()
	{
//		pipeline = "filesrc location=\""+VideoPath+"\" ! decodebin ! videoconvert ! video/x-raw,format=I420 ! appsink name=videoSink sync=true";
//		pipeline = "filesrc location=~/Documents/Projects/BeyondAR/Equirectangular_projection_SW.jpg ! jpegdec ! videoconvert ! imagefreeze ! videoconvert ! imagefreeze ! videoconvert ! video/x-raw,format=I420 ! appsink name=videoSink sync=true";
		string pipeline = "avfvideosrc device-index="+CameraIndex.ToString()+" ! videoconvert ! video/x-raw,format=I420 ! videoflip method=5 ! appsink name=videoSink sync=true";


		pipeline = "autovideosrc ! videoconvert ! video/x-raw,format=I420 ! videoflip method=5 ! appsink name=videoSink sync=true";

		return pipeline;
	}
}
