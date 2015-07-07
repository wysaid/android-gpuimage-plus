/*
* cgeMonochromeAdjust.cpp
*
*  Created on: 2013-12-29
*      Author: Wang Yang
*/

#include "cgeMonochromeAdjust.h"

const static char* const s_fshMonochrome = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform float red;
uniform float green;
uniform float blue;
uniform float cyan;
uniform float magenta;
uniform float yellow;

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	float maxc = max(max(src.r, src.g), src.b);
	float minc = min(min(src.r, src.g), src.b);
	float midc = src.r + src.g + src.b - maxc - minc;

	if(maxc == minc)
		gl_FragColor = src;

	vec3 ratioMax, ratioMin;
	ratioMax.xy = vec2(equal(src.rg, vec2(maxc)));
	float max_neg = 1.0 - ratioMax.x;
	ratioMax.y *= max_neg;
	ratioMax.z = (1.0 - ratioMax.y) * max_neg;

	vec3 compMax = vec3(red, green, blue) * ratioMax;

	ratioMin.xy = vec2(equal(src.rg, vec2(minc)));
	float min_neg = 1.0 - ratioMin.x;
	ratioMin.y *= min_neg;
	ratioMin.z = (1.0 - ratioMin.y) * min_neg;

	vec3 compMaxMid = vec3(cyan, magenta, yellow) * ratioMin;

	float total = (compMax.x + compMax.y + compMax.z) * (maxc - midc) + (compMaxMid.x + compMaxMid.y + compMaxMid.z) * (midc - minc) + minc;

	gl_FragColor = vec4(total, total, total, 1.0);
}
);

namespace CGE
{
	CGEConstString CGEMonochromeFilter::paramRed = "red";
	CGEConstString CGEMonochromeFilter::paramGreen = "green";
	CGEConstString CGEMonochromeFilter::paramBlue = "blue";
	CGEConstString CGEMonochromeFilter::paramCyan = "cyan";
	CGEConstString CGEMonochromeFilter::paramMagenta = "magenta";
	CGEConstString CGEMonochromeFilter::paramYellow = "yellow";

	bool CGEMonochromeFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshMonochrome))
		{
			return true;
		}
		return false;
	}

	void CGEMonochromeFilter::setRed(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramRed, value);
	}

	void CGEMonochromeFilter::setGreen(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramGreen, value);
	}

	void CGEMonochromeFilter::setBlue(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramBlue, value);
	}

	void CGEMonochromeFilter::setCyan(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramCyan, value);
	}

	void CGEMonochromeFilter::setMagenta(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramMagenta, value);
	}

	void CGEMonochromeFilter::setYellow(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramYellow, value);
	}
}