

#ifndef ___AndroidThread___
#define ___AndroidThread___


#include <IThread.h>
#include <pthread.h>

namespace mray{
namespace OS{

class  AndroidThread:public IThread
{
public :
	AndroidThread(IThreadFunction*f);
	virtual~AndroidThread();
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

protected:
	void run(void*arg);
	void*arg()const{return _Arg;}
	void arg(void*a){_Arg=a;}
	static void *entryPoint(void*arg);
private:
	void*_Arg;
	bool active;
	IThreadFunction* m_threadFunction;
	EThreadPriority m_priority;

	pthread_t threadHandle;
	ulong threadID;
	/*
*/
};



};//OS
};//mray

#endif



