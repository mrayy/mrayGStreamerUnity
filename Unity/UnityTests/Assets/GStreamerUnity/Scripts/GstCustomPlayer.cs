using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;	// For DllImport.
using System;

public class GstCustomPlayer:IGstPlayer  {



	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_gst_createCustomVideoPlayer();

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_customPlayerSetPipeline(System.IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string pipeline);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_customPlayerCreateStream(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_customPlayerGetFrameSize(System.IntPtr p, ref int w, ref int h, ref int comp);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_customPlayerGrabFrame(System.IntPtr p, ref int w, ref int h);


	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_customPlayerCopyFrame (System.IntPtr p, System.IntPtr target);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_customPlayerCropFrame (System.IntPtr p, System.IntPtr target,int x,int y,int width,int height);


	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private void mray_gst_customPlayerBlitImage(System.IntPtr p, System.IntPtr _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private System.IntPtr mray_gst_customPlayerBlitImageNativeGLCall(System.IntPtr p, System.IntPtr _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_customPlayerFrameCount(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_customPlayerGrabAudioFrame(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_customPlayerGetAudioFrameSize(System.IntPtr p);
	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private int mray_gst_customPlayerChannelsCount(System.IntPtr p);

	[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
	extern static private bool mray_gst_customPlayerCopyAudioFrame(System.IntPtr p,[In,Out]float[] data);


	class _AudioWrapper:IGstAudioPlayer
	{
		GstCustomPlayer _owner;
		public _AudioWrapper(GstCustomPlayer o)
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
			return _owner.SampleRate();
		}
		public bool IsUsingCustomOutput()
		{
			return true;
		}
		public bool GrabFrame()
		{
			return _owner.GrabAudioFrame();
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

	_AudioWrapper _audioWrapper;

	public IGstAudioPlayer AudioWrapper
	{
		get{ return _audioWrapper; }
	}

	public Vector2 FrameSize
	{
		get
		{
			int w=0,h=0,comp=0;
			mray_gst_customPlayerGetFrameSize(m_Instance,ref w,ref h,ref comp);
			return new Vector2(w,h);
		}
	}
	public Vector2 FrameSizeImage
	{
		get
		{
			int w=0,h=0,comp=0;
			mray_gst_customPlayerGetFrameSize(m_Instance,ref w,ref h,ref comp);
			if(comp==1)
				h=h*2/3;
			return new Vector2(w,h);
		}
	}

	public GstCustomPlayer()
	{
		m_Instance = mray_gst_createCustomVideoPlayer();	
		_audioWrapper = new _AudioWrapper (this);
	}

	public override int GetCaptureRate (int index)
	{
		return mray_gst_customPlayerFrameCount (m_Instance);
	}


	public void SetPipeline(string pipeline)
	{		
		mray_gst_customPlayerSetPipeline (m_Instance, pipeline);
	}
	public bool CreateStream()
	{		
		return mray_gst_customPlayerCreateStream (m_Instance);
	}

	public bool GrabFrame(out Vector2 frameSize,out int components)
	{
		int w=0,h=0,c=0;
		if(mray_gst_customPlayerGrabFrame(m_Instance,ref w,ref h))
		{
			mray_gst_customPlayerGetFrameSize(m_Instance,ref w,ref h,ref c);
			components=c;
			frameSize.x=w;
			frameSize.y=h;
			return true;
		}
		components=3;
		frameSize.x = frameSize.y = 0;
		return false;
	}
	public bool CopyFrame(GstImageInfo image)
	{
		bool ret= mray_gst_customPlayerCopyFrame (m_Instance, image.GetInstance ());
		if (ret)
			image.UpdateInfo ();
		return ret;
	}
	public bool CopyFrameCropped(GstImageInfo image,int x,int y,int w,int h)
	{
		bool ret= mray_gst_customPlayerCropFrame (m_Instance, image.GetInstance (),x,y,w,h);
		if (ret)
			image.UpdateInfo ();
		return ret;
	}

	public void BlitTexture( System.IntPtr _NativeTexturePtr, int _TextureWidth, int _TextureHeight )
	{
		if (_NativeTexturePtr == System.IntPtr.Zero) return;

		Vector2 sz = FrameSize;
		if (_TextureWidth != sz.x || _TextureHeight != sz.y) return;	// For now, only works if the texture has the exact same size as the webview.

		GL.IssuePluginEvent(mray_gst_customPlayerBlitImageNativeGLCall(m_Instance, _NativeTexturePtr, _TextureWidth, _TextureHeight), 1);
		//mray_gst_customPlayerBlitImage(m_Instance, _NativeTexturePtr, _TextureWidth, _TextureHeight);	// We pass Unity's width and height values of the texture
	}

	public bool GrabAudioFrame()
	{
		return mray_gst_customPlayerGrabAudioFrame (m_Instance);
	}
	public int GetFrameSize()
	{
		return mray_gst_customPlayerGetAudioFrameSize (m_Instance);
	}

	public bool CopyAudioFrame([In,Out]float[] data)
	{
		return mray_gst_customPlayerCopyAudioFrame (m_Instance,data);
	}

	public int ChannelsCount()
	{
		return mray_gst_customPlayerChannelsCount (m_Instance);
	}
	public int SampleRate()
	{
		return 32000;
	}
}
