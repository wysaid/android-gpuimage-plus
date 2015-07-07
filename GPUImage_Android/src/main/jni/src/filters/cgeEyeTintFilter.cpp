/*
* cgeEyeTintFilter.cpp
*
*  Created on: 2014-4-23
*      Author: Wang Yang
*/

#include "cgeEyeTintFilter.h"

static CGEConstString s_fshEyeTint = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec3 tintColor;

void main()
{
	float tintLum = dot(tintColor, vec3(0.299, 0.587, 0.114));
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	float srcLum = dot(src.rgb, vec3(0.299, 0.587, 0.114));
	vec3 tint = tintColor + srcLum - tintLum;
	float mn = min(tint.r, min(tint.g, tint.b));
	float mx = max(tint.r, max(tint.g, tint.b));
	if(mn < 0.0)
	{
		tint = srcLum + (tint - srcLum) * srcLum / (srcLum - mn);
	}
	if(mx > 1.0)
	{
		tint = srcLum + (tint - srcLum) * (1.0 - srcLum) / (mx - srcLum);
	}
	gl_FragColor = vec4(tint, src.a);
}
);

namespace CGE
{
	CGEConstString CGEEyeTintFilter::paramTintName = "tintColor";

	bool CGEEyeTintFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshEyeTint))
		{
			return true;
		}
		return false;
	}

	void CGEEyeTintFilter::setTintColor(float r, float g, float b)
	{
		m_program.bind();
		m_program.sendUniformf(paramTintName, r, g, b);
	}

}