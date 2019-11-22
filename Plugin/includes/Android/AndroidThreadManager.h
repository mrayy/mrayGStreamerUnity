
/********************************************************************
	created:	2009/02/26
	created:	26:2:2009   22:36
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\AndroidThreadManager.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	AndroidThreadManager
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___AndroidThreadManager___
#define ___AndroidThreadManager___

#include <IThreadManager.h>

namespace mray{
namespace OS{

class  AndroidThreadManager:public IThreadManager
{
public:
	AndroidThreadManager();
	~AndroidThreadManager();
	IThread* createThread(IThreadFunction*f,EThreadPriority priority=ETP_Normal);
	IMutex* createMutex();
	ICondition* createCondition();
	IThreadEvent* createEvent();

	IThread*getCurrentThread();

	void sleep(uint ms);

};

}
}


#endif //___AndroidThreadManager___
