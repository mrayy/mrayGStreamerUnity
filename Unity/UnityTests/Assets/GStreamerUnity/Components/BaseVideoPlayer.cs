using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class BaseVideoPlayer : DependencyRoot {

	private GstCustomTexture m_Texture = null;
	Material material;
	OffscreenProcessor _Processor;

	public Shader[] PostProcessors;

	OffscreenProcessor[] _postProcessors;

	public Texture VideoTexture;

	public bool ConvertToRGB=true;

	public GstCustomTexture InternalTexture
	{
		get{return m_Texture;}
	}


	public delegate void Delg_OnFrameAvailable(BaseVideoPlayer src,Texture tex);
	public event Delg_OnFrameAvailable OnFrameAvailable;

	protected abstract string _GetPipeline ();

	// Use this for initialization
	protected override void Start () {

		_Processor=new OffscreenProcessor();
		m_Texture = gameObject.GetComponent<GstCustomTexture>();

		material=gameObject.GetComponent<MeshRenderer> ().material;
		// Check to make sure we have an instance.
		if (m_Texture == null)
		{
			DestroyImmediate(this);
		}

		m_Texture.Initialize ();
		//		pipeline = "filesrc location=\""+VideoPath+"\" ! decodebin ! videoconvert ! video/x-raw,format=I420 ! appsink name=videoSink sync=true";
		//		pipeline = "filesrc location=~/Documents/Projects/BeyondAR/Equirectangular_projection_SW.jpg ! jpegdec ! videoconvert ! imagefreeze ! videoconvert ! imagefreeze ! videoconvert ! video/x-raw,format=I420 ! appsink name=videoSink sync=true";
		//		pipeline = "videotestsrc ! videoconvert ! video/x-raw,width=3280,height=2048,format=I420 ! appsink name=videoSink sync=true";
		m_Texture.SetPipeline (_GetPipeline());
		m_Texture.Play ();


		m_Texture.OnFrameGrabbed += OnFrameGrabbed;

		_Processor.ShaderName="Image/I420ToRGB";

		if (PostProcessors != null) {
			_postProcessors = new OffscreenProcessor[PostProcessors.Length];
			for (int i = 0; i < PostProcessors.Length; ++i) {
				_postProcessors [i] = new OffscreenProcessor ();
				_postProcessors [i].ProcessingShader = PostProcessors [i];
			}
		}

		Debug.Log ("Starting Base");
		base.Start ();
	}

	bool _newFrame=false;
	void OnFrameGrabbed(GstBaseTexture src,int index)
	{
		_newFrame = true;
	}

	void _processNewFrame()
	{
		_newFrame = false;
		if (m_Texture.PlayerTexture ().Length == 0)
			return;

		Texture tex=m_Texture.PlayerTexture () [0];

		if (ConvertToRGB) {
			if (m_Texture.PlayerTexture () [0].format == TextureFormat.Alpha8)
				VideoTexture = _Processor.ProcessTexture (tex);
			else
				VideoTexture = tex;
			
		} else {
			VideoTexture = tex;
		}

		if (_postProcessors != null) {
			foreach (var p in _postProcessors) {
				VideoTexture = p.ProcessTexture (VideoTexture);
			}
		}
		material.mainTexture = VideoTexture;

		if (OnFrameAvailable != null)
			OnFrameAvailable (this, VideoTexture);

	}

	// Update is called once per frame
	void Update () {

		if (_newFrame)
			_processNewFrame ();
	}
}
