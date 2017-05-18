/*
* cgeShadowHighlightAdjust.cpp
*
*  Created on: 2013-12-26
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeShadowHighlightAdjust.h"
#include <cmath>

const static char* const s_fshShadowHighlight = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform float shadows;
uniform float highlights;

vec3 highlightAndShadow(vec3 src, float l, float d) //l for highlight, d for shadow
{
	vec3 src2 = src * src;
	vec3 src3 = src2 * src;
	vec3 color1 = 4.0 * (d - 1.0) * src3 + 4.0 * (1.0 - d) * src2 + d * src;
	vec3 color2 = 4.0 * (l - 1.0) * src3 + 8.0 * (1.0 - l) * src2 + (5.0 * l - 4.0) * src + 1.0 - l;


	// if(src.r < 0.5)
	// 	src.r = 4.0 * (d - 1.0) * src.r * src.r * src.r + 4.0 * (1.0 - d) * src.r * src.r + d * src.r;
	// else
	// 	src.r = 4.0 * (l - 1.0) * src.r * src.r * src.r + 8.0 * (1.0 - l) * src.r * src.r + (5.0 * l - 4.0) * src.r + 1.0 - l;

	// if(src.g < 0.5)
	// 	src.g = 4.0 * (d - 1.0) * src.g * src.g * src.g + 4.0 * (1.0 - d) * src.g * src.g + d * src.g;
	// else
	// 	src.g = 4.0 * (l - 1.0) * src.g * src.g * src.g + 8.0 * (1.0 - l) * src.g * src.g + (5.0 * l - 4.0) * src.g + 1.0 - l;

	// if(src.b < 0.5)
	// 	src.b = 4.0 * (d - 1.0) * src.b * src.b * src.b + 4.0 * (1.0 - d) * src.b * src.b + d * src.b;
	// else
	// 	src.b = 4.0 * (l - 1.0) * src.b * src.b * src.b + 8.0 * (1.0 - l) * src.b * src.b + (5.0 * l - 4.0) * src.b + 1.0 - l;
	return mix(color1, color2, step(0.5, src));
}
void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	src.rgb = highlightAndShadow(src.rgb * src.a, highlights, shadows);	
	gl_FragColor = src;
}
);

/*
const static char* const s_fshShadowHighlight_Fast = SHADER_STRING_PRECISION_H
(
uniform sampler2D inputImageTexture;
varying vec2 textureCoordinate;

uniform float shadows;
uniform float highlights;

const vec3 luminanceWeighting = vec3(0.3, 0.3, 0.3);

void main()
{
	vec4 source = texture2D(inputImageTexture, textureCoordinate);
	float luminance = dot(source.rgb, luminanceWeighting);

	float shadow = clamp((pow(luminance, 1.0/(shadows+1.0)) + (-0.76)*pow(luminance, 2.0/(shadows+1.0))) - luminance, 0.0, 1.0);
	float highlight = clamp((1.0 - (pow(1.0-luminance, 1.0/(2.0-highlights)) + (-0.8)*pow(1.0-luminance, 2.0/(2.0-highlights)))) - luminance, -1.0, 0.0);
	vec3 result = (luminance + shadow + highlight) * (source.rgb / (luminance - 0.0));

	gl_FragColor = vec4(result.rgb, source.a);
}
);
 */

namespace CGE
{
	CGEConstString CGEShadowHighlightFilter::paramShadowName = "shadows";
	CGEConstString CGEShadowHighlightFilter::paramHighlightName = "highlights";

	bool CGEShadowHighlightFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshShadowHighlight))
		{
			setShadow(0.0f);
			setHighlight(0.0f);
			return true;
		}
		return false;
	}

	void CGEShadowHighlightFilter::setShadow(float value)
	{
		if(value > 0.0f) value *= 0.68f;
		value = tanf((value + 100.0f) * (3.14159f / 400.0f));
		m_program.bind();
		m_program.sendUniformf(paramShadowName, value);
	}

	void CGEShadowHighlightFilter::setHighlight(float value)
	{
		value = -value;
		if(value > 0.0f) value *= 0.68f;
		value = tanf((value + 100.0f) * (3.14159f / 400.0f));
		m_program.bind();
		m_program.sendUniformf(paramHighlightName, value);
	}

	bool CGEShadowHighlightFastFilter::init()
	{
		if(CGEFastAdjustRGBFilter::init())
		{
			assignCurveArray();
			return true;
		}
		return false;
	}

	void CGEShadowHighlightFastFilter::setShadowAndHighlight(float shadow, float highlight)
	{
		m_shadow = shadow;
		m_highlight = highlight;
		if(shadow > 0.0) shadow *= 0.68f;
		shadow = tanf((shadow + 100.0f) * (3.14159f / 400.0f));
		highlight = -highlight;
		if(highlight > 0.0) highlight *= 0.68f;
		highlight = tanf((highlight + 100.0f) * (3.14159f / 400.0f));
		const float l = highlight;
		const float d = shadow;
		const std::vector<float>::size_type sz = m_curveRGB.size();
		for(std::vector<float>::size_type t = 0; t != sz; ++t)
		{
			const float v = float(t) / (CGE_CURVE_PRECISION - 1.0f);
			if(v < 0.5)
				m_curveRGB[t] = 4.0f * (d - 1.0f) * v * v * v + 4.0f * (1.0f - d) * v * v + d * v;
			else
				m_curveRGB[t] = 4.0f * (l - 1.0f) * v * v * v + 8.0f * (1.0f - l) * v * v + (5.0f * l - 4.0f) * v + 1.0f - l;
		}
		assignCurveArray();
	}

}