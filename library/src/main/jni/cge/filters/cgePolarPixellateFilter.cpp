/*
 * cgePolarPixellateFilter.cpp
 *
 *  Created on: 2015-2-1
 *      Author: Wang Yang
 */

#include "cgePolarPixellateFilter.h"

static CGEConstString s_fshPolarPixellate = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform vec2 center;
uniform vec2 pixelSize;

void main()
{
	vec2 normCoord = 2.0 * textureCoordinate - 1.0;
	vec2 normCenter = 2.0 * center - 1.0;

	normCoord -= normCenter;

	float r = length(normCoord); // to polar coords 
	float phi = atan(normCoord.y, normCoord.x); // to polar coords 

	r = r - mod(r, pixelSize.x) + 0.03;
	phi = phi - mod(phi, pixelSize.y);

	normCoord.x = r * cos(phi);
	normCoord.y = r * sin(phi);

	normCoord += normCenter;

	vec2 textureCoordinateToUse = normCoord / 2.0 + 0.5;

	gl_FragColor = texture2D(inputImageTexture, textureCoordinateToUse);
});

namespace CGE
{
	CGEConstString CGEPolarPixellateFilter::paramCenter = "center";
	CGEConstString CGEPolarPixellateFilter::paramPixelSize = "pixelSize";

	bool CGEPolarPixellateFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshPolarPixellate))
		{
			setCenter(0.5f, 0.5f);
			setPixelSize(0.05f, 0.05f);
			return true;
		}
		return false;
	}

	void CGEPolarPixellateFilter::setCenter(float x, float y)
	{
		m_program.bind();
		m_program.sendUniformf(paramCenter, x, y);
	}

	void CGEPolarPixellateFilter::setPixelSize(float x, float y)
	{
		m_program.bind();
		m_program.sendUniformf(paramPixelSize, x, y);
	}
}