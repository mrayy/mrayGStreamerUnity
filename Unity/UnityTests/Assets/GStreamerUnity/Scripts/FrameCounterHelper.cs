using UnityEngine;
using System.Collections;
using System;

public class FrameCounterHelper  {

	int m_frameCounter = 0;
	float m_lastFramerate = 0;
	public float UpdateTime = 1;
	float dt=0;
	long _lastTime=0;

	public float FPS
	{
		get{
			return m_lastFramerate;
		}
	}
	public FrameCounterHelper()
	{
		_lastTime = DateTime.Now.Ticks;
	}

	public void AddFrame()
	{
		m_frameCounter++;
		long t = DateTime.Now.Ticks;
		dt = (t - _lastTime)/10000000.0f;
		if( dt>UpdateTime )
		{
			m_lastFramerate = (m_frameCounter);
			m_frameCounter = 0;
			_lastTime = t;
		}
	}
}
