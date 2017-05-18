/*
 * cgeEmbossFilter.cpp
 *
 *  Created on: 2013-12-27
 *      Author: Wang Yang
 */

#include "cgeEmbossFilter.h"
#include "cgeMat.h"

const static char* const s_fshEmboss = CGE_SHADER_STRING_PRECISION_M
(
uniform sampler2D inputImageTexture;
varying vec2 textureCoordinate;
uniform vec2 samplerSteps;
uniform float stride;
uniform float intensity;
uniform vec2 norm;

void main() {
  vec4 src = texture2D(inputImageTexture, textureCoordinate);
  vec3 tmp = texture2D(inputImageTexture, textureCoordinate + samplerSteps * stride * norm).rgb - src.rgb + 0.5;
  float f = (tmp.r + tmp.g + tmp.b) / 3.0;
  gl_FragColor = vec4(mix(src.rgb, vec3(f, f, f), intensity), src.a);
}
);

namespace CGE
{
	CGEConstString CGEEmbossFilter::paramIntensity = "intensity";
	CGEConstString CGEEmbossFilter::paramStride = "stride";
	CGEConstString CGEEmbossFilter::paramNorm = "norm";

	bool CGEEmbossFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshEmboss))
		{
			setIntensity(1.0f);
			setStride(2.0f);
			setAngle(M_PI * 0.75f);
			return true;
		}
		return false;
	}

	void CGEEmbossFilter::setIntensity(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramIntensity, value);
	}

	void CGEEmbossFilter::setStride(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramStride, value);
	}

	void CGEEmbossFilter::setAngle(float value)
	{
		Vec2f v = Mat2::makeRotation(value) * Vec2f(1.0f, 0.0f);
		m_program.bind();
		m_program.sendUniformf(paramNorm, v[0], v[1]);
	}
}