#include "stdafx.h"


#include "AndroidMutex.h"


namespace mray{
namespace OS{

AndroidMutex::AndroidMutex(){

	memset(&m_mutex, 0, sizeof(m_mutex));
	int res = pthread_mutex_init(&m_mutex, NULL);
	if (res != 0)
	{
		//ALOGE("Mutex::Mutex: fail init; error=0x%X", res);
	}
}
AndroidMutex::~AndroidMutex(){
	int res = pthread_mutex_destroy(&m_mutex);
	if (res != 0)
	{
		//ALOGE("Mutex::~Mutex: fail destroy; error=0x%X", res);
	}

}

void AndroidMutex::lock(){
	int res = pthread_mutex_lock(&m_mutex);
	if (res != 0)
	{
	//	ALOGE("Mutex::lock: fail lock; error=0x%X", res);
	}

}

bool AndroidMutex::tryLock()
{
	int res = pthread_mutex_trylock(&m_mutex);
	if ((res != 0) && (res != EBUSY))
	{
		//ALOGE("Mutex::tryLock: error=0x%X", res);
	}
	return res == 0;
}

void AndroidMutex::unlock(){
	int res = pthread_mutex_unlock(&m_mutex);
	if (res != 0)
	{
		//ALOGE("Mutex::unlock: fail unlock; error=0x%X", res);
	}
}




}
}




