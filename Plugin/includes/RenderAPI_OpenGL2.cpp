#include "RenderAPI.h"
#include "PlatformBase.h"

// OpenGL 2 (legacy, deprecated) implementation of RenderAPI.


#if SUPPORT_OPENGL_LEGACY

#include "GLEW/glew.h"


class RenderAPI_OpenGL2 : public RenderAPI
{
public:
	RenderAPI_OpenGL2();
	virtual ~RenderAPI_OpenGL2() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);

	virtual bool GetUsesReverseZ() { return false; }


	virtual void* BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch);
	virtual void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight,int comps, int rowPitch, void* dataPtr);

	virtual void* BeginModifyVertexBuffer(void* bufferHandle, size_t* outBufferSize);
	virtual void EndModifyVertexBuffer(void* bufferHandle);
};


RenderAPI* CreateRenderAPI_OpenGL2()
{
	return new RenderAPI_OpenGL2();
}


RenderAPI_OpenGL2::RenderAPI_OpenGL2()
{
}


void RenderAPI_OpenGL2::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	// not much to do :)
}


void* RenderAPI_OpenGL2::BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch)
{
	const int rowPitch = textureWidth * 4;
	// Just allocate a system memory buffer here for simplicity
    return 0;
}


void RenderAPI_OpenGL2::EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight,int comps, int rowPitch, void* dataPtr)
{
	GLuint gltex = (GLuint)(size_t)(textureHandle);
	// Update texture data, and free the memory buffer
    glBindTexture(GL_TEXTURE_2D, gltex);
    GLuint fmt=GL_RGBA;
    if(comps==1)
        fmt=GL_LUMINANCE8;
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, fmt, GL_UNSIGNED_BYTE, dataPtr);
}


void* RenderAPI_OpenGL2::BeginModifyVertexBuffer(void* bufferHandle, size_t* outBufferSize)
{
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)(size_t)bufferHandle);
	GLint size = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	*outBufferSize = size;
	void* mapped = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	return mapped;
}


void RenderAPI_OpenGL2::EndModifyVertexBuffer(void* bufferHandle)
{
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)(size_t)bufferHandle);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

#endif // #if SUPPORT_OPENGL_LEGACY
