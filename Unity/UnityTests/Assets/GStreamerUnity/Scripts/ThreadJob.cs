using UnityEngine;
using System.Collections;

public class ThreadJob
{
	private bool m_IsDone = false;
	private object m_Handle = new object();
	protected System.Threading.Thread m_Thread = null;

	public delegate void ThreadHandlerDeleg(ThreadJob t);
	public ThreadHandlerDeleg ThreadHandler; 

	public bool IsDone
	{
		get
		{
			bool tmp;
			lock (m_Handle)
			{
				tmp = m_IsDone;
			}
			return tmp;
		}
		set
		{
			lock (m_Handle)
			{
				m_IsDone = value;
			}
		}
	}
	
	public virtual void Start()
	{
		m_Thread = new System.Threading.Thread(Run);
		m_Thread.Start();
	//	Debug.Log (m_Thread.ManagedThreadId.ToString()+" has started");
	}
	public virtual void Abort()
	{
		IsDone = true;
		if (m_Thread != null) {
			m_Thread.Abort ();
			m_Thread.Join ();
			m_Thread = null;
		}
	}
	
	protected virtual void ThreadFunction() { }
	
	protected virtual void OnFinished() { }
	
	public virtual bool Update()
	{
		if (IsDone)
		{
			OnFinished();
			return true;
		}
		return false;
	}
	IEnumerator WaitFor()
	{
		while(!Update())
		{
			yield return null;
		}
	}
	private void Run()
	{
		if (ThreadHandler != null)
			ThreadHandler (this);
		ThreadFunction();
		//Debug.Log (m_Thread.ManagedThreadId.ToString()+" has stopped");
		IsDone = true;
	}
}
