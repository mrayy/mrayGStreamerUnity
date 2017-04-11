using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public class GstImageInfo {

	public enum EPixelFormat
	{
		EPixel_Unkown,

		EPixel_LUMINANCE8,
		EPixel_LUMINANCE16,

		EPixel_Alpha8,
		EPixel_Alpha4Luminance4,
		EPixel_Alpha8Luminance8,

		EPixel_R5G6B5,
		EPixel_R8G8B8,
		EPixel_R8G8B8A8,
		EPixel_X8R8G8B8,

		EPixel_B8G8R8,
		EPixel_B8G8R8A8,
		EPixel_X8B8G8R8,

		EPixel_Float16_R,
		EPixel_Float16_RGB,
		EPixel_Float16_RGBA,
		EPixel_Float16_GR,

		EPixel_Float32_R,
		EPixel_Float32_RGB,
		EPixel_Float32_RGBA,
		EPixel_Float32_GR,

		EPixel_Depth,
		EPixel_Stecil,


		EPixel_Short_RGBA,
		EPixel_Short_RGB,
		EPixel_Short_GR,

		EPixel_DXT1,
		EPixel_DXT2,
		EPixel_DXT3,
		EPixel_DXT4,
		EPixel_DXT5,

		EPixel_I420,
		EPixel_NV12,

		EPixelFormat_Count
	};

	[DllImport(GStreamerCore.DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_createImageData(int width,int height,EPixelFormat format);

	[DllImport(GStreamerCore.DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_resizeImageData(System.IntPtr ifo,int width,int height,EPixelFormat format);

	[DllImport(GStreamerCore.DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_getImageDataInfo(System.IntPtr ifo,ref int width,ref int height,ref EPixelFormat  format);

	[DllImport(GStreamerCore.DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_deleteImageData(System.IntPtr ifo);

	[DllImport(GStreamerCore.DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_BlitImageDataInfo(System.IntPtr ifo,System.IntPtr TextureNativePtr);

	[DllImport(GStreamerCore.DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_BlitImageNativeGLCall(System.IntPtr p, System.IntPtr _TextureNativePtr);

	[DllImport(GStreamerCore.DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_FlipImageData(System.IntPtr p, bool horizontal,bool vertical);

	[DllImport(GStreamerCore.DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_cloneImageData(System.IntPtr p, System.IntPtr dst);

	[DllImport(GStreamerCore.DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_copyCroppedImageData (System.IntPtr ifo, System.IntPtr dst, int x, int y, int width, int height, bool clamp);

	[DllImport(GStreamerCore.DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_getImageDataPtr(System.IntPtr p);

	System.IntPtr _instance;

	int _width,_height;
	EPixelFormat _format;

	public int Width{ get{ return _width;} }
	public int Height{ get { return _height; } }
	public EPixelFormat Format{get{return _format;}}

	public static TextureFormat ConvertFormat(EPixelFormat fmt)
	{
		switch (fmt) {
		case EPixelFormat.EPixel_LUMINANCE8:
		case EPixelFormat.EPixel_Alpha8:
		case EPixelFormat.EPixel_I420:
			return TextureFormat.Alpha8;

		case EPixelFormat.EPixel_R8G8B8:
		case EPixelFormat.EPixel_B8G8R8:
			return TextureFormat.RGB24;

		case EPixelFormat.EPixel_R8G8B8A8:
		case EPixelFormat.EPixel_X8R8G8B8:
		case EPixelFormat.EPixel_B8G8R8A8:
		case EPixelFormat.EPixel_X8B8G8R8:
			return TextureFormat.RGBA32;
		}

		return TextureFormat.Alpha8;
	}

	public static int GetChannelsCount(EPixelFormat fmt)
	{
		switch (fmt) {
		case EPixelFormat.EPixel_LUMINANCE8:
		case EPixelFormat.EPixel_Alpha8:
		case EPixelFormat.EPixel_I420:
			return 1;

		case EPixelFormat.EPixel_R8G8B8:
		case EPixelFormat.EPixel_B8G8R8:
			return 3;

		case EPixelFormat.EPixel_R8G8B8A8:
		case EPixelFormat.EPixel_X8R8G8B8:
		case EPixelFormat.EPixel_B8G8R8A8:
		case EPixelFormat.EPixel_X8B8G8R8:
			return 4;
		}

		return 1;
	}
	public GstImageInfo()
	{
		_instance = System.IntPtr.Zero;
		_width = _height = 0;
		_format = EPixelFormat.EPixel_Unkown;

	}

	public System.IntPtr GetInstance(){
		return _instance;
	}

	public void CloneTo(GstImageInfo ifo)
	{
		mray_cloneImageData (GetInstance (), ifo.GetInstance ());
	}
	public void CopyFrom(System.IntPtr ifo)
	{
		mray_cloneImageData (ifo, GetInstance());
		UpdateInfo ();
	}

	public void CopyCroppedFrom(System.IntPtr ifo,int x,int y,int width,int height,bool clamp)
	{
		mray_copyCroppedImageData (ifo, _instance, x, y, width, height, clamp);
		UpdateInfo ();
	}

	public void Create(int width,int height,EPixelFormat fmt)
	{
		_width = width;
		_height = height;
		_format = fmt;
		if (_instance != System.IntPtr.Zero)
			mray_resizeImageData (_instance, _width, _height, _format);
		else
			_instance = mray_createImageData (_width, _height, _format);
	}

	public void UpdateInfo()
	{
		mray_getImageDataInfo (_instance, ref _width,ref  _height,ref  _format);
	}

	public void BlitToTexture(Texture2D tex)
	{
		if (tex.width != _width || tex.height != _height || tex.format != ConvertFormat(_format)) {
			tex.Resize (_width, _height, ConvertFormat(_format), false);
			tex.Apply (false,false);

		}
		GL.IssuePluginEvent(mray_BlitImageNativeGLCall(_instance, tex.GetNativeTexturePtr()), 1);
		//mray_BlitImageDataInfo (_instance, tex.GetNativeTexturePtr ());
	}

	public void FlipImage(bool horizontal,bool vertical)
	{
		mray_FlipImageData (_instance, horizontal, vertical);
	}

	public void CopyImageData(ref byte[] result)
	{
		int targetLen = Width * Height*GetChannelsCount(_format);//*2/3;
		if (result == null || result.Length != targetLen) {
			result = new byte[targetLen];
		}

		var src=mray_getImageDataPtr (_instance);

		Marshal.Copy (src,  result, 0,targetLen);
	}

	public void Destory()
	{
		mray_deleteImageData (_instance);
		_instance = System.IntPtr.Zero;
	}

}
