#include "RendererHelpers.h"

#include "GraphicsInclude.h"
#include "PlatformBase.h"
#include "PixelUtil.h"
#include "ImageInfo.h"
#include "UnityGraphicsDevice.h"

#include <memory.h>

using namespace mray;
using namespace video;


void CopyToTexture(const ImageInfo* src, uchar* dst, video::EPixelFormat fmt)
{
	if (fmt == video::EPixel_I420 || fmt == video::EPixelFormat::EPixel_LUMINANCE8
		|| fmt == video::EPixelFormat::EPixel_R8G8B8 || fmt == video::EPixelFormat::EPixel_B8G8R8
		||
		((fmt == video::EPixel_LUMINANCE8 || fmt == video::EPixel_Alpha8) &&
		(src->format == video::EPixel_LUMINANCE8 || src->format == video::EPixel_Alpha8 || src->format == video::EPixel_LUMINANCE16)))
	{
		memcpy(dst, src->imageData, src->imageDataSize);
		return;
	}
	int len = src->Size.x*src->Size.y * 3;
	uchar* ptr = dst;
	uchar* srcPtr = src->imageData;
	for (int i = 0; i < len; i += 3)
	{
		if (src->format == video::EPixelFormat::EPixel_R8G8B8 ||
			src->format == video::EPixelFormat::EPixel_B8G8R8)
		{
			ptr[0] = srcPtr[0];
			ptr[1] = srcPtr[1];
			ptr[2] = srcPtr[2];
			srcPtr += 3;
		}
		else if (src->format == video::EPixelFormat::EPixel_Alpha8 ||
			src->format == video::EPixelFormat::EPixel_LUMINANCE8)
		{
			ptr[0] = srcPtr[0];
			ptr[1] = srcPtr[0];
			ptr[2] = srcPtr[0];
			srcPtr++;
		}
		ptr[3] = 255;
		ptr += 4;
	}
}

void CheckData(const ImageInfo* ifo, int _UnityTextureWidth, int _UnityTextureHeight, uchar** data, int* pitch, int* comps)
{

	if (ifo->format == video::EPixel_I420 ||
		ifo->format == video::EPixel_NV12 ||
		ifo->format == video::EPixel_Y42B ||
		(ifo->format == video::EPixel_LUMINANCE8 || ifo->format == video::EPixel_Alpha8))
	{
		*comps = 1;
		//data = new uchar[ifo->imageDataSize];
		*pitch = _UnityTextureWidth;

		*data = ifo->imageData;
	}
	else if (ifo->format == video::EPixel_LUMINANCE16)
	{
		*comps = 1;
		*pitch = _UnityTextureWidth;
		*data = ifo->imageData;
	}
	else
	{
		//				data = new uchar[_UnityTextureWidth*_UnityTextureHeight * 4];
		// 				pitch = _UnityTextureWidth * 3;
		// 				data = ifo->imageData;

		if (ifo->tmpBuffer->Size != ifo->Size)
		{
			ifo->tmpBuffer->createData(ifo->Size, video::EPixel_R8G8B8A8);
		}
		*pitch = _UnityTextureWidth * 4;
		*data = ifo->tmpBuffer->imageData;
		*comps = 4;
		CopyToTexture(ifo, (uchar*)*data, video::EPixel_R8G8B8A8);//ifo->format);
	}
}

void BlitImage(const ImageInfo* ifo, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight)
{
	if (!ifo || !_TextureNativePtr)
		return;

	if (ifo->tmpBuffer == 0)
		((video::ImageInfo*)ifo)->tmpBuffer = new ImageInfo();


	uchar* data = 0;
	int pitch = 0;
	int comps = 1;

#if 0
#if SUPPORT_D3D9
	// D3D9 case
	if (g_GraphicsDeviceType == kGfxRendererD3D9)
	{
		// Update native texture from code
		if (_TextureNativePtr)
		{
			IDirect3DTexture9* d3dtex = (IDirect3DTexture9*)_TextureNativePtr;
			D3DSURFACE_DESC desc;
			d3dtex->GetLevelDesc(0, &desc);
			D3DLOCKED_RECT lr;
			d3dtex->LockRect(0, &lr, nullptr, 0);

			//uchar* data = new uchar[desc.Width*desc.Height * 4];

			//memcpy((unsigned char*)lr.pBits, ifo->imageData, desc.Width*desc.Height * 3);
			CopyToTexture(ifo, (uchar*)lr.pBits, ifo->format);

			d3dtex->UnlockRect(0);
			//delete [] data;
		}
	}
#endif

#if SUPPORT_D3D11
	// D3D11 case
	if (g_GraphicsDeviceType == kGfxRendererD3D11)
	{
		ID3D11DeviceContext* ctx = nullptr;
		g_D3D11GraphicsDevice->GetImmediateContext(&ctx);

		// update native texture from code
		if (_TextureNativePtr)
		{
			ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)_TextureNativePtr;
			D3D11_TEXTURE2D_DESC desc;
			d3dtex->GetDesc(&desc);
			//ctx->UpdateSubresource(d3dtex, 0, nullptr, ifo->imageData, desc.Width * 3, 0);

			CheckData(ifo, _UnityTextureWidth, _UnityTextureWidth, &data, &pitch);
			ctx->UpdateSubresource(d3dtex, 0, nullptr, data, pitch, 0);
			//delete[] data;

		}

		ctx->Release();
	}
#endif
#endif

	if (_TextureNativePtr)
	{
		CheckData(ifo, _UnityTextureWidth, _UnityTextureHeight, &data, &pitch, &comps);

		GetRenderer()->BeginModifyTexture(_TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight, &pitch);
		GetRenderer()->EndModifyTexture(_TextureNativePtr, _UnityTextureWidth, _UnityTextureHeight, comps, pitch, data);

	}

}