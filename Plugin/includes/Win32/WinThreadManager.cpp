#include "stdafx.h"


#include "WinThreadManager.h"
#include "WinThread.h"
#include "WinMutex.h"

namespace mray{
namespace OS{



WinThreadManager::WinThreadManager(){
	m_tlsID=TlsAlloc();
}
WinThreadManager::~WinThreadManager(){
	TlsFree(m_tlsID);
}
IThread* WinThreadManager::createThread(IThreadFunction*f,EThreadPriority priority){
	IThread* t= new WinThread(f);
	t->setThreadPriority(priority);
	m_threads.push_back(t);
	return t;
}
IMutex* WinThreadManager::createMutex(){
	return new WinMutex();
}
void WinThreadManager::sleep(uint ms){
	Sleep(ms);
}

IThread*WinThreadManager::getCurrentThread(){
	void*data= TlsGetValue(m_tlsID);
	if(!data)
		return 0;
	return (IThread*)data;
}


}
}

