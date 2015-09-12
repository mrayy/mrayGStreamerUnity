#pragma once

// Which platform we are on?
#if _MSC_VER
#define UNITY_WIN 1
#else
#define UNITY_OSX 1
#endif


// Attribute to make function be exported from a plugin
#if UNITY_WIN
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif


// Which graphics device APIs we possibly support?
#if UNITY_WIN
#define SUPPORT_D3D9 0
#define SUPPORT_D3D11 1 // comment this out if you don't have D3D11 header/library files
#define SUPPORT_OPENGL 1
#endif

#if UNITY_OSX
#define SUPPORT_OPENGL 1
#endif

enum GfxDeviceRenderer
{
	kGfxRendererOpenGL = 0,              // OpenGL
	kGfxRendererD3D9 = 1,                // Direct3D 9
	kGfxRendererD3D11 = 2,               // Direct3D 11
	kGfxRendererGCM = 3,                 // Sony PlayStation 3 GCM
	kGfxRendererNull = 4,                // "null" device (used in batch mode)
	kGfxRendererHollywood = 5,           // Nintendo Wii
	kGfxRendererXenon = 6,               // Xbox 360
	kGfxRendererOpenGLES = 7,            // OpenGL ES 1.1
	kGfxRendererOpenGLES20Mobile = 8,    // OpenGL ES 2.0 mobile variant
	kGfxRendererMolehill = 9,            // Flash 11 Stage3D
	kGfxRendererOpenGLES20Desktop = 10,  // OpenGL ES 2.0 desktop variant (i.e. NaCl)
};

enum GfxDeviceEventType
{
	kGfxDeviceEventInitialize = 0,
	kGfxDeviceEventShutdown = 1,
	kGfxDeviceEventBeforeReset = 2,	// i.e.: Before resolution change.
	kGfxDeviceEventAfterReset = 3,
};

extern void* g_GraphicsDevice;
extern int g_GraphicsDeviceType;

// Forward declarations.
struct IDirect3DDevice9;
struct ID3D11Device;

#if SUPPORT_D3D9
extern IDirect3DDevice9* g_D3D9GraphicsDevice;
#endif
#if SUPPORT_D3D11
extern ID3D11Device* g_D3D11GraphicsDevice;
#endif