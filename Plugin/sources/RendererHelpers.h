#ifndef __RendererHelpers__
#define __RendererHelpers__

#include <stdio.h>
#include <string>
#include <sstream>
#include "mTypes.h"
namespace mray
{
	namespace video
	{
		class ImageInfo;
	}
}
extern void BlitImage(const mray::video::ImageInfo* ifo, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight);


#endif