using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(GstCustomTexture))]
public class FileVideoPlayer : BaseVideoPlayer
{

	public string VideoPath = "";
	public bool AudioSupport = false;
	public bool flipVideo = false;

	public long Position;
	public long Duration;

	// Use this for initialization
	protected override string _GetPipeline()
	{
		if (!VideoPath.Contains(":") || !VideoPath.StartsWith("/"))
			VideoPath = (Application.dataPath + "/" + VideoPath).Replace('\\', '/');

		string pipeline = "filesrc location=\"" + VideoPath + "\" ! qtdemux name=demux " +
			" demux.video_0 ! queue ! decodebin ! videoconvert ! video/x-raw,format=I420 ! " + (flipVideo ? "videoflip method=5 !" : "") + " appsink name=videoSink sync=true";

		if (AudioSupport)
			pipeline += " demux.audio_0 ! queue ! decodebin ! audioconvert ! autoaudiosink name=audioSink sync=true ";

		//		pipeline = "filesrc location=\""+VideoPath+"\" ! decodebin ! videoconvert ! video/x-raw,format=I420 ! appsink name=videoSink sync=true";
		//		pipeline = "filesrc location=~/Documents/Projects/BeyondAR/Equirectangular_projection_SW.jpg ! jpegdec ! videoconvert ! imagefreeze ! videoconvert ! imagefreeze ! videoconvert ! video/x-raw,format=I420 ! appsink name=videoSink sync=true";
		//		string pipeline = "avfvideosrc device-index=4 ! videoconvert ! video/x-raw,format=I420 ! appsink name=videoSink sync=true";

		return pipeline;
	}

	protected override void Update()
	{
		base.Update();
		Position = InternalTexture.Player.GetPosition() / 1000;
		Duration = InternalTexture.Player.GetDuration() / 1000;

	}
}
