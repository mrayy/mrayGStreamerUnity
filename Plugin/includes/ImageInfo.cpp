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
void ImageInfo::copyCroppedFrom(const ImageInfo* src,Vector2d pos,Vector2d sz,bool clamp,
                                EPixelFormat targetFormat)
{
    if(clamp)
    {
        if(pos.x<0)pos.x=0;
        if(pos.y<0)pos.y=0;
        if(sz.x+pos.x>src->Size.x)sz.x=src->Size.x-pos.x;
        if(sz.y+pos.y>src->Size.y)sz.y=src->Size.y-pos.y;
    }
    //else allow wrap data
    
    if(src->format==EPixel_I420)
    {
        createData(Vector2d(sz.x,sz.y), targetFormat);
        
        //copy pixels
        bool wrapX=false;
        bool wrapY=false;
        
        int targetHeight=src->Size.y*2/3;
        
        if(pos.x<0)
        {
            pos.x+=src->Size.x;
        }else if(pos.x>=src->Size.x)
        {
            pos.x-=src->Size.x;
        }
        if(pos.y<0)
        {
            pos.y+=targetHeight;
        }else if(pos.y>=targetHeight)
        {
            pos.y-=src->Size.y;
        }
        
        if(sz.x>src->Size.x)sz.x=src->Size.x;
        if(sz.y>targetHeight)sz.y=targetHeight;
        
        if(sz.x+pos.x>src->Size.x)wrapX=true;
        if(sz.y+pos.y>targetHeight)wrapY=true;
        
        
        int srcRowpitch=src->Size.x;
        int rowpitch=sz.x;
        
        const uchar* srcPtr0=src->imageData;
        uchar* dstPtr=imageData;
        
        int y0=pos.y;
        int srcOffset=y0*srcRowpitch;
        for(int i=0;i<sz.y;++i)
        {
            if( y0>=targetHeight)
            {
                y0=0;
            }
            srcOffset=y0*srcRowpitch;

            const uchar* srcPtr=srcPtr0+srcOffset;
            if(!wrapX)
            {
                //copy Y
                memcpy(dstPtr,srcPtr+pos.x,rowpitch);
                if(targetFormat==EPixel_I420)
                {
                    //look up U
                    
                }
            }else
            {
                //copy two parts
                //[******-------******]
                // ^****^       ^****^
                // >>p2>>       S>>p1>
                
                int S=pos.x;
                int p1=src->Size.x-S;
                int p2=sz.x-p1;
                memcpy(dstPtr,srcPtr+S,p1);
                memcpy(dstPtr+p1,srcPtr,p2);

            }
            
            y0++;
            dstPtr+=rowpitch;
            
        }
        
    }
}
    
void ImageInfo::FlipImage(bool horizontal,bool vertical)
{
    if(!horizontal && !vertical)
        return;
    
    int Rows=Size.y;
    int Cols=Size.x/2;
    if(vertical)
        Rows/=2;
    
    int rowPitch=(imageDataSize/Size.y)*sizeof(uchar);
    
    uchar* newData=new uchar[imageDataSize];
    
    
    uchar* srcPtr=imageData;
    uchar* dstPtr=newData;
    
    int y0=0;
    int y1=(Size.y-1)*rowPitch;
    
    for(int y=0;y<Rows;++y)
    {
        if(vertical)
        {
            //flip rows contents
            memcpy(dstPtr+y0,srcPtr+y1,rowPitch*sizeof(uchar));
            memcpy(dstPtr+y1,srcPtr+y0,rowPitch*sizeof(uchar));
        }
        if(horizontal)
        {
            uchar* ptr0=dstPtr+y0;
            uchar* ptr1=dstPtr+y0+rowPitch-1;
            for(int x=0;x<Cols;++x)
            {
                uchar t=*ptr0;
                *ptr0=*ptr1;
                *ptr1=t;
                ++ptr0;
                --ptr1;
            }
            if(Size.x%2==1)
            {
                *ptr0=*ptr1;
            }
        }
        
        y0+=rowPitch;
        y1-=rowPitch;
    }
    
    if(vertical && Size.y%2==1)
    {
        //copy the middle row
        memcpy(dstPtr+(Size.y/2+1)*rowPitch,srcPtr+(Size.y/2+1)*rowPitch,rowPitch);
    }
    
    delete [] imageData;
    imageData=newData;
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
