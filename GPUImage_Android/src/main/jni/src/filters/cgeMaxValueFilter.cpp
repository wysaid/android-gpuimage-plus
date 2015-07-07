/*
* cgeMaxValueFilter.cpp
*
*  Created on: 2015-3-20
*      Author: Wang Yang
* Description: 最大值滤波
*/

#include "cgeMaxValueFilter.h"

static CGEConstString s_maxValueFunc = CGE_SHADER_STRING
(
float lum(vec4 value)
{
	return dot(value.rgb, vec3(0.299, 0.587, 0.114));
}
vec4 getValue(vec4 newValue, vec4 originValue)
{
	return mix(newValue, originValue, step(lum(newValue), lum(originValue)));
});

namespace CGE
{
	const char* CGEMaxValueFilter3x3::getShaderCompFunc()
	{
		return s_maxValueFunc;
	}

	const char* CGEMaxValueFilter3x3Plus::getShaderCompFunc()
	{
		return s_maxValueFunc;
	}
}