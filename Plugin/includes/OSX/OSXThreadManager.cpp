#include "stdafx.h"


#include "OSXThreadManager.h"
#include "OSXThread.h"
#include "OSXMutex.h"
#include <unistd.h>

namespace mray{
namespace OS{



OSXThreadManager::OSXThreadManager(){}
OSXThreadManager::~OSXThreadManager(){}
IThread* OSXThreadManager::createThread(IThreadFunction*f,EThreadPriority priority){
	IThread* t= new OSXThread(f);
	t->setThreadPriority(priority);
	m_threads.push_back(t);
	return t;
}
IMutex* OSXThreadManager::createMutex(){
    return new OSXMutex();
}
void OSXThreadManager::sleep(uint ms){
    ::sleep(ms);
}

IThread*OSXThreadManager::getCurrentThread(){
    return 0;
}


}
}

