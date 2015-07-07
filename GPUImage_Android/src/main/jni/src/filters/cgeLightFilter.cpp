/*
* cgeLightFilter.cpp
*
*  Created on: 2014-6-3
*      Author: liu hailong
*/

#include "cgeLightFilter.h"

const char* s_fshLight = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 centrePosition;
uniform vec2 samplerSteps;
uniform vec3 color;

void main()
{
	vec4 currentColor = texture2D(inputImageTexture, textureCoordinate);
	//外面传进来的位置为真实的位置所以要通过samplerSteps来将虚拟坐标变成真实的坐标
	vec2 realPosition = textureCoordinate / samplerSteps;
	float dist = distance(realPosition , centrePosition);
	//gl_FragColor = vec4(currentColor + 100.0 * color / dist, 1.0);
	gl_FragColor = vec4(currentColor.rgb + color * exp(- dist / 500.0), currentColor.a);
}
);


namespace CGE
{
	CGEConstString CGELightFilter::paramColorName = "color";
	CGEConstString CGELightFilter::paramCentrePositionName = "centrePosition";

	bool CGELightFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshLight))
		{
			setCentrePosition(100,100);
			setColor(1.0f,1.0f,1.0f);
			return true;
		}
		return false;

	}

	void CGELightFilter::setCentrePosition(float x,float y)
	{
		m_program.bind();
		m_program.sendUniformf(paramCentrePositionName,x,y);
	}

	void CGELightFilter::setColor(float r,float g,float b)
	{
		m_program.bind();
		m_program.sendUniformf(paramColorName,r,g,b);
	}

}