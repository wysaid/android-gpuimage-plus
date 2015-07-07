/*
* cgeEyeBrightenFilter.cpp
*
*  Created on: 2014-4-23
*      Author: Wang Yang
*  Description: Eye Pop
*/

#include "cgeEyeBrightenFilter.h"

static CGEConstString s_fshEyeBrighten = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform vec2 samplerSteps;

const int samplerLimit = 5;
const float weight = float(samplerLimit) * 4.0 + 1.0;

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec3 sum = src.rgb;
	vec2 len = 2.0 * samplerSteps;

	for(int i = 1; i <= samplerLimit; ++i)
	{
		sum += texture2D(inputImageTexture, textureCoordinate - len).rgb;
		sum += texture2D(inputImageTexture, vec2(textureCoordinate.x - len.x, textureCoordinate.y + len.y)).rgb;
		sum += texture2D(inputImageTexture, vec2(textureCoordinate.x + len.x, textureCoordinate.y - len.y)).rgb;
		sum += texture2D(inputImageTexture, textureCoordinate + len).rgb;
		len *= 2.0;
	}

	vec3 dst = src.rgb - sum / weight;
	vec3 tmp = sign(dst) * 0.05;
	
	gl_FragColor = vec4(src.rgb + dst + tmp * 3.0 * dst, src.a);
}
);

namespace CGE
{
	bool CGEEyeBrightenFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshEyeBrighten);
	}
}