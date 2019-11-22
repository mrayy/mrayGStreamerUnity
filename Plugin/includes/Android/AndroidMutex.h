

#ifndef ___AndroidMutex___
#define ___AndroidMutex___

#include "IMutex.h"
#include <pthread.h>

namespace mray{
namespace OS{

class  AndroidMutex:public IMutex
{
	pthread_mutex_t m_mutex;
public:
	AndroidMutex();
	virtual~AndroidMutex();

	virtual void lock();
	virtual bool tryLock();
	virtual void unlock();
};


}
}


#endif




