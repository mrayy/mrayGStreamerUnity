

#ifndef ___ISingleton___
#define ___ISingleton___

#include "UnityHelpers.h"
#include <exception>

namespace mray{

template <typename T>
class ISingleton
{
protected:
	static T*m_instance;
public:
	ISingleton(){
		if (m_instance != 0)
		{
			LogMessage("Instance is already created!", ELL_ERROR);
			//throw new std::exception("Instance is already created");
		}

#if defined( _MSC_VER ) && _MSC_VER < 1200	 
		int offset = (int)(T*)1 - (int)(ISingleton <T>*)(T*)1;
		m_instance = (T*)((int)this + offset);
#else
		m_instance = static_cast< T* >( this );
#endif
	}
	virtual~ISingleton(){
		m_instance=0;
	}
	static T&getInstance(){
		if (m_instance == 0)
		{
			LogMessage("Instance object not created!", ELL_ERROR);
			//throw new std::exception("Instance object not created");
		}
		return *m_instance;
	}
	static bool isExist(){
		return m_instance!=0;
	}
	static T*getInstancePtr(){
		return m_instance;
	}
};
template <typename T> T* ISingleton <T>::m_instance = 0;

}


#endif

