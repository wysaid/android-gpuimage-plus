/*
* cgeMinValueFilter.cpp
*
*  Created on: 2015-9-24
*      Author: Wang Yang
* Description: 中值滤波
* 参考: http://www.cnblogs.com/BYTEMAN/archive/2012/07/21/2602181.html
*/

#include "cgeMidValueFilter.h"

static CGEConstString s_vshMidValue3x3 = CGE_SHADER_STRING
(

varying vec2 texCoord[9];
attribute vec2 vPosition;

uniform vec2 samplerSteps;

void main()
{
	gl_Position = vec4(vPosition, 0.0, 1.0);

	vec2 thisCoord = (vPosition.xy + 1.0) / 2.0;

	texCoord[0] = thisCoord - samplerSteps;
	texCoord[1] = thisCoord + vec2(0, -samplerSteps.y);
	texCoord[2] = thisCoord + vec2(samplerSteps.x, -samplerSteps.y);
	texCoord[3] = thisCoord + vec2(-samplerSteps.x, 0.0);
	texCoord[4] = thisCoord;
	texCoord[5] = thisCoord + vec2(samplerSteps.x, 0.0);
	texCoord[6] = thisCoord + vec2(-samplerSteps.x, samplerSteps.y);
	texCoord[7] = thisCoord + vec2(0.0, samplerSteps.y);
	texCoord[8] = thisCoord + samplerSteps;
}
);
// 
// static CGEConstString s_vshMidValue3x3_2 = CGE_SHADER_STRING
// (
// 
// //针对OpenGL ES 可能仅有8个varying的设备进行兼容
// varying vec4 texCoord0;
// varying vec4 texCoord1;
// varying vec4 texCoord2;
// varying vec4 texCoord3;
// varying vec2 texCoord4;
// attribute vec2 vPosition;
// 
// uniform vec2 samplerSteps;
// 
// void main()
// {
// 	gl_Position = vec4(vPosition, 0.0, 1.0);
// 
// 	vec2 thisCoord = (vPosition.xy + 1.0) / 2.0;
// 
// 	texCoord0.xy = thisCoord - samplerSteps;
// 	texCoord1.xy = thisCoord + vec2(0, -samplerSteps.y);
// 	texCoord2.xy = thisCoord + vec2(samplerSteps.x, -samplerSteps.y);
// 	texCoord3.xy = thisCoord + vec2(-samplerSteps.x, 0.0);
// 	texCoord4 = thisCoord;
// 	texCoord0.zw = thisCoord + vec2(samplerSteps.x, 0.0);
// 	texCoord1.zw = thisCoord + vec2(-samplerSteps.x, samplerSteps.y);
// 	texCoord2.zw = thisCoord + vec2(0.0, samplerSteps.y);
// 	texCoord3.zw = thisCoord + samplerSteps;
// }
// );

static CGEConstString s_fshMidValue3x3 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 texCoord[9];

uniform sampler2D inputImageTexture;

float lum(vec3 value)
{
	return dot(value, vec3(0.299, 0.587, 0.114));
}

vec3 max2(vec3 c0, vec3 c1)
{
	return mix(newValue, originValue, step(lum(newValue), lum(originValue)));
}

vec3 max3(vec3 c0, vec3 c1, vec3 c2)
{
	return max2(max2(c0, c1), c2);
}

vec3 min2(vec3 c0, vec3 c1)
{
	return mix(newValue, originValue, step(lum(originValue), lum(newValue)));
}

vec3 min3(vec3 c0, vec3 c1, vec3 c2)
{
	return min2(min2(c0, c1), c2);
}

vec3 mid3(vec3 c0, vec3 c1, vec3 c2)
{
	float f0 = lum(c0), f1 = lum(c1), f2 = lum(c2);
	if(f0 > f1)
	{
		if(f0 < f2)
			return c0; //f2 > f0 > f1
		else if(f1 > f2)
			return c1; // f0 > f1 > f2
		else
			return c2; // f0 > f2 > f1
	}
	else
	{
		if(f1 < f2)
			return c1; //f0 < f1 < f2
		else if(f0 > f2)
			return c0; //f2 < f0 < f1
		else
			return c2; //f0 < f2 < f1
	}
}

void main()
{
	vec3 maxGroup[3];
	vec3 midGroup[3];
	vec3 minGroup[3];

	for(int i = 0; i < 3; ++i)
	{
		vec3 a = texture2D(inputImageTexture, texCoord[i * 3]);
		vec3 b = texture2D(inputImageTexture, texCoord[i * 3 + 1]);
		vec3 c = texture2D(inputImageTexture, texCoord[i * 3 + 2]);

		maxGroup[i] = max3(a, b, c);
		midGroup[i] = mid3(a, b, c);
		minGroup[i] = min3(a, b, c);
	}

	maxGroup[0] = min3(maxGroup[0], maxGroup[1], maxGroup[2]);
	midGroup[0] = mid3(midGroup[0], midGroup[1], midGroup[2]);
	minGroup[0] = min3(minGroup[0], minGroup[1], minGroup[2]);

	vec3 result = mid3(maxGroup[0], midGroup[0], minGroup[0]);

	gl_FragColor = vec4(result, 1.0);
});
// 
// static CGEConstString s_fshMidValue3x3_2 = CGE_SHADER_STRING_PRECISION_M
// (
// varying vec4 texCoord0;
// varying vec4 texCoord1;
// varying vec4 texCoord2;
// varying vec4 texCoord3;
// varying vec2 texCoord4;
// 
// uniform sampler2D inputImageTexture;
// 
// %s\n
// 
// void main()
// {
// 	vec4 vMin;
// 
// 	{
// 		vec4 vTemp;
// 
// 		vMin = texture2D(inputImageTexture, texCoord0.xy);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord1.xy);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord2.xy);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord3.xy);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord4);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord0.zw);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord1.zw);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord2.zw);
// 		vMin = getValue(vTemp, vMin);
// 
// 		vTemp = texture2D(inputImageTexture, texCoord3.zw);
// 		vMin = getValue(vTemp, vMin);
// 	}
// 
// 	gl_FragColor = vMin;
// });

namespace CGE
{
	bool CGEMidValueFilter3x3::init()
	{
		if(!initShadersFromString(s_vshMidValue3x3, s_fshMidValue3x3))
			return false;

		initLocations();
		return true;
	}

}