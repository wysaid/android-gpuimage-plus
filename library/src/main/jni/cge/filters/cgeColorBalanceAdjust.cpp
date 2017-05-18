/*
 * cgeColorBalanceAdjust.cpp
 *
 *  Created on: 2015-3-30
 *      Author: Wang Yang
 */

#include "cgeColorBalanceAdjust.h"

//GIMP color balance filter shader

static CGEConstString s_colorBalance = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;

uniform sampler2D inputImageTexture;
uniform float redShift;
uniform float greenShift;
uniform float blueShift;

float RGBToL(vec3 color)
{
	float fmin = min(min(color.r, color.g), color.b);    //Min. value of RGB
	float fmax = max(max(color.r, color.g), color.b);    //Max. value of RGB

	return (fmax + fmin) / 2.0; // Luminance
}

vec3 RGBToHSL(vec3 color)
{
	vec3 hsl; // init to 0 to avoid warnings ? (and reverse if + remove first part)

	float fmin = min(min(color.r, color.g), color.b);    //Min. value of RGB
	float fmax = max(max(color.r, color.g), color.b);    //Max. value of RGB
	float delta = fmax - fmin;             //Delta RGB value

	hsl.z = (fmax + fmin) / 2.0; // Luminance

	if (delta == 0.0)		//This is a gray, no chroma...
	{
		hsl.x = 0.0;	// Hue
		hsl.y = 0.0;	// Saturation
	}
	else                                    //Chromatic data...
	{
		if (hsl.z < 0.5)
			hsl.y = delta / (fmax + fmin); // Saturation
		else
			hsl.y = delta / (2.0 - fmax - fmin); // Saturation

		float deltaR = (((fmax - color.r) / 6.0) + (delta / 2.0)) / delta;
		float deltaG = (((fmax - color.g) / 6.0) + (delta / 2.0)) / delta;
		float deltaB = (((fmax - color.b) / 6.0) + (delta / 2.0)) / delta;

		if (color.r == fmax )
			hsl.x = deltaB - deltaG; // Hue
		else if (color.g == fmax)
			hsl.x = (1.0 / 3.0) + deltaR - deltaB; // Hue
		else if (color.b == fmax)
			hsl.x = (2.0 / 3.0) + deltaG - deltaR; // Hue

		if (hsl.x < 0.0)
			hsl.x += 1.0; // Hue
		else if (hsl.x > 1.0)
			hsl.x -= 1.0; // Hue
	}

	return hsl;
}

float HueToRGB(float f1, float f2, float hue)
{
	if (hue < 0.0)
		hue += 1.0;
	else if (hue > 1.0)
		hue -= 1.0;
	float res;
	if ((6.0 * hue) < 1.0)
		res = f1 + (f2 - f1) * 6.0 * hue;
	else if ((2.0 * hue) < 1.0)
		res = f2;
	else if ((3.0 * hue) < 2.0)
		res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
	else
		res = f1;
	return res;
}

vec3 HSLToRGB(vec3 hsl)
{
	vec3 rgb;

	if (hsl.y == 0.0)
		rgb = vec3(hsl.z); // Luminance
	else
	{
		float f2;

		if (hsl.z < 0.5)
			f2 = hsl.z * (1.0 + hsl.y);
		else
			f2 = (hsl.z + hsl.y) - (hsl.y * hsl.z);

		float f1 = 2.0 * hsl.z - f2;

		rgb.r = HueToRGB(f1, f2, hsl.x + (1.0/3.0));
		rgb.g = HueToRGB(f1, f2, hsl.x);
		rgb.b= HueToRGB(f1, f2, hsl.x - (1.0/3.0));
	}

	return rgb;
}

void main()
{
	vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);

	// New way:
	float lightness = RGBToL(textureColor.rgb);
	vec3 shift = vec3(redShift, greenShift, blueShift);

	const float a = 0.25;
	const float b = 0.333;
	const float scale = 0.7;

	vec3 midtones = (clamp((lightness - b) /  a + 0.5, 0.0, 1.0) * clamp ((lightness + b - 1.0) / -a + 0.5, 0.0, 1.0) * scale) * shift;

	vec3 newColor = textureColor.rgb + midtones;
	newColor = clamp(newColor, 0.0, 1.0);

	// preserve luminosity
	vec3 newHSL = RGBToHSL(newColor);
	float oldLum = RGBToL(textureColor.rgb);
	textureColor.rgb = HSLToRGB(vec3(newHSL.x, newHSL.y, oldLum));

	gl_FragColor = textureColor;
}
);

namespace CGE
{
	CGEConstString CGEColorBalanceFilter::paramRedShiftName = "redShift";
	CGEConstString CGEColorBalanceFilter::paramGreenShiftName = "greenShift";
	CGEConstString CGEColorBalanceFilter::paramBlueShiftName = "blueShift";

	bool CGEColorBalanceFilter::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_colorBalance);
	}

	void CGEColorBalanceFilter::setRedShift(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramRedShiftName, value);
	}

	void CGEColorBalanceFilter::setGreenShift(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramGreenShiftName, value);
	}

	void CGEColorBalanceFilter::setBlueShift(float value)
	{
		m_program.bind();
		m_program.sendUniformf(paramBlueShiftName, value);
	}

}