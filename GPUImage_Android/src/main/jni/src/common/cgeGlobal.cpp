/*
* cgeGlobal.cpp
*
*  Created on: 2014-9-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeGlobal.h"

#ifndef _CGE_ONLY_FILTERS_

#include "cgeSprite2d.h"
#include "cgeAction.h"

#endif

namespace CGE
{
	bool g_isFastFilterImpossible = true;

#ifdef _CGE_USE_ES_API_3_0_
	bool g_shouldUsePBO = true;
#endif

	int CGEGlobalConfig::viewWidth = 1024;
	int CGEGlobalConfig::viewHeight = 768;
	GLuint CGEGlobalConfig::sVertexBufferCommon = 0;
	float CGEGlobalConfig::sVertexDataCommon[8] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};

	void cgeInitFilterStatus()
	{
		GLint iParam;

#ifdef GL_MAX_FRAGMENT_UNIFORM_VECTORS //GL ES
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &iParam);
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_COMPONENTS //GL Desktop
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &iParam);
#endif

		g_isFastFilterImpossible = (iParam < 300);

		CGE_LOG_INFO("Use Fast Filter: %d\n", !g_isFastFilterImpossible);

		cgeCheckGLError("cgeInitFilterStatus - before 'GL_PIXEL_PACK_BUFFER'");

#if defined(_CGE_USE_ES_API_3_0_) && defined(GL_PIXEL_PACK_BUFFER)

		//使用此段代码此时 GL_PIXEL_PACK_BUFFER 是否被支持
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		if(glGetError() == GL_FALSE)
		{
			g_shouldUsePBO = true;
		}
		else
		{
			g_shouldUsePBO = false;
			CGE_LOG_ERROR("GL_PIXEL_PACK_BUFFER is not supported!\n");
		}
#endif
	}

	bool cgeInitialize(int width, int height, CGEGlobalConfig::InitArguments arg)
	{
		cgeInitFilterStatus();

		CGEGlobalConfig::viewWidth = width;
		CGEGlobalConfig::viewHeight = height;

		if(arg == CGEGlobalConfig::CGE_INIT_LEAST)
		{
			CGE_LOG_INFO("You chosed CGE_INIT_LEAST for initialize, so the sprites could not be used! Once you used, unexpected problem would be happen\n");
			return true;
		}

		if(arg & CGEGlobalConfig::CGE_INIT_COMMONVERTEXBUFFER)
		{
			glGenBuffers(1, &CGEGlobalConfig::sVertexBufferCommon);
			if(CGEGlobalConfig::sVertexBufferCommon == 0)
				return false;

			glBindBuffer(GL_ARRAY_BUFFER, CGEGlobalConfig::sVertexBufferCommon);
			glBufferData(GL_ARRAY_BUFFER, sizeof(CGEGlobalConfig::sVertexDataCommon), CGEGlobalConfig::sVertexDataCommon, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

#ifndef _CGE_ONLY_FILTERS_

		if(width > 0 && height > 0)
		{
			SpriteCommonSettings::sSetCanvasSize(width, height);
		}

		if(arg & CGEGlobalConfig::CGE_INIT_SPRITEBUILTIN)
			cgeSpritesInitBuiltin();

#endif

		cgeCheckGLError("cgeInitialize");

		return true;
	}

	void cgeCleanup()
	{
		using namespace CGE;
		glDeleteBuffers(1, &CGEGlobalConfig::sVertexBufferCommon);
		CGEGlobalConfig::sVertexBufferCommon = 0;

#ifndef _CGE_ONLY_FILTERS_

		cgeSpritesCleanupBuiltin();

		CGE_LOG_CODE(
			if(!CGE::SpriteCommonSettings::s_spriteManager.empty())
			{
				CGE_LOG_ERROR("Waring! %d sprites are not deleted!\n", (int)CGE::SpriteCommonSettings::s_spriteManager.size());
			}
		);

		CGE_LOG_CODE(
			if(!CGE::TimeActionInterfaceAbstract::s_actionManager.empty())
			{
				CGE_LOG_ERROR("Waring! %d sprites are not deleted!\n", (int)CGE::TimeActionInterfaceAbstract::s_actionManager.size());
			}
			);
#endif
	}

	void cgeSetGlobalViewSize(int width, int height)
	{
		CGEGlobalConfig::viewWidth = width;
		CGEGlobalConfig::viewHeight = height;

#ifndef _CGE_ONLY_FILTERS_

		CGE::SpriteCommonSettings::sSetCanvasSize(width, height);

#endif
	}

	void cgePrintGLInfo()
	{
		CGE_LOG_INFO("===== Here are some information of your device =====\n\n");

		CGE::cgePrintGLString("Vendor", GL_VENDOR);
		CGE::cgePrintGLString("Renderer", GL_RENDERER);
		CGE::cgePrintGLString("GL Version", GL_VERSION);
		CGE::cgePrintGLString("Shading Language Version", GL_SHADING_LANGUAGE_VERSION);

		GLint iParam[2];

#ifdef GL_MAX_VERTEX_UNIFORM_VECTORS //GL ES
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, iParam);
#endif
#ifdef GL_MAX_VERTEX_UNIFORM_COMPONENTS //GL Desktop
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, iParam);
#endif
		CGE_LOG_INFO("Max Vertex Uniform Vectors: %d\n", iParam[0]);

#ifdef GL_MAX_FRAGMENT_UNIFORM_VECTORS
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, iParam);
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, iParam);
#endif
		CGE_LOG_INFO("Max Fragment Uniform Vectors : %d\n", iParam[0]);
		
#ifdef GL_MAX_VARYING_VECTORS
		glGetIntegerv(GL_MAX_VARYING_VECTORS, iParam);
#endif
#ifdef GL_MAX_VARYING_COMPONENTS
		glGetIntegerv(GL_MAX_VARYING_COMPONENTS, iParam);
#endif
		CGE_LOG_INFO("Max Varying Vectors: %d\n", iParam[0]);

		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, iParam);
		CGE_LOG_INFO("Max Texture Image Units : %d\n", iParam[0]);

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, iParam);
		CGE_LOG_INFO("Max Combined Texture Image Units : %d\n", iParam[0]);

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, iParam);
		CGE_LOG_INFO("Max Texture Size : %d\n", iParam[0]);		

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, iParam);
		CGE_LOG_INFO("Max Vertex Attribs: %d\n", iParam[0]);

		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, iParam);
		CGE_LOG_INFO("Max Viewport Dims : [%d, %d]\n", iParam[0], iParam[1]);

		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, iParam);
		CGE_LOG_INFO("Max Render Buffer Size: %d\n", iParam[0]);

		glGetIntegerv(GL_STENCIL_BITS, iParam);
		CGE_LOG_INFO("Stencil Buffer Bits : %d\n", iParam[0]);

		glGetIntegerv(GL_ALIASED_POINT_SIZE_RANGE, iParam);
		CGE_LOG_INFO("Point Size Range: [%d, %d]\n", iParam[0], iParam[1]);

		glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, iParam);
		CGE_LOG_INFO("Line Width Range: [%d, %d]\n", iParam[0], iParam[1]);

		glGetIntegerv(GL_DEPTH_BITS, iParam);
		CGE_LOG_INFO("Depth Bits: %d\n", iParam[0]);

// 		glGetIntegerv(GL_SHADER_COMPILER, iParam);
// 		if(!iParam[0])
// 		{
// 			CGE_LOG_ERROR("GL Shader compiler is disabled, the application may not work!\n");
// 		}

		//Shader precision
// 		GLint precision;
// 		const GLenum shaderTypes[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
// 		const char* shaderTypeName[] = { "Vertex Shader", "Fragment Shader" };
// 
// 		const GLenum precisionTypes[] = { GL_LOW_FLOAT, GL_MEDIUM_FLOAT, GL_HIGH_FLOAT, GL_LOW_INT, GL_MEDIUM_INT, GL_HIGH_INT };
// 		const char* precisionTypeName[] = { "low float", "medium float", "high float", "low int", "medium int", "high int" };
// 
// 		for(int i = 0; i != sizeof(shaderTypes) / sizeof(*shaderTypes); ++i)
// 		{
// 			for(int j = 0; j != sizeof(precisionTypes) / sizeof(*precisionTypes); ++j)
// 			{
// 				glGetShaderPrecisionFormat(shaderTypes[i], precisionTypes[j], iParam, &precision);
// 				CGE_LOG_INFO("%s -- %s: [%d, %d], %d\n", shaderTypeName[i], precisionTypeName[j], iParam[0], iParam[1], precision);
// 				cgeCheckGLError("glGetShaderPrecisionFormat");
// 			}
// 		}

		cgeCheckGLError("cgePrintGLInfo");
		CGE_LOG_INFO("\n===== Information end =====\n\n");
	}

}