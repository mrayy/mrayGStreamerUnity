


/********************************************************************
	created:	2011/02/06
	created:	6:2:2011   23:45
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\MutexLocks.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	MutexLocks
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef MutexLocks_h__
#define MutexLocks_h__

#include "IMutex.h"

namespace mray
{
namespace OS
{

class ScopedLock
{
private:
	ScopedLock(const ScopedLock&){}
	ScopedLock& operator=(const ScopedLock&){}
protected:
	IMutex* m_mutex;
public:
	explicit ScopedLock(IMutex* m):m_mutex(m)
	{
		m->lock();
	}
	~ScopedLock()
	{
		m_mutex->unlock();
	}
};
class ReverseScopedLock
{
private:
	ReverseScopedLock(const ScopedLock&){}
	ReverseScopedLock& operator=(const ScopedLock&){}
protected:
	IMutex* m_mutex;
public:
	explicit ReverseScopedLock(IMutex* m):m_mutex(m)
	{
		m->unlock();
	}
	~ReverseScopedLock()
	{
		m_mutex->lock();
	}
};

}
}
#endif // MutexLocks_h__
