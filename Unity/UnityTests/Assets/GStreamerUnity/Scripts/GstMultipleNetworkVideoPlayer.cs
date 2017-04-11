using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;	// For DllImport.
using System;

public class GstMultipleNetworkPlayer:IGstPlayer  {
	
	
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_gst_createNetworkMultiplePlayer();
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_multiNetPlayerSetIP(System.IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string ip, int baseVideoPort,int count, bool rtcp);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_multiNetPlayerCreateStream(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_multiNetPlayerGetFrameSize(System.IntPtr p, ref int w, ref int h, ref int components);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_multiNetPlayerGrabFrame(System.IntPtr p, ref int w, ref int h,int index);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_multiNetPlayerBlitImage(System.IntPtr p, System.IntPtr _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight, int index);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_multiNetPlayerCopyData(System.IntPtr p, System.IntPtr _data, int index);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_multiNetPlayerFrameCount(System.IntPtr p,int index);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private uint mray_gst_multiNetPlayerGetVideoPort(System.IntPtr p,int index);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_multiNetPlayerSetFormat(System.IntPtr p,int fmt);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_multiNetPlayerGetFormat(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private ulong mray_gst_multiNetPlayerGetNetworkUsage(System.IntPtr p);
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	static extern System.IntPtr mray_gst_multiNetPlayerBlitImageNativeGLCall(System.IntPtr p, System.IntPtr _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight, int index);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_multiNetPlayerSetDecoderType(System.IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string decoder);

	public GstImageInfo.EPixelFormat Format
	{
		set{
			mray_gst_multiNetPlayerSetFormat(m_Instance,(int)value);
		}
		get{
			return (GstImageInfo.EPixelFormat)mray_gst_multiNetPlayerGetFormat(m_Instance);
		}
	}

	public Vector2 FrameSize
	{
		get
		{
			int w=0,h=0,comp=0;
			mray_gst_multiNetPlayerGetFrameSize(m_Instance,ref w,ref h,ref comp);
			return new Vector2(w,h);
		}
	}
	IntPtr m_arrayPtr=IntPtr.Zero;
	int m_arraySize=0;

	public ulong NetworkUsage
	{
		get{
			return mray_gst_multiNetPlayerGetNetworkUsage(m_Instance);
		}
	}
	
	public GstMultipleNetworkPlayer()
	{
		m_Instance = mray_gst_createNetworkMultiplePlayer();	
	}

	public uint GetVideoPort(int index)
	{
		return mray_gst_multiNetPlayerGetVideoPort (m_Instance, index);
	}
	
	public override int GetCaptureRate (int index)
	{
		return mray_gst_multiNetPlayerFrameCount (m_Instance,index);
	}
	
	public void SetIP(string ip,int baseVideoPort,int count,bool rtcp)
	{		
		mray_gst_multiNetPlayerSetIP (m_Instance, ip, baseVideoPort,count, rtcp);
	}
	public void SetDecoder(string dec)
	{		
		mray_gst_multiNetPlayerSetDecoderType(m_Instance,dec);
	}
	public bool CreateStream()
	{		
		return mray_gst_multiNetPlayerCreateStream (m_Instance);
	}
	
	public bool GrabFrame(out Vector2 frameSize,out int comp,int index)
	{
		int w=0,h=0,c=0;
		if(mray_gst_multiNetPlayerGrabFrame(m_Instance,ref w,ref h,index))
		{
			mray_gst_multiNetPlayerGetFrameSize(m_Instance,ref w,ref h,ref c);
			comp=c;
			frameSize.x=w;
			frameSize.y=h;
			return true;
		}
		comp = 3;
		frameSize.x = frameSize.y = 0;
		return false;
	}

	public System.IntPtr CopyTextureData( byte[] _dataPtr,int index )
	{
		int w=0,h=0,comp=0;
		mray_gst_multiNetPlayerGetFrameSize(m_Instance,ref w,ref h,ref comp);
		int len = w * h * comp;
		if (_dataPtr!=null && _dataPtr.Length != w * h * comp)
			return System.IntPtr.Zero;
		if (m_arraySize!=len ||
			_dataPtr!=null && m_arraySize != _dataPtr.Length) {

			if(m_arrayPtr!=IntPtr.Zero)
				Marshal.FreeHGlobal(m_arrayPtr);

			m_arraySize = len;
			m_arrayPtr = Marshal.AllocHGlobal(m_arraySize);
		}

		mray_gst_multiNetPlayerCopyData(m_Instance, m_arrayPtr,index);	
		if(_dataPtr!=null)
			Marshal.Copy(m_arrayPtr,_dataPtr,0, m_arraySize);
		return m_arrayPtr;
	}
	
	public void BlitTexture( System.IntPtr _NativeTexturePtr, int _TextureWidth, int _TextureHeight,int index )
	{
		if (_NativeTexturePtr == System.IntPtr.Zero) return;
		
		Vector2 sz = FrameSize;
		if (_TextureWidth != sz.x || _TextureHeight != sz.y) return;	// For now, only works if the texture has the exact same size as the webview.
		
		GL.IssuePluginEvent(mray_gst_multiNetPlayerBlitImageNativeGLCall(m_Instance, _NativeTexturePtr, _TextureWidth, _TextureHeight,index), 1);
		//mray_gst_multiNetPlayerBlitImage(m_Instance, _NativeTexturePtr, _TextureWidth, _TextureHeight,index);	// We pass Unity's width and height values of the texture
	}
}







