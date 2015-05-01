using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;	// For DllImport.
using System;

public class IGstPlayer {
	
	internal const string DllName = "GStreamerUnityPlugin";
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_PlayerDestroy(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_PlayerPlay(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_PlayerPause(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_PlayerStop(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_PlayerIsLoaded(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_PlayerIsPlaying(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_PlayerClose(System.IntPtr p);
	
	protected System.IntPtr m_Instance;

	public bool IsLoaded {
		get {
			return mray_gst_PlayerIsLoaded(m_Instance);
		}
	}
	
	public bool IsPlaying {
		get {
			return mray_gst_PlayerIsPlaying(m_Instance);
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
		mray_gst_PlayerDestroy (m_Instance);
	}

	public void Play()
	{
		mray_gst_PlayerPlay (m_Instance);
	}
	
	public void Pause()
	{
		mray_gst_PlayerPause (m_Instance);
	}
	
	public void Stop()
	{
		mray_gst_PlayerStop (m_Instance);
	}
	
	public void Close()
	{
		mray_gst_PlayerClose (m_Instance);
	}
}







