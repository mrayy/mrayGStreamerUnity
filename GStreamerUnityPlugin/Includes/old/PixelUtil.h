
/********************************************************************
	created:	2009/05/22
	created:	22:5:2009   1:13
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\PixelUtil.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	PixelUtil
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___PixelUtil___
#define ___PixelUtil___

#include <string>
#include "mtypes.h"
#include "videoCommon.h"

namespace mray{
namespace video{

	struct PixelDescriptor
	{
		std::string name;
		uchar elemSizeB;	//element Size in Bytes

		uint flags;//see EPixelFlags
		EPixel_TYPE type;
		uchar componentsCount;
		uchar RBits,GBits,BBits,ABits;
		uint RMask,GMask,BMask,AMask;
		uchar RShift,GShift,BShift,AShift;

	};

class  PixelUtil
{
private:
protected:
	static PixelDescriptor m_pixelDescription[EPixelFormat_Count];
public:
	static EPixel_TYPE getPixelType(EPixelFormat f);

	static int getMaxMipmaps(uint w,uint h,uint d);

	static const PixelDescriptor& getPixelDescription(EPixelFormat f);

	//get a color from src data based on pixel format and return number of bytes 
	//that reprsents the color size
	static int unpackColor(const uchar*src,EPixelFormat f,float &r,float &g,float&b,float&a);

};

}
}


#endif //___PixelUtil___
