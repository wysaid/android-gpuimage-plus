/*
 * cgeVignetteAdjust.cpp
 *
 *  Created on: 2014-1-22
 *      Author: Wang Yang
 */

#include "cgeVignetteAdjust.h"

static CGEConstString s_fshVignette = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 vignette;
uniform vec2 vignetteCenter;

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	float d = distance(textureCoordinate, vignetteCenter);
	float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
	float alpha = 1.0 - percent;

	gl_FragColor = vec4(src.rgb * alpha, src.a);
}
);

static CGEConstString s_fshVignetteLinear = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 vignette;
uniform vec2 vignetteCenter;
uniform vec3 vignetteColor;

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	float d = distance(textureCoordinate, vignetteCenter);
	float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
	float alpha = 1.0 - percent;

	gl_FragColor = vec4(mix(vignetteColor, src.rgb, alpha), src.a);
}
);

namespace CGE
{
	CGEConstString CGEVignetteFilter::paramVignetteCenterName = "vignetteCenter";
	CGEConstString CGEVignetteFilter::paramVignetteName = "vignette";

	bool CGEVignetteFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshVignette))
		{
			setVignetteCenter(0.5, 0.5);
			return true;
		}
		return false;
	}

	void CGEVignetteFilter::setVignetteCenter(float x, float y)
	{
		m_program.bind();
		m_program.sendUniformf(paramVignetteCenterName, x, y);
	}

	void CGEVignetteFilter::setVignette(float start, float range)
	{
		m_program.bind();
		m_program.sendUniformf(paramVignetteName, start, range);
	}

	//////////////////////////////////////////////////////////////////////////

	CGEConstString CGEVignetteExtFilter::paramVignetteColor = "vignetteColor";

	bool CGEVignetteExtFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshVignetteLinear))
		{
			setVignetteCenter(0.5f, 0.5f);
			setVignetteColor(0.0f, 0.0f, 0.0f);
			return true;
		}
		return false;
	}

	void CGEVignetteExtFilter::setVignetteColor(float r, float g, float b)
	{
		m_program.bind();
		m_program.sendUniformf(paramVignetteColor, r, g, b);
	}

}