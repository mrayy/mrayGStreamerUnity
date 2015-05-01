using UnityEngine;
using System.Collections;

public class GstCustomTexture : MonoBehaviour {
	
	public int m_Width = 64;
	public int m_Height = 64;
	public bool m_FlipX = false;
	public bool m_FlipY = false;
	
	public string Pipeline="";
	
	private GstCustomPlayer _player;
	
	[SerializeField]	// Only for testing purposes.
	private Texture2D m_Texture = null;
	
	
	[SerializeField]
	private bool m_InitializeOnStart = true;
	private bool m_HasBeenInitialized = false;
	
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
	
	public Texture2D PlayerTexture
	{
		get
		{
			return m_Texture;	
		}
	}
	
	public void Initialize()
	{
		m_HasBeenInitialized = true;
		
		GStreamerCore.Ref();
		_player = new GstCustomPlayer ();
		
		// Call resize which will create a texture and a webview for us if they do not exist yet at this point.
		Resize(m_Width, m_Height);
		
		if (GetComponent<GUITexture>())
		{
			GetComponent<GUITexture>().texture = m_Texture;
		}
		else if (GetComponent<Renderer>() && GetComponent<Renderer>().material)
		{		
			GetComponent<Renderer>().material.mainTexture = m_Texture;
			GetComponent<Renderer>().material.mainTextureScale = new Vector2(	Mathf.Abs(GetComponent<Renderer>().material.mainTextureScale.x) * (m_FlipX ? -1.0f : 1.0f),
			                                                                 Mathf.Abs(GetComponent<Renderer>().material.mainTextureScale.y) * (m_FlipY ? -1.0f : 1.0f));
		}
		else
		{
			Debug.LogWarning("There is no Renderer or guiTexture attached to this GameObject! GstTexture will render to a texture but it will not be visible.");
		}
		
	}
	
	
	// Use this for initialization
	void Start () {
		
		if (m_InitializeOnStart && !m_HasBeenInitialized) 
		{
			Initialize ();
		}
	}
	
	// Update is called once per frame
	void Update () {
		
	}
	public void Resize( int _Width, int _Height )
	{
		m_Width = _Width;
		m_Height = _Height;
		
		if (m_Texture == null)
		{
			m_Texture = new Texture2D(m_Width, m_Height, TextureFormat.RGB24, false);
		}
		else
		{	
			m_Texture.Resize(m_Width, m_Height, TextureFormat.RGB24, false);
			m_Texture.Apply(false, false);
		}
		m_Texture.filterMode = FilterMode.Point;
		
	}

	public void SetPipeline(string p)
	{
		Pipeline = p;
		if(_player.IsLoaded || _player.IsPlaying)
			_player.Close ();
		_player.SetPipeline (Pipeline);
		_player.CreateStream();
	}
	
	
	public bool IsLoaded {
		get {
			return _player.IsLoaded;
		}
	}
	
	public bool IsPlaying {
		get {
			return _player.IsPlaying;
		}
	}
	
	public void Destroy()
	{
		if (_player != null)
		{
			_player.Destroy ();
			_player = null;
			GStreamerCore.Unref();
		}
	}
	
	public void Play()
	{
		_player.Play ();
	}
	
	public void Pause()
	{
		_player.Pause ();
	}
	
	public void Stop()
	{
		_player.Stop ();
	}
	
	public void Close()
	{
		_player.Close ();
	}
	void OnDestroy()
	{
		Destroy ();
	}
	
	void OnApplicationQuit()
	{
		Destroy ();
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
			if (_player.GrabFrame (out sz)) {
				Resize ((int)sz.x,(int) sz.y);
				if (m_Texture == null)
					Debug.LogError ("The GstTexture does not have a texture assigned and will not paint.");
				else
					_player.BlitTexture (m_Texture.GetNativeTexturePtr (), m_Texture.width, m_Texture.height);
			}
			break;	
		}
		}
	}
}
