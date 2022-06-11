using UnityEngine;
using System.Collections;
using System;
using UnityEngine.UI;

[RequireComponent(typeof(GstCustomTexture))]
public class CustomPipelinePlayer : BaseVideoPlayer {

	public string pipeline = "";

	// Use this for initialization
	protected override string _GetPipeline()
	{
		string P = pipeline + " ! video/x-raw,format=I420 ! videoconvert ! appsink name=videoSink";

		return P;
	}
}
