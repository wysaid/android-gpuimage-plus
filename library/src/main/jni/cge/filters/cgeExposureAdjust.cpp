/*
 * cgeExposureAdjust.cpp
 *
 *  Created on: 2015-1-29
 *      Author: Wang Yang
 */

#include "cgeExposureAdjust.h"

static CGEConstString s_fshExposure = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform float exposure;

void main()
{
	vec4 color = texture2D(inputImageTexture, textureCoordinate);
	gl_FragColor = vec4(color.rgb * exp2(exposure), color.a);
});

namespace CGE
{
	CGEConstString CGEExposureFilter::paramName = "exposure";

	bool CGEExposureFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshExposure);
	}

	void CGEExposureFilter::setIntensity(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramName, value);
	}
}