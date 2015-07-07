/*
* cgeSketchFilter.cpp
*
*  Created on: 2015-3-20
*      Author: Wang Yang
*/

#include "cgeSketchFilter.h"

static CGEConstString s_fshSketch = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D maxValueTex;

uniform float intensity;

float lum(vec4 v)
{
	return dot(v.rgb, vec3(0.299, 0.587, 0.114));
}

void main()
{
	vec4 origin = texture2D(inputImageTexture, textureCoordinate);
	vec4 maxValue = texture2D(maxValueTex, textureCoordinate);

	float luminance = lum(origin);
	float revMaxValue = 1.0 - lum(maxValue);

	float blendColor = min(luminance / (1.0 - revMaxValue), 1.0);

	//float blendColor = min(luminance + (luminance * revMaxValue) / (1.0 - revMaxValue), 1.0);

	gl_FragColor = vec4(mix(origin.rgb, vec3(blendColor), intensity), origin.a);
});

namespace CGE
{
	CGEConstString CGESketchFilter::paramCacheTextureName = "maxValueTex";
	CGEConstString CGESketchFilter::paramIntensityName= "intensity";

	CGESketchFilter::CGESketchFilter() : m_textureCache(0)
	{
		m_cacheSize.set(-1, -1);
	}

	CGESketchFilter::~CGESketchFilter()
	{
		glDeleteTextures(1, &m_textureCache);
	}
	
	bool CGESketchFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshSketch) && m_maxValueFilter.init())
		{
			setIntensity(1.0f);
			return true;
		}
		return false;
	}

	void CGESketchFilter::setIntensity(float intensity)
	{
		m_program.bind();
		m_program.sendUniformf(paramIntensityName, intensity);
	}

	void CGESketchFilter::render2Texture(CGE::CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		CGESizei sz = handler->getOutputFBOSize();

		if(m_textureCache == 0 || sz != m_cacheSize)
		{
			glDeleteTextures(1, &m_textureCache);
			m_textureCache = cgeGenTextureWithBuffer(NULL, sz.width, sz.height, GL_RGBA, GL_UNSIGNED_BYTE);
			m_cacheSize = sz;
		}

		handler->setAsTarget();

		{
//			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureCache, 0);
			ProgramObject& programMaxValue = m_maxValueFilter.getProgram();
			programMaxValue.bind();

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);		
			glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
			glBindTexture(GL_TEXTURE_2D, srcTexture);
			programMaxValue.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
			UniformParameters* param = m_maxValueFilter.getUniformParam();
			if(param != NULL)
				param->assignUniforms(handler, programMaxValue.programID());
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureCache, 0);
			glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
			glBindTexture(GL_TEXTURE_2D, handler->getTargetTextureID());
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		glFlush();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handler->getTargetTextureID(), 0);

		m_program.bind();

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);		
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);

		glActiveTexture(CGE_TEXTURE_START);
		glBindTexture(GL_TEXTURE_2D, m_textureCache);
		m_program.sendUniformi(paramCacheTextureName, CGE_TEXTURE_START_INDEX);

		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	void CGESketchFilter::flush()
	{
		glDeleteTextures(1, &m_textureCache);
		m_textureCache = 0;
	}

}