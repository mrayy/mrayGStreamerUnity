#include "stdafx.h"

#include "PixelUtil.h"



namespace mray{
namespace video{


PixelDescriptor PixelUtil::m_pixelDescription[]={
{
//name
mT("Unkown"),
//elemSizeB
0,
//flags
0,
//type
EPixelType_Byte,
//componentsCount
0,
//bits
0,0,0,0,
//mask
0,0,0,0,
//shift
0,0,0,0

},

{
//name
mT("LUMINANCE8"),
//elemSizeB
1,
//flags
EPixelFlags_Luminance | EPixelFlags_NativeEndian,
//type
EPixelType_Byte,
//componentsCount
1,
//bits
8,0,0,0,
//mask
0xFF,0,0,0,
//shift
0,0,0,0

},
{
//name
mT("LUMINANCE16"),
//elemSizeB
2,
//flags
EPixelFlags_Luminance| EPixelFlags_NativeEndian,
//type
EPixelType_Short,
//componentsCount
1,
//bits
16,0,0,0,
//mask
0xFFFF,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Alpha8"),
//elemSizeB
1,
//flags
EPixelFlags_HasAlpha| EPixelFlags_NativeEndian,
//type
EPixelType_Byte,
//componentsCount
1,
//bits
8,0,0,0,
//mask
0,0,0,0xFF,
//shift
0,0,0,0
},
{
//name
mT("Alpha4Luminance4"),
//elemSizeB
1,
//flags
EPixelFlags_HasAlpha | EPixelFlags_Luminance| EPixelFlags_NativeEndian,
//type
EPixelType_Byte,
//componentsCount
2,
//bits
4,0,0,4,
//mask
0x0F,0,0,0xF0,
//shift
0,0,0,4
},
{
//name
mT("Alpha8Luminance8"),
//elemSizeB
2,
//flags
EPixelFlags_HasAlpha | EPixelFlags_Luminance,
//type
EPixelType_Byte,
//componentsCount
2,
//bits
8,0,0,8,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("R5G6B5"),
//elemSizeB
3,
//flags
EPixelFlags_NativeEndian,
//type
EPixelType_Byte,
//componentsCount
3,
//bits
5,6,5,0,
//mask
0xF800,0x07E0,0x001F,0,
//shift
11,5,0,0
},
{
//name
mT("R8G8B8"),
//elemSizeB
3,
//flags
EPixelFlags_NativeEndian,
//type
EPixelType_Byte,
//componentsCount
3,
//bits
8,8,8,0,
//mask
0x0000FF,0x00FF00,0xFF0000,0,
//shift
16,8,0,0
},
{
//name
mT("A8R8G8B8"),
//elemSizeB
4,
//flags
EPixelFlags_HasAlpha| EPixelFlags_NativeEndian,
//type
EPixelType_Byte,
//componentsCount
4,
//bits
8,8,8,8,
//mask
0x000000FF,0x0000FF00,0x00FF0000,0xFF000000,
//shift
16,8,0,24
},
{
//name
mT("X8R8G8B8"),
//elemSizeB
4,
//flags
EPixelFlags_NativeEndian,
//type
EPixelType_Byte,
//componentsCount
3,
//bits
8,8,8,0,
//mask
0x000000FF,0x0000FF00,0x00FF0000,0xFF000000,
//shift
16,8,0,24
},
{
	//name
	mT("B8G8R8"),
	//elemSizeB
	3,
	//flags
	EPixelFlags_NativeEndian,
	//type
	EPixelType_Byte,
	//componentsCount
	3,
	//bits
	8,8,8,0,
	//mask
	0x0000FF,0x00FF00,0xFF0000,0,
	//shift
	16,8,0,0
},
{
	//name
	mT("A8B8G8R8"),
	//elemSizeB
	4,
	//flags
	EPixelFlags_HasAlpha| EPixelFlags_NativeEndian,
	//type
	EPixelType_Byte,
	//componentsCount
	4,
	//bits
	8,8,8,8,
	//mask
	0x000000FF,0x0000FF00,0x00FF0000,0xFF000000,
	//shift
	16,8,0,24
},
{
	//name
	mT("X8B8G8R8"),
	//elemSizeB
	4,
	//flags
	EPixelFlags_NativeEndian,
	//type
	EPixelType_Byte,
	//componentsCount
	3,
	//bits
	8,8,8,0,
	//mask
	0x000000FF,0x0000FF00,0x00FF0000,0xFF000000,
	//shift
	16,8,0,24
},
{
//name
mT("Float16_R"),
//elemSizeB
2,
//flags
EPixelFlags_Float,
//type
EPixelType_Float16,
//componentsCount
1,
//bits
16,0,0,0,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Float16_RGB"),
//elemSizeB
6,
//flags
EPixelFlags_Float,
//type
EPixelType_Float16,
//componentsCount
3,
//bits
16,16,16,0,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Float16_RGBA"),
//elemSizeB
8,
//flags
EPixelFlags_Float | EPixelFlags_HasAlpha,
//type
EPixelType_Float16,
//componentsCount
4,
//bits
16,16,16,16,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Float16_GR"),
//elemSizeB
4,
//flags
EPixelFlags_Float,
//type
EPixelType_Float16,
//componentsCount
2,
//bits
16,16,0,0,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Float32_R"),
//elemSizeB
4,
//flags
EPixelFlags_Float,
//type
EPixelType_Float32,
//componentsCount
1,
//bits
32,0,0,0,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Float32_RGB"),
//elemSizeB
12,
//flags
EPixelFlags_Float,
//type
EPixelType_Float32,
//componentsCount
3,
//bits
32,32,32,0,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Float32_RGBA"),
//elemSizeB
16,
//flags
EPixelFlags_Float | EPixelFlags_HasAlpha,
//type
EPixelType_Float32,
//componentsCount
4,
//bits
32,32,32,32,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Float32_GR"),
//elemSizeB
8,
//flags
EPixelFlags_Float,
//type
EPixelType_Float32,
//componentsCount
2,
//bits
32,32,0,0,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Depth"),
//elemSizeB
4,
//flags
EPixelFlags_Depth,
//type
EPixelType_Float32,
//componentsCount
1,
//bits
0,0,0,0,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Stecil"),
//elemSizeB
4,
//flags
EPixelFlags_Luminance,
//type
EPixelType_Byte,
//componentsCount
1,
//bits
0,0,0,0,
//mask
0,0,0,0,
//shift
0,0,0,0
},

{
//name
mT("Short_RGBA"),
//elemSizeB
8,
//flags
EPixelFlags_HasAlpha,
//type
EPixelType_Short,
//componentsCount
4,
//bits
16,16,16,16,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Short_RGB"),
//elemSizeB
6,
//flags
0 ,
//type
EPixelType_Short,
//componentsCount
2,
//bits
16,16,16,0,
//mask
0,0,0,0,
//shift
0,0,0,0
},
{
//name
mT("Short_GR"),
//elemSizeB
4,
//flags
0,
//type
EPixelType_Short,
//componentsCount
2,
//bits
16,16,0,0,
//mask
0x0000FFFF, 0xFFFF0000,0,0,
//shift
0,0,0,0
},

{
//name
mT("DXT1"),
//elemSizeB
0,
//flags
EPixelFlags_HasAlpha | EPixelFlags_Compressed,
//type
EPixelType_Byte,
//componentsCount
3,
//bits
0,0,0,0,
//mask
0, 0,0,0,
//shift
0,0,0,0
},

{
//name
mT("DXT2"),
//elemSizeB
0,
//flags
EPixelFlags_HasAlpha | EPixelFlags_Compressed,
//type
EPixelType_Byte,
//componentsCount
4,
//bits
0,0,0,0,
//mask
0, 0,0,0,
//shift
0,0,0,0
},

{
//name
mT("DXT3"),
//elemSizeB
0,
//flags
EPixelFlags_HasAlpha | EPixelFlags_Compressed,
//type
EPixelType_Byte,
//componentsCount
4,
//bits
0,0,0,0,
//mask
0, 0,0,0,
//shift
0,0,0,0
},

{
//name
mT("DXT4"),
//elemSizeB
0,
//flags
EPixelFlags_HasAlpha | EPixelFlags_Compressed,
//type
EPixelType_Byte,
//componentsCount
4,
//bits
0,0,0,0,
//mask
0, 0,0,0,
//shift
0,0,0,0
},

{
//name
mT("DXT5"),
//elemSizeB
0,
//flags
EPixelFlags_HasAlpha | EPixelFlags_Compressed,
//type
EPixelType_Byte,
//componentsCount
4,
//bits
0,0,0,0,
//mask
0, 0,0,0,
//shift
0,0,0,0
},
{
	//name
	mT("I420"),
	//elemSizeB
	1,
	//flags
	EPixelFlags_Compressed,
	//type
	EPixelType_Byte,
	//componentsCount
	1,
	//bits
	0, 0, 0, 0,
	//mask
	0, 0, 0, 0,
	//shift
	0, 0, 0, 0
},
{
	//name
	mT("Y42B"),
	//elemSizeB
	1,
	//flags
	EPixelFlags_Compressed,
	//type
	EPixelType_Byte,
	//componentsCount
	1,
	//bits
	0, 0, 0, 0,
	//mask
	0, 0, 0, 0,
	//shift
	0, 0, 0, 0
},
    {
        //name
        mT("NV12"),
        //elemSizeB
        1,
        //flags
        EPixelFlags_Compressed,
        //type
        EPixelType_Byte,
        //componentsCount
        1,
        //bits
        0, 0, 0, 0,
        //mask
        0, 0, 0, 0,
        //shift
        0, 0, 0, 0
    }
};

int PixelUtil::getMaxMipmaps(uint w,uint h,uint d){
	int cnt=0;
	do{
		w>>=1;
		h>>=1;
		d>>=1;
		++cnt;
	}while(w!=1 || h!=1 || d!=1);
	return cnt;
}

const PixelDescriptor& PixelUtil::getPixelDescription(EPixelFormat f){
	return m_pixelDescription[f];
}
EPixel_TYPE PixelUtil::getPixelType(EPixelFormat f){
	return getPixelDescription(f).type;
}

int PixelUtil::unpackColor(const uchar*src,EPixelFormat f,float &r,float &g,float&b,float&a){

	const PixelDescriptor& desc=getPixelDescription(f);

	if(desc.flags & EPixelFlags_NativeEndian ){
		if(desc.flags & EPixelFlags_Luminance){
		}
	}else {
		switch(f){

		case EPixel_Float16_R:
			r=g=b=((const ushort*)src)[0];
			a=1;
			break;
		case EPixel_Float16_RGB:
			r=((const ushort*)src)[0];
			g=((const ushort*)src)[1];
			b=((const ushort*)src)[2];
			a=1;
			break;
		case EPixel_Float16_RGBA:
			r=((const ushort*)src)[0];
			g=((const ushort*)src)[1];
			b=((const ushort*)src)[2];
			a=((const ushort*)src)[3];
			break;
		case EPixel_Float16_GR:
			g=((const ushort*)src)[0];
			r=b=((const ushort*)src)[1];
			a=1;
			break;

		case EPixel_Float32_R:
			r=g=b=((const float*)src)[0];
			a=1;
			break;
		case EPixel_Float32_RGB:
			r=((const float*)src)[0];
			g=((const float*)src)[1];
			b=((const float*)src)[2];
			a=1;
			break;
		case EPixel_Float32_RGBA:
			r=((const float*)src)[0];
			g=((const float*)src)[1];
			b=((const float*)src)[2];
			a=((const float*)src)[3];
			break;
		case EPixel_Float32_GR:
			g=((const float*)src)[0];
			r=b=((const float*)src)[1];
			a=1;
			break;
		}
	}
	return desc.elemSizeB;
}


}
}

