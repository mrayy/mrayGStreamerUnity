using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;	// For DllImport.
using System;
using System.IO;

public class GStreamerCore {
	
	public const string DllName = "GStreamerUnityPlugin";

#if UNITY_ANDROID
    [DllImport("gstreamer_android", CallingConvention = CallingConvention.Cdecl)]
    extern static private UIntPtr gst_android_get_application_class_loader();
#endif

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gstreamer_initialize(  );

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gstreamer_shutdown(  );
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gstreamer_isActive(  );


	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_SetDebugFunction(FuncPtr str );

    [DllImport("RenderUnityPlugin", CallingConvention = CallingConvention.Cdecl)]
    extern static private void mray_Renderer_SetDebugFunction(FuncPtr str);


    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void FuncPtr(
        [MarshalAs(UnmanagedType.LPStr)]string message);


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

    public static void SetupPlugin()
    {

        Debug.Log("Setting up plugin");
#if UNITY_ANDROID && !UNITY_EDITOR
        // Force loading of libgstreamer_android.so 
        gst_android_get_application_class_loader();
       AndroidJNIHelper.debug = true;
        AndroidJavaClass unityPlayer = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
        AndroidJavaObject activity = unityPlayer.GetStatic<AndroidJavaObject>("currentActivity");
     /*   AndroidJavaClass gstAndroid = new AndroidJavaClass("org.freedesktop.gstreamer.GStreamer");
        gstAndroid.CallStatic("init", activity); */
#endif

        // Setup the PATH environment variable so it can find the GstUnityBridge dll.
        var currentPath = Environment.GetEnvironmentVariable("PATH",
            EnvironmentVariableTarget.Process);
        var dllPath = "";
#if UNITY_EDITOR

#if UNITY_EDITOR_32
        dllPath = Application.dataPath + "/Plugins/GStreamer/x86";
#elif UNITY_EDITOR_64
        dllPath = Application.dataPath + "/Plugins/GStreamer/x86_64";
#endif

        if (currentPath != null && currentPath.Contains(dllPath) == false)
            Environment.SetEnvironmentVariable("PATH",
                dllPath + Path.PathSeparator +
                dllPath + "/GStreamer/bin" + Path.PathSeparator +
                currentPath,
                EnvironmentVariableTarget.Process);
#else
        dllPath = Application.dataPath + "/Plugins";
        if (currentPath != null && currentPath.Contains(dllPath) == false)
            Environment.SetEnvironmentVariable("PATH",
                dllPath + Path.PathSeparator +
                currentPath,
                EnvironmentVariableTarget.Process);
        Environment.SetEnvironmentVariable("GST_PLUGIN_PATH", dllPath, EnvironmentVariableTarget.Process);
#endif
        Debug.Log("Done setting up plugin");
    }

    static int refCount = 0;
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
        if (refCount == 0)
        {
            SetupPlugin();

            //if (!IsActive)
            {
                FuncPtr log_handler = null;
                log_handler = (string message) =>  Debug.Log("mrayGST: " + message);
                mray_SetDebugFunction(log_handler);
                mray_Renderer_SetDebugFunction(log_handler);
                Debug.Log("GStreamer Initializing");
                mray_gstreamer_initialize();
                Debug.Log("GStreamer Initialization done");
            }
        }
        refCount++;
    }
    public static void Unref()
	{
        refCount--;
        if (refCount <= 0)
        {
            refCount = 0;
            Debug.Log("GStreamer Shutting down");
            /*if (IsActive)
                mray_gstreamer_shutdown();*/
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


}
