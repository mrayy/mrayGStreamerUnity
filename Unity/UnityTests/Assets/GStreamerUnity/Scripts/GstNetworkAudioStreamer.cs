using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class GstNetworkAudioStreamer:IGstStreamer {
	
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_gst_createAudioNetworkStreamer();
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_audioStreamerAddClient(System.IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string ip, int port);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_audioStreamerGetClientCount(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_audioStreamerRemoveClient(System.IntPtr p,int i);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_audioStreamerClearClients(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private string mray_gst_audioStreamerGetClient(System.IntPtr p,int i);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_audioStreamerSetClientVolume(System.IntPtr p,int i,float volume);


	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_audioStreamerSetChannels(System.IntPtr p,int c);

	GstUnityImageGrabber _grabber;

	public GstNetworkAudioStreamer()
	{
		m_Instance = mray_gst_createAudioNetworkStreamer();	
	}
	
	
	public void AddClient(string ip,int port)
	{		
		mray_gst_audioStreamerAddClient(m_Instance, ip, port);
	}
	public void RemoveClient(int i)
	{		
		mray_gst_audioStreamerRemoveClient(m_Instance, i);
	}
	public int GetClientCount()
	{		
		return mray_gst_audioStreamerGetClientCount(m_Instance);
	}
	public string GetClient(int i)
	{		
		return mray_gst_audioStreamerGetClient(m_Instance, i);
	}

	public void ClearClients()
	{		
		mray_gst_audioStreamerClearClients(m_Instance);
	}

	public void SetClientVolume(int i,float volume)
	{		
		mray_gst_audioStreamerSetClientVolume(m_Instance,i,volume);
	}


	public void SetChannels(int c)
	{
		mray_gst_audioStreamerSetChannels (m_Instance, c);
	}

}
