#include "stdafx.h"


#include "AndroidThreadManager.h"
#include "AndroidThread.h"
#include "AndroidMutex.h"
#include <unistd.h>

namespace mray{
namespace OS{



AndroidThreadManager::AndroidThreadManager(){
}
AndroidThreadManager::~AndroidThreadManager(){
}
IThread* AndroidThreadManager::createThread(IThreadFunction*f,EThreadPriority priority){
	IThread* t= new AndroidThread(f);
	t->setThreadPriority(priority);
	m_threads.push_back(t);
	return t;
}
IMutex* AndroidThreadManager::createMutex(){
	return new AndroidMutex();
}
void AndroidThreadManager::sleep(uint ms){
	::sleep(ms);
}

IThread*AndroidThreadManager::getCurrentThread(){
	pid_t data= gettid();
	if(!data)
		return 0;
	return (IThread*)data;
}


}
}

