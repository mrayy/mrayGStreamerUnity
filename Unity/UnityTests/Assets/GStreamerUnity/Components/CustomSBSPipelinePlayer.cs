using UnityEngine;
using UnityEngine.UI;

/**
 * Custom Side By Side 3D Player
 * Using UI Elements to show the both streams to the user
 * Used WVGA video settings from stereolab's ZED camera
 */
[RequireComponent(typeof(GstCustomTexture))]
public class CustomSBSPipelinePlayer : MonoBehaviour
{
    private GstCustomTexture m_Texture;

	protected Texture2D blittedImageLeft;
	protected Texture2D blittedImageRight;


	public Texture2D LeftEye {
		get{ return blittedImageLeft; }
	}
	public Texture2D RightEye{
		get{ return blittedImageRight; }
	}

	public RawImage leftUIImage;
    public RawImage rightUIImage;

	public int Port = 1337;

    private GstImageInfo _img_left;
    private GstImageInfo _img_right;

    private bool _newFrame = false;

    // Use this for initialization
    void Start()
    {
        m_Texture = gameObject.GetComponent<GstCustomTexture>();
        m_Texture.Initialize();
		string pipeline = "udpsrc port=" + Port.ToString () + " ! application/x-rtp, encoding-name=H264, payload=96 ! rtph264depay ! h264parse ! avdec_h264";
        m_Texture.SetPipeline(pipeline + " ! video/x-raw,format=I420 ! videoconvert ! appsink name=videoSink");
        m_Texture.Player.CreateStream();
        m_Texture.Player.Play();

        m_Texture.OnFrameBlitted += OnFrameBlitted;
        _img_left = new GstImageInfo();
        _img_left.Create(1, 1, GstImageInfo.EPixelFormat.EPixel_R8G8B8);

        _img_right = new GstImageInfo();
        _img_right.Create(1, 1, GstImageInfo.EPixelFormat.EPixel_R8G8B8);

        blittedImageLeft = new Texture2D(1, 1);
        blittedImageLeft.filterMode = FilterMode.Bilinear;
        blittedImageLeft.anisoLevel = 0;
        blittedImageLeft.wrapMode = TextureWrapMode.Clamp;

        blittedImageRight = new Texture2D(1, 1);
        blittedImageRight.filterMode = FilterMode.Bilinear;
        blittedImageRight.anisoLevel = 0;
        blittedImageRight.wrapMode = TextureWrapMode.Clamp;

        if (leftUIImage != null)
        {
			leftUIImage.texture = blittedImageLeft;
        }

        if (rightUIImage != null)
        {
			rightUIImage.texture = blittedImageRight;
        }
    }

    void OnFrameBlitted(GstBaseTexture src, int index)
    {
        m_Texture.Player.CopyFrame(_img_left);
        m_Texture.Player.CopyFrame(_img_right);
        float w = m_Texture.Player.FrameSizeImage.x;
        float h = m_Texture.Player.FrameSizeImage.y;
        m_Texture.Player.CopyFrameCropped(_img_left, 0, 0, (int)(w / 2), (int)(h));
        m_Texture.Player.CopyFrameCropped(_img_right, (int)(w / 2), 0, (int)(w / 2), (int)(h));
        _newFrame = true;

		//can be moved update function in case of performance issues
		if (_newFrame)
		{
			_img_left.BlitToTexture(blittedImageLeft);
			_img_right.BlitToTexture(blittedImageRight);
			_newFrame = false;
		}
    }

    void OnDestroy()
    {
        if (_img_left != null)
            _img_left.Destory();
        if (_img_right != null)
            _img_right.Destory();
    }

    // Update is called once per frame
    void Update()
	{
    }
}
