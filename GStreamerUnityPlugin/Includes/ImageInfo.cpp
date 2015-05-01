#include "stdafx.h"

#include "ImageInfo.h"
#include <memory>

#include "PixelUtil.h"



namespace mray{
namespace video{

ImageInfo::ImageInfo():format(EPixel_LUMINANCE8),
		imageData(0),imageDataSize(0)
{
	autoDel=true;
}
ImageInfo::~ImageInfo(){
	if(imageData && autoDel)
		delete [] imageData;
}

int ImageInfo::getPitch()const{
	return Size.x*PixelUtil::getPixelDescription(format).elemSizeB;
}

void ImageInfo::clear(){
	delete [] imageData;
	imageDataSize=0;
	imageData=0;
	Size = 0;
}

void ImageInfo::setData(const void*data,const math::vector3di&size,EPixelFormat format){
	
	if(Size!=size || format != this->format)
	{
		clear();
		Size=size;
		this->format=format;
		imageDataSize=Size.x*Size.y*Size.z*PixelUtil::getPixelDescription(format).elemSizeB;
		if(imageDataSize>0)
			imageData=new uchar[imageDataSize];
		else
			return;

	}
	memcpy(imageData,data,imageDataSize);
}

void ImageInfo::createData(const math::vector3di& size,EPixelFormat format){
	if(Size==size && format == this->format)
		return;
	clear();
	Size=size;
	this->format=format;

	imageDataSize=Size.x*Size.y*Size.z*PixelUtil::getPixelDescription(format).elemSizeB;
	if(imageDataSize>0){
		imageData=new uchar[imageDataSize];
		//mraySystem::memSet(imageData,0,imageDataSize);
	}
}

uchar*ImageInfo::getSurface(int d){
	if(d>=Size.z)
		return 0;
	return imageData+d*(Size.x*Size.y*PixelUtil::getPixelDescription(format).elemSizeB);
}

const uchar*ImageInfo::getSurface(int d)const
{
	if(d>=Size.z)
		return 0;
	return imageData+d*(Size.x*Size.y*PixelUtil::getPixelDescription(format).elemSizeB);
}

}
}
