
#include "RenderAPI.h"
#include "PlatformBase.h"


// Metal implementation of RenderAPI.


#if SUPPORT_METAL

#include "Unity/IUnityGraphicsMetal.h"
#import <Metal/Metal.h>


class RenderAPI_Metal : public RenderAPI
{
public:
	RenderAPI_Metal();
	virtual ~RenderAPI_Metal() { }
	
	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);
	
	virtual bool GetUsesReverseZ() { return true; }
	virtual void* BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch);
	virtual void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight,int comps, int rowPitch, void* dataPtr);

	virtual void* BeginModifyVertexBuffer(void* bufferHandle, size_t* outBufferSize);
	virtual void EndModifyVertexBuffer(void* bufferHandle);
	
private:
	void CreateResources();
	
private:
	IUnityGraphicsMetal*	m_MetalGraphics;
	id<MTLBuffer>			m_VertexBuffer;
	id<MTLBuffer>			m_ConstantBuffer;

	id<MTLDepthStencilState> m_DepthStencil;
	id<MTLRenderPipelineState>	m_Pipeline;
};


RenderAPI* CreateRenderAPI_Metal()
{
	return new RenderAPI_Metal();
}


void RenderAPI_Metal::CreateResources()
{
	
}


RenderAPI_Metal::RenderAPI_Metal()
{
}


void RenderAPI_Metal::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	if (type == kUnityGfxDeviceEventInitialize)
	{
		m_MetalGraphics = interfaces->Get<IUnityGraphicsMetal>();
		NSBundle* metalBundle = m_MetalGraphics->MetalBundle();
		CreateResources();
	}
	else if (type == kUnityGfxDeviceEventShutdown)
	{
		//@TODO: release resources
	}
}


void* RenderAPI_Metal::BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch)
{
	const int rowPitch = textureWidth * 4;
	// Just allocate a system memory buffer here for simplicity
	unsigned char* data = new unsigned char[rowPitch * textureHeight];
	*outRowPitch = rowPitch;
	return data;
}


void RenderAPI_Metal::EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight,int comps, int rowPitch, void* dataPtr)
{
	id<MTLTexture> tex = (__bridge id<MTLTexture>)textureHandle;
	// Update texture data, and free the memory buffer
	[tex replaceRegion:MTLRegionMake3D(0,0,0, textureWidth,textureHeight,1) mipmapLevel:0 withBytes:dataPtr bytesPerRow:rowPitch];
	delete[](unsigned char*)dataPtr;
}


void* RenderAPI_Metal::BeginModifyVertexBuffer(void* bufferHandle, size_t* outBufferSize)
{
	id<MTLBuffer> buf = (__bridge id<MTLBuffer>)bufferHandle;
	*outBufferSize = [buf length];
	return [buf contents];
}


void RenderAPI_Metal::EndModifyVertexBuffer(void* bufferHandle)
{
#	if UNITY_OSX
	id<MTLBuffer> buf = (__bridge id<MTLBuffer>)bufferHandle;
	[m_VertexBuffer didModifyRange:NSMakeRange(0, buf.length)];
#	endif // if UNITY_OSX
}


#endif // #if SUPPORT_METAL
