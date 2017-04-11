using UnityEngine;
using System.Collections;

public abstract class GstBaseTexture : MonoBehaviour {
	
	//public int m_Width = 64;
	//public int m_Height = 64;
	public bool m_FlipX = false;
	public bool m_FlipY = false;

	FrameCounterHelper[] m_fpsHelper;

	[SerializeField]	// Only for testing purposes.
	protected Texture2D[] m_Texture = null;
	
	
	[SerializeField]
	protected bool m_InitializeOnStart = true;
	protected bool m_HasBeenInitialized = false;

	public abstract int GetTextureCount ();

	public int GetCaptureRate (int index)
	{
		if (m_fpsHelper==null || index >= m_fpsHelper.Length)
			return 0;
		return (int)m_fpsHelper [index].FPS;
	}

	protected abstract void _initialize ();
	public abstract IGstPlayer GetPlayer();


	public delegate void Delg_OnFrameGrabbed(GstBaseTexture src,int index);
	public event Delg_OnFrameGrabbed OnFrameGrabbed;

	public delegate void Delg_OnFrameBlitted(GstBaseTexture src,int index);
	public event Delg_OnFrameBlitted OnFrameBlitted;

	public virtual Vector4 GetEyeGaze(int index)
	{
		return Vector4.zero;
	}

	protected void _triggerOnFrameGrabbed(int index)
	{
		if (this.OnFrameGrabbed != null)
			this.OnFrameGrabbed (this,index);
	}
	protected void _triggerOnFrameBlitted(int index)
	{
		if (this.OnFrameBlitted != null)
			this.OnFrameBlitted(this,index);
	}
	public virtual Texture2D[] PlayerTexture()
	{
		return m_Texture;	
	}

	public bool IsLoaded {
		get {
			if(GetPlayer()!=null)
				return GetPlayer().IsLoaded;
			return false;
		}
	}
	
	public bool IsPlaying {
		get {
			if(GetPlayer()!=null)
				return GetPlayer().IsPlaying;
			return false;
		}
	}

	public void OnFrameCaptured(int index)
	{
		if (m_fpsHelper == null)
			return;
		m_fpsHelper [index].AddFrame ();
	}


	public void Initialize()
	{
		m_HasBeenInitialized = true;
		
		GStreamerCore.Ref();
		_initialize ();

		
	}

	public virtual void Destroy()
	{
		if (GetPlayer() != null)
		{
			GetPlayer().Stop();
			GetPlayer().Close();
			GetPlayer().Destroy ();
			GStreamerCore.Unref();
		}
	}
	
	void OnApplicationQuit()
	{
	//	Destroy ();
	}
	
	public void Play()
	{
		if(GetPlayer()!=null)
			GetPlayer().Play ();
	}
	
	public void Pause()
	{
		if(GetPlayer()!=null)
			GetPlayer().Pause ();
	}
	
	public void Stop()
	{
		if(GetPlayer()!=null)
			GetPlayer().Stop ();
	}
	
	public void Close()
	{
		if(GetPlayer()!=null)
			GetPlayer().Close ();
	}
	void OnDestroy()
	{
		Destroy ();
	}
	TextureFormat GetFormat(int components)
	{
		if (components == 1)
			return TextureFormat.Alpha8;
		if (components == 3)
			return TextureFormat.RGBA32;
		return TextureFormat.RGBA32;
	}
	public virtual void Resize( int _Width, int _Height,int components,int index )
	{
		//m_Width = _Width;
		//m_Height = _Height;

		if (m_Texture!=null && index>= GetTextureCount () )
			return;
		if (m_Texture == null || m_Texture.Length!=GetTextureCount())
		{
			m_fpsHelper=new FrameCounterHelper[GetTextureCount()];
			m_Texture=new Texture2D[GetTextureCount()];
			for(int i=0;i<GetTextureCount();++i)
			{
				m_fpsHelper[i]=new FrameCounterHelper();
				m_Texture[i] = new Texture2D(16, 16, GetFormat(components), false);
				m_Texture[i].filterMode = FilterMode.Bilinear;
				m_Texture[i].anisoLevel=0;
				m_Texture[i].wrapMode=TextureWrapMode.Clamp;
			}
		}
		if (m_Texture [index].width != _Width || m_Texture [index].height != _Height) {
			Debug.Log("Creating Texture video stream: "+_Width.ToString()+"x"+_Height.ToString());
			m_Texture [index].Resize (_Width, _Height, GetFormat (components), false);
			m_Texture [index].Apply (false, false);
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
}
