

#ifndef ___OSXMutex___
#define ___OSXMutex___

#include "IMutex.h"
#include <mutex>

namespace mray{
namespace OS{

class  OSXMutex:public IMutex
{
    std::mutex m_mutex;
public:
	OSXMutex();
	virtual~OSXMutex();

	virtual void lock();
	virtual bool tryLock();
	virtual void unlock();
};


}
}


#endif




