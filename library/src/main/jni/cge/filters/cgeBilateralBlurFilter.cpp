/* cgeBilateralBlurFilter.cpp
*
*  Created on: 2014-4-1
*      Author: Wang Yang
*/

#include "cgeBilateralBlurFilter.h"
#include <cmath>

CGEConstString s_fshBilateralBlur = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

//const int GAUSSIAN_SAMPLES = 9;
uniform float blurFactors[9];// = float[GAUSSIAN_SAMPLES](0.05, 0.09, 0.12, 0.15, 0.18, 0.15, 0.12, 0.09, 0.05);

uniform float distanceNormalizationFactor;
uniform float blurSamplerScale;
uniform vec2 samplerSteps;

const int samplerRadius = 4;

float random(vec2 seed)
{
	return fract(sin(dot(seed ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec4 centralColor = texture2D(inputImageTexture, textureCoordinate);
	float gaussianWeightTotal = blurFactors[4];
	vec4 sum = centralColor * blurFactors[4];
	vec2 stepScale = blurSamplerScale * samplerSteps;
	float offset = random(textureCoordinate) - 0.5;

    for(int i = 0; i < samplerRadius; ++i)
	{
		vec2 dis = (float(i) + offset) * stepScale;
		
        float blurfactor = blurFactors[samplerRadius-i];

		{
			vec4 sampleColor1 = texture2D(inputImageTexture, textureCoordinate + dis);
			float distanceFromCentralColor1 = min(distance(centralColor, sampleColor1) * distanceNormalizationFactor, 1.0);
            float gaussianWeight1 = blurfactor * (1.0 - distanceFromCentralColor1);
			gaussianWeightTotal += gaussianWeight1;
			sum += sampleColor1 * gaussianWeight1;
		}

		//////////////////////////////////////////////////////////////////////////

		{
			vec4 sampleColor2 = texture2D(inputImageTexture, textureCoordinate - dis);
			float distanceFromCentralColor2 = min(distance(centralColor, sampleColor2) * distanceNormalizationFactor, 1.0);
            float gaussianWeight2 = blurfactor * (1.0 - distanceFromCentralColor2);
			gaussianWeightTotal += gaussianWeight2;
			sum += sampleColor2 * gaussianWeight2;
		}
	}

	gl_FragColor = sum / gaussianWeightTotal;
}
);

CGEConstString s_fshBilateralBlur2 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform float distanceNormalizationFactor;
uniform float blurSamplerScale;
uniform vec2 samplerSteps;

uniform int samplerRadius;

const float arg = 0.5;

float random(vec2 seed)
{
	return fract(sin(dot(seed ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec4 centralColor = texture2D(inputImageTexture, textureCoordinate);
	float lum = dot(centralColor.rgb, vec3(0.299, 0.587, 0.114));
	float factor = (1.0 + arg) / (arg + lum) * distanceNormalizationFactor;

	float gaussianWeightTotal = 1.0;
	vec4 sum = centralColor * gaussianWeightTotal;
	vec2 stepScale = blurSamplerScale * samplerSteps / float(samplerRadius);
	float offset = random(textureCoordinate) - 0.5;

	for(int i = 1; i <= samplerRadius; ++i)
	{
		vec2 dis = (float(i) + offset) * stepScale;
		float percent = 1.0 - (float(i) + offset) / float(samplerRadius);

		{
			vec4 sampleColor1 = texture2D(inputImageTexture, textureCoordinate + dis);
			float distanceFromCentralColor1 = min(distance(centralColor, sampleColor1) * factor, 1.0);
			float gaussianWeight1 = percent * (1.0 - distanceFromCentralColor1);
			gaussianWeightTotal += gaussianWeight1;
			sum += sampleColor1 * gaussianWeight1;
		}

		//////////////////////////////////////////////////////////////////////////

		{
			vec4 sampleColor2 = texture2D(inputImageTexture, textureCoordinate - dis);
			float distanceFromCentralColor2 = min(distance(centralColor, sampleColor2) * factor, 1.0);
			float gaussianWeight2 = percent * (1.0 - distanceFromCentralColor2);
			gaussianWeightTotal += gaussianWeight2;
			sum += sampleColor2 * gaussianWeight2;
		}
	}

	gl_FragColor = sum / gaussianWeightTotal;
}
);

namespace CGE
{
	CGEConstString CGEBilateralBlurFilter::paramDistanceFactorName = "distanceNormalizationFactor";
	CGEConstString CGEBilateralBlurFilter::paramBlurSamplerScaleName = "blurSamplerScale";
	CGEConstString CGEBilateralBlurFilter::paramBlurFactorsName = "blurFactors";

	bool CGEBilateralBlurFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshBilateralBlur))
		{
			setBlurScale(4.0f);
			setDistanceNormalizationFactor(8.0);
			GLint loc = m_program.uniformLocation(paramBlurFactorsName);
			if(loc < 0)
				return false;
			const float factors[9] = {0.05f, 0.09f, 0.12f, 0.15f, 0.18f, 0.15f, 0.12f, 0.09f, 0.05f};
			glUniform1fv(loc, 9, factors);
			return true;
		}
		return false;
	}

	void CGEBilateralBlurFilter::setBlurScale(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramBlurSamplerScaleName, value / 4.0f);
	}

	void CGEBilateralBlurFilter::setDistanceNormalizationFactor(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramDistanceFactorName, value);
	}

    CGEConstString CGEBilateralBlurBetterFilter::paramBlurRadiusName = "samplerRadius";

    bool CGEBilateralBlurBetterFilter::init()
    {
        if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshBilateralBlur2))
        {
            setBlurScale(4.0f);
            setDistanceNormalizationFactor(8.0f);
            setSamplerRadiusLimit(15);
            return true;
        }
        return false;
    }

    void CGEBilateralBlurBetterFilter::setSamplerRadiusLimit(int limit)
    {
        m_limit = limit;
    }

    void CGEBilateralBlurBetterFilter::setBlurScale(float value)
    {
        m_program.bind();
        m_program.sendUniformf(paramBlurSamplerScaleName, value);
        int radius = CGE_MIN(m_limit, (int)value);
        if(radius < 0)
            radius = 0;
        m_program.sendUniformi(paramBlurRadiusName, radius);
    }

	//////////////////////////////////////////////////////////////////////////

	bool CGEBilateralWrapperFilter::init()
	{
		m_proc = new CGEBilateralBlurFilter;

		if(!m_proc->init())
		{
			delete m_proc;
			m_proc = nullptr;
		}
		return true;
	}

	void CGEBilateralWrapperFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
	{
		assert(m_proc != nullptr); // Filter 尚未初始化成功

		float blurScale = 200.0f * powf(0.5f, m_blurScale / 50.0f);

		CGESizei sz = handler->getOutputFBOSize();

		m_proc->setBlurScale(CGE::CGE_MIN(sz.width, sz.height) / blurScale);

		for(int i = 0; i < m_repeatTimes; )
		{
			m_proc->render2Texture(handler, srcTexture, vertexBufferID);

			if(++i < m_repeatTimes)
			{
				handler->swapBufferFBO();
			}
		}
	}

}
