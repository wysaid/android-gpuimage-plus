/*
* cgeMinValueFilter.cpp
*
*  Created on: 2015-3-20
*      Author: Wang Yang
*/

#include "cgeMinValueFilter.h"

static CGEConstString s_minValueFunc = CGE_SHADER_STRING
(

float lum(vec4 value)
{
	return dot(value.rgb, vec3(0.299, 0.587, 0.114));
}
vec4 getValue(vec4 newValue, vec4 originValue)
{
	return mix(newValue, originValue, step(lum(originValue), lum(newValue)));
});

static CGEConstString s_vshMinValue3x3 = CGE_SHADER_STRING
(

//针对OpenGL ES 可能仅有8个varying的设备进行优化
varying vec4 texCoord0;
varying vec4 texCoord1;
varying vec4 texCoord2;
varying vec4 texCoord3;
varying vec2 texCoord4;
attribute vec2 vPosition;

uniform vec2 samplerSteps;

void main()
{
	gl_Position = vec4(vPosition, 0.0, 1.0);

	vec2 thisCoord = (vPosition.xy + 1.0) / 2.0;

	texCoord0.xy = thisCoord - samplerSteps;
	texCoord1.xy = thisCoord + vec2(0, -samplerSteps.y);
	texCoord2.xy = thisCoord + vec2(samplerSteps.x, -samplerSteps.y);
	texCoord3.xy = thisCoord + vec2(-samplerSteps.x, 0.0);
	texCoord4 = thisCoord;
	texCoord0.zw = thisCoord + vec2(samplerSteps.x, 0.0);
	texCoord1.zw = thisCoord + vec2(-samplerSteps.x, samplerSteps.y);
	texCoord2.zw = thisCoord + vec2(0.0, samplerSteps.y);
	texCoord3.zw = thisCoord + samplerSteps;
}
);

static CGEConstString s_fshMinValue3x3 = CGE_SHADER_STRING_PRECISION_M
(
varying vec4 texCoord0;
varying vec4 texCoord1;
varying vec4 texCoord2;
varying vec4 texCoord3;
varying vec2 texCoord4;

uniform sampler2D inputImageTexture;

%s\n

void main()
{
	vec4 vMin;

	{
		vec4 vTemp;

		vMin = texture2D(inputImageTexture, texCoord0.xy);

		vTemp = texture2D(inputImageTexture, texCoord1.xy);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord2.xy);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord3.xy);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord4);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord0.zw);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord1.zw);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord2.zw);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord3.zw);
		vMin = getValue(vTemp, vMin);
	}

	gl_FragColor = vMin;
});

namespace CGE
{
	CGEConstString CGEMinValueFilter3x3::paramSamplerStepsName = "samplerSteps";

	bool CGEMinValueFilter3x3::init()
	{
		char buffer[4096];

		sprintf(buffer, s_fshMinValue3x3, getShaderCompFunc());

		if(initShadersFromString(s_vshMinValue3x3, buffer))
		{
			if(m_uniformParam == NULL)
				m_uniformParam = new UniformParameters;
			m_uniformParam->requireStepsFactor(paramSamplerStepsName);
			return true;
		}
		return false;
	}

	const char* CGEMinValueFilter3x3::getShaderCompFunc()
	{
		return s_minValueFunc;
	}
}

//////////////////////////////////////////////////////////////////////////

static CGEConstString s_vshMinValue3x3Plus = CGE_SHADER_STRING
(
varying vec4 texCoord0;
varying vec4 texCoord1;
varying vec4 texCoord2;
varying vec4 texCoord3;
varying vec4 texCoord4;
varying vec4 texCoord5;
varying vec2 texCoord6;

attribute vec2 vPosition;

uniform vec2 samplerSteps;

void main()
{
	gl_Position = vec4(vPosition, 0.0, 1.0);

	vec2 thisCoord = (vPosition.xy + 1.0) / 2.0;

	texCoord0.xy = thisCoord + vec2(0, -samplerSteps.y * 2.0);
	texCoord1.xy = thisCoord - samplerSteps;
	texCoord2.xy = thisCoord + vec2(0, -samplerSteps.y);
	texCoord3.xy = thisCoord + vec2(samplerSteps.x, -samplerSteps.y);
	texCoord4.xy = thisCoord + vec2(-samplerSteps.x * 2.0, 0.0);
	texCoord5.xy = thisCoord + vec2(-samplerSteps.x, 0.0);
	texCoord6 = thisCoord;
	texCoord0.zw = thisCoord + vec2(samplerSteps.x, 0.0);
	texCoord1.zw = thisCoord + vec2(samplerSteps.x * 2.0, 0.0);
	texCoord2.zw = thisCoord + vec2(-samplerSteps.x, samplerSteps.y);
	texCoord3.zw = thisCoord + vec2(0.0, samplerSteps.y);
	texCoord4.zw = thisCoord + samplerSteps;
	texCoord5.zw = thisCoord + vec2(0.0, 2.0* samplerSteps.y);
}
);

