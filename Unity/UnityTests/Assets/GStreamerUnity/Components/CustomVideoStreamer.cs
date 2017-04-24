using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;

public class CustomVideoStreamer : MonoBehaviour,IDependencyNode {

	public CameraCapture camCap;
	public string Pipeline;

	public int width=1280,height=720,fps=30;

	bool _created=false;

	GstCustomVideoStreamer _streamer;
	public void OnDependencyStart(DependencyRoot root)
	{
		Debug.Log ("node start");
		_streamer.SetGrabber (camCap._grabber);
		_streamer.SetPipelineString (Pipeline);
		_streamer.SetResolution(width,height,fps);
	}

	void Destroy()
	{
		_streamer.Pause ();
		Thread.Sleep (100);
		_streamer.Stop ();
		_streamer.Close ();
		return;
		//_streamer.SetGrabber (null);
		_streamer.Destroy ();
		_streamer = null;
	}
	public void OnDependencyDestroyed(DependencyRoot root)
	{
		Debug.Log ("node destroyed");
		Destroy ();
	}

	// Use this for initialization
	void Start () {

		Debug.Log ("start");

		_streamer = new GstCustomVideoStreamer ();

		camCap.AddDependencyNode (this);
	}

	void OnDestroy()
	{
		Destroy ();
	}
	// Update is called once per frame
	void Update () {

		if (!_created && camCap._grabber!=null && camCap.HasData) {

			//important to create stream after data is confirmed
			_streamer.CreateStream ();
			_streamer.Stream ();
			_created = true;
		}
		
	}
}
