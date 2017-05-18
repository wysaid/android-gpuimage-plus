/*
 * cgeSelectiveColorAdjust.cpp
 *
 *  Created on: 2014-11-18
 */

#include "cgeSelectiveColorAdjust.h"
#include <cmath>



static CGEConstString s_fshSelectiveColor = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec4 red;
uniform vec4 yellow;
uniform vec4 green;
uniform vec4 cyan;
uniform vec4 blue;
uniform vec4 magenta;
uniform vec4 white;
uniform vec4 gray;
uniform vec4 black;

vec4 getFac(vec4 facPre,vec4 facNex,float h,float s,float v)
{
	vec4 ret = mix(facPre,facNex,h); // facNex * h + facPre - h * facPre = (facNex - facPre) * h + facPre

	vec4 rets0;
	v = v * 2.0 - 1.0;

	if(v > 0.0)
	{
		rets0 = mix(gray,white,v); 
	}
	else
	{
		rets0 = mix(gray,black,-v); 
	}

	s = 1.0 - s;
	s = s * s * s;
	s = 1.0 - s;

	return mix(rets0,ret,s);
}

vec3 adjColor(vec3 src,float a,vec4 fac)
{
	vec3 tfac = fac.xyz *  vec3(fac.w,fac.w,fac.w);

	tfac = min(tfac,vec3(2.0,2.0,2.0));

	vec3 tfc1 = clamp(tfac - vec3(1.0,1.0,1.0),0.0,0.5);
	vec3 tfc2 = max(tfac - vec3(1.5,1.5,1.5),0.0);

	src = vec3(1.0,1.0,1.0) - src;

	src = src * (tfac -  src * (src * (tfc1 - tfc2) + vec3(2.0,2.0,2.0) * tfc2));

	return vec3(1.0,1.0,1.0) - src;
}

vec3 hsvAdjust(vec3 src)
{
	vec3 temp;
	vec4 color1, color2;

	if(src.r > src.g)
	{
		if(src.g > src.b)//red - yellow
		{
			temp = src.rgb;
			color1 = red;
			color2 = yellow;
		}
		else if(src.b > src.r)//blue - Magenta 
		{
			temp = src.brg;
			color1 = blue;
			color2 = magenta;
		}
		else//red - magenta
		{
			temp = src.rbg;
			color1 = red;
			color2 = magenta;
		}
	}
	else
	{
		if(src.r > src.b)// green - yellow
		{
			temp = src.grb;
			color1 = green;
			color2 = yellow;
		}
		else if(src.b > src.g)// blue - cyan
		{
			temp = src.bgr;
			color1 = blue;
			color2 = cyan;
		}
		else// green - cyan
		{
			temp = src.gbr;
			color1 = green;
			color2 = cyan;			
		}
	}

	float d = temp.x - temp.z + 0.0001;
	float s = temp.y - temp.z;

	vec4 fac = getFac(color1, color2, s/d, d, temp.x);

	return adjColor(src, temp.x, fac);
}


void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	src.rgb = hsvAdjust(src.rgb);
	gl_FragColor = src;
}
);

//
//static CGEConstString s_fshReplaceColor = CGE_SHADER_STRING_PRECISION_M
//(
//varying vec2 textureCoordinate;
//uniform sampler2D inputImageTexture;
//uniform sampler2D curveTexture;
//
//void main()
//{
//	vec3 src = texture2D(inputImageTexture, textureCoordinate).rgb;
//
//	float p = dot(src,vec3(0.299,0.587,0.114));
//
//
//	src =  texture2D(curveTexture, vec2(p,0.5)).rgb;
//
//	gl_FragColor = vec4(src, 1.0);
//}
//);

//static const char * PN[] =
//{
//	"red",
//	"yellow",
//	"green",
//	"cyan",
//	"blue",
//	"magenta",
//	"white",
//	"gray",
//	"black",
//};

namespace CGE
{
	CGEConstString CGESelectiveColorFilter::paramName = "intensity";

