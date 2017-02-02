#include "stdafx.h"

#define _WIN32_WINNT 0x0501 

#include "WinMutex.h"


namespace mray{
namespace OS{

WinMutex::WinMutex(){
	InitializeCriticalSection(&m_mutex);
}
WinMutex::~WinMutex(){
	DeleteCriticalSection(&m_mutex);
}

void WinMutex::lock(){
	EnterCriticalSection(&m_mutex);
}

bool WinMutex::tryLock()
{
	return TryEnterCriticalSection(&m_mutex) ? true : false;
}

void WinMutex::unlock(){
	LeaveCriticalSection(&m_mutex);
}




}
}




