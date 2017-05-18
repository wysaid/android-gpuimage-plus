/*
 * cgeTiltshiftAdjust.cpp
 *
 *  Created on: 2014-1-24
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
*/

#include "cgeTiltshiftAdjust.h"
#include "assert.h"
#include <cmath>

#define TEXTURE_BIND_ID 0

static CGEConstString s_fshTiltshiftVector = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D blurredImageTexture;
uniform vec2 blurGradient; // x for start, y for range.
uniform vec2 blurPassPos;
uniform vec2 blurNormal;
uniform vec2 vSteps;

void main()
{
	float dis = abs(dot(blurNormal, textureCoordinate / vSteps - blurPassPos));
	if(dis <= blurGradient.x)
	{
		gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
		return;
	}

	vec4 color = texture2D(blurredImageTexture, textureCoordinate);
	dis -= blurGradient.x;
	if(dis <= blurGradient.y)
	{
		vec4 src = texture2D(inputImageTexture, textureCoordinate);
		float radius = dis / blurGradient.y;)
#if defined(DEBUG) || defined(_DEBUG)
		"color.rb = vec2(1.0, 0.0);"
#endif
CGE_SHADER_STRING(
		color = mix(src, color, radius);
	}
	gl_FragColor = color;
}
);

static CGEConstString s_fshTiltshiftEllipse = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D blurredImageTexture;
uniform float blurGradient;
uniform vec2 blurCentralPos;
uniform vec2 radiusStart;
uniform mat2 mRot;
uniform vec2 vSteps;

void main()
{
	vec2 dis = textureCoordinate / vSteps - blurCentralPos;
	vec2 coord = dis * mRot;

	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	if(length(coord / radiusStart) <= 1.0)
	{
		gl_FragColor = src;
		return;
	}
	vec2 gradientRadiusStart = radiusStart * blurGradient;

	vec4 color = texture2D(blurredImageTexture, textureCoordinate);
	if(length(coord / gradientRadiusStart) <= 1.0)
	{
		vec2 norm = coord;
		if(abs(norm.x) < 0.0001) norm.x = 0.0001;
		float radius = length(norm);
		float k = norm.y / norm.x;
		float sa2 = radiusStart.x * radiusStart.x;
		float sb2 = radiusStart.y * radiusStart.y;
		float sxPoint = sqrt(sa2 * sb2 / (sb2 + sa2 * k * k));
		float sRadius = abs(sxPoint / (norm.x / radius));

		float ea2 = gradientRadiusStart.x * gradientRadiusStart.x;
		float eb2 = gradientRadiusStart.y * gradientRadiusStart.y;
		float exPoint = sqrt(ea2 * eb2 / (eb2 + ea2 * k * k));
		float eRadius = abs(exPoint / (norm.x / radius));
		float range = eRadius - sRadius;)

#if defined(DEBUG) || defined(_DEBUG)
		"color.rb = vec2(1.0, 0.0);"
#endif

CGE_SHADER_STRING(
        color = mix(src, color, (radius - sRadius) / range);
	}
	gl_FragColor = color;
}
);

static void normalize(GLfloat* x, GLfloat* y)
{
	assert(*x != 0.0f || *y != 0.0f);
	GLfloat len = sqrtf(*x**x+*y**y);
	*x /= len;
	*y /= len;
}
 
// static void getNormalizedOrthogonalityVector(float x, float y, float* fx, float* fy)
// {
// 	assert(x != 0.0f || y != 0.0f);
// 	GLfloat len = sqrtf(x*x+y*y);
// 	*fx = sqrt(y*y / len);
// 	*fy = sqrt(x*x / len);
// 	if(y < 0.0f) *fx = -*fx;
// 	if(x < 0.0f) *fy = -*fy;
// }

static CGEConstString paramBlurredTexName = "blurredImageTexture";
static CGEConstString paramStepsName = "vSteps";

namespace CGE
{
	CGEConstString CGETiltshiftVectorFilter::paramGradientName = "blurGradient";
	CGEConstString CGETiltshiftVectorFilter::paramBlurPassPosName = "blurPassPos";
	CGEConstString CGETiltshiftVectorFilter::paramBlurNormalName = "blurNormal";

