using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;	// For DllImport.
using System;

public class GstNetworkPlayer:IGstPlayer  {
	
	
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_gst_createNetworkPlayer();
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_netPlayerSetIP(System.IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string ip, int videoPort, bool rtcp);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_netPlayerCreateStream(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_netPlayerGetFrameSize(System.IntPtr p, ref int w, ref int h, ref int components);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_netPlayerGrabFrame(System.IntPtr p, ref int w, ref int h);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_netPlayerBlitImage(System.IntPtr p, System.IntPtr _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_netPlayerFrameCount(System.IntPtr p);

	public Vector2 FrameSize
	{
		get
		{
			int w=0,h=0,comp=0;
			mray_gst_netPlayerGetFrameSize(m_Instance,ref w,ref h,ref comp);
			return new Vector2(w,h);
		}
	}
	
	public override int GetCaptureRate (int index)
	{
		return mray_gst_netPlayerFrameCount (m_Instance);
	}
	public GstNetworkPlayer()
	{
		m_Instance = mray_gst_createNetworkPlayer();	
	}


	public void SetIP(string ip,int videoPort,bool rtcp)
	{		
		mray_gst_netPlayerSetIP (m_Instance, ip, videoPort, rtcp);
	}
	public bool CreateStream()
	{		
		return mray_gst_netPlayerCreateStream (m_Instance);
	}

	public bool GrabFrame(out Vector2 frameSize,out int comp)
	{
		int w=0,h=0,c=0;
		if(mray_gst_netPlayerGrabFrame(m_Instance,ref w,ref h))
		{
			mray_gst_netPlayerGetFrameSize(m_Instance,ref w,ref h,ref c);
			comp=c;
			frameSize.x=w;
			frameSize.y=h;
			return true;
		}
		comp = 3;
		frameSize.x = frameSize.y = 0;
		return false;
	}
	
	public void BlitTexture( System.IntPtr _NativeTexturePtr, int _TextureWidth, int _TextureHeight )
	{
		if (_NativeTexturePtr == System.IntPtr.Zero) return;

		Vector2 sz = FrameSize;
		if (_TextureWidth != sz.x || _TextureHeight != sz.y) return;	// For now, only works if the texture has the exact same size as the webview.
		
		mray_gst_netPlayerBlitImage(m_Instance, _NativeTexturePtr, _TextureWidth, _TextureHeight);	// We pass Unity's width and height values of the texture
	}
}







