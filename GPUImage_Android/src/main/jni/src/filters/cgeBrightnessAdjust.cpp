/*
* cgeBrightnessAdjust.cpp
*
*  Created on: 2013-12-26
*      Author: Wang Yang
*/

#include "cgeBrightnessAdjust.h"
#include <cmath>

const static char* const s_fshBrightness = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform float intensity;
const float SQRT2 = 1.41421;
void main()
{
	vec4 tmp = texture2D(inputImageTexture, textureCoordinate);
	vec3 src = tmp.rgb;
	float alpha = tmp.a;

	if (intensity != 0.0)
	{
		float fac = SQRT2 / intensity;

		if(intensity > 0.0)
		{
			src = 1.0 - src - (fac / SQRT2) + sqrt(1.0 - SQRT2*fac + (2.0*SQRT2) * src * fac + 0.5 * fac * fac);
		}
		else
		{
			src = 1.0 - src - (fac / SQRT2) - sqrt(1.0 - SQRT2*fac + (2.0*SQRT2) * src * fac + 0.5 * fac * fac);
		}
	}
	gl_FragColor = vec4(src, alpha);
}
);

namespace CGE
{
	CGEConstString CGEBrightnessFilter::paramName = "intensity";

	bool CGEBrightnessFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshBrightness))
		{
			return true;
		}
		return false;
	}

	void CGEBrightnessFilter::setIntensity(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramName, value);
	}

	//////////////////////////////////////////////////////////////////////////

	bool CGEBrightnessFastFilter::init()
	{
		if(CGEFastAdjustRGBFilter::init())
		{
			assignCurveArray();
			return true;
		}
		return false;
	}

	void CGEBrightnessFastFilter::setIntensity(float value)
	{
		if(fabsf(value) < 0.001) 
		{
			initCurveArray();
			assignCurveArray();
			return;
		}
		std::vector<float>::size_type sz = m_curveRGB.size();
		int iSign = value > 0.0f ? 1 : -1;
		const float SQRT2 = 1.41421f;
		const float fac = SQRT2 / value;
		for(std::vector<float>::size_type t = 0; t != sz; ++t)
		{
			const float v = float(t) / (CGE_CURVE_PRECISION - 1.0f);
			m_curveRGB[t] = 1.0f - v - (fac / SQRT2) + iSign * sqrtf(1.0f - SQRT2 * fac + 2.0f * SQRT2 * v * fac + 0.5f * fac * fac);
		}
		assignCurveArray();
	}

}