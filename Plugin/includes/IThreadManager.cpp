#include "stdafx.h"

#include "IThreadManager.h"



namespace mray{
namespace OS{

IThreadManager::IThreadManager(){
}
IThreadManager::~IThreadManager(){
//	traceFunction(OS);
	shutdown();
}

void IThreadManager::killThread(IThread* thread){
	if(!thread)
		return;
	ThreadsList::iterator it=m_threads.begin();
	for (;it!=m_threads.end();++it)
	{
		if((*it)==thread){
			m_threads.erase(it);
			break;
		}
	}
	thread->terminate();
}

void IThreadManager::shutdown(){
	ThreadsList::iterator it=m_threads.begin();
	bool isDone=false;
	while(!isDone)
	{
		isDone=true;
		for (;it!=m_threads.end();++it)
		{
			if((*it)->isActive()){
				(*it)->terminate();
				isDone=false;
			}
		}
	}
	m_threads.clear();
}


}
}
