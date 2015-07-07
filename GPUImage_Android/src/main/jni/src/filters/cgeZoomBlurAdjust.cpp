/*
* cgeZoomBlurAdjust.cpp
*
*  Created on: 2014-9-17
*      Author: wxfred
*/

#include "cgeZoomBlurAdjust.h"

const static char* const s_fshZoomBlur = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 center;
uniform float strength;

float random(vec2 seed)
{
	return fract(sin(dot(seed ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec4 color = vec4(0.0);
	float total = 0.0;
	vec2 toCenter = center - textureCoordinate;
	float offset = random(textureCoordinate);
	for (float t=0.0;t<=40.0;t++)
	{
		float percent = (t + offset) / 40.0;
		float weight = 4.0 * (percent - percent*percent);
		vec4 sample = texture2D(inputImageTexture, textureCoordinate + toCenter*percent*strength);
		sample.rgb *= sample.a;
		color += sample*weight;
		total += weight;
	}

	gl_FragColor = color / total;
	gl_FragColor.rgb /= gl_FragColor.a + 0.00001;
}
);

static CGEConstString s_fshZoomBlur2 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 center;
uniform float strength;

const float radius = 10.0;

float random(vec2 seed)
{
	return fract(sin(dot(seed ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec2 vNorm = (center - textureCoordinate) * strength;
	float weight = 0.0;
	vec4 total = vec4(0.0);
	float offset = random(textureCoordinate);

	for(float f = -radius; f <= radius; ++f)
	{
		float pt = (f + offset) / radius;		
//		float pt = f / radius;
		float npt = 1.0 - abs(pt);
		total += npt * texture2D(inputImageTexture, textureCoordinate + vNorm * pt);
		weight += npt;
	}

	gl_FragColor = vec4(total / weight);
}
);

namespace CGE
{
	CGEConstString CGEZoomBlurFilter::paramCenterName = "center";
	CGEConstString CGEZoomBlurFilter::paramIntensityName = "strength";

	bool CGEZoomBlurFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshZoomBlur))
		{
			setCenter(0.5, 0.5);
			return true;
		}
		return false;
	}

	void CGEZoomBlurFilter::setCenter(float x, float y)
	{
		m_program.bind();
		m_program.sendUniformf(paramCenterName, x, y);
	}

	void CGEZoomBlurFilter::setIntensity(float strength)
	{
		m_program.bind();
		m_program.sendUniformf(paramIntensityName, strength);
	}

	//////////////////////////////////////////////////////////////////////////

	CGEConstString CGEZoomBlur2Filter::paramStepsName = "samplerSteps";

	bool CGEZoomBlur2Filter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshZoomBlur2))
		{
			setCenter(0.5, 0.5);
// 			m_uniformParam = new UniformParameters();
// 			m_uniformParam->requireStepsFactor(paramStepsName);
// 			setAdditionalUniformParameter(m_uniformParam)
			return true;
		}
		return false;
	}

}