static CGEConstString s_fshSketch3x3Plus = CGE_SHADER_STRING_PRECISION_M
(
varying vec4 texCoord0;
varying vec4 texCoord1;
varying vec4 texCoord2;
varying vec4 texCoord3;
varying vec4 texCoord4;
varying vec4 texCoord5;
varying vec2 texCoord6;

uniform sampler2D inputImageTexture;

%s\n

void main()
{
	vec4 vMin;

	{
		vec4 vTemp;

		vMin = texture2D(inputImageTexture, texCoord0.xy);

		vTemp = texture2D(inputImageTexture, texCoord1.xy);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord2.xy);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord3.xy);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord4.xy);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord5.xy);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord6);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord0.zw);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord1.zw);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord2.zw);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord3.zw);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord4.zw);
		vMin = getValue(vTemp, vMin);

		vTemp = texture2D(inputImageTexture, texCoord5.zw);
		vMin = getValue(vTemp, vMin);
	}
	

	gl_FragColor = vMin;
});

// static CGEConstString s_fshMinValue3x3Plus2 = CGE_SHADER_STRING_PRECISION_M
// (
// varying vec2 textureCoordinate;
// uniform sampler2D inputImageTexture;
// 
// uniform vec2 samplerSteps;
// 
// %s\n
// 
// void main()
// {
// 	vec4 vMin;
// 
// 	vec2 texCoord[13];
// 
// 	texCoord[0] = textureCoordinate + vec2(0, -samplerSteps.y * 2.0);
// 	texCoord[1] = textureCoordinate - samplerSteps;
// 	texCoord[2] = textureCoordinate + vec2(0, -samplerSteps.y);
// 	texCoord[3] = textureCoordinate + vec2(samplerSteps.x, -samplerSteps.y);
// 	texCoord[4] = textureCoordinate + vec2(-samplerSteps.x * 2.0, 0.0);
// 	texCoord[5] = textureCoordinate + vec2(-samplerSteps.x, 0.0);
// 	texCoord[6] = textureCoordinate;
// 	texCoord[7] = textureCoordinate + vec2(samplerSteps.x, 0.0);
// 	texCoord[8] = textureCoordinate + vec2(samplerSteps.x * 2.0, 0.0);
// 	texCoord[9] = textureCoordinate + vec2(-samplerSteps.x, samplerSteps.y);
// 	texCoord[10] = textureCoordinate + vec2(0.0, samplerSteps.y);
// 	texCoord[11] = textureCoordinate + samplerSteps;
// 	texCoord[12] = textureCoordinate + vec2(0.0, 2.0* samplerSteps.y);
// 
// 	{
// 		vec4 vTemp;
// 
// 		vMin = texture2D(inputImageTexture, texCoord[0]);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[1]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[2]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[3]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[4]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[5]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[6]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[7]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[8]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[9]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[10]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[11]);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord[12]);
// 		vMin = getValue(vTemp, vMin);
// 	}
// 	
// 	gl_FragColor = vMin;
// });
// 
namespace CGE
{
	bool CGEMinValueFilter3x3Plus::init()
	{
// 		GLint iParam;
// #ifdef GL_MAX_VARYING_COMPONENTS
// 		glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &iParam);
// #else
// 		glGetIntegerv(GL_MAX_VARYING_VECTORS, &iParam);
// #endif

		const char *vsh, *fsh;

//		if(iParam > 13)
		{
			vsh = s_vshMinValue3x3Plus;
			fsh = s_fshSketch3x3Plus;
		}
// 		else
// 		{
// 			vsh = g_vshDefaultWithoutTexCoord;
// 			fsh = s_fshMinValue3x3Plus2;
// 		}

		char buffer[4096];

		sprintf(buffer, fsh, getShaderCompFunc());

		if(initShadersFromString(vsh, buffer))
		{
			if(m_uniformParam == NULL)
				m_uniformParam = new UniformParameters;
			m_uniformParam->requireStepsFactor(paramSamplerStepsName);
			return true;
		}
		return false;
	}
}
