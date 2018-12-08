/*
 * cgeHueAdjust.cpp
 *
 *  Created on: 2015-1-29
 *      Author: Wang Yang
 */

#include "cgeHueAdjust.h"

static CGEConstString s_fshHue = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform float hueAdjust;

vec3 rgb2yiq(vec3 src)
{
	return src * mat3(0.299, 0.587, 0.114,
		0.595716, -0.274453, -0.321263,
		0.211456, -0.522591, 0.31135);
}

vec3 yiq2rgb(vec3 src)
{
	return src * mat3(1.0, 0.9563, 0.6210,
		1.0, -0.2721, -0.6474,
		1.0, -1.1070, 1.7046);
}

void main()
{
	vec4 color = texture2D(inputImageTexture, textureCoordinate);
	vec3 yiq = rgb2yiq(color.rgb);
	float hue = atan(yiq.z, yiq.y);
	float chroma = length(yiq.yz);

	hue -= hueAdjust;
	yiq.yz = vec2(cos(hue), sin(hue)) * chroma;

	gl_FragColor = vec4(yiq2rgb(yiq), color.a);	
});

namespace CGE
{
	CGEConstString CGEHueAdjustFilter::paramName = "hueAdjust";

	bool CGEHueAdjustFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshHue);
	}

	void CGEHueAdjustFilter::setHue(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramName, value);
	}

	void CGEHueAdjustFilter::setIntensity(float intensity)
	{
		setHue(intensity);
	}

}