	bool CGETiltshiftVectorFilter::init()
	{
		if(m_blurProc.initWithoutFixedRadius() && initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshTiltshiftVector))
		{
			setBlurGradient(100.0f, 100.0f);
			setBlurPassPos(0.0f, 0.0f);
			setBlurNormal(1.0f, 1.0f);
			UniformParameters* param = new UniformParameters;
			param->pushSampler2D(paramBlurredTexName, &m_texture, TEXTURE_BIND_ID);
			param->requireStepsFactor(paramStepsName);
			setAdditionalUniformParameter(param);
			return true;
		}
		return false;
	}

	void CGETiltshiftVectorFilter::setBlurGradient(GLfloat start, GLfloat gradient)
	{
		m_program.bind();
		m_program.sendUniformf(paramGradientName, start, gradient);
	}

	void CGETiltshiftVectorFilter::setBlurPassPos(GLfloat x, GLfloat y)
	{
		m_program.bind();
		m_program.sendUniformf(paramBlurPassPosName, x, y);
	}

	void CGETiltshiftVectorFilter::setBlurNormal(GLfloat x, GLfloat y)
	{
		m_program.bind();
		normalize(&x, &y);
		m_program.sendUniformf(paramBlurNormalName, y, x);
	}

	void CGETiltshiftVectorFilter::setRotation(GLfloat angle)
	{
		GLfloat x = cosf(angle), y = sinf(angle);
		m_program.bind();
		m_program.sendUniformf(paramBlurNormalName, y, x);
	}

	void CGETiltshiftVectorFilter::setBlurRadiusScale(int radius)
	{
		if(m_samplerScale == radius)
			return ;
		m_samplerScale = radius;
		m_blurProc.setSamplerScale(radius);
		flush();
	}

	void CGETiltshiftVectorFilter::setBlurRadiusLimit(int limit)
	{
		m_blurProc.setSamplerLimit(limit);
	}

	void CGETiltshiftVectorFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
	{
		handler->setAsTarget();
		m_program.bind();
		const CGESizei& sz = handler->getOutputFBOSize();
		if(m_texture == 0)
		{
			m_texture = cgeGenTextureWithBuffer(nullptr, sz.width, sz.height, GL_RGBA, GL_UNSIGNED_BYTE);
			m_blurProc.render2Texture(handler, m_texture, vertexBufferID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handler->getTargetTextureID(), 0);
		}

		CGEImageFilterInterface::render2Texture(handler, srcTexture, vertexBufferID);
	}

	void CGETiltshiftVectorFilter::flush()
	{
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	
	CGEConstString CGETiltshiftEllipseFilter::paramRotationName = "mRot";
	CGEConstString CGETiltshiftEllipseFilter::paramGradientName = "blurGradient";
	CGEConstString CGETiltshiftEllipseFilter::paramCentralPosName = "blurCentralPos";
	CGEConstString CGETiltshiftEllipseFilter::paramRadiusStartName = "radiusStart";

	bool CGETiltshiftEllipseFilter::init()
	{
		if(m_blurProc.initWithoutFixedRadius() && initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshTiltshiftEllipse))
		{
			setBlurGradient(1.5f);
			setBlurCentralPos(500.0f, 500.0f);
			setRadiusStart(200.0f, 100.0f);
			setRotation(0.0f);
			UniformParameters* param = new UniformParameters;
			param->pushSampler2D(paramBlurredTexName, &m_texture, TEXTURE_BIND_ID);
			param->requireStepsFactor(paramStepsName);
			setAdditionalUniformParameter(param);
			return true;
		}
		return false;
	}

	void CGETiltshiftEllipseFilter::setBlurGradient(GLfloat gradient)
	{
		if(gradient < 1.0f) gradient = 1.0f;
		m_program.bind();
		m_program.sendUniformf(paramGradientName, gradient);
	}

	void CGETiltshiftEllipseFilter::setBlurCentralPos(GLfloat centralX, GLfloat centralY)
	{
		m_program.bind();
		m_program.sendUniformf(paramCentralPosName, centralX, centralY);
	}

	void CGETiltshiftEllipseFilter::setRadiusStart(GLfloat radiusX, GLfloat radiusY)
	{
		m_program.bind();
		m_program.sendUniformf(paramRadiusStartName, radiusX, radiusY);
	}

	void CGETiltshiftEllipseFilter::setBlurRadiusScale(int radius)
	{
		if(m_samplerScale == radius)
			return ;
		m_samplerScale = radius;
		m_blurProc.setSamplerScale(radius);
		flush();
	}

	void CGETiltshiftEllipseFilter::setBlurRadiusLimit(int limit)
	{
		m_blurProc.setSamplerLimit(limit);
	}

	void CGETiltshiftEllipseFilter::setRotation(GLfloat rot)
	{
		m_program.bind();
		GLint loc = glGetUniformLocation(m_program.programID(), paramRotationName);
		GLfloat mat[4];
		mat[0] = cosf(rot);
		mat[1] = -sinf(rot);
		mat[2] = -mat[1];
		mat[3] = mat[0];
		glUniformMatrix2fv(loc, 1, GL_FALSE, mat);
	}

	void CGETiltshiftEllipseFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
	{
		handler->setAsTarget();
		m_program.bind();
		const CGESizei& sz = handler->getOutputFBOSize();
		if(m_texture == 0)
		{
			m_texture = cgeGenTextureWithBuffer(nullptr, sz.width, sz.height, GL_RGBA, GL_UNSIGNED_BYTE);
			m_blurProc.render2Texture(handler, m_texture, vertexBufferID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handler->getTargetTextureID(), 0);
		}
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CGEImageFilterInterface::render2Texture(handler, srcTexture, vertexBufferID);
	}

	void CGETiltshiftEllipseFilter::flush()
	{
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}


	//////////////////////////////////////////////////////////////////////////
	
	bool CGETiltshiftVectorWithFixedBlurRadiusFilter::init()
	{
		if(m_blurProc.initWithoutFixedRadius(false) && initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshTiltshiftVector))
		{
			setBlurGradient(100.0f, 100.0f);
			setBlurPassPos(0.0f, 0.0f);
			setBlurNormal(1.0f, 1.0f);
			UniformParameters* param = new UniformParameters;
			param->pushSampler2D(paramBlurredTexName, &m_texture, TEXTURE_BIND_ID);
			param->requireStepsFactor(paramStepsName);
			setAdditionalUniformParameter(param);
			return true;
		}
		return false;
	}

	bool CGETiltshiftEllipseWithFixedBlurRadiusFilter::init()
	{
		if(m_blurProc.initWithoutFixedRadius(false) && initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshTiltshiftEllipse))
		{
			setBlurGradient(1.5f);
			setBlurCentralPos(500.0f, 500.0f);
			setRadiusStart(200.0f, 100.0f);
			setRotation(0.0f);
			UniformParameters* param = new UniformParameters;
			param->pushSampler2D(paramBlurredTexName, &m_texture, TEXTURE_BIND_ID);
			param->requireStepsFactor(paramStepsName);
			setAdditionalUniformParameter(param);
			return true;
		}
		return false;
	}
}