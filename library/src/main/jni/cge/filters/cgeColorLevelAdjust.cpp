/*
 * cgeColorLevelAdjust.cpp
 *
 *  Created on: 2014-1-20
 *      Author: Wang Yang
 */

#include "cgeColorLevelAdjust.h"

const static char* const s_fshColorLevel = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform vec2 colorLevel;
uniform float gamma;

vec3 levelFunc(vec3 src, vec2 colorLevel) 
{
	return clamp((src - colorLevel.x) / (colorLevel.y - colorLevel.x), 0.0, 1.0);
}

vec3 gammaFunc(vec3 src, float value) //value: 0~1
{
	return clamp(pow(src, vec3(value)), 0.0, 1.0);
}

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	src.rgb = levelFunc(src.rgb, colorLevel);
	src.rgb = gammaFunc(src.rgb, gamma);
	gl_FragColor = src;
}
);

namespace CGE
{
	CGEConstString CGEColorLevelFilter::paramLevelName = "colorLevel";
	CGEConstString CGEColorLevelFilter::paramGammaName = "gamma";

	bool CGEColorLevelFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshColorLevel))
		{
			setGamma(1.0f);
			setLevel(0.0f, 1.0f);
			return true;
		}
		return false;
	}

	void CGEColorLevelFilter::setLevel(float dark, float light)
	{
		m_program.bind();
		m_program.sendUniformf(paramLevelName, dark, light);
	}

	void CGEColorLevelFilter::setGamma(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramGammaName, value);
	}

}