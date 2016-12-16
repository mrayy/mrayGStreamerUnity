

#ifndef ___WinThread___
#define ___WinThread___


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <IThread.h>


namespace mray{
namespace OS{

class  WinThread:public IThread
{
public :
	WinThread(IThreadFunction*f);
	virtual~WinThread();
	virtual void*start(void*arg);
	virtual void terminate();
	virtual bool isActive();

	virtual void setThreadPriority(EThreadPriority priority);
	virtual EThreadPriority getThreadPriority();

	virtual bool sleep(uint milliseconds);
	virtual bool suspend();
	virtual bool resume();

	IThreadFunction*getThreadFunction();

	virtual void setThreadFunction(IThreadFunction*f);

	HANDLE getCancelEvent(){return m_cancelEvent;}

protected:
	void run(void*arg);
	void*arg()const{return _Arg;}
	void arg(void*a){_Arg=a;}
	static DWORD WINAPI entryPoint(void*arg);
private:
	bool active;
	DWORD threadID;
	HANDLE threadHandle;
	void*_Arg;
	EThreadPriority m_priority;

	HANDLE m_cancelEvent;

	IThreadFunction* m_threadFunction;
};



};//OS
};//mray

#endif



