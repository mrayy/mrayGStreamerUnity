
/********************************************************************
	created:	2009/04/27
	created:	27:4:2009   19:17
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\ImageInfo.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	ImageInfo
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___ImageInfo___
#define ___ImageInfo___

#include "CompileConfig.h"
#include "mTypes.h"
#include "videoCommon.h"
#include <Point3d.h>

namespace mray{
namespace video{

class  ImageInfo
{
private:
protected:
public:
	ImageInfo();
	virtual~ImageInfo();

	int getPitch()const;

	void clear();
	void setData(const void*data,const math::vector3di&size,EPixelFormat format);
	void createData(const math::vector3di& size,EPixelFormat format);

	uchar*getSurface(int d);
	const uchar*getSurface(int d)const;

public:

	bool autoDel;

	math::vector3di Size;
	EPixelFormat format;
	ulong	imageDataSize;
	uchar* imageData;
};


}
}


#endif //___ImageInfo___
