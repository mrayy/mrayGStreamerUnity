/********************************************************************
	created:	2009/01/02
	created:	2:1:2009   16:26
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\IThread.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	IThread
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___IThread___
#define ___IThread___

#include "IThreadFunction.h"
#include "mTypes.h"

namespace mray{
namespace OS{

	// For any given priority class, each individual thread’s priority determines how much CPU time it 
	// receives within its process. When a thread is first created, it is given normal priority,
	// but you can change a thread’s priority—even while it is executing.
	enum EThreadPriority{
		ETP_TimeCritical,
		ETP_Highest,
		ETP_AboveNormal,
		ETP_Normal,
		ETP_BelowNormal,
		ETP_Lowest,
		ETP_Idle
	};

class IThread
{
protected:
	IThreadFunction*m_threadFunction;
public :
	IThread(IThreadFunction*f):m_threadFunction(f)
	{}
	virtual~IThread()
	{}
	virtual void* start(void*arg)=0;
	virtual void terminate()=0;
	virtual bool isActive()=0;

	IThreadFunction*GetThreadFunction(){return m_threadFunction; }

	virtual void setThreadPriority(EThreadPriority priority)=0;
	virtual EThreadPriority getThreadPriority()=0;

	virtual bool sleep(uint milliseconds)=0;
	virtual bool suspend()=0;
	virtual bool resume()=0;

	virtual void setThreadFunction(IThreadFunction*f)=0;

protected:

};


}
}


#endif //___IThread___