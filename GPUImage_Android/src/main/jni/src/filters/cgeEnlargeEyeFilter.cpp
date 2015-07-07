/*
* cgeEnlargeEyeFilter.cpp
*
*  Created on: 2014-4-23
*      Author: Wang Yang
*/

#include "cgeEnlargeEyeFilter.h"

static CGEConstString s_fshEnlargeEye = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform vec2 samplerSteps;
uniform vec2 centralPos; //Real position for pixels.
uniform float radius; //Range: [1.0, 1000.0]
uniform float intensity; //Range: [-1.0, 1.0]

void main()
{
	vec2 realPosition = textureCoordinate / samplerSteps;
	vec2 dis = centralPos - realPosition;
	float len = length(dis);
	if(len > radius)
	{
		gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
		return;
	}
	
	float offset = 1.0 - len / radius;
	//smoothstep:
	offset = offset * offset * (3.0 - 2.0 * offset);
	vec2 step = intensity * dis * samplerSteps * offset;
	gl_FragColor = texture2D(inputImageTexture, textureCoordinate + step);
}
);

namespace CGE
{
	CGEConstString CGEEnlargeEyeFilter::paramRadiusName = "radius";
	CGEConstString CGEEnlargeEyeFilter::paramIntensityName = "intensity";
	CGEConstString CGEEnlargeEyeFilter::paramCentralPosName = "centralPos";

	bool CGEEnlargeEyeFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshEnlargeEye))
		{
			setCentralPosition(100.0f, 100.0f);
			setEnlargeRadius(100.0f);
			setIntensity(1.0f);
			return true;
		}
		return false;
	}

	void CGEEnlargeEyeFilter::setEnlargeRadius(float radius)
	{
		m_program.bind();
		m_program.sendUniformf(paramRadiusName, radius);
	}

	void CGEEnlargeEyeFilter::setIntensity(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramIntensityName, value);
	}

	void CGEEnlargeEyeFilter::setCentralPosition(float x, float y)
	{
		m_program.bind();
		m_program.sendUniformf(paramCentralPosName, x, y);
	}

}