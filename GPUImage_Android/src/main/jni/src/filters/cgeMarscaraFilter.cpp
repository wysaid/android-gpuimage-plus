/*
* cgeMarscara.cpp
*
*  Created on: 2014-5-5
*      Author: Wang Yang
*/

#include "cgeMarscaraFilter.h"

#define MARSCARA_SAMPLER_RADIUS 5.0f

CGEConstString s_fshMarscara = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
//uniform vec3 avgColor;
uniform vec2 samplerSteps;

const int samplerRadius = 5;
uniform float samplerScale;

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec3 sum = src.rgb;
	float cnt = 1.0;
	for(int i = 1; i <= samplerRadius; ++i)
	{
		float scale = float(i) * samplerScale;
		sum += texture2D(inputImageTexture, textureCoordinate + scale * vec2(samplerSteps.x, 0.0)).rgb;
		sum += texture2D(inputImageTexture, textureCoordinate + scale * vec2(-samplerSteps.x, 0.0)).rgb;
		sum += texture2D(inputImageTexture, textureCoordinate + scale * vec2(0.0, samplerSteps.y)).rgb;
		sum += texture2D(inputImageTexture, textureCoordinate + scale * vec2(0.0, -samplerSteps.y)).rgb;
		cnt += 4.0;
	}

	gl_FragColor = vec4(src.rgb - max(sum / cnt - src.rgb, 0.0), src.a);
}
);

namespace CGE
{
	CGEConstString CGEMarscaraFilter::paramSamplerScaleName = "samplerScale";

	bool CGEMarscaraFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshMarscara))
		{
			setSamplerScale(4);
			return true;
		}
		return false;
	}

	void CGEMarscaraFilter::setSamplerScale(int value)
	{
		m_program.bind();
		m_program.sendUniformf(paramSamplerScaleName, value / MARSCARA_SAMPLER_RADIUS);
	}

}