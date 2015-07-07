/*
 * cgePolkaDotFilter.cpp
 *
 *  Created on: 2015-2-1
 *      Author: Wang Yang
 */

#include "cgePolkaDotFilter.h"

static CGEConstString s_fshPolkaDot = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform vec2 dotPercent;
uniform float aspectRatio;
uniform float dotScaling;

void main()
{
	vec2 samplePos = textureCoordinate - mod(textureCoordinate, dotPercent) + 0.5 * dotPercent;
	vec2 coordToUse = vec2(textureCoordinate.x, (textureCoordinate.y - 0.5) * aspectRatio + 0.5);
	vec2 adjustedPos = vec2(samplePos.x, (samplePos.y - 0.5) * aspectRatio + 0.5);
	float dis = distance(coordToUse, adjustedPos);
	float checkForPresenceWithinDot = step(dis, (dotPercent.x * 0.5) * dotScaling);
	vec4 color = texture2D(inputImageTexture, samplePos);
	gl_FragColor = vec4(color.rgb * checkForPresenceWithinDot, color.a);

});

namespace CGE
{
	CGEConstString CGEPolkaDotFilter::paramDotScaling = "dotScaling";

	bool CGEPolkaDotFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshPolkaDot))
		{
			setDotScaling(0.9f);
			setDotSize(1.0f);
			return true;
		}
		return false;
	}

	void CGEPolkaDotFilter::setDotScaling(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramDotScaling, value);
	}
}