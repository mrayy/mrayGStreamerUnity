using UnityEngine;
using System.Collections;
using System;
using System.Runtime.InteropServices;

//credits: http://runningdimensions.com/blog/?p=5
public class NativeDLL : MonoBehaviour {
	public static T Invoke<T, T2>(IntPtr library, params object[] pars)
	{
		IntPtr funcPtr = GetProcAddress(library, typeof(T2).Name);
		if (funcPtr == IntPtr.Zero)
		{
			Debug.LogWarning("Could not gain reference to method address.");
			return default(T);
		}
		
		var func = Marshal.GetDelegateForFunctionPointer(GetProcAddress(library, typeof(T2).Name), typeof(T2));
		return (T)func.DynamicInvoke(pars);
	}
	
	public static void Invoke<T>(IntPtr library, params object[] pars)
	{
		IntPtr funcPtr = GetProcAddress(library, typeof(T).Name);
		if (funcPtr == IntPtr.Zero)
		{
			Debug.LogWarning("Could not gain reference to method address.");
			return;
		}
		
		var func = Marshal.GetDelegateForFunctionPointer(funcPtr, typeof(T));
		func.DynamicInvoke(pars);
	}

	public static void UnloadModule(string name)
	{
		foreach (System.Diagnostics.ProcessModule m in System.Diagnostics.Process.GetCurrentProcess().Modules) {
			if (m.ModuleName == name) {
				FreeLibrary (m.BaseAddress);
			}
		}
	}

	[DllImport("kernel32", SetLastError = true)]
	[return: MarshalAs(UnmanagedType.Bool)]
	public static extern bool FreeLibrary(IntPtr hModule);
	
	[DllImport("kernel32", SetLastError = true, CharSet = CharSet.Unicode)]
	public static extern IntPtr LoadLibrary(string lpFileName);
	
	[DllImport("kernel32")]
	public static extern IntPtr GetProcAddress(IntPtr hModule, string procedureName);

}
