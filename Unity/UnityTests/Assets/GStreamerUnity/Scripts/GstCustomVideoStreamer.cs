using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class GstCustomVideoStreamer:IGstStreamer {
	
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_gst_createCustomImageStreamer();
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_customImageStreamerSetPipeline(System.IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string ip);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_customImageStreamerSetGrabber(System.IntPtr p,System.IntPtr g);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_customImageStreamerSetResolution(System.IntPtr p,int width, int height, int fps);

	GstUnityImageGrabber _grabber;

	public GstCustomVideoStreamer()
	{
		GStreamerCore.Ref();
		m_Instance = mray_gst_createCustomImageStreamer();	
	}
	
	
	public void SetPipelineString(string pipeline)
	{		
		mray_gst_customImageStreamerSetPipeline (m_Instance, pipeline);
	}

	public void SetGrabber(GstUnityImageGrabber grabber)
	{
		_grabber = grabber;
		if(grabber!=null)
			mray_gst_customImageStreamerSetGrabber (m_Instance, grabber.Instance);
		else
			mray_gst_customImageStreamerSetGrabber (m_Instance, System.IntPtr.Zero);
	}

	public void SetResolution(int w,int h,int fps)
	{
		mray_gst_customImageStreamerSetResolution (m_Instance, w,h,fps);
	}
}
