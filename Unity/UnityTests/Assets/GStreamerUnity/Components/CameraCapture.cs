using UnityEngine;

[RequireComponent(typeof(Camera))]
public class CameraCapture : DependencyRoot
{
	[SerializeField] bool _setResolution = true;
	[SerializeField] int _width = 1280;
	[SerializeField] int _height = 720;
	[SerializeField] bool _allowSlowDown = true;

	public GstUnityImageGrabber _grabber;

	[SerializeField, HideInInspector] Shader _shader;
	Material _material;

	bool _prepared=false;

	RenderTexture _tempTarget;
	public Texture2D _tempTex;

	static int _activePipeCount;

	public bool HasData=false;

	void OnValidate()
	{
	}

	void OnEnable()
	{
		PrepareTexture ();
	}

	void OnDisable()
	{
		DestroyTexture ();
	}

	protected override void OnDestroy()
	{
		base.OnDestroy();
		_grabber.Destroy();
		DestroyTexture ();
	}

	protected override void Start()
	{
//		_material = new Material(_shader);
		_grabber=new GstUnityImageGrabber();
		_width = Screen.width;
		_height = Screen.height;

		base.Start ();
	}

	void Update()
	{
	}

	void OnRenderImage(RenderTexture source, RenderTexture destination)
	{
		if (_prepared)
		{
			var tempRT = RenderTexture.GetTemporary(source.width, source.height);
			Graphics.Blit(source, tempRT);
			if (_tempTex == null) {
				_tempTex = new Texture2D (source.width, source.height, TextureFormat.RGB24, false);

			}
			_tempTex.ReadPixels(new Rect(0, 0, source.width, source.height), 0, 0, false);
			_tempTex.Apply();
			_grabber.SetTexture2D (_tempTex);
			_grabber.Update ();
			HasData = true;
			//_pipe.Write(tempTex.GetRawTextureData());

			//Destroy(tempTex);
			RenderTexture.ReleaseTemporary(tempRT);
		}

		Graphics.Blit(source, destination);
	}

	void PrepareTexture()
	{
		if (_prepared) return;

		var camera = GetComponent<Camera>();
		var width = _width;
		var height = _height;

		// Apply the screen resolution settings.
		if (_setResolution)
		{
			_tempTarget = RenderTexture.GetTemporary(width, height);
			camera.targetTexture = _tempTarget;
		}
		else
		{
			width = camera.pixelWidth;
			height = camera.pixelHeight;
		}
		_prepared = true;
	}

	void DestroyTexture()
	{
		var camera = GetComponent<Camera>();

		// Release the temporary render target.
		if (_tempTarget != null && _tempTarget == camera.targetTexture)
		{
			camera.targetTexture = null;
			RenderTexture.ReleaseTemporary(_tempTarget);
			_tempTarget = null;
		}

		_prepared = false;
	}

}