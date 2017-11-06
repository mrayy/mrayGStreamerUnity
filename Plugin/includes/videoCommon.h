/********************************************************************
	created:	2009/02/09
	created:	9:2:2009   18:41
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\videoCommon.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	videoCommon
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___videoCommon___
#define ___videoCommon___

#include "common.h"

namespace mray{
namespace video{

	enum EMeshRenderType
	{
		MR_POINTS,
		MR_LINES,
		MR_LINE_LOOP,
		MR_LINE_STRIP,
		MR_TRIANGLES,
		MR_TRIANGLE_STRIP,
		MR_TRIANGLE_FAN,
		MR_QUADS,
		MR_QUAD_STRIP,
		MR_POLYGON
	};

	/*!
	types of matrices used in engine
	*/
	enum ETransformationState
	{
		//! View transformation
		TS_VIEW = 0,
		//! World transformation
		TS_WORLD,
		//! Projection transformation
		TS_PROJECTION,
		//Textures
		TS_TEXTURE0,
		TS_TEXTURE1,
		TS_TEXTURE2,
		TS_TEXTURE3,
		//! Count of the matrices
		TS_COUNT
	};
	enum ERenderMode
	{
		RM_NONE,
		RM_2D=1,
		RM_3D
	};

	/*!
	types of buffers in the engine
	*/
	enum EDeviceBuffer{
		//! Depth buffer
		EDB_DEPTH,
		//! Color buffer
		EDB_COLOR,
		//! Accum buffer
		EDB_ACCUM,
		//! Stencil buffer
		EDB_STENCIL
	};

	enum EPassType
	{
		EPT_Normal,
		EPT_Shadow
	};


	enum ETextureType{
		ETT_1DTex,
		ETT_2DTex,
		ETT_3DTex,
		ETT_CubeTex
	};

	enum EPixelFormat
	{
		EPixel_Unkown,

		EPixel_LUMINANCE8,
		EPixel_LUMINANCE16,

		EPixel_Alpha8,
		EPixel_Alpha4Luminance4,
		EPixel_Alpha8Luminance8,

		EPixel_R5G6B5,
		EPixel_R8G8B8,
		EPixel_R8G8B8A8,
		EPixel_X8R8G8B8,

		EPixel_B8G8R8,
		EPixel_B8G8R8A8,
		EPixel_X8B8G8R8,

		EPixel_Float16_R,
		EPixel_Float16_RGB,
		EPixel_Float16_RGBA,
		EPixel_Float16_GR,

		EPixel_Float32_R,
		EPixel_Float32_RGB,
		EPixel_Float32_RGBA,
		EPixel_Float32_GR,

		EPixel_Depth,
		EPixel_Stecil,


		EPixel_Short_RGBA,
		EPixel_Short_RGB,
		EPixel_Short_GR,

		EPixel_DXT1,
		EPixel_DXT2,
		EPixel_DXT3,
		EPixel_DXT4,
		EPixel_DXT5,

		EPixel_I420,
		EPixel_Y42B,
        EPixel_NV12,

		EPixelFormat_Count
	};

	enum EPixelFlags
	{
		EPixelFlags_HasAlpha=BIT(0),
		EPixelFlags_Float=BIT(1),
		EPixelFlags_Depth=BIT(2),
		EPixelFlags_Luminance=BIT(3),
		EPixelFlags_Compressed=BIT(4),
		EPixelFlags_NativeEndian=BIT(5)
	};

	enum EPixel_TYPE
	{
		EPixelType_Byte,
		EPixelType_Short,
		EPixelType_Float16,
		EPixelType_Float32,
		EPixelType_Count
	};

	enum ETextureClamp{
		ETC_REPEAT,
		ETC_CLAMP,
		ETC_CLAMP_TO_EDGE,
		ETC_CLAMP_TO_BORDER,
		ETC_MIRROR,
		ETC_COUNT
	};

	enum ETextureWrap{
		ETW_WrapS,
		ETW_WrapT,
		ETW_WrapR,
		ETW_Count
	};

	enum ETextureGenType{
		ETGT_GenS,
		ETGT_GenT,
		ETGT_GenR,
		ETGT_GenQ,

		ETGT_Count
	};
	enum ETextureGenMode{
		ETGM_EyeLinear,
		ETGM_ObjectLinear,
		ETGM_SphereMap,

		ETGM_Count
	};

	enum EMeshStreamType
	{
		EMST_Position,
		EMST_BlendWeights,
		EMST_BlendIndicies,
		EMST_Normal,
		EMST_Tangent,
		EMST_Binormal,
		EMST_Color,
		EMST_Specular,
		EMST_Texcoord,
		EMST_StreamsCount
	};
	enum EStreamDataType
	{
		//float
		ESDT_Point1f,
		ESDT_Point2f,
		ESDT_Point3f,
		ESDT_Point4f,

		//short
		ESDT_Point1s,
		ESDT_Point2s,
		ESDT_Point3s,
		ESDT_Point4s,

		//byte
		ESDT_Color4b
	};

	enum ECubeFace{
		ECF_X_Positive,
		ECF_X_Negative,
		ECF_Y_Positive,
		ECF_Y_Negative,
		ECF_Z_Positive,
		ECF_Z_Negative,
		ECF_Count
	};

	enum EMaterialRenderType
	{
		MRT_SOLID,
		MRT_TRANSPARENT,
		MRT_MUL,
		MRT_ADD,
		MRT_ADDALPHA
		/*
		,MRT_ONE_TEXTURE_BLEND,
		MRT_SOLID_2LAYERS,
		MRT_VERTEX_ALPHA,
		MRT_ALPHA_CHANNEL,
		MRT_ALPHA_CHANNEL_REF,
		MRT_LIGHT_MAP,
		MRT_DETAIL_MAP,
		MRT_SPHERE_MAP,
		MRT_REFLECTION_2LAYERS,
		MRT_TRANS_REFLECTION_2LAYERS,
		MRT_LASER*/

	};

	enum RenderStates
	{
		RS_Wireframe=0,
		RS_Smooth,
		RS_Lighting,
		RS_ZTest,
		RS_ZWrite,
		RS_Points,
		RS_CullFace,
		RS_AlphaTest,
		RS_Fog,
		RS_Blend,
		RS_RenderStates_Count// just to know flags count
	};

	enum EState{
		ES_DontUse=0,
		ES_Use=1,
		//let device use it own
		ES_Unknown
	};

	enum ECullFaceMode{
		ECFM_CW,
		ECFM_CCW
	};


	enum EBlendFactor{
		EBF_ZERO=0,
		EBF_ONE,
		EBF_DST_COLOR,
		EBF_ONE_MINUS_DST_COLOR,
		EBF_SRC_COLOR,
		EBF_ONE_MINUS_SRC_COLOR,
		EBF_SRC_ALPHA,
		EBF_ONE_MINUS_SRC_ALPHA,
		EBF_DST_ALPHA,
		EBF_ONE_MINUS_DST_ALPHA,
		EBF_SRC_ALPHA_SATURATE
	};
	enum EAlphaFunc{
		EAF_Never,
		EAF_Less,
		EAF_Equal,
		EAF_LessEqual,
		EAF_Greater,
		EAF_NotEqual,
		EAF_GreaterEqual,
		EAF_Always
	};


	enum EMODULATE_FUNC{
		EMF_1X=1,
		EMF_2X=2,
		EMF_4X=4,
	};
}
}


#endif //___videoCommon___

