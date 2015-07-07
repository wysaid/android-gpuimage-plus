/*
* cgeWhiteBalanceAdjust.cpp
*
*  Created on: 2013-12-26
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeWhiteBalanceAdjust.h"

const static char* const s_fshWhiteBalance = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform float temperature;
uniform float tint;

vec3 whiteBalance(vec3 src, float temp, float tint)
{
	temp = clamp(temp, 1200.0, 12000.0);
	tint = clamp(tint, 0.02, 5.0);
	float xD;
	temp /= 1000.0;

	if(temp < 4.0)
		xD = 0.27475 / (temp * temp * temp) - 0.98598 / (temp * temp) + 1.17444 / temp + 0.145986;
	else if(temp < 7.0)
		xD = -4.6070 / (temp * temp * temp) + 2.9678 / (temp * temp) + 0.09911 / temp + 0.244063;
	else xD = -2.0064 / (temp * temp * temp) + 1.9018 / (temp * temp) + 0.24748 / temp + 0.237040;

	float yD = -3.0 * xD * xD + 2.87 * xD - 0.275;

	float X = xD / yD;
	float Z = (1.0 - xD - yD) / yD;

	vec3 color;

	color.r = X * 3.24074 - 1.53726 - Z * 0.498571;
	color.g = -X * 0.969258 + 1.87599 + Z * 0.0415557;
	color.b = X * 0.0556352 - 0.203996 + Z * 1.05707;

	color.g /= tint;

	color /= max(max(color.r, color.g), color.b);
	color = 1.0 / color;
	color /= color.r * 0.299 + color.g * 0.587 + color.b * 0.114;

	return src * color;
}

vec3 map_color(vec3 src, float lum)
{
	vec3 h = src - lum;
	if(src.r > 1.0)
	{
		float tmp = 1.0 - lum;
		h.g = h.g * tmp / h.r;
		h.b = h.b * tmp / h.r;
		h.r = tmp;
	}
	float t3r = h.b + lum;
	if(t3r < -0.00003)
	{
		src.rg = lum - h.rg * lum / h.b;
		src.b = 0.0;
	}
	else
	{
		src.rg = lum + h.rg;
		src.b = t3r;
	}
	return src;
}

vec3 dispatch(vec3 src)
{
	float lum = dot(src, vec3(0.299, 0.587, 0.114));
	if(src.g > src.b)
	{
		if(src.r > src.g)
		{
			src = map_color(src, lum);
		}
		else if(src.r > src.b)
		{
			src.grb = map_color(src.grb, lum);
		}
		else
		{
			src.gbr = map_color(src.gbr, lum);
		}
	}
	else
	{
		if(src.g > src.r)
		{
			src.bgr = map_color(src.bgr, lum);
		}
		else if(src.b > src.r)
		{
			src.brg = map_color(src.brg, lum);
		}
		else
		{
			src.rbg = map_color(src.rbg, lum);
		}
	}
	return src;
}

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	src.rgb = dispatch(whiteBalance(src.rgb, temperature, tint));
	gl_FragColor = src;
}
);

const static char* const s_fshWhiteBalanceOptimized = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec3 balance;

vec3 map_color(vec3 src, float lum)
{
	vec3 h = src - lum;
	if(src.r > 1.0)
	{
		float tmp = 1.0 - lum;
		h.g = h.g * tmp / h.r;
		h.b = h.b * tmp / h.r;
		h.r = tmp;
	}
	float t3r = h.b + lum;
	if(t3r < -0.00003)
	{
		src.rg = lum - h.rg * lum / h.b;
		src.b = 0.0;
	}
	else
	{
		src.rg = lum + h.rg;
		src.b = t3r;
	}
	return src;
}

vec3 dispatch(vec3 src)
{
	float lum = dot(src, vec3(0.299, 0.587, 0.114));
	if(src.g > src.b)
	{
		if(src.r > src.g)
		{
			src = map_color(src, lum);
		}
		else if(src.r > src.b)
		{
			src.grb = map_color(src.grb, lum);
		}
		else
		{
			src.gbr = map_color(src.gbr, lum);
		}
	}
	else
	{
		if(src.g > src.r)
		{
			src.bgr = map_color(src.bgr, lum);
		}
		else if(src.b > src.r)
		{
			src.brg = map_color(src.brg, lum);
		}
		else
		{
			src.rbg = map_color(src.rbg, lum);
		}
	}
	return src;
}

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	src.rgb = dispatch(src.rgb * balance);
	gl_FragColor = src;
}

);

/*
const static char* const s_fshWhiteBalance_Fast = SHADER_STRING_PRECISION_H
(
uniform sampler2D inputImageTexture;
varying vec2 textureCoordinate;

uniform float temperature;
uniform float tint;

const vec3 warmFilter = vec3(0.93, 0.54, 0.0);

const mat3 RGBtoYIQ = mat3(0.299, 0.587, 0.114, 0.596, -0.274, -0.322, 0.212, -0.523, 0.311);
const mat3 YIQtoRGB = mat3(1.0, 0.956, 0.621, 1.0, -0.272, -0.647, 1.0, -1.105, 1.702);

void main()
{
	vec4 source = texture2D(inputImageTexture, textureCoordinate);

	vec3 yiq = RGBtoYIQ * source.rgb; //adjusting tint
	yiq.b = clamp(yiq.b + tint*0.5226*0.1, -0.5226, 0.5226);
	vec3 rgb = YIQtoRGB * yiq;

	vec3 processed = vec3(
		(rgb.r < 0.5 ? (2.0 * rgb.r * warmFilter.r) : (1.0 - 2.0 * (1.0 - rgb.r) * (1.0 - warmFilter.r))), //adjusting temperature
		(rgb.g < 0.5 ? (2.0 * rgb.g * warmFilter.g) : (1.0 - 2.0 * (1.0 - rgb.g) * (1.0 - warmFilter.g))), 
		(rgb.b < 0.5 ? (2.0 * rgb.b * warmFilter.b) : (1.0 - 2.0 * (1.0 - rgb.b) * (1.0 - warmFilter.b))));

	gl_FragColor = vec4(mix(rgb, processed, temperature), source.a);
}
);
*/

