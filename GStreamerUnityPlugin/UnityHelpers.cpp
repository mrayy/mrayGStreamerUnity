

#include "UnityHelpers.h"

#include "UnityPlugin.h"
#include <windows.h>
#include "GraphicsInclude.h"
#include "ImageInfo.h"

using namespace mray;
using namespace video;

FuncPtr Debug;
FuncFloatRetPtr GetEngineTimePtr;

void LogMessage(const std::string& msg, ELogLevel level)
{
	std::string m;
	if (level == ELL_INFO)
		m = "Info: ";
	if (level == ELL_WARNING)
		m = "Warning: ";
	if (level == ELL_SUCCESS)
		m = "Success: ";
	if (level == ELL_ERROR)
		m = "Error: ";

	m += msg;
#if UNITY_WIN
	OutputDebugStringA(m.c_str());
#else
	printf("%s", m.c_str());
#endif
// 	if (Debug)
// 		Debug(m.c_str());
}
float GetEngineTime()
{
	if (GetEngineTimePtr)
	{
		return GetEngineTimePtr();
	}
	return 0;
}
extern "C" EXPORT_API void mray_SetDebugFunction(FuncPtr f)
{
	Debug = f;
}
extern "C" EXPORT_API void mray_SetGetEngineTime(FuncFloatRetPtr f)
{
	GetEngineTimePtr = f;
}

LogManager* LogManager::s_instance=0;
LogManager* LogManager::Instance()
{
	if (!s_instance)
	{
		s_instance = new LogManager();
	}
	return s_instance;
}

LogManager::LogManager()
{
	m_logFile = fopen("GStreamerLog.txt", "w");
}
LogManager::~LogManager()
{
	fclose(m_logFile);
}
void LogManager::LogMessage(const std::string& msg)
{
	fprintf(m_logFile, "%s\n", msg.c_str());
}



void CopyToTexture(const ImageInfo* src, uchar* dst,video::EPixelFormat fmt)
{
	if (fmt == video::EPixel_I420)
	{
		memcpy(dst, src->imageData, src->imageDataSize);
		return;
	}
	int len = src->Size.x*src->Size.y * 3;
	uchar* ptr = dst;
	uchar* srcPtr = src->imageData;
	for (int i = 0; i < len; i += 3)
	{
		ptr[0] = srcPtr[0];
		ptr[1] = srcPtr[1];
		ptr[2] = srcPtr[2];
		ptr[3] = 1;
		ptr += 4;
		srcPtr += 3;
	}
}

void BlitImage(const ImageInfo* ifo, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight)
{

	if (!ifo || !_TextureNativePtr)
		return;

	if (ifo->tmpBuffer == 0)
		((video::ImageInfo*)ifo)->tmpBuffer = new ImageInfo();


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
			CopyToTexture(ifo, (uchar*)lr.pBits,ifo->format);

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
			
			uchar* data = 0;
			int pitch = 0;
			if (ifo->format == video::EPixel_I420)
			{
				//data = new uchar[ifo->imageDataSize];
				pitch = _UnityTextureWidth;

				data = ifo->imageData;
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
				pitch = _UnityTextureWidth * 4;
				data = ifo->tmpBuffer->imageData;
				CopyToTexture(ifo, (uchar*)data, ifo->format);
			}

			ctx->UpdateSubresource(d3dtex, 0, nullptr, data, pitch, 0);
			//delete[] data;

		}

		ctx->Release();
	}
#endif


#if SUPPORT_OPENGL
	// OpenGL case
	if (g_GraphicsDeviceType == kGfxRendererOpenGL)
	{
		// update native texture from code
		if (_TextureNativePtr)
		{
			GLuint gltex = (GLuint)(size_t)(_TextureNativePtr);
			glBindTexture(GL_TEXTURE_2D, gltex);
			int texWidth, texHeight;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, ifo->imageData);

		}
	}
#endif
}