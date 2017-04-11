using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class ImageSampler {
	
	public delegate void OnImageSampledDeleg(ImageSampler sampler,GstImageInfo img);
	public OnImageSampledDeleg OnImageSampled;


	GstBaseTexture _SrcTexture;

	Texture2D _blittedImg;
	GstImageInfo _img;

	Rect _blitRect;
	bool _sampleImage=false;
	bool _sampleRect=false;
	bool _newImg=false;
	int _targetStream=0;

	GstImageInfo.EPixelFormat _format;

	public GstBaseTexture SourceTexture {
		get{ return _SrcTexture; }
	}

	public bool NewImage
	{
		get{
			return _newImg;
		}
	}
	public Rect BlitRect
	{
		get{ return _blitRect; }
	}
	public GstImageInfo SampledImage {
		get {
			return _img;
		}
	}

	public Texture2D BlittedImage {
		get {
			if (_newImg) {
				lock (_img) {
					_img.BlitToTexture (_blittedImg);
					_newImg = false;
				}
			}
			return _blittedImg;
		}
	}

	void _initTexture()
	{
		_SrcTexture.OnFrameGrabbed += OnFrameGrabbed;
		_img = new GstImageInfo ();
		_img.Create (1, 1, _format);

		_blittedImg = new Texture2D (1, 1);
		_blittedImg.filterMode = FilterMode.Bilinear;
		_blittedImg.anisoLevel=0;
		_blittedImg.wrapMode=TextureWrapMode.Clamp;

	}

	void OnFrameGrabbed(GstBaseTexture src,int index)
	{
		if (_sampleImage && index==_targetStream) {
			lock (_img) {
				//_SrcTexture.Player.CopyFrameCropped (_img, (int)BlitRect.x, (int)BlitRect.y, (int)BlitRect.width, (int)BlitRect.height);
				if(_sampleRect)
					_img.CopyCroppedFrom(src.GetPlayer().GetLastImage(index),(int)BlitRect.x, (int)BlitRect.y, (int)BlitRect.width, (int)BlitRect.height,true);
				else
					_img.CopyFrom(src.GetPlayer().GetLastImage(index));


			}
			_sampleImage = false;

			//	_img.FlipImage (false, true);
			if (OnImageSampled != null)
				OnImageSampled (this, _img);
			
			_sampleImage = false;
			_newImg = true;
		}
	}

	public ImageSampler(GstBaseTexture src,GstImageInfo.EPixelFormat format=GstImageInfo.EPixelFormat.EPixel_Alpha8)
	{
		_format = format;
		_SrcTexture = src;
		_initTexture ();
	}
	public void Destroy()
	{
		_img.Destory ();
		_img = null;
	}

	public void SampleRect(Rect rc,int targetStream=0)
	{
		_targetStream = targetStream;
		_blitRect=rc;
		_sampleImage = true;
		_sampleRect = true;
	}
	public void SampleImage(int targetStream=0)
	{
		_targetStream = targetStream;
		_sampleImage = true;
		_sampleRect = false;
	}
}
