#include "cgeRedeyeRemoveFilter.h"

static CGEConstString s_fshRedeyeRemove = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform vec2 samplerSteps;
uniform vec2 eyePostion;
uniform float radius;
uniform float alpha;
void main()
{
	vec2 realPosition = textureCoordinate / samplerSteps;
	float dis1 = distance(realPosition,eyePostion);
	vec4 color = texture2D(inputImageTexture, textureCoordinate);
	if(dis1 >= radius)
	{
		gl_FragColor = color;
	}
	else
	{
		vec3 v = color.rgb;
		v.r = (v.g + v.b) / 2.0;
		v.g = (v.g + v.r) / 2.0;
		v.b = (v.b + v.r) / 2.0;

		if (v.g > v.b)
			v.r = v.g;
		vec3 v3 = color.rgb - v;
		v = v + v3 * pow(dis1 / radius,alpha);
		gl_FragColor = vec4(v,color.a);
		
	}

}
);

namespace CGE
{

	CGEConstString CGERedeyeRemoveFilter::paramRadiusName = "radius";
	CGEConstString CGERedeyeRemoveFilter::paramEyePostionName = "eyePostion";
	CGEConstString CGERedeyeRemoveFilter::paramAlphaName = "alpha";

	bool CGERedeyeRemoveFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshRedeyeRemove))
		{
			setEyePostion(100.0f,130.0f);
			setRadius(100.0f);
			setAlpha(1.5f);
			return true;
		}
		return false;
	}

	void CGERedeyeRemoveFilter::setEyePostion(float x, float y)
	{
		m_program.bind();
		m_program.sendUniformf(paramEyePostionName, x, y);
	}

	void CGERedeyeRemoveFilter::setRadius(float radius)
	{
		m_program.bind();
		m_program.sendUniformf(paramRadiusName,radius);
	}

	void CGERedeyeRemoveFilter::setAlpha(float alpha)
	{
		m_program.bind();
		m_program.sendUniformf(paramAlphaName,alpha);
	}

}
