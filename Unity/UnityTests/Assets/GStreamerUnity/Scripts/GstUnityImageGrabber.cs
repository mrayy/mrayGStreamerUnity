using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System;

public class GstUnityImageGrabber {
	
	enum EPixelFormat
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
		
		EPixelFormat_Count
	};

	internal const string DllName = "GStreamerUnityPlugin";
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_gst_createUnityImageGrabber();
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_UnityImageGrabberSetData(System.IntPtr p, System.IntPtr _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight,int Format);

	byte[] m_Texture;
	int m_width,m_height;
	//protected Color32[] m_Pixels;
	//protected GCHandle m_PixelsHandle;
	EPixelFormat m_format;

	protected System.IntPtr m_Instance;

	IntPtr m_lastArrayPtr=IntPtr.Zero;
	int m_lastArrayLength = 0;

	public System.IntPtr Instance
	{
		get{
			return m_Instance;
		}
	}

	public GstUnityImageGrabber()
	{
		m_Instance = mray_gst_createUnityImageGrabber();	
	}

	public void SetTexture2D(Texture2D tex)
	{
		SetTexture2D (tex.GetRawTextureData (), tex.width, tex.height, tex.format);
	}
	public void SetTexture2D(byte[] texture,int width,int height,TextureFormat format)
	{
		m_Texture = texture;
		m_width = width;
		m_height = height;
		if (m_Texture == null) {
			return;
		}

		if (m_Texture.Length != m_lastArrayLength) {
			if(m_lastArrayPtr!=IntPtr.Zero)
				Marshal.FreeHGlobal(m_lastArrayPtr);
			
			m_lastArrayPtr = Marshal.AllocHGlobal(m_Texture.Length);
		}

		switch (format) {
		case TextureFormat.ARGB32:
		case TextureFormat.RGBA32:
			m_format=EPixelFormat.EPixel_R8G8B8A8;
			break;
		case TextureFormat.Alpha8:
			m_format=EPixelFormat.EPixel_Alpha8;
			break;
		case TextureFormat.RGB24:
			m_format=EPixelFormat.EPixel_R8G8B8;
			break;
		}
	}

	public void Update()
	{
		if (m_Texture == null)
			return;
	//	m_Pixels = m_Texture.GetPixels32 (0);
	//	m_PixelsHandle = GCHandle.Alloc(m_Pixels, GCHandleType.Pinned);

		Marshal.Copy(m_Texture, 1, m_lastArrayPtr, m_Texture.Length-1);

		mray_gst_UnityImageGrabberSetData (m_Instance, m_lastArrayPtr, m_width,m_height, (int)m_format);
	}
}
