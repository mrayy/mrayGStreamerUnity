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
	tmpBuffer = 0;
}
ImageInfo::~ImageInfo(){
	if(imageData && autoDel)
		delete [] imageData;

	if (tmpBuffer)
		delete tmpBuffer;
}

int ImageInfo::getPitch()const{
	return Size.x*PixelUtil::getPixelDescription(format).elemSizeB;
}

void ImageInfo::clear(){
	delete [] imageData;
	imageDataSize=0;
	imageData=0;
	Size = Vector2d(0,0);
}

void ImageInfo::setData(const void*data, const Vector2d&size, EPixelFormat format){
	
	if(Size!=size || format != this->format)
	{
		clear();
		Size=size;
		this->format=format;
		imageDataSize=Size.x*Size.y*PixelUtil::getPixelDescription(format).elemSizeB;
		if(imageDataSize>0)
			imageData=new uchar[imageDataSize];
		else
			return;

	}
	memcpy(imageData,data,imageDataSize);
}
    
void ImageInfo::copyFrom(const ImageInfo* i)
{
    setData(i->imageData,i->Size,i->format);
}

void ImageInfo::createData(const Vector2d& size, EPixelFormat format){
	if(Size==size && format == this->format)
		return;
	clear();
	Size=size;
	this->format=format;

	imageDataSize=Size.x*Size.y*PixelUtil::getPixelDescription(format).elemSizeB;
	if(imageDataSize>0){
		imageData=new uchar[imageDataSize];
		//mraySystem::memSet(imageData,0,imageDataSize);
	}
}

uchar*ImageInfo::getSurface(int d){
	return imageData+(Size.x*Size.y*PixelUtil::getPixelDescription(format).elemSizeB);
}

const uchar*ImageInfo::getSurface(int d)const
{
	return imageData+(Size.x*Size.y*PixelUtil::getPixelDescription(format).elemSizeB);
}

}
}
