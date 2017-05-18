/*
* cgeBrightnessAdjust.cpp
*
*  Created on: 2013-12-26
*      Author: Wang Yang
*/

#include "cgeBrightnessAdjust.h"
#include <cmath>

const static char* const s_fshBrightness = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform float intensity;

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

    float fac = 1.0 / intensity;
    float fac2 = 1.0 - fac;
    vec3 tmp1 = fac2 - src.rgb;
    vec3 tmp2 = sqrt(fac2 * fac2 + (4.0 * fac) * src.rgb);
    
    src.rgb = tmp1 + tmp2 * (step(0.0, intensity) * 2.0 - 1.0);
    
	gl_FragColor = src;
}
);

namespace CGE
{
	CGEConstString CGEBrightnessFilter::paramName = "intensity";

	bool CGEBrightnessFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshBrightness))
		{
			return true;
		}
		return false;
	}

	void CGEBrightnessFilter::setIntensity(float value)
	{
		m_program.bind();
        
        //优先保证低精度设备不出现失真
        m_intensity = fabsf(value) < 0.05f ? 0.0f : value;
        m_program.sendUniformf(paramName, m_intensity);
	}
    
    void CGEBrightnessFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
    {
        if(m_intensity == 0.0f)
        {
            handler->swapBufferFBO();
        }
        else
        {
            CGEImageFilterInterface::render2Texture(handler, srcTexture, vertexBufferID);
        }
    }

	//////////////////////////////////////////////////////////////////////////

	bool CGEBrightnessFastFilter::init()
	{
		if(CGEFastAdjustRGBFilter::init())
		{
			assignCurveArray();
			return true;
		}
		return false;
	}

	void CGEBrightnessFastFilter::setIntensity(float value)
	{
		if(fabsf(value) < 0.001) 
		{
			initCurveArray();
			assignCurveArray();
			return;
		}
		std::vector<float>::size_type sz = m_curveRGB.size();
		int iSign = value > 0.0f ? 1 : -1;
		const float SQRT2 = 1.41421f;
		const float fac = SQRT2 / value;
		for(std::vector<float>::size_type t = 0; t != sz; ++t)
		{
			const float v = float(t) / (CGE_CURVE_PRECISION - 1.0f);
			m_curveRGB[t] = 1.0f - v - (fac / SQRT2) + iSign * sqrtf(1.0f - SQRT2 * fac + 2.0f * SQRT2 * v * fac + 0.5f * fac * fac);
		}
		assignCurveArray();
	}

}