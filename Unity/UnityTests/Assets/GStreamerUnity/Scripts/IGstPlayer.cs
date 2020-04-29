using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;	// For DllImport.
using System;

public abstract class IGstPlayer
{

    internal const string DllName = "GStreamerUnityPlugin";

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected void mray_gst_PlayerDestroy(System.IntPtr p);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected void mray_gst_PlayerPlay(System.IntPtr p);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected void mray_gst_PlayerPause(System.IntPtr p);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected void mray_gst_PlayerStop(System.IntPtr p);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected bool mray_gst_PlayerIsLoaded(System.IntPtr p);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected bool mray_gst_PlayerIsPlaying(System.IntPtr p);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected void mray_gst_PlayerClose(System.IntPtr p);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected bool mray_gst_PlayerSetPosition(System.IntPtr p, long pos);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected long mray_gst_PlayerGetPosition(System.IntPtr p);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected long mray_gst_PlayerGetDuration(System.IntPtr p);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected System.IntPtr mray_gst_PlayerGetLastImage(System.IntPtr p, int index);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected ulong mray_gst_PlayerGetLastImageTimestamp(System.IntPtr p, int index);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected void mray_gst_PlayerSendRTPMetaToHost(System.IntPtr p, int index, [MarshalAs(UnmanagedType.LPStr)]string ip, int port);


    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected void mray_gst_PlayerRTPGetEyeGazeData(System.IntPtr p, int index, ref int x, ref int y, ref int w, ref int h);
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    extern static protected bool mray_gst_playerGrabFrame(System.IntPtr p, ref int w, ref int h, ref int c, int index);




    protected System.IntPtr m_Instance;

    public bool IsLoaded
    {
        get
        {
            return mray_gst_PlayerIsLoaded(m_Instance);
        }
    }

    public bool IsPlaying
    {
        get
        {
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

    public abstract int GetCaptureRate(int index);
    public virtual void Destroy()
    {
        mray_gst_PlayerDestroy(m_Instance);
        m_Instance = IntPtr.Zero;
    }

    public void Play()
    {
        mray_gst_PlayerPlay(m_Instance);
    }

    public void Pause()
    {
        mray_gst_PlayerPause(m_Instance);
    }

    public void Stop()
    {
        mray_gst_PlayerStop(m_Instance);
    }

    public void Close()
    {
        mray_gst_PlayerClose(m_Instance);
    }

    public bool Seek(long pos)
    {
        return mray_gst_PlayerSetPosition(m_Instance, pos);
    }

    public long GetPosition()
    {
        return mray_gst_PlayerGetPosition(m_Instance);
    }

    public long GetDuration()
    {
        return mray_gst_PlayerGetDuration(m_Instance);
    }

    public System.IntPtr GetLastImage(int index)
    {
        return mray_gst_PlayerGetLastImage(m_Instance, index);
    }
    public ulong GetLastImageTimestamp(int index)
    {
        return mray_gst_PlayerGetLastImageTimestamp(m_Instance, index);
    }

    public void SendRTPMetaToHost(int index, string host, int port)
    {
        mray_gst_PlayerSendRTPMetaToHost(m_Instance, index, host, port);
    }
    public Vector4 RTPGetEyegaze(int index)
    {
        int x = 0, y = 0, w = 0, h = 0;
        mray_gst_PlayerRTPGetEyeGazeData(m_Instance, index, ref x, ref y, ref w, ref h);
        return new Vector4(x, y, w, h);
    }
}







