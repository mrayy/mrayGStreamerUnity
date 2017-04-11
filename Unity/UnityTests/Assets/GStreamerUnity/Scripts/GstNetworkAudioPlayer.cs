using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;	// For DllImport.
using System;

public class GstNetworkAudioPlayer:IGstPlayer  {
	
	
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_gst_createNetworkAudioPlayer();
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_netAudioPlayerSetIP(System.IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string ip, int audioPort, bool rtcp);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_netAudioPlayerCreateStream(System.IntPtr p);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_netAudioPlayerSetVolume(System.IntPtr p, float v);
	
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private uint mray_gst_netAudioPlayerGetAudioPort(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private uint mray_gst_netAudioPlayerUseCustomOutput(System.IntPtr p,bool use);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_netAudioPlayerIsUsingCustomOutput(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_netAudioPlayerGrabFrame(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_netAudioPlayerGetFrameSize(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_netAudioPlayerCopyAudioFrame(System.IntPtr p,[In,Out]float[] data);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_netAudioPlayerChannelsCount(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_netAudioPlayerSetSampleRate(System.IntPtr p,int rate);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_netAudioPlayerSampleRate(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private float mray_gst_ProcessAudioPackets([In,Out] float[] srcData,int startIndex,int channelIndex,int count,int stride,int srcChannels,[In,Out] float[] data,int length,int channels);

	public static float ProcessAudioPackets([In,Out] float[] srcData,int startIndex,int channelIndex,int count,int stride,int srcChannels,[In,Out] float[] data,int length,int channels)
	{
		return mray_gst_ProcessAudioPackets (srcData, startIndex, channelIndex, count, stride, srcChannels, data, length, channels);
	}

	class Wrapper:IGstAudioPlayer
	{
		GstNetworkAudioPlayer _owner;
		public Wrapper(GstNetworkAudioPlayer o)
		{
			_owner=o;
		}
		public bool IsLoaded()
		{
			return _owner.IsLoaded;
		}
		public bool IsPlaying()
		{
			return _owner.IsPlaying;
		}
		public void Play ()
		{
			_owner.Play();
		}
		public void Pause()
		{
			_owner.Pause();
		}
		public void Stop()
		{
			_owner.Stop();
		}
		public void Destroy()
		{
			_owner.Destroy();
		}
		public void Close()
		{
			_owner.Close();
		}
		public int ChannelsCount()
		{
			return _owner.ChannelsCount();
		}
		public int SampleRate()
		{
			return _owner.SampleRate ();
		}
		public bool IsUsingCustomOutput()
		{
			return _owner.IsUsingCustomOutput();
		}
		public bool GrabFrame()
		{
			return _owner.GrabFrame();
		}
		public int GetFrameSize()
		{
			return _owner.GetFrameSize();
		}
		public bool CopyAudioFrame([In,Out]float[] data)
		{
			return _owner.CopyAudioFrame(data);
		}
	}

	Wrapper _audioWrapper;

	public IGstAudioPlayer AudioWrapper
	{
		get{ return _audioWrapper; }
	}

	public GstNetworkAudioPlayer()
	{
		m_Instance = mray_gst_createNetworkAudioPlayer();	
		_audioWrapper = new Wrapper (this);
	}
	
	public uint GetAudioPort()
	{
		return mray_gst_netAudioPlayerGetAudioPort (m_Instance);
	}
	public override int GetCaptureRate (int index)
	{
		return 0;
	}
	
	public void SetIP(string ip,int audioPort,bool rtcp)
	{		
		mray_gst_netAudioPlayerSetIP (m_Instance, ip, audioPort, rtcp);
	}
	public bool CreateStream()
	{		
		return mray_gst_netAudioPlayerCreateStream (m_Instance);
	}

	public void SetVolume(float v)
	{
		mray_gst_netAudioPlayerSetVolume (m_Instance, v);
	}

	public void SetUseCustomOutput(bool use)
	{
		mray_gst_netAudioPlayerUseCustomOutput (m_Instance,use);
	}
	public bool IsUsingCustomOutput()
	{
		return mray_gst_netAudioPlayerIsUsingCustomOutput (m_Instance);
	}

	public bool GrabFrame()
	{
		return mray_gst_netAudioPlayerGrabFrame (m_Instance);
	}
	public int GetFrameSize()
	{
		return mray_gst_netAudioPlayerGetFrameSize (m_Instance);
	}

	public bool CopyAudioFrame([In,Out]float[] data)
	{
		return mray_gst_netAudioPlayerCopyAudioFrame (m_Instance,data);
	}

	public int ChannelsCount()
	{
		return mray_gst_netAudioPlayerChannelsCount(m_Instance);
	}
	public void SetSampleRate(int r)
	{
		 mray_gst_netAudioPlayerSetSampleRate(m_Instance,r);
	}
	public int SampleRate()
	{
		return mray_gst_netAudioPlayerSampleRate(m_Instance);
	}
}







