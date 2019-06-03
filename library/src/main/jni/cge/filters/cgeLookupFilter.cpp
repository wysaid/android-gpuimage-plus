/*
* cgeLookupFilter.cpp
*
*  Created on: 2016-7-4
*      Author: Wang Yang
* Description: 全图LUT滤镜
*/

#include "cgeLookupFilter.h"

static CGEConstString s_fsh = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D lookupTexture;

const float stepDis = 1.0 / 8.0;
const float perPixel = 1.0 / 512.0;
const float halfPixel = 0.5 / 512.0;

void main()
{
	vec4 color = texture2D(inputImageTexture, textureCoordinate);

	float blue = color.b * 63.0;
	vec2 coord1;
	coord1.y = floor(floor(blue) / 8.0);
	coord1.x = floor(blue) - (coord1.y * 8.0);
	
    vec2 coord2;
    coord2.y = floor(ceil(blue) / 8.0);
    coord2.x = ceil(blue) - (coord2.y * 8.0);
    
    vec2 stepsCalc = halfPixel + (stepDis - perPixel) * color.xy;
    
	coord1 = coord1 * stepDis + stepsCalc;
    coord2 = coord2 * stepDis + stepsCalc;
    
    vec3 lutColor1 = texture2D(lookupTexture, coord1).rgb;
    vec3 lutColor2 = texture2D(lookupTexture, coord2).rgb;
    
	gl_FragColor.rgb = mix(lutColor1, lutColor2, fract(blue));
	gl_FragColor.a = color.a;
}
);

namespace CGE
{
	CGELookupFilter::CGELookupFilter() : m_lookupTexture(0)
	{

	}

	CGELookupFilter::~CGELookupFilter()
	{
		glDeleteTextures(1, &m_lookupTexture);
	}

	bool CGELookupFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fsh))
		{
			m_program.bind();
			m_program.sendUniformi("lookupTexture", 1);
			return true;
		}
		return false;
	}

	void CGELookupFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_lookupTexture);
		CGEImageFilterInterface::render2Texture(handler, srcTexture, vertexBufferID);
	}

}