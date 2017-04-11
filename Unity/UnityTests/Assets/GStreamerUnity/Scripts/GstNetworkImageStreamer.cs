using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class GstNetworkImageStreamer:IGstStreamer {
	
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_gst_createNetworkStreamer();
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_netStreamerSetIP(System.IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string ip, int videoPort, bool rtcp);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_netStreamerCreateStream(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_netStreamerSetGrabber(System.IntPtr p,System.IntPtr g);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_netStreamerSetBitRate(System.IntPtr p,int bitRate);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_netStreamerSetResolution(System.IntPtr p,int width, int height, int fps);

	GstUnityImageGrabber _grabber;

	public GstNetworkImageStreamer()
	{
		m_Instance = mray_gst_createNetworkStreamer();	
	}
	
	
	public void SetIP(string ip,int videoPort,bool rtcp)
	{		
		mray_gst_netStreamerSetIP (m_Instance, ip, videoPort, rtcp);
	}
	public bool CreateStream()
	{		
		return mray_gst_netStreamerCreateStream (m_Instance);
	}

	public void SetGrabber(GstUnityImageGrabber grabber)
	{
		_grabber = grabber;
		mray_gst_netStreamerSetGrabber (m_Instance, grabber.Instance);
	}
	
	public void SetBitRate(int bitrate)
	{
		mray_gst_netStreamerSetBitRate (m_Instance, bitrate);
	}
	public void SetResolution(int w,int h,int fps)
	{
		mray_gst_netStreamerSetResolution (m_Instance, w,h,fps);
	}
}
