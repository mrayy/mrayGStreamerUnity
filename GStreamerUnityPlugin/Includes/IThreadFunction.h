

/********************************************************************
	created:	2009/01/02
	created:	2:1:2009   16:27
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\IThreadFunction.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	IThreadFunction
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___IThreadFunction___
#define ___IThreadFunction___


namespace mray{
namespace OS{

	class IThread;
	//! abstract class, just override the methods (setup,execute) to your own use
class IThreadFunction
{
public:
	virtual~IThreadFunction(){}
	virtual void setup(){}
	virtual void execute(IThread*caller,void*arg)=0;
};

}
}


#endif //___IThreadFunction___