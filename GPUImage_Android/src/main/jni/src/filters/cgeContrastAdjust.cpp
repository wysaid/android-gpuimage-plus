/*
* cgeContrastAdjust.cpp
*
*  Created on: 2013-12-26
*      Author: Wang Yang
*/

#include "cgeContrastAdjust.h"

const static char* const s_fshContrast = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform float intensity;

void main()
{
	vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
	gl_FragColor = vec4(((textureColor.rgb - vec3(0.5)) * intensity + vec3(0.5)), textureColor.a);
}

);

namespace CGE
{
	CGEConstString CGEContrastFilter::paramName = "intensity";

	bool CGEContrastFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshContrast))
		{
			return true;
		}
		return false;
	}

	void CGEContrastFilter::setIntensity(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramName, value);
	}
}