	bool CGESelectiveColorFilter::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshSelectiveColor))
		{
			setRed(0.0f,0.0f,0.0f,0.0f);
			setGreen(0.0f,0.0f,0.0f,0.0f);
			setBlue(0.0f,0.0f,0.0f,0.0f);
			setCyan(0.0f,0.0f,0.0f,0.0f);
			setMagenta(0.0f,0.0f,0.0f,0.0f);
			setYellow(0.0f,0.0f,0.0f,0.0f);
			setWhite(0.0f,0.0f,0.0f,0.0f);
			setGray(0.0f,0.0f,0.0f,0.0f);
			setBlack(0.0f,0.0f,0.0f,0.0f);
			return true;
		}
		return false;

		//if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshReplaceColor))
		//{
		//	setIntensity(0.0f);
		//	UniformParameters* param = new UniformParameters;

		//	param->pushSampler2D("curveTexture", &m_curveTexture, 0);
		//	setAdditionalUniformParameter(param);
		//	return true;
		//}
		//return false;

		

	}

	
	void CGESelectiveColorFilter::setRed    (float cyan,float magenta,float yellow,float key)
	{
		m_program.bind();
		m_program.sendUniformf("red",cyan + 1.0f,magenta + 1.0f,yellow + 1.0f,key + 1.0f);
	}
	void CGESelectiveColorFilter::setGreen  (float cyan,float magenta,float yellow,float key)
	{
		m_program.bind();
		m_program.sendUniformf("green",cyan + 1.0f,magenta + 1.0f,yellow + 1.0f,key + 1.0f);
	}
	void CGESelectiveColorFilter::setBlue   (float cyan,float magenta,float yellow,float key)
	{
		m_program.bind();
		m_program.sendUniformf("blue",cyan + 1.0f,magenta + 1.0f,yellow + 1.0f,key + 1.0f);
	}
	void CGESelectiveColorFilter::setCyan   (float cyan,float magenta,float yellow,float key)
	{
		m_program.bind();
		m_program.sendUniformf("cyan",cyan + 1.0f,magenta + 1.0f,yellow + 1.0f,key + 1.0f);
	}
	void CGESelectiveColorFilter::setMagenta(float cyan,float magenta,float yellow,float key)
	{
		m_program.bind();
		m_program.sendUniformf("magenta",cyan + 1.0f,magenta + 1.0f,yellow + 1.0f,key + 1.0f);
	}
	void CGESelectiveColorFilter::setYellow (float cyan,float magenta,float yellow,float key)
	{
		m_program.bind();
		m_program.sendUniformf("yellow",cyan + 1.0f,magenta + 1.0f,yellow + 1.0f,key + 1.0f);
	}
	void CGESelectiveColorFilter::setWhite  (float cyan,float magenta,float yellow,float key)
	{
		m_program.bind();
		m_program.sendUniformf("white",cyan + 1.0f,magenta + 1.0f,yellow + 1.0f,key + 1.0f);
	}
	void CGESelectiveColorFilter::setGray   (float cyan,float magenta,float yellow,float key)
	{
		m_program.bind();
		m_program.sendUniformf("gray",cyan + 1.0f,magenta + 1.0f,yellow + 1.0f,key + 1.0f);
	}
	void CGESelectiveColorFilter::setBlack  (float cyan,float magenta,float yellow,float key)
	{
		m_program.bind();
		m_program.sendUniformf("black",cyan + 1.0f,magenta + 1.0f,yellow + 1.0f,key + 1.0f);
	}

	//void CGESelectiveColorFilter::setIntensity(float value)
	//{
		//std::vector<int> points;

		//points.push_back(0);
		//points.push_back(255);
		//points.push_back(0);
		//points.push_back(0);

		//points.push_back(64);
		//points.push_back(0);
		//points.push_back(255);
		//points.push_back(0);

		//points.push_back(128);
		//points.push_back(255);
		//points.push_back(0);
		//points.push_back(255);

		//points.push_back(192);
		//points.push_back(255);
		//points.push_back(255);
		//points.push_back(0);

		//

		//points.push_back(256);
		//points.push_back(0);
		//points.push_back(0);
		//points.push_back(0);
		//
		//std::vector<unsigned char> curve(256 * 3);

		//for(int pi = 0;pi < 4;++pi)
		//{
		//	int start = points[pi * 4];
		//	int end = points[pi * 4 + 4];
		//	int l = end - start;
		//	int s = l / 2;

		//	for(int i=start;i< end;++i)
		//	{
		//		int idx = i * 3;
		//		int m = i - start;

		//		int pidx = pi * 4;

		//		curve[idx    ] = (points[pidx + 1] * (l - m) + points[pidx + 5] * m + s) / l;
		//		curve[idx + 1] = (points[pidx + 2] * (l - m) + points[pidx + 6] * m + s) / l;
		//		curve[idx + 2] = (points[pidx + 3] * (l - m) + points[pidx + 7] * m + s) / l;
		//	}
		//}
		//
		//

		//glActiveTexture(CGE_TEXTURE_START);		
		//if(m_curveTexture != 0)
		//	glDeleteTextures(1, &m_curveTexture);
		//glGenTextures(1, &m_curveTexture);
		//glBindTexture(GL_TEXTURE_2D, m_curveTexture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &curve[0]);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//}

}