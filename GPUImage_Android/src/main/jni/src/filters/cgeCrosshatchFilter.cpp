/*
 * cgeCrosshatchFilter.cpp
 *
 *  Created on: 2015-2-1
 *      Author: Wang Yang
 */

#include "cgeCrosshatchFilter.h"

CGEConstString s_fshCrosshatch = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform float crossHatchSpacing;
uniform float lineWidth;

const vec3 W = vec3(0.2125, 0.7154, 0.0721);

void main()
{
	vec4 color = texture2D(inputImageTexture, textureCoordinate);
	float luminance = dot(color.rgb, W);

	vec4 colorToDisplay = vec4(1.0, 1.0, 1.0, color.a);
	if (luminance < 1.00) 
	{
		if (mod(textureCoordinate.x + textureCoordinate.y, crossHatchSpacing) <= lineWidth) 
		{
			colorToDisplay.rgb = vec3(0.0, 0.0, 0.0);
		}
	}
	if (luminance < 0.75) 
	{
		if (mod(textureCoordinate.x - textureCoordinate.y, crossHatchSpacing) <= lineWidth) 
		{
			colorToDisplay.rgb = vec3(0.0, 0.0, 0.0);
		}
	}
	if (luminance < 0.50) 
	{
		if (mod(textureCoordinate.x + textureCoordinate.y - (crossHatchSpacing / 2.0), crossHatchSpacing) <= lineWidth) 
		{
			colorToDisplay.rgb = vec3(0.0, 0.0, 0.0);
		}
	}
	if (luminance < 0.3) 
	{
		if (mod(textureCoordinate.x - textureCoordinate.y - (crossHatchSpacing / 2.0), crossHatchSpacing) <= lineWidth) 
		{
			colorToDisplay.rgb = vec3(0.0, 0.0, 0.0);
		}
	}

	gl_FragColor = colorToDisplay;
}
);


namespace CGE
{
	CGEConstString CGECrosshatchFilter::paramCrosshatchSpacing = "crossHatchSpacing";
	CGEConstString CGECrosshatchFilter::paramLineWidth = "lineWidth";

	bool CGECrosshatchFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshCrosshatch))
		{
			setCrosshatchSpacing(0.03f);
			setLineWidth(0.003f);
			return true;
		}
		return false;
	}

	void CGECrosshatchFilter::setCrosshatchSpacing(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramCrosshatchSpacing, value);
	}

	void CGECrosshatchFilter::setLineWidth(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramLineWidth, value);
	}
}