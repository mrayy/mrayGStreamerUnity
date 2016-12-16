
/********************************************************************
	created:	2009/02/26
	created:	26:2:2009   22:36
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\OSXThreadManager.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	OSXThreadManager
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___OSXThreadManager___
#define ___OSXThreadManager___

#include <IThreadManager.h>

namespace mray{
namespace OS{

class  OSXThreadManager:public IThreadManager
{
	ulong m_tlsID;
public:
	OSXThreadManager();
	~OSXThreadManager();
	IThread* createThread(IThreadFunction*f,EThreadPriority priority=ETP_Normal);
	IMutex* createMutex();
	ICondition* createCondition();
	IThreadEvent* createEvent();

	IThread*getCurrentThread();

	void sleep(uint ms);

	ulong getTlsID(){return m_tlsID;}

};

}
}


#endif //___OSXThreadManager___
