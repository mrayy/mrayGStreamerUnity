using UnityEngine;
using System.Collections;
using System.Threading;
using System.Collections.Generic;
using System;

public class GstNetworkMultipleTexture : GstBaseTexture {

	public abstract class ITextureProcessor
	{
		public abstract void Close();
		public abstract Texture2D[] GetTextures();
		public abstract bool Process(GstMultipleNetworkPlayer player);
		public abstract GstImageInfo.EPixelFormat GetFormat();
	}

	public string TargetIP="127.0.0.1";
	public string EncoderType="H264";
	public int TargetPort=7000;
	public int StreamsCount=1;

	public ulong[] Timestamp;
	
	private GstMultipleNetworkPlayer _player;

	ITextureProcessor _processor;

	public Texture2D[] ProcessedTextures;

	public string profileType;

	public override Texture2D[] PlayerTexture()
	{
		if (_processor == null)
			return m_Texture;
		else
			return _processor.GetTextures ();
	}

	
	public GstMultipleNetworkPlayer Player
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
	


	Thread _processingThread;
	bool _IsDone=false;

	class FrameData
	{
		public bool arrived=false;
		public Vector2 size;
		public Vector4 Eyegaze;
		public int components;
		public ulong _bufferID;

	}
	FrameData[] _frames;

	public override Vector4 GetEyeGaze(int index)
	{
		return _frames [index].Eyegaze;
	}

	//[SerializeField]
//	List<string> _framesID=new List<string>();

	public ulong GetGrabbedBufferID (int index)
	{
		if(_frames ==null || index>=_frames.Length)
			return 0;
		return _frames [index]._bufferID;
	}

	public bool IsSynced()
	{
		bool s = true;
		if (_frames.Length == 0)
			return true;
		for (int i = 1; i < _frames.Length && s==true; ++i) {
			s = s && (_frames [0]._bufferID == _frames [i]._bufferID);
		}
		return s;
	}

	public override int GetTextureCount ()
	{
		if (_processor == null)
			return StreamsCount;
		else
			return _processor.GetTextures ().Length;
	}
	/*public override int GetCaptureRate (int index)
	{
		return _player.GetCaptureRate (index);
	}*/
	
	public override IGstPlayer GetPlayer(){
		return _player;
	}
	
	protected override void _initialize()
	{
		_initialize (profileType);
	}
	protected void _initialize(string profile)
	{
		
		_player = new GstMultipleNetworkPlayer ();
		_IsDone = false;
	}
	

	public override void Destroy ()
	{
		if (_processingThread != null) {
			_IsDone = true;
			_processingThread.Join ();
			_processingThread = null;
		}
		base.Destroy ();
		_player = null;
		Debug.Log ("Destroying Network Texture");
		if (_processor!=null) {
			_processor.Close ();
			_processor = null;
		}
	}
	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}
	public void ConnectToHost(string ip,int port,int count,bool ovrvision=false)
	{
		TargetIP = ip;
		TargetPort = port;
	//	_ovrvision = ovrvision;
		this.StreamsCount = count;
		Timestamp = new ulong[StreamsCount];
		_frames=new FrameData[StreamsCount];
		for (int i = 0; i < _frames.Length; ++i) {
			_frames [i] = new FrameData ();
			//_framesID.Add ("");
		}
		if(_player.IsLoaded || _player.IsPlaying)
			_player.Close ();

		_player.Format = GstImageInfo.EPixelFormat.EPixel_I420;
		if(_processor!=null)
			_player.Format = _processor.GetFormat();

		_player.SetDecoder (EncoderType);
		_player.SetIP (TargetIP, TargetPort,count,false);
		_player.CreateStream();

		_processingThread = new Thread(new ThreadStart(this.ImageGrabberThread));
		_processingThread.Start();
	}

	void ImageGrabberThread()
	{
		Vector2 sz;
		int c;
		while (!_IsDone) {
			for (int i = 0; i < _frames.Length; ++i) {
				lock (_frames[i]) {
					if (_player.GrabFrame (out sz, out c, i)) {
						_frames [i].size = sz;
						_frames [i].components = c;
						_frames [i]._bufferID++;
						_frames [i].arrived = true;
						_triggerOnFrameGrabbed (i);
					}
				}
			}
		}
	}
	void OnGUI()
	{
		if (_player == null || _frames==null)
			return;
		
		Event e = Event.current;
		
		switch (e.type) {
		case EventType.Repaint:	
		{
				for (int i = 0; i < _frames.Length; ++i) {

					lock (_frames[i]) {
						if (_frames [i].arrived) {
							Resize ((int)_frames [i].size.x, (int)_frames [i].size.y, _frames [i].components, i);
							if (_processor == null) {
								if (m_Texture [i] == null)
									Debug.LogError ("The GstTexture does not have a texture assigned and will not paint.");
								else {
									_frames [i].Eyegaze = Player.RTPGetEyegaze (i);
									_player.BlitTexture (m_Texture [i].GetNativeTexturePtr (), m_Texture [i].width, m_Texture [i].height, i);
								}
							} else
								_processor.Process (_player);
							_frames [i].arrived = false;
							Timestamp [i] = _player.GetLastImageTimestamp (i);

							//For Nakakura's work
							//_player.SendRTPMetaToHost(i,"127.0.0.1",60000);

							_triggerOnFrameBlitted (i);
							OnFrameCaptured (i);
						}
					}
				}
			break;	
		}
		}
	}
}
