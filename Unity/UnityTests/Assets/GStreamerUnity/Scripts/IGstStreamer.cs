using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class IGstStreamer : MonoBehaviour {
	
	internal const string DllName = "GStreamerUnityPlugin";
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_StreamerDestroy(System.IntPtr p);


	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_StreamerCreateStream(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_StreamerStream(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_StreamerPause(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_StreamerResume(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_StreamerStop(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_StreamerIsStreaming(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_StreamerClose(System.IntPtr p);
	
	protected System.IntPtr m_Instance;
	
	public bool IsStreaming {
		get {
			return mray_gst_StreamerIsStreaming(m_Instance);
		}
	}

	
	public System.IntPtr NativePtr
	{
		get
		{
			return m_Instance;
		}
	}
	
	public void Destroy()
	{
		mray_gst_StreamerDestroy (m_Instance);
	}
	public bool CreateStream()
	{		
		return mray_gst_StreamerCreateStream (m_Instance);
	}
	
	public void Stream()
	{
		mray_gst_StreamerStream (m_Instance);
	}
	
	public void Pause()
	{
		mray_gst_StreamerPause (m_Instance);
	}
	public void Resume()
	{
		mray_gst_StreamerResume (m_Instance);
	}
	
	public void Stop()
	{
		mray_gst_StreamerStop (m_Instance);
	}
	
	public void Close()
	{
		mray_gst_StreamerClose (m_Instance);
	}
}
