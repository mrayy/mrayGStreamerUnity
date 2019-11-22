 #include "stdafx.h"

#include "AndroidThread.h"
#include "AndroidThreadManager.h"


#include <unistd.h>

namespace mray{
namespace OS{


AndroidThread::AndroidThread(IThreadFunction*f):IThread(f)
{
	active=false;
	threadHandle=0;
	threadID=0;
	_Arg=0;
	setThreadFunction(f);
	//m_cancelEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

/*	m_initMutex=IThreadManager::getInstance().createMutex();
	m_initSync=IThreadManager::getInstance().createCondition();*/
};
AndroidThread::~AndroidThread(){
	terminate();
/*	m_initMutex=0;
	m_initSync=0;*/
}
void* AndroidThread::start(void*arg){
	this->arg(arg);
//	CreateMutex(0,false,0);

	//threadHandle = CreateThread(0,0,(LPTHREAD_START_ROUTINE)AndroidThread::entryPoint,this,0,&threadID);
	pthread_create(&threadHandle, NULL, AndroidThread::entryPoint, this);


	setThreadPriority(ETP_Normal);
	
//	HANDLE code = _beginthreadex(0,0,(LPTHREAD_START_ROUTINE)AndroidThread::entryPoint,this,0,&threadID);
	return &threadHandle;
}

void AndroidThread::setThreadFunction(IThreadFunction*f){
	m_threadFunction=f;
}

void AndroidThread::run(void*arg){
	if(!m_threadFunction)
		return;
	AndroidThreadManager* mngr=dynamic_cast<AndroidThreadManager*>(IThreadManager::getInstancePtr());
	//TlsSetValue(mngr->getTlsID(),(IThread*)this);

	m_threadFunction->setup();

	m_threadFunction->execute(this,arg);
	active=false;
}

bool AndroidThread::isActive(){
	return active;
}
IThreadFunction*AndroidThread::getThreadFunction(){
	return m_threadFunction;
}

void AndroidThread::setThreadPriority(EThreadPriority priority){
	int p;
	m_priority = priority;
	/*switch(m_priority){
		case ETP_TimeCritical:
			p=THREAD_PRIORITY_TIME_CRITICAL;
			break;
		case ETP_Highest:
			p=THREAD_PRIORITY_HIGHEST;
			break;
		case ETP_AboveNormal:
			p=THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case ETP_Normal:
			p=THREAD_PRIORITY_NORMAL;
			break;
		case ETP_BelowNormal:
			p=THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case ETP_Lowest:
			p=THREAD_PRIORITY_LOWEST;
			break;
		case ETP_Idle:
			p=THREAD_PRIORITY_IDLE;
			break;
		default:
			return;
	}
	SetThreadPriority(threadHandle,p);*/
}
EThreadPriority AndroidThread::getThreadPriority(){
	return m_priority;
}
void AndroidThread::terminate()
{
	active=false;
	if (threadHandle)
	{
		resume();
	//	WaitForSingleObject(threadHandle, INFINITE);
	 	/*DWORD res=WaitForSingleObject( threadHandle, INFINITE );
	 	if(res==WAIT_TIMEOUT){
	 	}
		TerminateThread(threadHandle,0);
		CloseHandle( threadHandle );*/
		int ret;
		pthread_exit(&ret);
		pthread_join(threadHandle, 0);
		threadHandle = 0;
	}
	
}

void * AndroidThread::entryPoint(void*pthis){
	AndroidThread*pt=(AndroidThread*)pthis;
	pt->active=true;
	pt->run(pt->arg());
	return 0;
}

bool AndroidThread::sleep(mray::uint milliseconds){
	::sleep(milliseconds);
	return true;
}

bool AndroidThread::suspend(){
	return false;// SuspendThread(threadHandle) != -1;
	
}
bool AndroidThread::resume(){
	return false;// ResumeThread(threadHandle) != -1;

}

}
}




