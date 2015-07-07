/*
* cgeMultipleEffectsSpecial.cpp
*
*  Created on: 2014-1-13
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeMultipleEffectsSpecial.h"

#include <cstdlib>

//////////////////////////////////////////////////////////////////////////

static CGEConstString s_fshE16 = CGE_SHADER_STRING_PRECISION_M
(	
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

const float fh_red = 0.1;
const float fh_yellow = -0.7;
const float fh_green = -0.7;
const float fh_cyan = -0.7;
const float fh_blue = -0.7;
const float fh_magenta = 0.1;

void main(void)
{
	vec4 src;
	vec3 dst;
	vec3 hsv;
	vec3 tmp_s;
	float fmax;
	float fmin;
	float fdelta;
	float fs_off;

	src = texture2D(inputImageTexture, textureCoordinate);
	dst = src.rgb;
	//rgb2hsv
	fmax = max(dst.r,max(dst.g,dst.b));
	fmin = min(dst.r,min(dst.g,dst.b));
	fdelta = fmax - fmin;

	hsv.z = fmax;
	if(0.0 == fdelta)
	{
		hsv.x = 0.0;
		hsv.y = 0.0;
		tmp_s = dst;
	}
	else
	{
		hsv.y = fdelta/fmax;
		if(fmax == dst.r)
		{
			if(dst.g >= dst.b)
			{
				//0
				hsv.x = (dst.g - dst.b)/fdelta;
				fs_off = (fh_yellow - fh_red)*hsv.x + fh_red;
				//saturation adjust
				hsv.y = hsv.y*(1.0 + fs_off);
				hsv.y = min(1.0 , hsv.y);
				hsv.y = max(0.0 , hsv.y);
				//rgb2hsv end
				//hsv2rgb
				dst.r = hsv.z;
				dst.b = hsv.z*(1.0 - hsv.y);
				dst.g = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);

				tmp_s = dst;
				hsv.x = 0.658325;
				//hsv2rgb
				dst.r = hsv.z;
				dst.b = hsv.z*(1.0 - hsv.y);
				dst.g = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);
			}
			else
			{
				//5
				//hsv.x = (dst.b - dst.g)/fdelta;
				hsv.x = (dst.r - dst.b)/fdelta;
				fs_off = (fh_red - fh_magenta)*hsv.x + fh_magenta;
				//saturation adjust
				hsv.y = hsv.y*(1.0 + fs_off);
				hsv.y = min(1.0 , hsv.y);
				hsv.y = max(0.0 , hsv.y);
				//rgb2hsv end
				//hsv2rgb
				dst.r = hsv.z;
				dst.g = hsv.z*(1.0 - hsv.y);
				dst.b = hsv.z*(1.0 - hsv.y*hsv.x);

				tmp_s = dst;
				hsv.x = 0.658325;
				//hsv2rgb
				dst.r = hsv.z;
				dst.g = hsv.z*(1.0 - hsv.y);
				dst.b = hsv.z*(1.0 - hsv.y*hsv.x);
			}
		}
		else if(fmax == dst.g)
		{

			if(dst.r > dst.b)
			{
				//1
				//hsv.x = (dst.r - dst.b)/fdelta;
				hsv.x = (dst.g - dst.r)/fdelta;
				fs_off = (fh_green - fh_yellow)*hsv.x + fh_yellow;
				//saturation adjust
				hsv.y = hsv.y*(1.0 + fs_off);
				hsv.y = min(1.0 , hsv.y);
				hsv.y = max(0.0 , hsv.y);
				//rgb2hsv end
				//hsv2rgb
				dst.g = hsv.z;
				dst.r = hsv.z*(1.0 - hsv.y*hsv.x);
				dst.b = hsv.z*(1.0 - hsv.y);

				tmp_s = dst;
				hsv.x = 0.658325;
				//hsv2rgb
				dst.g = hsv.z;
				dst.r = hsv.z*(1.0 - hsv.y*hsv.x);
				dst.b = hsv.z*(1.0 - hsv.y);
			}
			else
			{
				//2
				hsv.x = (dst.b - dst.r)/fdelta;
				fs_off = (fh_cyan - fh_green)*hsv.x + fh_green;
				//saturation adjust
				hsv.y = hsv.y*(1.0 + fs_off);
				hsv.y = min(1.0 , hsv.y);
				hsv.y = max(0.0 , hsv.y);
				//rgb2hsv end
				//hsv2rgb
				dst.g = hsv.z;
				dst.r = hsv.z*(1.0 - hsv.y);
				dst.b = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);

				tmp_s = dst;
				hsv.x = 0.658325;
				//hsv2rgb
				dst.g = hsv.z;
				dst.r = hsv.z*(1.0 - hsv.y);
				dst.b = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);
			}
		}
		else
		{
			if(dst.g > dst.r)
			{
				//3
				//hsv.x = (dst.g - dst.r)/fdelta;
				hsv.x = (dst.b - dst.g)/fdelta;
				fs_off = (fh_blue - fh_cyan)*hsv.x + fh_cyan;
				//saturation adjust
				hsv.y = hsv.y*(1.0 + fs_off);
				hsv.y = min(1.0 , hsv.y);
				hsv.y = max(0.0 , hsv.y);
				//rgb2hsv end
				//hsv2rgb
				dst.b = hsv.z;
				dst.r = hsv.z*(1.0 - hsv.y);
				dst.g = hsv.z*(1.0 - hsv.y*hsv.x);

				tmp_s = dst;
				hsv.x = 0.658325;
				//hsv2rgb
				dst.b = hsv.z;
				dst.r = hsv.z*(1.0 - hsv.y);
				dst.g = hsv.z*(1.0 - hsv.y*hsv.x);
			}
			else
			{
				//4
				hsv.x = (dst.r - dst.g)/fdelta;
				fs_off = (fh_magenta - fh_blue)*hsv.x + fh_blue;
				//saturation adjust
				hsv.y = hsv.y*(1.0 + fs_off);
				hsv.y = min(1.0 , hsv.y);
				hsv.y = max(0.0 , hsv.y);
				//rgb2hsv end
				//hsv2rgb
				dst.b = hsv.z;
				dst.r = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);
				dst.g = hsv.z*(1.0 - hsv.y);

				tmp_s = dst;
				hsv.x = 0.658325;
				//hsv2rgb
				dst.b = hsv.z;
				dst.r = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);
				dst.g = hsv.z*(1.0 - hsv.y);
			}
		}
	}

	dst.r = hsv.z;
	dst.g = hsv.z*(1.0 - hsv.y);
	dst.b = hsv.z*(1.0 - hsv.y*hsv.x);

	dst = tmp_s * 0.69 + dst * 0.31;

	gl_FragColor = vec4(dst,src.a);
});

static CGEConstString s_fshE21 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 vSteps;

const vec2 vignetteCenter = vec2(0.5, 0.5);

vec3 hardLight(vec3 src1, vec3 src2, float alpha)
{
	if(src2.r < 0.5)
		src2.r = (src1.r * src2.r) * 2.0;
	else src2.r = (src1.r + src2.r) * 2.0 - (src1.r * src2.r) * 2.0 - 1.0;

	if(src2.g < 0.5)
		src2.g = (src1.g * src2.g) * 2.0;
	else src2.g = (src1.g + src2.g) * 2.0 - (src1.g * src2.g) * 2.0 - 1.0;

	if(src2.b < 0.5)
		src2.b = (src1.b * src2.b) * 2.0;
	else src2.b = (src1.b + src2.b) * 2.0 - (src1.b * src2.b) * 2.0 - 1.0;

	return mix(src1, src2, alpha);
}

void main(void)
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	float max_r = 0.87055 / min(vSteps.x, vSteps.y);
	float lens_fac = max_r * max_r;
	float lens_facinv = 1073741824.0 / lens_fac;
	float lens_amount = 102.0;
	float dis = length(textureCoordinate - vignetteCenter);
	float bd = (lens_fac + dis) * lens_facinv / 262144.0;
	bd = bd * bd / 1024.0;
	bd = 536870912.0 / bd;
	float alpha = 1.0 + lens_amount*(bd - 32768.0)/256.0/32768.0;
	alpha = clamp(alpha, 0.0, 1.0);
	vec3 dst = src.rgb * alpha;

	float lum = dot(dst, vec3(0.299, 0.587, 0.114));
	vec3 tmp = vec3(lum, lum, 1.0 - lum);
	dst = hardLight(dst, tmp, 0.5);
	gl_FragColor = vec4(dst,src.a);
});

static CGEConstString s_fshE28 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform vec2 colorScale;  
uniform vec2 vignette;
const vec2 vignetteCenter = vec2(0.5, 0.5);

void main(void)
{   
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	float d = distance(textureCoordinate, vignetteCenter);    
	float percent = clamp((d-vignette.x)/vignette.y, 0.0, 1.0);
	float alpha = 1.0 - percent * percent;
	float lum = dot(src.rgb, vec3(0.299, 0.587,0.114));
	lum = (lum - colorScale.x) / colorScale.y * alpha;

	gl_FragColor = vec4(vec3(lum),src.a);
} );

static CGEConstString s_fshE29 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 colorScale;

void main(void)
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);   
	float lum = dot(src.rgb, vec3(0.299, 0.587,0.114));
	lum = (lum - colorScale.x) / colorScale.y;
	gl_FragColor = vec4(vec3(lum),src.a);
});

static CGEConstString s_fshE30To32 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 colorScale;

vec3 colorMono(vec3 src)
{
	float f = dot(src, vec3(0.299, 0.587, 0.114));
	return vec3(f,f,f);
}

void main(void)
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	src.rgb = colorMono(src.rgb);
	src.rgb = (src.rgb - colorScale.x) / colorScale.y;

	gl_FragColor = vec4(src.rgb, src.a);
});

static CGEConstString s_fshE39 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D SamplerBackTex1;
uniform  float saturation;
uniform vec2 colorScale;
void main(void)
{
	vec4 src;
	vec3 dst;
	vec3 tmp_s;
	float alpha;
	float lum;
	vec3 txt;

	src = texture2D(inputImageTexture, textureCoordinate);
	dst = src.rgb;

	vec2 smp_coord = textureCoordinate;
	txt = texture2D(SamplerBackTex1, smp_coord).rgb;
	lum = dst.r*0.299 + dst.g*0.587 + dst.b*0.114;
	tmp_s = lum*(1.0-saturation) + saturation*dst;
	tmp_s = (tmp_s - colorScale.x) / colorScale.y;
	dst = tmp_s;

	dst.r = min(1.0,dst.r);
	dst.g = min(1.0,dst.g);
	dst.b = min(1.0,dst.b);

	vec3 tmp_txt;
	tmp_txt.r = min(1.0,dst.r + txt.r - 2.0 * txt.r * dst.r);
	tmp_txt.g = min(1.0,dst.g + txt.g - 2.0 * txt.g * dst.g);
	tmp_txt.b = min(1.0,dst.b + txt.b - 2.0 * txt.b * dst.b);

	dst = dst*0.13 + tmp_txt*0.87;

	lum = (txt.r+txt.g+txt.b)/3.0;

	tmp_txt.r = min(1.0,lum + dst.r);
	tmp_txt.g = min(1.0,lum + dst.g);
	tmp_txt.b = min(1.0,lum + dst.b);

	dst = dst*0.35 + tmp_txt*0.65;

	if(dst.r + lum < 1.0)
	{
		tmp_txt.r = 0.0;
	}
	else
	{
		tmp_txt.r = 1.0;
	}
	if(dst.g + lum < 1.0)
	{
		tmp_txt.g = 0.0;
	}
	else
	{
		tmp_txt.g = 1.0;
	}
	if(dst.b + lum < 1.0)
	{
		tmp_txt.b = 0.0;
	}
	else
	{
		tmp_txt.b = 1.0;
	}
	dst = dst*0.84 + tmp_txt*0.16;

	gl_FragColor = vec4(dst,src.a);
}
);

static CGEConstString s_fshE87 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
const vec3 vr = vec3(1.0, 0.835, 0.835);
const vec3 vg = vec3(0.0, 0.588, 1.0);

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	src.rgb = 1.0 - (1.0 - vr * src.r) * (1.0 - vg * src.g);
	gl_FragColor = src;
}
);

static CGEConstString s_fshE88 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform vec2 vSteps;
const float intensity = 0.45;

float getLum(vec3 src)
{
	return dot(src, vec3(0.299, 0.587, 0.114));
}

void main()
{
	mat3 m;
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	m[0][0] = getLum(texture2D(inputImageTexture, textureCoordinate - vSteps).rgb);
	m[0][1] = getLum(texture2D(inputImageTexture, textureCoordinate - vec2(0.0, vSteps.y)).rgb);
	m[0][2] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(vSteps.x, -vSteps.y)).rgb);
	m[1][0] = getLum(texture2D(inputImageTexture, textureCoordinate - vec2(vSteps.x, 0.0)).rgb);
	m[1][1] = getLum(src.rgb);
	m[1][2] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(vSteps.x, 0.0)).rgb);
	m[2][0] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(-vSteps.x, vSteps.y)).rgb);
	m[2][1] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(0.0, vSteps.y)).rgb);
	m[2][2] = getLum(texture2D(inputImageTexture, textureCoordinate + vSteps).rgb);

	float nx = m[0][0] + m[0][1] + m[0][2] - m[2][0] - m[2][1] - m[2][2];
	float ny = m[0][0] + m[1][0] + m[2][0] - m[0][2] - m[1][2] - m[2][2];
	float ndl = abs(nx + ny + intensity);
	float shade = 0.0;

	float norm = (nx * nx + ny * ny + intensity * intensity);
	shade = (ndl * 0.577) / sqrt(norm);

	gl_FragColor = vec4(src.rgb * shade, src.a);
}
);

static CGEConstString s_fshE89 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 vSteps;

const float intensity = 0.45;

float getLum(vec3 src)
{
	return dot(src, vec3(0.299, 0.587, 0.114));
}

void main()
{
	mat3 m;
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	m[0][0] = getLum(texture2D(inputImageTexture, textureCoordinate - vSteps).rgb);
	m[0][1] = getLum(texture2D(inputImageTexture, textureCoordinate - vec2(0.0, vSteps.y)).rgb);
	m[0][2] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(vSteps.x, -vSteps.y)).rgb);
	m[1][0] = getLum(texture2D(inputImageTexture, textureCoordinate - vec2(vSteps.x, 0.0)).rgb);
	m[1][1] = getLum(src.rgb);
	m[1][2] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(vSteps.x, 0.0)).rgb);
	m[2][0] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(-vSteps.x, vSteps.y)).rgb);
	m[2][1] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(0.0, vSteps.y)).rgb);
	m[2][2] = getLum(texture2D(inputImageTexture, textureCoordinate + vSteps).rgb);

	float nx = m[0][0] + m[0][1] + m[0][2] - m[2][0] - m[2][1] - m[2][2];
	float ny = m[0][0] + m[1][0] + m[2][0] - m[0][2] - m[1][2] - m[2][2];
	float ndl = abs(nx + ny + intensity);
	float shade = 0.0;

	float norm = (nx * nx + ny * ny + intensity * intensity);
	shade = (ndl * 0.577) / sqrt(norm);
	gl_FragColor = vec4(vec3(shade), src.a);
}
);

static CGEConstString s_fshE90 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec3 colorGradient[5];
const float ratio = 1.25;

vec3 soft_light_l3s(vec3 a, vec3 b)
{
	vec3 src;
	a = a * 2.0 - 32768.0;

	float tmpr = a.r > 0.0 ? sqrt(b.r)  - b.r : b.r - b.r * b.r;
	src.r = a.r * tmpr / 128.0 + b.r * 256.0;

	float tmpg = a.g > 0.0 ? sqrt(b.g)  - b.g : b.g - b.g * b.g;
	src.g = a.g * tmpg / 128.0 + b.g * 256.0;

	float tmpb = a.b > 0.0 ? sqrt(b.b)  - b.b : b.b - b.b * b.b;
	src.b = a.b * tmpb / 128.0 + b.b * 256.0;
	return src;
}

void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec2 tmpCoord = textureCoordinate * 32768.0 * ratio;
	float ps = tmpCoord.x + tmpCoord.y;
	int pi = int(ps / 16384.0);
	float pr = mod(ps, 16384.0) / 16384.0;
	vec3 v1 = colorGradient[pi];
	vec3 v2 = colorGradient[pi + 1];
	vec3 tmp1 = v1 * (1.0 - pr) + v2 * pr;
	vec3 tmp2 = src.rgb * mat3(0.509, 0.4109, 0.07978,
		0.209, 0.7109, 0.07978,
		0.209, 0.4109, 0.3798);
	src.rgb = soft_light_l3s(tmp1, tmp2) / 255.0;
	gl_FragColor = src;
}
);

static CGEConstString s_fshE91 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 vignette;

const vec3 c1 = vec3(0.992,0.137,0.314);
const vec3 c2 = vec3(0.204,0.98,0.725);
const vec2 vignetteCenter = vec2(0.5, 0.5);

void main(void)
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);

	float d = distance(textureCoordinate, vec2(vignetteCenter.x, vignetteCenter.y));
	float percent = clamp((d-vignette.x)/vignette.y, 0.0, 1.0);
	float alpha = 1.0-percent;

	src.rgb = src.rgb * alpha;

	src.r = 1.0 - (1.0 - src.r*c1.r) * (1.0 - src.g*c2.r);
	src.g = 1.0 - (1.0 - src.r*c1.g) * (1.0 - src.g*c2.g);
	src.b = 1.0 - (1.0 - src.r*c1.b) * (1.0 - src.g*c2.b);

	gl_FragColor = src;
}
);

static CGEConstString s_fshE92 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform vec2 vSteps;
uniform vec2 halfToneVars;

void main()
{
	float scale = vSteps.y < vSteps.x ? 502.65 * vSteps.y : 502.65 * vSteps.x;
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec2 gradient = textureCoordinate / vSteps * scale;
	float lum = dot(src.rgb, vec3(0.299, 0.587, 0.114));
	float ps = gradient.x;
	float ps1 = gradient.x + scale;
	float alfx = (sin(ps1) - sin(ps)) / (ps1 - ps);
	ps = gradient.y;
	ps1 = gradient.y + scale;
	float alfy = (sin(ps1) - sin(ps)) / (ps1 - ps);
	float fa = alfx * alfy * 0.1668 + 0.8333;
	lum = lum * fa;
	lum = (lum - halfToneVars.x) * halfToneVars.y;

	src.rgb = mix(vec3(0.3137, 0.1569, 0.6275),vec3(1.0, 1.0, 0.0), lum);
	gl_FragColor = src;
}
);

static CGEConstString s_fshE99 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

uniform vec2 vSteps;
uniform vec2 halfToneVars;

void main()
{
	float scale = vSteps.y < vSteps.x ? 502.65 * vSteps.y : 502.65 * vSteps.x;
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec2 gradient = textureCoordinate / vSteps * scale;
	float lum = dot(src.rgb, vec3(0.299, 0.587, 0.114));
	float ps = gradient.x;
	float ps1 = gradient.x + scale;
	float alfx = (sin(ps1) - sin(ps)) / (ps1 - ps);
	ps = gradient.y;
	ps1 = gradient.y + scale;
	float alfy = (sin(ps1) - sin(ps)) / (ps1 - ps);
	float fa = alfx * alfy * 0.1668 + 0.8333;
	lum = lum * fa;
	lum = (lum - halfToneVars.x) * halfToneVars.y;

	gl_FragColor = vec4(lum, lum, lum, src.a);
}
);

static CGEConstString s_fshE118 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 colorScale;

const vec2 vignetteCenter = vec2(0.5, 0.5);
const float vignetteStart = 0.0;
const float vignetteRange = 0.8;

void main(void)
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	float d = distance(textureCoordinate, vignetteCenter);
	float percent = clamp((d-vignetteStart)/vignetteRange, 0.0, 1.0);
	float alpha = 1.0 - percent;
	float lum = src.r*0.299 + src.g*0.587 + src.b*0.114;

	src.rgb = vec3((lum - colorScale.x) / colorScale.y * alpha) *
		mat3(0.393, 0.769, 0.189,
		0.349, 0.686, 0.168,
		0.272, 0.534, 0.131);

	gl_FragColor = src;
}
);

static CGEConstString s_fshE73 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D SamplerBackTex1;
uniform vec3 curveArray1[256];
uniform vec3 curveArray2[256];

const float curvePrecision = 255.0;

void main(void)
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec4 tex = texture2D(SamplerBackTex1, textureCoordinate);

	vec3 dst_a = vec3(curveArray1[int(src.r * curvePrecision)].r,
		curveArray1[int(src.g * curvePrecision)].g,
		curveArray1[int(src.b * curvePrecision)].b);

	vec3 dst_b = vec3(curveArray2[int(src.r * curvePrecision)].r,
		curveArray2[int(src.g * curvePrecision)].g,
		curveArray2[int(src.b * curvePrecision)].b);

	gl_FragColor = vec4(mix(dst_a, dst_b, tex.g * tex.a), src.a);
}
);

static CGEConstString s_fshE74 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D SamplerBackTex1;

uniform vec3 curveArray1[256];
uniform vec3 curveArray2[256];

const float curvePrecision = 255.0;

vec3 hsvAdjust(vec3 src, vec3 color1, vec3 color2) //color1:red green blue, color2: magenta yellow cyan
{
	float fmax = max(src.r,max(src.g,src.b));
	float fmin = min(src.r,min(src.g,src.b));
	float fdelta = fmax - fmin;
	float fs_off;
	vec3 hsv;
	hsv.z = fmax;
	if(0.0 == fdelta)
	{
		return src;
	}
	//hue calculate
	hsv.y = fdelta/fmax;
	if(fmax == src.r)
	{
		if(src.g >= src.b)
		{
			hsv.x = (src.g - src.b)/fdelta;
			fs_off = (color2.g - color1.r)*hsv.x + color1.r;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end

			//hsv2rgb
			src.r = hsv.z;
			src.b = hsv.z*(1.0 - hsv.y);
			src.g = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);
		}
		else
		{
			hsv.x = (src.r - src.b)/fdelta;
			fs_off = (color1.r - color2.r)*hsv.x + color2.r;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end
			//hsv2rgb
			src.r = hsv.z;
			src.g = hsv.z*(1.0 - hsv.y);
			src.b = hsv.z*(1.0 - hsv.y*hsv.x);
		}
	}
	else if(fmax == src.g)
	{

		if(src.r > src.b)
		{
			hsv.x = (src.g - src.r)/fdelta;
			fs_off = (color1.g - color2.g)*hsv.x + color2.g;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end
			//hsv2rgb
			src.g = hsv.z;
			src.r = hsv.z*(1.0 - hsv.y*hsv.x);
			src.b = hsv.z*(1.0 - hsv.y);
		}
		else
		{
			//2
			hsv.x = (src.b - src.r)/fdelta;
			fs_off = (color2.b - color1.g)*hsv.x + color1.g;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end
			//hsv2rgb
			src.g = hsv.z;
			src.r = hsv.z*(1.0 - hsv.y);
			src.b = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);
		}
	}
	else
	{
		if(src.g > src.r)
		{
			hsv.x = (src.b - src.g)/fdelta;
			fs_off = (color1.b - color2.b)*hsv.x + color2.b;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end
			//hsv2rgb
			src.b = hsv.z;
			src.r = hsv.z*(1.0 - hsv.y);
			src.g = hsv.z*(1.0 - hsv.y*hsv.x);
		}
		else
		{
			//4
			hsv.x = (src.r - src.g)/fdelta;
			fs_off = (color2.r - color1.b)*hsv.x + color1.b;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end
			//hsv2rgb
			src.b = hsv.z;
			src.r = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);
			src.g = hsv.z*(1.0 - hsv.y);
		}
	}
	return src;
}

void main(void)
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec4 tex = texture2D(SamplerBackTex1, textureCoordinate);

	vec3 tmp = hsvAdjust(src.rgb, vec3(-0.45, -0.45, -0.45), vec3(-0.45, -0.45, -0.45));
	vec3 dst_a = vec3(curveArray1[int(tmp.r * curvePrecision)].r,
		curveArray1[int(tmp.g * curvePrecision)].g,
		curveArray1[int(tmp.b * curvePrecision)].b);

	vec3 dst_b =vec3(curveArray2[int(tmp.r * curvePrecision)].r,
		curveArray2[int(tmp.g * curvePrecision)].g,
		curveArray2[int(tmp.b * curvePrecision)].b);

	gl_FragColor = vec4(mix(dst_a, dst_b, tex.g * tex.a),src.a);
}
);

static CGEConstString s_fshE76 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D SamplerBackTex1;

uniform vec3 curveArray1[256];
uniform vec3 curveArray2[256];

const float curvePrecision = 255.0;

vec3 prcSaturate50(vec3 src)
{
	return src * mat3(
		0.650, 0.2935, 0.057,
		0.1495, 0.7935, 0.057,
		0.1495, 0.2935, 0.557
		);
}

void main(void)
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec4 tex = texture2D(SamplerBackTex1, textureCoordinate);
	src.rgb = prcSaturate50(src.rgb);
	vec3 dst_a = vec3(curveArray1[int(src.r * curvePrecision)].r,
		curveArray1[int(src.g * curvePrecision)].g,
		curveArray1[int(src.b * curvePrecision)].b);
	vec3 dst_b = vec3(curveArray2[int(src.r * curvePrecision)].r,
		curveArray2[int(src.g * curvePrecision)].g,
		curveArray2[int(src.b * curvePrecision)].b);
	gl_FragColor = vec4(mix(dst_a, dst_b, tex.g * tex.a), src.a);
}
);

static CGEConstString s_fshE79 = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D SamplerBackTex1;

uniform vec3 curveArray1[256];
uniform vec3 curveArray2[256];

const float curvePrecision = 255.0;

vec3 hsvAdjust(vec3 src, vec3 color1, vec3 color2) //color1:red green blue, color2: magenta yellow cyan
{
	float fmax = max(src.r,max(src.g,src.b));
	float fmin = min(src.r,min(src.g,src.b));
	float fdelta = fmax - fmin;
	float fs_off;
	vec3 hsv;
	hsv.z = fmax;
	if(0.0 == fdelta)
	{
		return src;
	}
	//hue calculate
	hsv.y = fdelta/fmax;
	if(fmax == src.r)
	{
		if(src.g >= src.b)
		{
			hsv.x = (src.g - src.b)/fdelta;
			fs_off = (color2.g - color1.r)*hsv.x + color1.r;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end

			//hsv2rgb
			src.r = hsv.z;
			src.b = hsv.z*(1.0 - hsv.y);
			src.g = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);
		}
		else
		{
			hsv.x = (src.r - src.b)/fdelta;
			fs_off = (color1.r - color2.r)*hsv.x + color2.r;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end
			//hsv2rgb
			src.r = hsv.z;
			src.g = hsv.z*(1.0 - hsv.y);
			src.b = hsv.z*(1.0 - hsv.y*hsv.x);
		}
	}
	else if(fmax == src.g)
	{

		if(src.r > src.b)
		{
			hsv.x = (src.g - src.r)/fdelta;
			fs_off = (color1.g - color2.g)*hsv.x + color2.g;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end
			//hsv2rgb
			src.g = hsv.z;
			src.r = hsv.z*(1.0 - hsv.y*hsv.x);
			src.b = hsv.z*(1.0 - hsv.y);
		}
		else
		{
			//2
			hsv.x = (src.b - src.r)/fdelta;
			fs_off = (color2.b - color1.g)*hsv.x + color1.g;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end
			//hsv2rgb
			src.g = hsv.z;
			src.r = hsv.z*(1.0 - hsv.y);
			src.b = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);
		}
	}
	else
	{
		if(src.g > src.r)
		{
			hsv.x = (src.b - src.g)/fdelta;
			fs_off = (color1.b - color2.b)*hsv.x + color2.b;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end
			//hsv2rgb
			src.b = hsv.z;
			src.r = hsv.z*(1.0 - hsv.y);
			src.g = hsv.z*(1.0 - hsv.y*hsv.x);
		}
		else
		{
			//4
			hsv.x = (src.r - src.g)/fdelta;
			fs_off = (color2.r - color1.b)*hsv.x + color1.b;
			//saturation adjust
			hsv.y = hsv.y*(1.0 + fs_off);
			hsv.y = min(1.0 , hsv.y);
			hsv.y = max(0.0 , hsv.y);
			//rgb2hsv end
			//hsv2rgb
			src.b = hsv.z;
			src.r = hsv.z*(1.0 - hsv.y + hsv.y*hsv.x);
			src.g = hsv.z*(1.0 - hsv.y);
		}
	}
	return src;
}

void main(void)
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec4 tex = texture2D(SamplerBackTex1, textureCoordinate);

	src.rgb = hsvAdjust(src.rgb, vec3(-0.35, -0.35, -0.35), vec3(-0.35, -0.35, -0.35));
	vec3 dst_a = vec3(curveArray1[int(src.r * curvePrecision)].r,
		curveArray1[int(src.g * curvePrecision)].g,
		curveArray1[int(src.b * curvePrecision)].b);
	vec3 dst_b = vec3(curveArray2[int(src.r * curvePrecision)].r,
		curveArray2[int(src.g * curvePrecision)].g,
		curveArray2[int(src.b * curvePrecision)].b);

	gl_FragColor = vec4(mix(dst_a, dst_b, tex.g * tex.a),src.a);
}
);

//////////////////////////////////////////////////////////////////////////

static CGEConstString paramStepsFactorName = "vSteps";
static CGEConstString paramVignetteName = "vignette";
static CGEConstString paramBlendTextureName1 = "SamplerBackTex1";
//static CGEConstString paramBlendTextureName2 = "SamplerBackTex2";
//static CGEConstString paramBlendTextureName3 = "SamplerBackTex3";
static CGEConstString paramCurve1Name = "curveArray1";
static CGEConstString paramCurve2Name = "curveArray2";

namespace CGE
{
	bool CGESpecialFilterE16::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE16);
	}

	bool CGESpecialFilterE21::init()
	{
		UniformParameters* param = new UniformParameters;
		param->requireStepsFactor(paramStepsFactorName);
		setAdditionalUniformParameter(param);
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE21);
	}

	bool CGESpecialFilterE28::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE28))
		{
			m_program.bind();
			setColorScale(0.01f, 0.15f);
			m_program.sendUniformf(paramVignetteName, 0.0f, 0.8f);
			setSaturation(-1.0f);
			return true;
		}
		return false;
	}

	bool CGESpecialFilterE29::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE29))
		{
			setColorScale(0.2f, 0.2f);
			setSaturation(-1.0f);
			return true;
		}
		return false;
	}

	bool CGESpecialFilterE30To32::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE30To32))
		{
			setSaturation(-1.0f);
			return true;
		}
		return false;
	}

	bool CGEFilterColorScaleAndTex1::assignSamplerID(GLuint texID)
	{
		return (m_texture = texID) != (GLuint)0;
	}

	void CGEFilterColorScaleAndTex1::initSampler()
	{
		UniformParameters* param;
		if(m_uniformParam == NULL) param = new UniformParameters;
		else param = m_uniformParam;
		param->pushSampler2D(paramBlendTextureName1, &m_texture, 0);
		setAdditionalUniformParameter(param);
	}

	CGEConstString CGESpecialFilterE39::paramTextureNmae = "e39_t.jpg";

	bool CGESpecialFilterE39::initWithinCommonFilter(CGEMutipleEffectFilter* htProc)
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE39))
		{
			setColorScale(0.01f, 0.01f);
			setSaturation(0.3f);
			initSampler();
			return assignSamplerID(htProc->loadResources(paramTextureNmae));
		}
		return false;
	}

	bool CGESpecialFilterE87::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE87);
	}

	bool CGESpecialFilterE88::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE88))
		{
			UniformParameters* param = new UniformParameters;
			param->requireStepsFactor(paramStepsFactorName);
			setAdditionalUniformParameter(param);
			return true;
		}
		return false;
	}

	bool CGESpecialFilterE89::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE89))
		{
			UniformParameters* param = new UniformParameters;
			param->requireStepsFactor(paramStepsFactorName);
			setAdditionalUniformParameter(param);
			return true;
		}
		return false;
	}

	CGEConstString CGESpecialFilterE90::paramColorGradientName = "colorGradient";
	const GLfloat CGESpecialFilterE90::paramColorGradientValue[] = 
	{
		0.0f, 0.0f, 32768.0f,
		8000.0f, 7000.0f, 24576.0f,
		16000.0f, 14000.0f, 16384.0f,
		24000.0f, 21000.0f, 8192.0f,
		32000.0f, 28000.0f, 0.0f 
	};

	bool CGESpecialFilterE90::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE90))
		{
			m_program.bind();
			GLint index = glGetUniformLocation(m_program.programID(), paramColorGradientName);
			if(index < 0)
			{
				CGE_LOG_ERROR("CGESpecialFilterE90 - uniform %s doesnot exist!\n", paramColorGradientName);
			}
			else glUniform3fv(index, 5, paramColorGradientValue);
			return true;
		}
		return false;
	}

	const GLfloat CGESpecialFilterE96::paramColorGradientValue[] =
	{
		0.0f, 8000.0f, 25000.0f,
		0.0f, 32768.0f, 0.0f,
		32768.0f, 32768.0f, 0.0f,
		32768.0f, 8000.0f, 0.0f,
		32768.0f, 0.0f, 0.0f 
	};

	bool CGESpecialFilterE96::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE90))
		{
			m_program.bind();
			GLint index = glGetUniformLocation(m_program.programID(), paramColorGradientName);
			if(index < 0)
			{
				CGE_LOG_ERROR("CGESpecialFilterE96 - uniform %s doesnot exist!\n", paramColorGradientName);
			}
			else glUniform3fv(index, 5, paramColorGradientValue);
			return true;
		}
		return false;
	}

	bool CGESpecialFilterE91::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE91))
		{
			m_program.bind();
			m_program.sendUniformf(paramVignetteName, 0.0f, 0.85f);
			return true;
		}
		return false;
	}

	CGEConstString CGESpecialFilterE92::paramHalfToneName = "halfToneVars";

	bool CGESpecialFilterE92::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE92);
	}

	void CGESpecialFilterE92::getHalfToneLowFac(GLfloat* low, GLfloat* fac, GLint w, GLint h)
	{
		unsigned char *data = (unsigned char*)malloc(w*h*4 * sizeof(GLubyte)), *tmpBuffer = data;
		if(data == NULL)
		{
			*low = 0.2f;
			*fac = 1.4f;
			return;
		}

		glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

		int lum[256] = {0};
		int total = w * h;

		for(int i = 0; i != total; ++i)
		{
			++lum[(unsigned char)(data[0] * 0.299 + data[1] * 0.587 + data[2] * 0.114)];
			data += 4;
		}

		for(int i = 1; i != 256; ++i)
		{
			lum[i] += lum[i-1];
		}

		*low = 0.0f;
		for(int i = 0; i != 256; ++i)
		{
			if(lum[i] / (GLfloat)lum[255] > 0.1f)
			{
				*low = (GLfloat)i;
				break;
			}
		}
		int up = 255;
		for(int i = 255; i > 0; --i)
		{
			if((GLfloat)(lum[255] - lum[i]) / lum[255] > 0.1)
			{
				up = i;
				break;
			}
		}
		*fac = 255.0f / (up - *low);
		*low /= 255.0f;
		free(tmpBuffer);
	}

	void CGESpecialFilterE92::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{
		handler->setAsTarget();
		m_program.bind();
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, posVertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, srcTexture);
		m_program.sendUniformi(paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		if(m_uniformParam != NULL)
			m_uniformParam->assignUniforms(handler, m_program.programID());
		//More initialization...
		{
			const CGESizei& sz = handler->getOutputFBOSize();
			float low, range;

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTexture, 0);
            CGE_LOG_CODE(clock_t t = clock();)
			getHalfToneLowFac(&low, &range, sz.width, sz.height);
			CGE_LOG_INFO("getHalfToneLowFac用时%gs\n", (double)(clock() -t) / CLOCKS_PER_SEC);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handler->getTargetTextureID(), 0);
			m_program.sendUniformf(paramHalfToneName, low, range);
			m_program.sendUniformf(paramStepsFactorName, 1.0f / sz.width, 1.0f / sz.height);
		}

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	bool CGESpecialFilterE99::init()
	{
		return initShadersFromString(g_vshDefaultWithoutTexCoord,s_fshE99);
	}

	bool CGESpecialFilterE118::init()
	{
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE118))
		{
			setColorScale(0.01f, 0.2f);
			setSaturation(-1.0f);
			return true;
		}
		return false;
	}

	CGEConstString CGESpecialFilterE73To80Help::paramTextureName = "vtg.png";

	bool CGESpecialFilterE73To80Help::assignSamplerID(GLuint texID)
	{
		return (m_texture = texID) != (GLuint)0;
	}

	void CGESpecialFilterE73To80Help::initSampler()
	{
		UniformParameters* param;
		if(m_uniformParam == NULL) param = new UniformParameters;
		else param = m_uniformParam;
		param->pushSampler2D(paramBlendTextureName1, &m_texture, 0);
		setAdditionalUniformParameter(param);
	}


	bool CGESpecialFilterE73::initWithinCommonFilter(CGEMutipleEffectFilter* htProc)
	{
		
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE73))
		{
			m_program.bind();

			/////////          Curve   1    ////////

			const int precision = CGECurveInterface::getPrecision();
			std::vector<float> vecR, vecG, vecB;
			std::vector<float> vecRGB(precision * 3);

			CGECurveInterface::CurvePoint pnt1R[4] = {
				{0.0627451f, 0.0f},{0.541176f, 0.521569f},{0.792157f, 0.72549f},{1.0f, 0.835294f}
			};
			CGECurveInterface::genCurve(vecR, pnt1R, 4);

			CGECurveInterface::CurvePoint pnt1G[4] = {
				{0.321569f, 0.0f},{0.501961f, 0.333333f},{0.678431f, 0.568627f},{1.0f, 0.811765f}
			};
			CGECurveInterface::genCurve(vecG, pnt1G, 4);

			CGECurveInterface::CurvePoint pnt1B[3] = {
				{0.423529f, 0.243137f},{0.690196f, 0.537255f},{1.0f, 0.721569f}
			};
			CGECurveInterface::genCurve(vecB, pnt1B, 3);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve1Name), precision, vecRGB.data());

			//////////////  Curve 2        /////////

			CGECurveInterface::CurvePoint pnt2R[5] = {
				{0.0f, 0.411765f},{0.239216f, 0.654902f},{0.501961f, 0.866667f},{0.745098f, 0.92549f},{1.0f, 1.0f}
			};
			CGECurveInterface::genCurve(vecR, pnt2R, 5);

			CGECurveInterface::CurvePoint pnt2G[5] = {
				{0.0117647f, 0.0f},{0.25098f, 0.384314f},{0.509804f, 0.74902f},{0.74902f, 0.894118f},{1.0f, 1.0f}
			};
			CGECurveInterface::genCurve(vecG, pnt2G, 5);

			CGECurveInterface::CurvePoint png2B[5] = {
				{0.211765f, 0.243137f},{0.337255f, 0.396078f},{0.505882f, 0.607843f},{0.74902f, 0.752941f},{1.0f, 0.870588f}
			};
			CGECurveInterface::genCurve(vecB, png2B, 5);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve2Name), precision, vecRGB.data());

			initSampler();
			return assignSamplerID(htProc->loadResources(paramTextureName));
		}
		return false;
	}

	bool CGESpecialFilterE74::initWithinCommonFilter(CGEMutipleEffectFilter* htProc)
	{
		
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE74))
		{
			m_program.bind();
			const int precision = CGECurveInterface::getPrecision();
			std::vector<float> vecR, vecG, vecB;
			std::vector<float> vecRGB(precision * 3);

			CGECurveInterface::CurvePoint pnt1R[3] = {
				{0.247059f, 0.137255f},{0.580392f, 0.454902f},{1.0f, 0.815686f}
			};
			CGECurveInterface::genCurve(vecR, pnt1R, 3);

			CGECurveInterface::CurvePoint pnt1G[3] = {
				{0.278431f, 0.0f},{0.584314f, 0.329412f},{1.0f, 0.733333f}
			};
			CGECurveInterface::genCurve(vecG, pnt1G, 3);

			CGECurveInterface::CurvePoint pnt1B[3] = {
				{0.333333f, 0.0862745f},{0.694118f, 0.341176f},{1.0f, 0.580392f}
			};
			CGECurveInterface::genCurve(vecB, pnt1B, 3);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve1Name), precision, vecRGB.data());

			//////////////  Curve 2        /////////

			CGECurveInterface::CurvePoint pnt2R[5] = {
				{0.0196078f, 0.164706f},{0.305882f, 0.505882f},{0.458824f, 0.698039f},{0.713726f, 0.858824f},{0.988235f, 0.992157f}
			};
			CGECurveInterface::genCurve(vecR, pnt2R, 5);

			CGECurveInterface::CurvePoint pnt2G[5] = {
				{0.0f, 0.0f},{0.247059f, 0.329412f},{0.458824f, 0.615686f},{0.678431f, 0.8f},{1.0f, 0.945098f}
			};
			CGECurveInterface::genCurve(vecG, pnt2G, 5);

			CGECurveInterface::CurvePoint pnt2B[4] = {
				{0.0313726f, 0.101961f},{0.509804f, 0.576471f},{0.745098f, 0.772549f},{1.0f, 0.85098f}
			};
			CGECurveInterface::genCurve(vecB, pnt2B, 4);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve2Name), precision, vecRGB.data());

			initSampler();
			return assignSamplerID(htProc->loadResources(paramTextureName));
		}
		return false;
	}

	bool CGESpecialFilterE75::initWithinCommonFilter(CGEMutipleEffectFilter* htProc)
	{
		
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE73))
		{
			m_program.bind();

			/////////          Curve   1    ////////

			const int precision = CGECurveInterface::getPrecision();
			std::vector<float> vecR, vecG, vecB;
			std::vector<float> vecRGB(precision * 3);

			CGECurveInterface::CurvePoint pnt1R[4] = {
				{0.278431f, 0.0f},{0.439216f, 0.243137f},{0.615686f, 0.580392f},{1.0f, 0.909804f}
			};
			CGECurveInterface::genCurve(vecR, pnt1R, 4);

			CGECurveInterface::CurvePoint pnt1G[4] = {
				{0.32549f, 0.0f},{0.541176f, 0.443137f},{0.67451f, 0.678431f},{1.0f, 0.87451f}
			};
			CGECurveInterface::genCurve(vecG, pnt1G, 4);

			CGECurveInterface::CurvePoint pnt1B[2] = {
				{0.0627451f, 0.0f},{1.0f, 0.760784f}
			};
			CGECurveInterface::genCurve(vecB, pnt1B, 2);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve1Name), precision, vecRGB.data());

			//////////////  Curve 2        /////////

			CGECurveInterface::CurvePoint pnt2R[4] = {
				{0.168627f, 0.0f},{0.427451f, 0.345098f},{0.576471f, 0.654902f},{0.843137f, 1.0f}
			};
			CGECurveInterface::genCurve(vecR, pnt2R, 4);

			CGECurveInterface::CurvePoint pnt2G[4] = {
				{0.188235f, 0.00392157f},{0.490196f, 0.458824f},{0.678431f, 0.827451f},{0.937255f, 1.0f}
			};
			CGECurveInterface::genCurve(vecG, pnt2G, 4);

			CGECurveInterface::CurvePoint png2B[4] = {
				{0.0f, 0.0980392f},{0.392157f, 0.411765f},{0.741176f, 0.690196f},{1.0f, 0.894118f},
			};
			CGECurveInterface::genCurve(vecB, png2B, 4);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve2Name), precision, vecRGB.data());

			initSampler();
			return assignSamplerID(htProc->loadResources(paramTextureName));
		}
		return false;
	}

	bool CGESpecialFilterE76::initWithinCommonFilter(CGEMutipleEffectFilter* htProc)
	{
		
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE76))
		{
			m_program.bind();

			/////////          Curve   1    ////////

			const int precision = CGECurveInterface::getPrecision();
			std::vector<float> vecR, vecG, vecB;
			std::vector<float> vecRGB(precision * 3);

			CGECurveInterface::CurvePoint pnt1R[5] = {
				{0.0156863f, 0.137255f},{0.278431f, 0.309804f},{0.54902f, 0.627451f},{0.698039f, 0.87451f},{0.819608f, 1.0f}
			};
			CGECurveInterface::genCurve(vecR, pnt1R, 5);
			
			CGECurveInterface::CurvePoint pnt1G[5] = {
				{0.0352941f, 0.0f},{0.286275f, 0.239216f},{0.54902f, 0.635294f},{0.796078f, 0.878431f},{0.972549f, 0.972549f}
			};
			CGECurveInterface::genCurve(vecG, pnt1G, 5);

			CGECurveInterface::CurvePoint pnt1B[6] = {
				{0.0f, 0.121569f},{0.431373f, 0.4f},{0.568627f, 0.580392f},{0.764706f, 0.717647f},{0.913725f, 0.835294f},{0.992157f, 0.92549f}
			};
			CGECurveInterface::genCurve(vecB, pnt1B, 6);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve1Name), precision, vecRGB.data());

			//////////////  Curve 2        /////////

			CGECurveInterface::CurvePoint pnt2R[5] = {
				{0.0196078f, 0.145098f},{0.270588f, 0.333333f},{0.466667f, 0.592157f},{0.615686f, 0.835294f},{0.815686f, 1.0f}
			};
			CGECurveInterface::genCurve(vecR, pnt2R, 5);

			CGECurveInterface::CurvePoint pnt2G[5] = {
				{0.0509804f, 0.0196078f},{0.290196f, 0.305882f},{0.427451f, 0.564706f},{0.611765f, 0.788235f},{0.980392f, 0.980392f}
			};
			CGECurveInterface::genCurve(vecG, pnt2G, 5);

			CGECurveInterface::CurvePoint png2B[6] = {
				{0.00784314f, 0.141176f},{0.247059f, 0.286275f},{0.364706f, 0.439216f},{0.686275f, 0.745098f},{0.945098f, 0.890196f},{1.0f, 0.921569f}
			};
			CGECurveInterface::genCurve(vecB, png2B, 6);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve2Name), precision, vecRGB.data());

			initSampler();
			return assignSamplerID(htProc->loadResources(paramTextureName));
		}
		return false;
	}

	bool CGESpecialFilterE77::initWithinCommonFilter(CGEMutipleEffectFilter* htProc)
	{
		
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE73))
		{
			m_program.bind();

			/////////          Curve   1    ////////

			const int precision = CGECurveInterface::getPrecision();
			std::vector<float> vecR, vecG, vecB;
			std::vector<float> vecRGB(precision * 3);

			CGECurveInterface::CurvePoint pnt1R[4] = {
				{0.0627451f, 0.0f},{0.541176f, 0.521569f},{0.792157f, 0.72549f},{1.0f, 0.835294f}
			};
			CGECurveInterface::genCurve(vecR, pnt1R, 4);

			CGECurveInterface::CurvePoint pnt1G[4] = {
				{0.321569f, 0.0f},{0.501961f, 0.333333f},{0.678431f, 0.568627f},{1.0f, 0.811765f}
			};
			CGECurveInterface::genCurve(vecG, pnt1G, 4);

			CGECurveInterface::CurvePoint pnt1B[3] = {
				{0.423529f, 0.243137f},{0.690196f, 0.537255f},{1.0f, 0.721569f}
			};
			CGECurveInterface::genCurve(vecB, pnt1B, 3);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve1Name), precision, vecRGB.data());

			//////////////  Curve 2        /////////

			CGECurveInterface::CurvePoint pnt2R[5] = {
				{0.0f, 0.411765f},{0.239216f, 0.654902f},{0.501961f, 0.866667f},{0.745098f, 0.92549f},{1.0f, 1.0f},
			};
			CGECurveInterface::genCurve(vecR, pnt2R, 5);

			CGECurveInterface::CurvePoint pnt2G[5] = {
				{0.0117647f, 0.0f},{0.25098f, 0.384314f},{0.509804f, 0.74902f},{0.74902f, 0.894118f},{1.0f, 1.0f}
			};
			CGECurveInterface::genCurve(vecG, pnt2G, 5);

			CGECurveInterface::CurvePoint png2B[5] = {
				{0.211765f, 0.243137f},{0.337255f, 0.396078f},{0.505882f, 0.607843f},{0.74902f, 0.752941f},{1.0f, 0.870588f}
			};
			CGECurveInterface::genCurve(vecB, png2B, 5);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve2Name), precision, vecRGB.data());

			initSampler();
			return assignSamplerID(htProc->loadResources(paramTextureName));
		}
		return false;
	}

	bool CGESpecialFilterE78::initWithinCommonFilter(CGEMutipleEffectFilter* htProc)
	{
		
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE73))
		{
			m_program.bind();

			/////////          Curve   1    ////////

			const int precision = CGECurveInterface::getPrecision();
			std::vector<float> vecR, vecG, vecB;
			std::vector<float> vecRGB(precision * 3);

			CGECurveInterface::CurvePoint pnt1R[5] = {
				{0.0627451f, 0.0980392f},{0.231373f, 0.301961f},{0.396078f, 0.560784f},{0.678431f, 0.827451f},{1.0f, 0.929412f}
			};
			CGECurveInterface::genCurve(vecR, pnt1R, 5);

			CGECurveInterface::CurvePoint pnt1G[5] = {
				{0.0941176f, 0.027451f},{0.333333f, 0.286275f},{0.486275f, 0.584314f},{0.72549f, 0.811765f},{0.976471f, 0.898039f}
			};
			CGECurveInterface::genCurve(vecG, pnt1G, 5);

			CGECurveInterface::CurvePoint pnt1B[5] = {
				{0.117647f, 0.027451f},{0.411765f, 0.305882f},{0.588235f, 0.580392f},{0.784314f, 0.756863f},{1.0f, 0.8f},
			};
			CGECurveInterface::genCurve(vecB, pnt1B, 5);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve1Name), precision, vecRGB.data());

			//////////////  Curve 2        /////////

			CGECurveInterface::CurvePoint pnt2R[5] = {
				{0.0627451f, 0.156863f},{0.203922f, 0.435294f},{0.486275f, 0.760784f},{0.67451f, 0.878431f},{0.905882f, 0.988235f}
			};
			CGECurveInterface::genCurve(vecR, pnt2R, 5);

			CGECurveInterface::CurvePoint pnt2G[5] = {
				{0.0745098f, 0.0196078f},{0.278431f, 0.329412f},{0.392157f, 0.568627f},{0.654902f, 0.831373f},{0.870588f, 0.980392f}
			};
			CGECurveInterface::genCurve(vecG, pnt2G, 5);

			CGECurveInterface::CurvePoint png2B[4] = {
				{0.0823529f, 0.0156863f},{0.262745f, 0.196078f},{0.560784f, 0.654902f},{0.929412f, 0.894118f}
			};
			CGECurveInterface::genCurve(vecB, png2B, 4);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve2Name), precision, vecRGB.data());

			initSampler();
			return assignSamplerID(htProc->loadResources(paramTextureName));
		}
		return false;
	}

	bool CGESpecialFilterE79::initWithinCommonFilter(CGEMutipleEffectFilter* htProc)
	{
		
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE79))
		{
			m_program.bind();

			/////////          Curve   1    ////////

			const int precision = CGECurveInterface::getPrecision();
			std::vector<float> vecR, vecG, vecB;
			std::vector<float> vecRGB(precision * 3);

			CGECurveInterface::CurvePoint pnt1R[3] = {
				{0.282353f, 0.14902f},{0.776471f, 0.588235f},{1.0f, 0.694118f},
			};
			CGECurveInterface::genCurve(vecR, pnt1R, 3);

			CGECurveInterface::CurvePoint pnt1G[3] = {
				{0.239216f, 0.0862745f},{0.639216f, 0.52549f},{1.0f, 0.784314f}
			};
			CGECurveInterface::genCurve(vecG, pnt1G, 3);

			CGECurveInterface::CurvePoint pnt1B[3] = {
				{0.282353f, 0.0117647f},{0.764706f, 0.52549f},{1.0f, 0.690196f}
			};
			CGECurveInterface::genCurve(vecB, pnt1B, 3);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve1Name), precision, vecRGB.data());

			//////////////  Curve 2        /////////

			CGECurveInterface::CurvePoint pnt2R[5] = {
				{0.0392157f, 0.168627f},{0.243137f, 0.439216f},{0.45098f, 0.690196f},{0.643137f, 0.85098f},{1.0f, 0.952941f}
			};
			CGECurveInterface::genCurve(vecR, pnt2R, 5);

			CGECurveInterface::CurvePoint pnt2G[6] = {
				{0.0117647f, 0.109804f},{0.207843f, 0.380392f},{0.32549f, 0.568627f},{0.427451f, 0.721569f},{0.705882f, 0.905882f},{0.94902f, 0.988235f},
			};
			CGECurveInterface::genCurve(vecG, pnt2G, 6);

			CGECurveInterface::CurvePoint png2B[6] = {
				{0.0313726f, 0.0313726f},{0.270588f, 0.4f},{0.388235f, 0.572549f},{0.65098f, 0.870588f},{0.768627f, 0.921569f},{1.0f, 0.945098f}
			};
			CGECurveInterface::genCurve(vecB, png2B, 6);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve2Name), precision, vecRGB.data());

			initSampler();
			return assignSamplerID(htProc->loadResources(paramTextureName));
		}
		return false;
	}

	bool CGESpecialFilterE80::initWithinCommonFilter(CGEMutipleEffectFilter* htProc)
	{
		
		if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshE73))
		{
			m_program.bind();

			/////////          Curve   1    ////////

			const int precision = CGECurveInterface::getPrecision();
			std::vector<float> vecR, vecG, vecB;
			std::vector<float> vecRGB(precision * 3);

			CGECurveInterface::CurvePoint pnt1R[4] = {
				{0.0862745f, 0.0f},{0.470588f, 0.305882f},{0.815686f, 0.541176f},{1.0f, 0.666667f}
			};
			CGECurveInterface::genCurve(vecR, pnt1R, 4);

			CGECurveInterface::CurvePoint pnt1G[3] = {
				{0.301961f, 0.0f},{0.560784f, 0.270588f},{1.0f, 0.690196f}
			};
			CGECurveInterface::genCurve(vecG, pnt1G, 3);

			CGECurveInterface::CurvePoint pnt1B[3] = {
				{0.176471f, 0.0f},{0.623529f, 0.380392f},{1.0f, 0.678431f}
			};
			CGECurveInterface::genCurve(vecB, pnt1B, 3);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve1Name), precision, vecRGB.data());

			//////////////  Curve 2        /////////

			CGECurveInterface::CurvePoint pnt2R[3] = {
				{0.027451f, 0.670588f},{0.360784f, 0.788235f},{1.0f, 0.992157f}
			};
			CGECurveInterface::genCurve(vecR, pnt2R, 3);

			CGECurveInterface::CurvePoint pnt2G[5] = {
				{0.0f, 0.0117647f},{0.239216f, 0.309804f},{0.431373f, 0.556863f},{0.729412f, 0.788235f},{1.0f, 1.0f}
			};
			CGECurveInterface::genCurve(vecG, pnt2G, 5);

			CGECurveInterface::CurvePoint png2B[5] = {
				{0.0f, 0.0901961f},{0.207843f, 0.341176f},{0.403922f, 0.54902f},{0.729412f, 0.815686f},{0.937255f, 0.972549f}
			};
			CGECurveInterface::genCurve(vecB, png2B, 5);

			for(int i = 0; i != precision; ++i)
			{
				const int j = i * 3;
				vecRGB[j] = vecR[i];
				vecRGB[j + 1] = vecG[i];
				vecRGB[j + 2] = vecB[i];
			}

			glUniform3fv(m_program.uniformLocation(paramCurve2Name), precision, vecRGB.data());

			initSampler();
			return assignSamplerID(htProc->loadResources(paramTextureName));
		}
		return false;
	}

}