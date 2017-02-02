#pragma once

#if SUPPORT_D3D9
	#include <d3d9.h>
#endif
#if SUPPORT_D3D11
	#include <d3d11.h>
#endif
#if SUPPORT_OPENGL
	#if UNITY_WIN
		#include <gl/GL.h>
	#else
		#include <OpenGL/OpenGL.h>
	#endif
#endif