/*
 * cgeCardEffects.cpp
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#include "cgeCardEffects.h"

#define BORDERBIND_ID 0

static CGEConstString s_fshBorderCard = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D borderTexture;

uniform float originAspectRatio;

//The relative position about the card-rects
uniform vec2 start;
uniform vec2 range;
uniform vec2 end;
uniform float align; 

void main(void)
{
	vec2 scale;
	vec2 stepsStart;
	
	if(textureCoordinate.x >= start.x && textureCoordinate.x <= end.x
		&& textureCoordinate.y >= start.y && textureCoordinate.y <= end.y)
	{
		vec2 coord = textureCoordinate;
		if(originAspectRatio > 1.0)
		{
			coord /= vec2(originAspectRatio, 1.0);
			stepsStart = vec2((1.0 - 1.0 / originAspectRatio)/2.0, 0.0);
			coord = (coord+align*stepsStart*range-vec2(start.x / originAspectRatio, start.y))/range;
		}
		else
		{
			coord *= vec2(1.0, originAspectRatio);
			stepsStart = vec2(0.0, (1.0 - originAspectRatio)/2.0);
			coord = (coord+align*stepsStart*range-vec2(start.x, start.y * originAspectRatio))/range;
		}
		gl_FragColor = texture2D(inputImageTexture, coord);
	}
	else
	{
		gl_FragColor = texture2D(borderTexture, textureCoordinate);
	}
}
);

namespace CGE
{

	CGEConstString CGEBorderCardFilter::paramTextureName = "borderTexture";
	CGEConstString CGEBorderCardFilter::paramOriginAspectRatioName = "originAspectRatio";

	CGEConstString CGEBorderCardFilter::paramZoneStartName = "start";
	CGEConstString CGEBorderCardFilter::paramZoneRangeName = "range";
	CGEConstString CGEBorderCardFilter::paramZoneEndName = "end";
	CGEConstString CGEBorderCardFilter::paramAlignName = "align";

	bool CGEBorderCardFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshBorderCard))
		{
			UniformParameters* param = new UniformParameters;
			param->requireStepsRatio(paramOriginAspectRatioName);
			setAlign(1.0f);
			setPictureZone(0.3f, 0.3f, 0.4f, 0.4f);
			setAdditionalUniformParameter(param);
			param->pushSampler2D(paramTextureName, &m_texture, BORDERBIND_ID);
			return true;
		}
		return false;
	}

	void CGEBorderCardFilter::setPictureZone(float x, float y, float w, float h)
	{
		m_program.bind();
		m_program.sendUniformf(paramZoneStartName, x, y);
		m_program.sendUniformf(paramZoneRangeName, w, h);
		m_program.sendUniformf(paramZoneEndName, x + w, y + h);
	}

	void CGEBorderCardFilter::setAlign(float align)
	{
		m_program.bind();
		m_program.sendUniformf(paramAlignName, align);
	}

}