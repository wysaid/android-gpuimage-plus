/*
* cgeWrinkleRemoverFilter.cpp
*
*  Created on: 2014-5-5
*      Author: Wang Yang
*/

#include "cgeWrinkleRemoverFilter.h"

#define WRINKLE_SAMPLER_RADIUS 5.0f

static CGEConstString s_fshWrinkleRemove = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform float samplerScale;
uniform vec2 noiseRange;
uniform vec2 samplerSteps;
uniform float noiseIntensity;

const int samplerRadius = 5;

float random(vec2 seed)
{
	return fract(sin(dot(seed ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 screen(vec3 src1, vec3 src2, float alpha)
{
	return mix(src1, src1 + src2 - src1 * src2, alpha);
}

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec3 sum = src.rgb;
	float weight = 1.0;

	float offset = random(textureCoordinate) - 0.5;
	vec2 fScale = samplerScale * samplerSteps;
	for(int i = 1; i <= samplerRadius; ++i)
	{
		vec2 coordOffset = (float(i) + offset) * fScale;
		sum += texture2D(inputImageTexture, textureCoordinate + coordOffset).rgb;
		sum += texture2D(inputImageTexture, textureCoordinate - coordOffset).rgb;

// 		sum += texture2D(inputImageTexture, textureCoordinate + scale * vec2(samplerSteps.x, 0.0)).rgb;
// 		sum += texture2D(inputImageTexture, textureCoordinate + scale * vec2(-samplerSteps.x, 0.0)).rgb;
// 		sum += texture2D(inputImageTexture, textureCoordinate + scale * vec2(0.0, samplerSteps.y)).rgb;
// 		sum += texture2D(inputImageTexture, textureCoordinate + scale * vec2(0.0, -samplerSteps.y)).rgb;
		weight += 2.0;		
	}

	float noise = random(textureCoordinate + src.rg + src.ba) * (noiseRange.y - noiseRange.x) + noiseRange.x;
	vec3 result = screen(sum / weight, vec3(noise), noiseIntensity);
	gl_FragColor = vec4(result, src.a);
}
);


namespace CGE
{
	CGEConstString CGEWrinkleRemoveFilter::paramSamplerScaleName = "samplerScale";
	CGEConstString CGEWrinkleRemoveFilter::paramNoiseRangeName = "noiseRange";
	CGEConstString CGEWrinkleRemoveFilter::paramNoiseIntensityName = "noiseIntensity";

	bool CGEWrinkleRemoveFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshWrinkleRemove))
		{
			setBlurScale(4.0f);
			setNoiseRange(0.0f, 0.07f);
			setNoiseIntensity(1.0);
			return true;
		}
		return false;
	}

	void CGEWrinkleRemoveFilter::setBlurScale(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramSamplerScaleName, value / WRINKLE_SAMPLER_RADIUS);
	}

	void CGEWrinkleRemoveFilter::setNoiseRange(float low, float high)
	{
		m_program.bind();
		m_program.sendUniformf(paramNoiseRangeName, low, high);
	}

	void CGEWrinkleRemoveFilter::setNoiseIntensity(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramNoiseIntensityName, value);
	}
}