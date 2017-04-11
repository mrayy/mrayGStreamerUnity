using UnityEngine;
using System.Collections;

public class GstNetworkTexture : GstBaseTexture {


	public string TargetIP="127.0.0.1";
	public int TargetPort=7000;

	private GstNetworkPlayer _player;

	
	public GstNetworkPlayer Player
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
	
	protected override void _initialize()
	{
		_player = new GstNetworkPlayer ();
	}
	

	// Use this for initialization
	void Start () {

	}
	
	// Update is called once per frame
	void Update () {
	
	}
	public void ConnectToHost(string ip,int port)
	{
		TargetIP = ip;
		TargetPort = port;
		if(_player.IsLoaded || _player.IsPlaying)
			_player.Close ();
		_player.SetIP (TargetIP, TargetPort,false);
		_player.CreateStream();
	}
	
	public override void Destroy ()
	{
		base.Destroy ();
		_player = null;
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
				Vector2 sz;
				int components;
				if (_player.GrabFrame (out sz,out components)) {
					Resize ((int)sz.x,(int) sz.y,components,0);
					OnFrameCaptured(0);
					if (m_Texture[0] == null)
						Debug.LogError ("The GstTexture does not have a texture assigned and will not paint.");
					else
						_player.BlitTexture (m_Texture[0].GetNativeTexturePtr (), m_Texture[0].width, m_Texture[0].height);
				}
				break;	
			}
		}
	}
}
