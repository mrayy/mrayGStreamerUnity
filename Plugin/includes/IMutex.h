
/********************************************************************
	created:	2009/01/02
	created:	2:1:2009   16:52
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\IMutex.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	IMutex
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___IMutex___
#define ___IMutex___



namespace mray{
namespace OS{

class IMutex
{
public:
	IMutex(){}
	virtual~IMutex(){}

	virtual void lock()=0;
	virtual bool tryLock()=0;
	virtual void unlock()=0;
};

}
}


#endif //___IMutex___
