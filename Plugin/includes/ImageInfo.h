
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

	void _copyCroppedFromI420(const ImageInfo* i, Vector2d pos, Vector2d size, bool clamp, EPixelFormat targetFormat);


public:
	ImageInfo();
	virtual~ImageInfo();

	int getPitch()const;

	void clear();
	void setData(const void*data, const Vector2d&size, EPixelFormat format);
	void createData(const Vector2d& size,EPixelFormat format);
    void copyFrom(const ImageInfo* i);
    void copyCroppedFrom(const ImageInfo* i,Vector2d pos,Vector2d size,bool clamp,EPixelFormat targetFormat);
    
    void FlipImage(bool horizontal,bool vertical);

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
