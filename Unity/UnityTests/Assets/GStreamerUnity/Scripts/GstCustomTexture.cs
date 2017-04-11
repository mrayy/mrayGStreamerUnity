using UnityEngine;
using System.Collections;
using System.Threading;

public class GstCustomTexture : GstBaseTexture {
	public string Pipeline="";

	private GstCustomPlayer _player;


	public GstCustomPlayer Player
	{
		get	
		{
			return _player;
		}
		set
		{
			if (value != null)
				_player = value;
		}
	}

	public override int GetTextureCount ()
	{
		return 1;
	}
	/*public override int GetCaptureRate (int index)
	{
		return _player.GetCaptureRate (index);
	}*/

	public override IGstPlayer GetPlayer(){
		return _player;
	}

	Thread _imageGrabber;
	bool _isDone=false;

	protected override void _initialize()
	{
		_player = new GstCustomPlayer ();

		_imageGrabber = new Thread (new ThreadStart (ImageGrabberThread));
		_imageGrabber.Start ();
	}
	public override void Destroy ()
	{
		_isDone = true;
		if (_imageGrabber != null)
			_imageGrabber.Join ();

		base.Destroy ();
		_player = null;
	}

	public void SetPipeline(string p)
	{
		Pipeline = p;
		if(_player.IsLoaded || _player.IsPlaying)
			_player.Close ();

		_player.SetPipeline (Pipeline);
		_player.CreateStream();
	}

	bool _imageGrabed=false;
	Vector2 _grabbedSize;
	int _grabbedComponents;
	void ImageGrabberThread()
	{
		Vector2 sz;
		int c;
		while (!_isDone) {
			if (_player.GrabFrame (out sz, out c)) {
				_grabbedSize = sz;
				_grabbedComponents = c;
				_imageGrabed = true;
				_triggerOnFrameGrabbed (0);
			}
		}
	}

	void OnGUI()
	{
		// This function should do input injection (if enabled), and drawing.
		if (_player == null)
			return;

		Event e = Event.current;

		switch (e.type) {
		case EventType.Repaint:	
			{
				//if (_player.GrabFrame (out sz,out components)) {
				if(_imageGrabed){
					Resize ((int)_grabbedSize.x,(int) _grabbedSize.y,_grabbedComponents,0);
					if (m_Texture[0] == null)
						Debug.LogError ("The GstTexture does not have a texture assigned and will not paint.");
					else {
						_player.BlitTexture (m_Texture [0].GetNativeTexturePtr (), m_Texture [0].width, m_Texture [0].height);
					}
					_imageGrabed = false;
					OnFrameCaptured(0);
					_triggerOnFrameBlitted (0);
				}
				break;	
			}
		}
	}

	// Use this for initialization
	void Start () {
	}

	// Update is called once per frame
	void Update () {

	}

}
