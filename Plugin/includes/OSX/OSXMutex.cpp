#include "stdafx.h"


#include "OSXMutex.h"


namespace mray{
namespace OS{

OSXMutex::OSXMutex(){
    
}
OSXMutex::~OSXMutex(){
    
}

void OSXMutex::lock(){
    m_mutex.lock();
}

bool OSXMutex::tryLock()
{
    return m_mutex.try_lock();
}

void OSXMutex::unlock(){
    m_mutex.unlock();
}




}
}




