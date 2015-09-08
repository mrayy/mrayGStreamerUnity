
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

//#include "CompileConfig.h"
#include "mTypes.h"
#include "videoCommon.h"
#include "UnityHelpers.h"
//#include <Point3d.h>

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
	void setData(const void*data, const Vector2d&size, EPixelFormat format);
	void createData(const Vector2d& size,EPixelFormat format);

	uchar*getSurface(int d);
	const uchar*getSurface(int d)const;

public:

	bool autoDel;

	Vector2d Size;
	EPixelFormat format;
	ulong	imageDataSize;
	uchar* imageData;

	ImageInfo* tmpBuffer;
};


}
}


#endif //___ImageInfo___
