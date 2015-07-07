/*
 * cgeMultipleEffects.cpp
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
*/

#include "cgeMultipleEffects.h"
#include "cgeDataParsingEngine.h"
#include <cmath>
#include <cctype>

static CGEConstString s_fshHTFilterMix = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D originImageTexture;
uniform float intensity;

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec4 origin = texture2D(originImageTexture, textureCoordinate);
	gl_FragColor = mix(origin, src, intensity);
}
);

namespace CGE
{

	CGEConstString CGEMutipleMixFilter::paramIntensityName = "intensity";
	CGEConstString CGEMutipleMixFilter::paramOriginImageName = "originImageTexture";

	bool CGEMutipleMixFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshHTFilterMix))
		{
			setIntensity(1.0f);
			return true;
		}
		return false;
	}

	void CGEMutipleMixFilter::setIntensity(float value)
	{
		m_intensity = value;		
	}

	void CGEMutipleMixFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, handler->getBufferTextureID());
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());
		
		m_program.sendUniformf(paramIntensityName, m_intensity);

		glActiveTexture(CGE_TEXTURE_START);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramOriginImageName, CGE_TEXTURE_START_INDEX);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	bool CGEMutipleMixFilter::needToMix()
	{
		return fabsf(m_intensity - 1.0f) > 0.01f;
	}

	bool CGEMutipleMixFilter::noIntensity()
	{
		return fabsf(m_intensity) < 0.01f;
	}

	//////////////////////////////////////////////////////////////////////////

    CGEMutipleEffectFilter::CGEMutipleEffectFilter() : m_loadFunc(NULL), m_unloadFunc(NULL), m_loadParam(NULL), m_unloadParam(NULL)
	{
        
	}

	CGEMutipleEffectFilter::~CGEMutipleEffectFilter()
	{
		clearFilters();
		CGE_LOG_INFO("CGEMutipleEffectFilter Release...\n");
	}

	void CGEMutipleEffectFilter::clearFilters()
	{
		for(std::vector<CGEImageFilterInterface*>::iterator iter = m_vecFilters.begin();
			iter != m_vecFilters.end(); ++iter)
		{
			delete *iter;
		}
		m_vecFilters.clear();
	}

	void CGEMutipleEffectFilter::setLoadFunction(CGEBufferLoadFun fLoad, void* loadParam, CGEBufferUnloadFun fUnload, void* unloadParam)
	{
		m_loadFunc = fLoad;
		m_loadParam = loadParam;
		m_unloadFunc = fUnload;
		m_unloadParam = unloadParam;
	}

	GLuint CGEMutipleEffectFilter::loadResources(const char* textureName, int* width, int* height)
	{
		int w, h;
		void* bufferData = NULL;
		CGEBufferLoadFun loadFunc = m_loadFunc;
		CGEBufferUnloadFun unloadFunc = m_unloadFunc;
		void* loadArg = m_loadParam;
		void* unloadArg = m_unloadParam;

		if(loadFunc == NULL)
		{
			loadFunc = cgeGetCommonLoadFunc();
			loadArg = cgeGetCommonLoadArg();
			unloadFunc = cgeGetCommonUnloadFunc();
			unloadArg = cgeGetCommonUnloadArg();
		}		

		CGEBufferFormat fmt;
		void* pArg;
		if(loadFunc == NULL || (pArg = loadFunc(textureName, &bufferData, &w, &h, &fmt, loadArg)) == NULL)
		{
			CGE_LOG_ERROR("Load texture %s failed!\n", textureName);
			return 0;
		}

		GLenum dataFmt, channelFmt;
		cgeGetDataAndChannelByFormat(fmt, &dataFmt, &channelFmt, NULL);
		GLuint texture = cgeGenTextureWithBuffer(bufferData, w, h, channelFmt, dataFmt);
		if(width != NULL)
			*width = w;
		if(height != NULL)
			*height = h;
		if(unloadFunc != NULL) unloadFunc(pArg, unloadArg);
		return texture;
	}

	bool CGEMutipleEffectFilter::initCustomize()
	{
		return m_mixFilter.init();
	}

	bool CGEMutipleEffectFilter::initWithEffectString(const char* effectString)
	{
		const char* ptr = effectString;
		if(ptr == NULL || *ptr == '\0' || strncmp(ptr, "@unavailable", 12) == 0 || !m_mixFilter.init()) return false;

		//////////////////////////////////////////////////////////////////////////
		//Hardcode for parsing the common effects.
		//////////////////////////////////////////////////////////////////////////
		
		while(*ptr != '\0')
		{
			while(*ptr != '\0' && *ptr != '@') ++ptr;
			while(*ptr != '\0' && (*ptr == '@' || *ptr == ' ' || *ptr == '\t')) ++ptr;
			if(*ptr == '\0') break;

			//2015-1-29 隐患fix, 将下一条指令直接取出再进行判断
			char buffer[128], *pBuffer = buffer;
			
			while(*ptr != '\0' && !isspace(*ptr) && (pBuffer - buffer) < sizeof(buffer))
			{
				*pBuffer++ = *ptr++;
			}

			*pBuffer = '\0';

			if(strcmp(buffer, "blend") == 0)
			{
				CGEDataParsingEngine::blendParser(ptr, this);
			}
			else if(strcmp(buffer, "curve") == 0)
			{
				CGEDataParsingEngine::curveParser(ptr, this);
			}
			else if(strcmp(buffer, "adjust") == 0)
			{
				CGEDataParsingEngine::adjustParser(ptr, this);
			}
			else if(strcmp(buffer, "cvlomo") == 0)
			{
				CGEDataParsingEngine::lomoWithCurveParser(ptr, this);
			}
			else if(strcmp(buffer, "lomo") == 0)
			{
				CGEDataParsingEngine::lomoParser(ptr, this);
			}
			else if(strcmp(buffer, "colorscale") == 0)
			{
				CGEDataParsingEngine::colorScaleParser(ptr, this);
			}
			else if(strcmp(buffer, "pixblend") == 0)
			{
				CGEDataParsingEngine::pixblendParser(ptr, this);
			}
			else if(strcmp(buffer, "krblend") == 0)
			{
				CGEDataParsingEngine::krblendParser(ptr, this);
			}
			else if(strcmp(buffer, "vignette") == 0)
			{
				CGEDataParsingEngine::vignetteParser(ptr, this);
			}
			else if(strcmp(buffer, "selfblend") == 0)
			{
				CGEDataParsingEngine::selfblendParser(ptr, this);
			}
			else if(strcmp(buffer, "colormul") == 0)
			{
				CGEDataParsingEngine::colorMulParser(ptr, this);
			}
			else if(strcmp(buffer, "vigblend") == 0)
			{
				CGEDataParsingEngine::vignetteBlendParser(ptr, this);
			}
			else if(strcmp(buffer, "selcolor") == 0)
			{
				CGEDataParsingEngine::selectiveColorParser(ptr, this);
			}
			else if(strcmp(buffer, "tileblend") == 0)
			{
				CGEDataParsingEngine::blendTileParser(ptr, this);
			}
			else if(strcmp(buffer, "style") == 0)
			{
				CGEDataParsingEngine::advancedStyleParser(ptr, this);
			}
			else if(strcmp(buffer, "beautify") == 0)
			{
				CGEDataParsingEngine::beautifyParser(ptr, this);
			}

			//Add more parsing rules before this one
			else
			{
				CGE_LOG_ERROR("指令未被解析(Invalid parameters):%s", ptr);
			}
		}

		if(m_vecFilters.empty())
		{
			CGE_LOG_ERROR("特效指令 \"%s\" 无法生成任何特效!\n", effectString);
			return false;
		}

		return true;
	}

	void CGEMutipleEffectFilter::setIntensity(float value)
	{
		m_mixFilter.setIntensity(value);
	}

	void CGEMutipleEffectFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		if(m_vecFilters.empty() || m_mixFilter.noIntensity())
		{
			CGE_LOG_ERROR("CGEMutipleEffectFilter::render2Texture did nothing!\n");
			handler->swapBufferFBO();
			return;
		}
		GLuint bufferTexID = 0;
		void* param = NULL;
		if(m_mixFilter.needToMix())
			bufferTexID = handler->copyLastResultTexture(&param);

		std::vector<CGEImageFilterInterface*>::iterator iter = m_vecFilters.begin();
		for(;;)
		{
			(*iter)->render2Texture(handler, handler->getBufferTextureID(), posVertices);
			if(++iter != m_vecFilters.end())
			{
				handler->swapBufferFBO();
			}
			else break;
		}

		if(bufferTexID != 0)
		{
			handler->swapBufferFBO();
			m_mixFilter.render2Texture(handler, bufferTexID, posVertices);
			handler->delCopiedTexture(&param, bufferTexID);
		}
	}
}