namespace CGE
{
	CGEConstString CGEWhiteBalanceFilter::paramTemperatureName = "temperature";
	CGEConstString CGEWhiteBalanceFilter::paramTintName = "tint";

	bool CGEWhiteBalanceFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshWhiteBalance))
		{
			setTemperature(0.0f);
			setTint(1.0f);
			return true;
		}
		return false;
	}

	void CGEWhiteBalanceFilter::setTemperature(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramTemperatureName, value * 5400.0f + 6500.0f);
	}

	void CGEWhiteBalanceFilter::setTint(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramTintName, value);
	}

	//////////////////////////////////////////////////////////////////////////

	CGEConstString CGEWhiteBalanceFastFilter::paramBalanceName = "balance";

	bool CGEWhiteBalanceFastFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshWhiteBalanceOptimized))
		{
			setTempAndTint(0.0f, 1.0f);
			return true;
		}
		return false;
	}

	void CGEWhiteBalanceFastFilter::setTempAndTint(float temp, float tint)
	{
		m_temp = temp;
		m_tint = tint;
		struct Color
		{
			Color& operator /= (float value)
			{
				r /= value;
				g /= value;
				b /= value;
				return *this;
			}
			float r, g, b;
		}color;
		temp = CGE_MID(temp * 5400.0f + 6500.0f, 1200.0f, 12000.0f);
		tint = CGE_MID(tint, 0.02f, 5.0f);
		float xD;
		temp /= 1000.0f;
		if(temp < 4.0f)
			xD = 0.27475f / (temp * temp * temp) - 0.98598f / (temp * temp) + 1.17444f / temp + 0.145986f;
		else if(temp < 7.0f)
			xD = -4.6070f / (temp * temp * temp) + 2.9678f / (temp * temp) + 0.09911f / temp + 0.244063f;
		else xD = -2.0064f / (temp * temp * temp) + 1.9018f / (temp * temp) + 0.24748f / temp + 0.237040f;

		float yD = -3.0f * xD * xD + 2.87f * xD - 0.275f;

		float X = xD / yD;
		float Z = (1.0f - xD - yD) / yD;

		color.r = X * 3.24074f - 1.53726f - Z * 0.498571f;
		color.g = -X * 0.969258f + 1.87599f + Z * 0.0415557f;
		color.b = X * 0.0556352f - 0.203996f + Z * 1.05707f;

		color.g /= tint;

		color /= CGE_MAX(CGE_MAX(color.r, color.g), color.b);
		color.r = 1.0f / color.r;
		color.g = 1.0f / color.g;
		color.b = 1.0f / color.b;
		color /= color.r * 0.299f + color.g * 0.587f + color.b * 0.114f;

		m_program.bind();
		m_program.sendUniformf(paramBalanceName, color.r, color.g, color.b);
	}

}