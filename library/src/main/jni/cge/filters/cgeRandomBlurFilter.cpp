/*
 * cgeRandomBlurFilter.cpp
 *
 *  Created on: 2013-12-29
 *      Author: Wang Yang
 */

#include "cgeRandomBlurFilter.h"

const static char* const s_fshRandomBlur = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 samplerSteps;
uniform float blurSamplerScale;
uniform float intensity;
uniform float samplerRadius;

float random(vec2 seed)
{
	return fract(sin(dot(seed ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec3 sumColor = vec3(0.0, 0.0, 0.0);
	float dir = random(textureCoordinate);
	float samplingCount = 0.0;
	for(float fy = -samplerRadius; fy <= samplerRadius; ++fy)
	{
		for(float fx = -samplerRadius; fx <= samplerRadius; ++fx)
		{
			float dis = length(vec2(fx, fy) * samplerSteps) * blurSamplerScale;
			vec2 coord = dis * vec2(cos(dir), sin(dir));
			sumColor += texture2D(inputImageTexture, textureCoordinate + coord).rgb;
			++samplingCount;
		}
	}

	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	vec3 dst = mix(src.rgb, sumColor / samplingCount, intensity);
	gl_FragColor = vec4(dst, src.a);
}
);

namespace CGE
{
	CGEConstString CGERandomBlurFilter::paramIntensity = "intensity";
	CGEConstString CGERandomBlurFilter::paramSamplerScale = "blurSamplerScale";
	CGEConstString CGERandomBlurFilter::paramSamplerRadius = "samplerRadius";

	bool CGERandomBlurFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshRandomBlur))
		{
			setIntensity(1.0f);
			setSamplerScale(2.0f);
			return true;
		}
		return false;
	}

	void CGERandomBlurFilter::setIntensity(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramIntensity, value);
	}

	void CGERandomBlurFilter::setSamplerScale(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramSamplerScale, value);
		if(value > 10.0f) value = 5.0f;
		m_program.sendUniformf(paramSamplerRadius, value);
	}
}