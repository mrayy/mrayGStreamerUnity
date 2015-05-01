

#ifndef ___WinMutex___
#define ___WinMutex___

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "IMutex.h"

namespace mray{
namespace OS{

class  WinMutex:public IMutex
{
	CRITICAL_SECTION m_mutex;
public:
	WinMutex();
	virtual~WinMutex();

	virtual void lock();
	virtual bool tryLock();
	virtual void unlock();
};


}
}


#endif




