 #include "stdafx.h"

#include "OSXThread.h"
#include "OSXThreadManager.h"



namespace mray{
namespace OS{


OSXThread::OSXThread(IThreadFunction*f):IThread(f)
{
	active=false;
    _thread=0;
    _Arg=0;
	setThreadFunction(f);

/*	m_initMutex=IThreadManager::getInstance().createMutex();
	m_initSync=IThreadManager::getInstance().createCondition();*/
};
OSXThread::~OSXThread(){
	terminate();
/*	m_initMutex=0;
	m_initSync=0;*/
}
void* OSXThread::start(void*arg){
	this->arg(arg);

	_thread = pthread_create(&_threadHandle,0,OSXThread::entryPoint,this);
	setThreadPriority(ETP_Normal);
	
//	HANDLE code = _beginthreadex(0,0,(LPTHREAD_START_ROUTINE)OSXThread::entryPoint,this,0,&threadID);
	return &_threadHandle;
}

void OSXThread::setThreadFunction(IThreadFunction*f){
	m_threadFunction=f;
}

void OSXThread::run(void*arg){
	if(!m_threadFunction)
		return;
	
	m_threadFunction->setup();

	m_threadFunction->execute(this,arg);
	active=false;
}

bool OSXThread::isActive(){
	return active;
}
IThreadFunction*OSXThread::getThreadFunction(){
	return m_threadFunction;
}

void OSXThread::setThreadPriority(EThreadPriority priority){
	
}
EThreadPriority OSXThread::getThreadPriority(){
	return m_priority;
}
void OSXThread::terminate()
{
	active=false;
	if (_thread>0)
	{
		resume();
	//	WaitForSingleObject(threadHandle, INFINITE);
        
        pthread_kill(_threadHandle, 0);
        pthread_join(_threadHandle,0);
        _thread=0;
	}
	
}

void* OSXThread::entryPoint(void*pthis){
	OSXThread*pt=(OSXThread*)pthis;
	pt->active=true;
	pt->run(pt->arg());
	return 0;
}

bool OSXThread::sleep(mray::uint milliseconds){
	sleep(milliseconds);
	return true;
}

bool OSXThread::suspend(){
    return false;
	
}
bool OSXThread::resume(){
    return false;

}

}
}




