/*
* cgeMosaicBlur.cpp
*
*  Created on: 2014-4-10
*      Author: Wang Yang
*/

#include "cgeMosaicBlurFilter.h"

static CGEConstString s_fshMosaic = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 samplerSteps;
uniform float blurPixels;

void main()
{
	vec2 coord = floor(textureCoordinate / samplerSteps / blurPixels) * samplerSteps * blurPixels;
	gl_FragColor = texture2D(inputImageTexture, coord + samplerSteps * 0.5);
}
);

namespace CGE
{
	CGEConstString CGEMosaicBlurFilter::paramBlurPixelsName = "blurPixels";

	bool CGEMosaicBlurFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshMosaic))
		{
			setBlurPixels(1.0f);
			return true;
		}
		return false;
	}

	void CGEMosaicBlurFilter::setBlurPixels(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramBlurPixelsName, value);
	}

}