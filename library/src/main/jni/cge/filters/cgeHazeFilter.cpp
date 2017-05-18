/*
 * cgeHazeFilter.cpp
 *
 *  Created on: 2015-2-1
 *      Author: Wang Yang
 */

#include "cgeHazeFilter.h"

static CGEConstString s_fshHaze = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;

uniform sampler2D inputImageTexture;

uniform float dis;
uniform float slope;
uniform vec3 hazeColor;

void main()
{
	float d = textureCoordinate.y * slope + dis;

	vec4 c = texture2D(inputImageTexture, textureCoordinate);

	c.rgb = (c.rgb - d * hazeColor.rgb) / (1.0 -d);

	gl_FragColor = c;
});

namespace CGE
{
	CGEConstString CGEHazeFilter::paramDistance = "dis";
	CGEConstString CGEHazeFilter::paramSlope = "slope";
	CGEConstString CGEHazeFilter::paramHazeColor = "hazeColor";

	bool CGEHazeFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshHaze))
		{
			setDistance(0.2f);
			setHazeColor(1.0f, 1.0f, 1.0f);
			return true;
		}
		return false;
	}

	void CGEHazeFilter::setDistance(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramDistance, value);
	}

	void CGEHazeFilter::setSlope(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramSlope, value);
	}

	void CGEHazeFilter::setHazeColor(float r, float g, float b)
	{
		m_program.bind();
		m_program.sendUniformf(paramHazeColor, r, g, b);
	}
}