

/********************************************************************
	created:	2009/02/26
	created:	26:2:2009   15:38
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\IThreadManager.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	IThreadManager
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___IThreadManager___
#define ___IThreadManager___

#include "ISingleton.h"
#include "IThread.h"
#include <list>

namespace mray{
namespace OS{

	class IMutex;
	class ICondition;
	class MultiThreadListener;
	class IThreadFunction;
	class IThreadEvent;

class  IThreadManager:public ISingleton<IThreadManager>
{
public:
	IThreadManager();
	virtual~IThreadManager();
	virtual IThread* createThread(IThreadFunction*f,EThreadPriority priority=ETP_Normal)=0;
	virtual IMutex* createMutex()=0;

	virtual void killThread(IThread* thread);

	virtual IThread*getCurrentThread()=0;

	virtual void sleep(uint ms)=0;

	virtual void shutdown();

protected:
	typedef std::list<IThread*> ThreadsList; 
	ThreadsList m_threads;
	typedef std::list<MultiThreadListener*> ThreadsListenerList; 
	ThreadsListenerList m_listener;

};

}
}


#endif //___IThreadManager___
