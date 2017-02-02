

#ifndef ___OSXThread___
#define ___OSXThread___

#include <IThread.h>
#include <pthread.h>

namespace mray{
namespace OS{

class  OSXThread:public IThread
{
public :
	OSXThread(IThreadFunction*f);
	virtual~OSXThread();
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

	//HANDLE getCancelEvent(){return m_cancelEvent;}

protected:
	void run(void*arg);
	void*arg()const{return _Arg;}
	void arg(void*a){_Arg=a;}
	static void* entryPoint(void*arg);
private:
	bool active;
    int _thread;
    pthread_t _threadHandle;
	void*_Arg;
	EThreadPriority m_priority;

	IThreadFunction* m_threadFunction;
};



};//OS
};//mray

#endif



