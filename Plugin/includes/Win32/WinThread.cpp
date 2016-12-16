 #include "stdafx.h"

#include "WinThread.h"
#include "WinThreadManager.h"



namespace mray{
namespace OS{


WinThread::WinThread(IThreadFunction*f):IThread(f)
{
	active=false;
	threadHandle=0;
	threadID=0;
	_Arg=0;
	setThreadFunction(f);
	m_cancelEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

/*	m_initMutex=IThreadManager::getInstance().createMutex();
	m_initSync=IThreadManager::getInstance().createCondition();*/
};
WinThread::~WinThread(){
	terminate();
/*	m_initMutex=0;
	m_initSync=0;*/
}
HANDLE WinThread::start(void*arg){
	this->arg(arg);
//	CreateMutex(0,false,0);

	threadHandle = CreateThread(0,0,(LPTHREAD_START_ROUTINE)WinThread::entryPoint,this,0,&threadID);
	setThreadPriority(ETP_Normal);
	
//	HANDLE code = _beginthreadex(0,0,(LPTHREAD_START_ROUTINE)WinThread::entryPoint,this,0,&threadID);
	return threadHandle;
}

void WinThread::setThreadFunction(IThreadFunction*f){
	m_threadFunction=f;
}

void WinThread::run(void*arg){
	if(!m_threadFunction)
		return;
	WinThreadManager* mngr=dynamic_cast<WinThreadManager*>(IThreadManager::getInstancePtr());
	TlsSetValue(mngr->getTlsID(),(IThread*)this);

	m_threadFunction->setup();

	m_threadFunction->execute(this,arg);
	active=false;
}

bool WinThread::isActive(){
	return active;
}
IThreadFunction*WinThread::getThreadFunction(){
	return m_threadFunction;
}

void WinThread::setThreadPriority(EThreadPriority priority){
	int p;
	m_priority = priority;
	switch(m_priority){
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
	SetThreadPriority(threadHandle,p);
}
EThreadPriority WinThread::getThreadPriority(){
	return m_priority;
}
void WinThread::terminate()
{
	active=false;
	if (threadHandle)
	{
		resume();
	//	WaitForSingleObject(threadHandle, INFINITE);
	 	DWORD res=WaitForSingleObject( threadHandle, INFINITE );
	 	if(res==WAIT_TIMEOUT){
	 	}
		TerminateThread(threadHandle,0);
		CloseHandle( threadHandle );
		threadHandle = NULL;
	}
	
}

DWORD WINAPI WinThread::entryPoint(void*pthis){
	WinThread*pt=(WinThread*)pthis;
	pt->active=true;
	pt->run(pt->arg());
	return 0;
}

bool WinThread::sleep(mray::uint milliseconds){
	Sleep(milliseconds);
	return true;
}

bool WinThread::suspend(){
	return SuspendThread(threadHandle)!=-1;
	
}
bool WinThread::resume(){
	return ResumeThread(threadHandle)!=-1;

}

}
}




