using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;	// For DllImport.
using System;

public class GStreamerCore {
	
	public const string DllName = "GStreamerUnityPlugin";
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gstreamer_initialize(  );

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gstreamer_shutdown(  );
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gstreamer_isActive(  );


	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_SetDebugFunction( IntPtr str );


	static public float Time=0;

	static float GetEngineTime()
	{
		return Time;
	}

	static void CallBackFunction(string str)
	{
		Debug.Log("mrayGStreamer: " + str);
	}


	static IntPtr _nativeLibraryPtr;


	public static bool IsActive
	{
		get 	
		{
			return mray_gstreamer_isActive();	
		}
	}
	public static void Ref()
	{
	/*	if (_nativeLibraryPtr == IntPtr.Zero) {
			_nativeLibraryPtr=NativeDLL.LoadLibrary(DllName);
			if(_nativeLibraryPtr==IntPtr.Zero)
			{
				Debug.LogError("Failed to load mrayGStreamer Library:"+DllName);
			}
		}
		if (!IsActive) {

			MyDelegate callback=new MyDelegate(CallBackFunction);
			IntPtr intptr_del=Marshal.GetFunctionPointerForDelegate(callback);
			mray_SetDebugFunction(intptr_del);
		}*/
		mray_gstreamer_initialize();
	}
	public static void Unref()
	{
		if (IsActive) 
			mray_gstreamer_shutdown();
		/*
		if (!IsActive) {
			if(_nativeLibraryPtr!=IntPtr.Zero)
			{
			//	NativeDLL.UnloadModule(DllName);
				Debug.Log(NativeDLL.FreeLibrary(_nativeLibraryPtr)?
				          "mrayGStreamer Library successfuly Unloaded":
				          "Failed to unload mrayGStreamer Library");
			}
		}*/
	}


}
