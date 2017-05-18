/*
 * cgeEdgeFilter.cpp
 *
 *  Created on: 2013-12-29
 *      Author: Wang Yang
 */

#include "cgeEdgeFilter.h"
#include "cgeMat.h"

const static char* const s_fshEdge = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 samplerSteps;
uniform float stride;
uniform float intensity;
uniform vec2 norm;

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec3 tmpColor = texture2D(inputImageTexture, textureCoordinate + samplerSteps * stride * norm).rgb;
	tmpColor = abs(src.rgb - tmpColor) * 2.0;
	gl_FragColor = vec4(mix(src.rgb, tmpColor, intensity), src.a);
}
);

CGEConstString s_vshEdgeSobel = CGE_SHADER_STRING
(
attribute vec2 vPosition;
varying vec2 textureCoordinate;
varying vec2 coords[8];

uniform vec2 samplerSteps;
uniform float stride;

void main()
{
	gl_Position = vec4(vPosition, 0.0, 1.0);
	textureCoordinate = (vPosition.xy + 1.0) / 2.0;

	coords[0] = textureCoordinate - samplerSteps * stride;
	coords[1] = textureCoordinate + vec2(0.0, -samplerSteps.y) * stride;
	coords[2] = textureCoordinate + vec2(samplerSteps.x, -samplerSteps.y) * stride;

	coords[3] = textureCoordinate - vec2(samplerSteps.x, 0.0) * stride;
	coords[4] = textureCoordinate + vec2(samplerSteps.x, 0.0) * stride;

	coords[5] = textureCoordinate + vec2(-samplerSteps.x, samplerSteps.y) * stride;
	coords[6] = textureCoordinate + vec2(0.0, samplerSteps.y) * stride;
	coords[7] = textureCoordinate + vec2(samplerSteps.x, samplerSteps.y) * stride;

}
);

const static char* const s_fshEdgeSobel = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 samplerSteps;
uniform float stride;
uniform float intensity;

varying vec2 coords[8];

void main()
{
	vec3 colors[8];

	for(int i = 0; i < 8; ++i)
	{
		colors[i] = texture2D(inputImageTexture, coords[i]).rgb;
	}

	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	vec3 h = -colors[0] - 2.0 * colors[1] - colors[2] + colors[5] + 2.0 * colors[6] + colors[7];
	vec3 v = -colors[0] + colors[2] - 2.0 * colors[3] + 2.0 * colors[4] - colors[5] + colors[7];

	gl_FragColor = vec4(mix(src.rgb, sqrt(h * h + v * v), intensity), 1.0);
}
);

namespace CGE
{

	bool CGEEdgeFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshEdge))
		{
			setIntensity(1.0f);
			setStride(2.0f);
			setAngle(M_PI * 0.75f);
			return true;
		}
		return false;
	}

	bool CGEEdgeSobelFilter::init()
	{
		if(initShadersFromString(s_vshEdgeSobel, s_fshEdgeSobel))
		{
			setIntensity(1.0f);
			setStride(2.0f);
			return true;
		}
		return false;
	}
}