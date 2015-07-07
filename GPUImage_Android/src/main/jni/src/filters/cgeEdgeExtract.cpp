/*
 * cgeFastTiltShift.cpp
 *
 *  Created on: 2015-04-5
 *      Author: Su Zhiyun
 */

#include "cgeEdgeExtract.h"
#include <cmath>


static CGEConstString s_fshFilter = CGE_SHADER_STRING_PRECISION_H
	(
	varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 sft;


void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec4 dst =  src * vec4(12.0);

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0,-1.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0,-1.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0, 1.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 1.0));
	dst += src;

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0, 0.0));
	vec4 dst1 = src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 0.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0,-1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0, 1.0));
	dst1 += src;

	dst += dst1 * vec4(4.0);

	gl_FragColor = dst * vec4(1.0 / 32.0);
}
);

static CGEConstString s_fshDownOdd = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 sft;


void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec4 dst =  src * vec4(20.0);

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-2.0, 0.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 2.0, 0.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0,-2.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0, 2.0));
	dst += src;

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0,-1.0));
	vec4 dst1 = src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0,-1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0, 1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 1.0));
	dst1 += src;

	dst += dst1 * vec4(2.0);

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0, 0.0));
	dst1 = src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 0.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0,-1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0, 1.0));
	dst1 += src;

	dst += dst1 * vec4(8.0);

	gl_FragColor = dst * vec4(1.0 / 64.0);
}
);


static CGEConstString s_fshDownEvn = CGE_SHADER_STRING_PRECISION_H
	(
	varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 sft;


void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate - sft);
	vec4 dst =  src * vec4(20.0);

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-5.0,-5.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 3.0,-5.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-5.0, 3.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 3.0, 3.0));
	dst += src;

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-5.0,-1.0));
	vec4 dst1 = src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 3.0,-1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0,-5.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0, 3.0));
	dst1 += src;

	dst += dst1 * vec4(2.0);

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-3.0,-3.0));
	dst1 = src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0,-3.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-3.0, 1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 1.0));
	dst1 += src;

	dst += dst1 * vec4(8.0);

	gl_FragColor = dst * vec4(1.0 / 64.0);
}
);

static CGEConstString s_fshResize = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 fac;


void main()
{
	gl_FragColor = texture2D(inputImageTexture, textureCoordinate * fac);
}
);


static CGEConstString s_fshEdge = CGE_SHADER_STRING_PRECISION_H
	(
	varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 sft;
uniform float evn;
const vec4 weight = vec4(0.299,0.587,0.114,0.0);



const mat4 m1 = mat4
	(
	-1.0/6.0, 0.0    , 1.0/6.0,-1.0/6.0,
	-1.0/6.0,-1.0/6.0,-1.0/6.0, 0.0    ,

	 0.1,-0.2, 0.1 , 0.3,
	 0.1, 0.3, 0.1 ,-0.2
	);


const mat4 m2 = mat4
	(
	 1.0/6.0,-1.0/6.0, 0.0    , 1.0/6.0,
	 0.0    , 1.0/6.0, 1.0/6.0, 1.0/6.0,

	 0.3, 0.1,-0.2, 0.1,
	-0.2, 0.1, 0.3, 0.1
	);


void main()
{
	vec4 v1;

	v1.x = dot(texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0 + evn,-1.0 - evn)),weight);

	v1.y = dot(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0 + evn,-1.0)),weight);

	v1.z = dot(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0 + evn,-1.0 + evn)),weight);

	v1.w = dot(texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0, 0.0 - evn)),weight);


	vec4 v2;

	v2.x = dot(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 0.0 + evn)),weight);

	v2.y = dot(texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0 - evn, 1.0 - evn)),weight);

	v2.z = dot(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0 - evn, 1.0)),weight);

	v2.w = dot(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0 - evn, 1.0 + evn)),weight);

	float z0 = dot(texture2D(inputImageTexture, textureCoordinate),weight);

	v1 -= vec4(z0);
	v2 -= vec4(z0);

	vec4 uvab = v1 * m1 + v2 * m2;// |u|,|v| < 0.4063

	float c = 0.25 * (v1.x - v1.z - v2.y + v2.w);

	vec4 uvab2 = uvab * uvab;

	float ee = (dot(uvab2,vec4(1.0,1.0,2.0,2.0)) + c * c);

	//vec2 k2;


	//k2.x =  dot(uvab2,vec4(1.0,-1.0,2.0,-2.0));

	//float uv2 = 2.0 * uvab.x * uvab.y;

	//float d2ss = 2.0 * c * (uvab.z + uvab.w);

	//k2.y =  uv2 + d2ss;

	//float A2 = dot(k2,k2);

	//float A = sqrt(A2);

	//float nf = 2.0 * ee * A / (A2 + ee * ee + 1e-12);

	//k2 = normalize(k2);

	//vec2 k;

	//k.y = sign(k2.y) * sqrt((1.0 - k2.x)*0.5);
	//k.x = sqrt((1.0 + k2.x)*0.5);

	//vec2 dtn = k * vec2(nf);

	//vec2 de;

	//de.x = dot(k2,vec2(uvab2.x - uvab2.y,uv2)) + uvab2.x + uvab2.y;
	//de.y = dot(k2,vec2(2.0*(uvab2.z - uvab2.w),d2ss)) + 2.0 * (uvab2.z + uvab2.w) + c * c;

	//de = sqrt(de);



	//float r1 = de.y * 2.0;
	//float r2 = de.x * 2.0;
	////float r = dot(de * de,vec2(1.0,1.0)) * 4.0;


	//float r2 = sqrt(uvab2.x + uvab2.y) * 2.0;
	//float r1 = sqrt(2.0 * (uvab2.z + uvab2.w) + c * c) * 2.0 / 3.0 * 5.8;

	//r1 = min(r1 * 12.92,pow(1.055 * r1,1.0/2.4) - 0.055);
	//r2 = min(r2 * 12.92,pow(1.055 * r2,1.0/2.4) - 0.055);

	float r = sqrt(ee)*2.0;

	//r = min(r * 12.92,pow(1.055 * r,1.0/2.4) - 0.055) * 4.0;


	vec4 dst;

	dst.rgb = vec3(r);//,r2,0.0);
	dst.a = 1.0;



	//abc *= vec4(1.0/0.4375);

	//abc = sign(abc) * pow(abs(abc),vec4(1.0/3.0));

	//abc = abc * vec4(0.5) + vec4(257.0/512.0);


	//vec4 dst;

	//dst.xyz = abc.xyz;
	//dst.w = 1.0;


	gl_FragColor =  dst;
}
);

static CGEConstString s_fshReg = CGE_SHADER_STRING_PRECISION_H
	(
	varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
//uniform vec2 sft;
//uniform float evn;
//uniform float edge_mul;
//uniform float edge_min;
//uniform float sat;
//
//
//const mat3 cm = mat3
//	(
//	1.0,  -0.000331743106699,   0.500000000000000,
//	1.0,  -0.122602264271553,  -0.254753736033794,
//	1.0,   0.632152541901295,   0.000353145887777
//	);
//
//vec3 extract(vec4 v0)
//{
//	vec3 v = v0.xyz;
//	v = (v - vec3(257.0/512.0))*vec3(2.0);
//	v = v * v * v;
//	v = v * vec3(0.4375);
//	return v;
//}

void main()
{
	//vec3 p1 = extract(texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0 + evn,-1.0 - evn)));

	//vec3 p2 = extract(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0 + evn,-1.0      )));

	//vec3 p3 = extract(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0 + evn,-1.0 + evn)));

	//vec3 p4 = extract(texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0      , 0.0 - evn)));

	//vec3 p5 = extract(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0      , 0.0      )));

	//vec3 p6 = extract(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0      , 0.0 + evn)));

	//vec3 p7 = extract(texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0 - evn, 1.0 - evn)));

	//vec3 p8 = extract(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0 - evn, 1.0      )));

	//vec3 p9 = extract(texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0 - evn, 1.0 + evn)));


	//vec3 abc2 = (p1 * p1 + p3 * p3 + p7  * p7 + p9 * p9) * vec3(0.6) + p2 * p2 + p4 * p4 + p6  * p6 + p8 * p8 + p5 * p5;

	//vec3 abcx = (p1 * p1.yzx + p3 * p3.yzx + p7  * p7.yzx + p9 * p9.yzx) * vec3(0.6) + p2.yzx * p2 + p4 * p4.yzx + p6  * p6.yzx + p8 * p8.yzx + p5 * p5.yzx;

	//float a2 = abc2.x;
	//float b2 = abc2.y;
	//float c2 = abc2.z;

	//float ab = abcx.x;
	//float bc = abcx.y;
	//float ac = abcx.z;

	//vec3 luv;


	//vec2 k2 = vec2(a2 - b2,ac + bc);

	//float A = length(k2);

	//k2 = k2 / vec2(A + 0.0001);

	//float th = 0.02;
	//float ee = a2 + b2 + c2 * 0.5 + th;
	//A = A / ee;


	//A = min(A * 12.92,pow(1.055 * A,1.0/2.4) - 0.055);

	//k2 = k2 * vec2(A * 0.25);

	//luv.x = A;
	//luv.yz = k2;


	////vec2 k;


	////k.y = sign(k2.y) * sqrt((1 - k2.x)*0.5);
	////k.x = sqrt((1 + k2.x)*0.5);


	//vec3 color = luv * cm;

	//vec4 dst;

	//dst.rgb = color;
	//dst.a = 1.0;


	vec4 src = texture2D(inputImageTexture, textureCoordinate );

	float r1 = max(dot(src,vec4(1.25,-0.5,-0.25,-0.125)),0.0);

	float r2 = max(dot(src,vec4(0.0,1.125,-0.5,-0.25)),0.0);

	float r3 = max(dot(src,vec4(0.0,0.0,0.875,-0.5)),0.0);
	//float r = dot(src,vec4(1.0,0.0,0.0,0.0));

	float r = r1 * 2.0 + r2 * 0.5 + r3 * 0.25;
	r *= 1.5;
	//r = min(r * 12.92,pow(1.055 * r,1.0/2.4) - 0.055);

	vec4 dst = vec4(1.0 - r);

	dst.a = 1.0;

	gl_FragColor =  dst;
}
);

static CGEConstString s_fshUpOdd = CGE_SHADER_STRING_PRECISION_H
	(
	varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 isft;
uniform vec2 sft;


void main()
{
	float odd = fract(floor(dot(textureCoordinate,isft) - 0.5) * 0.5) * 2.0;

	float evn = 1.0 - odd;

	vec4 dst = vec4(0.0);

	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	dst += src * vec4(20.0) * vec4(evn);


	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0 - evn, 0.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0 + evn, 0.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0,-1.0 - evn));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0, 1.0 + evn));
	dst += src;

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0,-1.0));
	vec4 dst1 = src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0,-1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0, 1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 1.0));
	dst1 += src;

	dst += dst1 * vec4(2.0) * vec4(evn);

	vec4 bs = vec4(1.0 / 4.0) - vec4(7.0 / 32.0) * vec4(evn);

	gl_FragColor = dst * bs;
}
);


static CGEConstString s_fshUpEvn = CGE_SHADER_STRING_PRECISION_H
	(
	varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 isft;
uniform vec2 sft;


void main()
{
	vec2 odd = fract(floor(textureCoordinate * isft) * vec2(0.5)) * vec2(2.0);

	float evn = 1.0 - odd.x * odd.y;
	float evn2 = evn * 2.0;

	vec4 dst = vec4(0.0);

	vec4 src = texture2D(inputImageTexture, textureCoordinate + sft);
	dst += src * vec4(20.0) * vec4(evn);


	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0 - evn2,-1.0 - evn2));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 3.0 + evn2,-1.0 - evn2));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0 - evn2, 3.0 + evn2));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 3.0 + evn2, 3.0 + evn2));
	dst += src;

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-3.0, 1.0));
	vec4 dst1 = src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 5.0, 1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0,-3.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 5.0));
	dst1 += src;

	dst += dst1 * vec4(2.0) * vec4(evn);

	vec4 bs = vec4(1.0 / 4.0) - vec4(7.0 / 32.0) * vec4(evn);

	gl_FragColor = dst * bs;
}
);


static CGEConstString s_fshUpCompressOdd = CGE_SHADER_STRING_PRECISION_H
	(
	varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D inputImageTextureS;
uniform vec2 isft;
uniform vec2 sft;


void main()
{
	float odd = fract(floor(dot(textureCoordinate,isft) - 0.5) * 0.5) * 2.0;

	float evn = 1.0 - odd;

	vec4 dst = vec4(0.0);

	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	dst += src * vec4(20.0) * vec4(evn);


	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0 - evn, 0.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0 + evn, 0.0));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0,-1.0 - evn));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0, 1.0 + evn));
	dst += src;

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0,-1.0));
	vec4 dst1 = src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0,-1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0, 1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 1.0));
	dst1 += src;

	dst += dst1 * vec4(2.0) * vec4(evn);

	vec4 bs = vec4(1.0 / 4.0) - vec4(7.0 / 32.0) * vec4(evn);

	dst *= bs;


	src = texture2D(inputImageTextureS, textureCoordinate);

	dst.yzw = dst.xyz;
	dst.x = src.g;

	gl_FragColor = dst;
}
);


static CGEConstString s_fshUpCompressEvn = CGE_SHADER_STRING_PRECISION_H
	(
	varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D inputImageTextureS;
uniform vec2 isft;
uniform vec2 sft;


void main()
{
	vec2 odd = fract(floor(textureCoordinate * isft) * vec2(0.5)) * vec2(2.0);

	float evn = 1.0 - odd.x * odd.y;
	float evn2 = evn * 2.0;

	vec4 dst = vec4(0.0);

	vec4 src = texture2D(inputImageTexture, textureCoordinate + sft);
	dst += src * vec4(20.0) * vec4(evn);


	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0 - evn2,-1.0 - evn2));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 3.0 + evn2,-1.0 - evn2));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0 - evn2, 3.0 + evn2));
	dst += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 3.0 + evn2, 3.0 + evn2));
	dst += src;

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-3.0, 1.0));
	vec4 dst1 = src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 5.0, 1.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0,-3.0));
	dst1 += src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 5.0));
	dst1 += src;

	dst += dst1 * vec4(2.0) * vec4(evn);

	vec4 bs = vec4(1.0 / 4.0) - vec4(7.0 / 32.0) * vec4(evn);

	dst *= bs;


	src = texture2D(inputImageTextureS, textureCoordinate);

	dst.yzw = dst.xyz;
	dst.x = src.g;

	gl_FragColor = dst;
}
);


namespace CGE
{


	class CGEEdgeExtract : public CGEEdgeExtractInterface
	{
	public:

		CGEEdgeExtract();
		~CGEEdgeExtract();

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

		bool init();

		void setEdgeSaturation(float sat = 1.0);


		void setEdgeMinMax(float emin = 0.0,float emax = 1.0);


		void setEdgeLevel(int level = -1);


	private:

		void init_tmp_tex(CGESizei sz);

		void size_downOdd(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices);

		void size_downEvn(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices);

		void size_upOdd(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices);

		void size_upEvn(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices);

		void size_upCompressOdd(GLuint srcTexture,GLuint cpTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices);

		void size_upCompressEvn(GLuint srcTexture,GLuint cpTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices);


		void draw_tmp_tex(GLuint srcTexture,const GLfloat* posVertices);

		void draw_tex(GLuint srcTexture,GLuint disTexture,CGESizei disSize,float wf,float hf,const GLfloat* posVertices);

		void draw_edge(int idx,bool evn,const GLfloat* posVertices);

		void draw_reg(int idx,bool evn,const GLfloat* posVertices);

		void make_small_map(CGESizei sz,GLuint srcTexture, const GLfloat* posVertices);

		void start_shader(CGE::ProgramObject &shader, const GLfloat* posVertices)
		{

			shader.bind();
			GLint vPosition = glGetAttribLocation(shader.programID(), paramPositionIndexName);
			glEnableVertexAttribArray(vPosition);
			glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, posVertices);

			m_texIdx = 1;
		}

		void add_tex(CGE::ProgramObject &shader,const char * pm,GLuint tex)
		{

			glActiveTexture(CGE_TEXTURE_START + m_texIdx);
			glBindTexture(GL_TEXTURE_2D, tex);
			shader.sendUniformi(pm, CGE_TEXTURE_START_INDEX + m_texIdx);

			++m_texIdx;
		}

		CGE::ProgramObject m_filterShader;
		CGE::ProgramObject m_downOddShader;
		CGE::ProgramObject m_downEvnShader;
		CGE::ProgramObject m_upOddShader;
		CGE::ProgramObject m_upEvnShader;
		CGE::ProgramObject m_upCompressOddShader;
		CGE::ProgramObject m_upCompressEvnShader;
		CGE::ProgramObject m_resizeShader;
		CGE::ProgramObject m_edgeShader;
		CGE::ProgramObject m_regShader;
		std::vector<std::vector<GLuint> > m_tmpTexs;
		std::vector<CGESizei> m_texSizes;
		CGESizei m_size;
		int m_texIdx;
		float m_wFac,m_hFac;

		float m_sat,m_emin,m_emax;
		int m_level;

	};

	CGEEdgeExtract::CGEEdgeExtract():
		m_size(0,0),
		m_sat(1.0),
		m_emin(0.0),
		m_emax(1.0),
		m_level(-1)
	{
	}

	CGEEdgeExtractInterface * CGEEdgeExtractInterface::Create()
	{
		CGEEdgeExtractInterface* filter = new CGEEdgeExtract();
        if(!filter->init())
        {
            CGE_LOG_ERROR("CGEFastTiltShiftInterface create failed\n");
            CGE_DELETE(filter);
        }
        return filter;
	}

	inline void releas_tex(std::vector<GLuint> &tex)
	{

		if(!tex.empty() )
		{
			glDeleteTextures((int)tex.size(),&tex[0]);
			tex.clear();
		}
	}

	CGEEdgeExtract::~CGEEdgeExtract()
	{
		for(int i=0;i<(int)m_tmpTexs.size();++i)
		{
			releas_tex(m_tmpTexs[i]);

		}
	}



	inline bool initShader(CGE::ProgramObject &shader,CGEConstString s)
	{
		return shader.initWithShaderStrings(g_vshDefaultWithoutTexCoord, s);
	}

	inline bool initShader(CGE::ProgramObject &shader,CGEConstString s,CGEConstString p)
	{
		int size = int(strlen(s) + strlen(p) + 10);
		char * tmp = new char[size];

		sprintf(tmp,s,p);

		bool ret = shader.initWithShaderStrings(g_vshDefaultWithoutTexCoord, tmp);

		delete[] tmp;

		return ret;
	}


	inline bool initShader(CGE::ProgramObject &shader,CGEConstString s,CGEConstString p1,CGEConstString p2)
	{
		int size = int(strlen(s) + strlen(p1) + strlen(p2) + 10);
		char * tmp = new char[size];

		sprintf(tmp,s,p1,p2);

		bool ret = shader.initWithShaderStrings(g_vshDefaultWithoutTexCoord, tmp);

		delete[] tmp;

		return ret;
	}


	bool CGEEdgeExtract::init()
	{

		if(!initShader(m_filterShader,s_fshFilter))
			return false;

		if(!initShader(m_downOddShader,s_fshDownOdd))
			return false;

		if(!initShader(m_downEvnShader,s_fshDownEvn))
			return false;

		if(!initShader(m_upOddShader,s_fshUpOdd))
			return false;

		if(!initShader(m_upEvnShader,s_fshUpEvn))
			return false;

		if(!initShader(m_upCompressOddShader,s_fshUpCompressOdd))
			return false;

		if(!initShader(m_upCompressEvnShader,s_fshUpCompressEvn))
			return false;

		if(!initShader(m_resizeShader,s_fshResize))
			return false;

		if(!initShader(m_edgeShader,s_fshEdge))
			return false;

		if(!initShader(m_regShader,s_fshReg))
			return false;

		return true;
	}


	void CGEEdgeExtract::setEdgeSaturation(float sat)
	{
		m_sat = sat;
	}


	void CGEEdgeExtract::setEdgeMinMax(float emin,float emax)
	{
		m_emin = emin;
		m_emax  = emax;
	}


	void CGEEdgeExtract::setEdgeLevel(int level)
	{
		m_level = level;
	}


	void CGEEdgeExtract::size_downOdd(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices)
	{
		glViewport(0, 0, dstSize.width,dstSize.height);

		start_shader(m_downOddShader,posVertices);

		m_downOddShader.sendUniformf("sft",1.0f / dstSize.width,1.0f / dstSize.height);

		add_tex(m_downOddShader,paramInputImageName,srcTexture);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void CGEEdgeExtract::size_downEvn(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices)
	{
		glViewport(0, 0, dstSize.width,dstSize.height);

		start_shader(m_downEvnShader,posVertices);

		m_downEvnShader.sendUniformf("sft",0.25f / dstSize.width,0.25f / dstSize.height);

		add_tex(m_downEvnShader,paramInputImageName,srcTexture);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}



	void CGEEdgeExtract::size_upOdd(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices)
	{
		glViewport(0, 0, dstSize.width,dstSize.height);

		start_shader(m_upOddShader,posVertices);

		m_upOddShader.sendUniformf("isft",dstSize.width,dstSize.height);
		m_upOddShader.sendUniformf("sft",1.0f / dstSize.width,1.0f / dstSize.height);

		add_tex(m_upOddShader,paramInputImageName,srcTexture);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void CGEEdgeExtract::size_upEvn(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices)
	{
		glViewport(0, 0, dstSize.width,dstSize.height);

		start_shader(m_upEvnShader,posVertices);

		m_upEvnShader.sendUniformf("isft",dstSize.width,dstSize.height);
		m_upEvnShader.sendUniformf("sft",0.5f / dstSize.width,0.5f / dstSize.height);

		add_tex(m_upEvnShader,paramInputImageName,srcTexture);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void CGEEdgeExtract::size_upCompressOdd(GLuint srcTexture,GLuint cpTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices)
	{
		glViewport(0, 0, dstSize.width,dstSize.height);

		start_shader(m_upCompressOddShader,posVertices);

		m_upCompressOddShader.sendUniformf("isft",dstSize.width,dstSize.height);
		m_upCompressOddShader.sendUniformf("sft",1.0f / dstSize.width,1.0f / dstSize.height);

		add_tex(m_upCompressOddShader,paramInputImageName,srcTexture);
		add_tex(m_upCompressOddShader,"inputImageTextureS",cpTexture);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void CGEEdgeExtract::size_upCompressEvn(GLuint srcTexture,GLuint cpTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices)
	{
		glViewport(0, 0, dstSize.width,dstSize.height);

		start_shader(m_upCompressEvnShader,posVertices);

		m_upCompressEvnShader.sendUniformf("isft",dstSize.width,dstSize.height);
		m_upCompressEvnShader.sendUniformf("sft",0.5f / dstSize.width,0.5f / dstSize.height);

		add_tex(m_upCompressEvnShader,paramInputImageName,srcTexture);
		add_tex(m_upCompressEvnShader,"inputImageTextureS",cpTexture);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void CGEEdgeExtract::draw_tex(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,float wf,float hf,const GLfloat* posVertices)
	{
		glViewport(0, 0, dstSize.width, dstSize.height);

		start_shader(m_resizeShader,posVertices);

		m_resizeShader.sendUniformf("fac",wf,hf);

		add_tex(m_resizeShader,paramInputImageName,srcTexture);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void CGEEdgeExtract::draw_edge(int idx,bool evn,const GLfloat* posVertices)
	{
		CGESizei sz = m_texSizes[idx];
		glViewport(0, 0,sz.width, sz.height);

		start_shader(m_edgeShader,posVertices);

		m_edgeShader.sendUniformf("sft",1.0f/sz.width,1.0f/sz.height);
		m_edgeShader.sendUniformf("evn",evn ? 1.0f : 0.0f);

		add_tex(m_edgeShader,paramInputImageName,m_tmpTexs[idx][0]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tmpTexs[idx][1], 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		//draw_reg(idx,evn,posVertices);
	}

	void CGEEdgeExtract::draw_reg(int idx,bool evn,const GLfloat* posVertices)
	{
		CGESizei sz = m_texSizes[idx];
		glViewport(0, 0,sz.width, sz.height);

		start_shader(m_regShader,posVertices);

		//m_regShader.sendUniformf("sft",1.0f/sz.width,1.0f/sz.height);
		//m_regShader.sendUniformf("evn",evn ? 1.0f : 0.0f);
		//m_regShader.sendUniformf("sat",m_sat);
		//m_regShader.sendUniformf("edge_min",m_emin);
		//m_regShader.sendUniformf("edge_mul",1.0f / (m_emax - m_emin));

		add_tex(m_regShader,paramInputImageName,m_tmpTexs[idx][0]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tmpTexs[idx][1], 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	}

	inline GLuint create_tex(CGESizei & size,bool nearest = true)
	{
		return cgeGenTextureWithBuffer(NULL , size.width, size.height, GL_RGBA, GL_UNSIGNED_BYTE, 4, 0, nearest ? GL_NEAREST : GL_LINEAR);
	}

	void CGEEdgeExtract::draw_tmp_tex(GLuint srcTexture,const GLfloat* posVertices)
	{

		int idx = (int)m_texSizes.size() - 1;
		draw_tex(srcTexture,m_tmpTexs[idx][1],m_texSizes[idx],m_wFac,m_hFac,posVertices);

		{
			glViewport(0, 0,m_texSizes[idx].width,m_texSizes[idx].height);

			start_shader(m_filterShader,posVertices);

			m_filterShader.sendUniformf("sft",1.0f / m_texSizes[idx].width,1.0f / m_texSizes[idx].height);

			add_tex(m_filterShader,paramInputImageName,m_tmpTexs[idx][1]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tmpTexs[idx][0], 0);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		draw_edge(idx,!(idx % 2),posVertices);

		--idx;
		
		size_downOdd(m_tmpTexs[idx + 1][0],m_tmpTexs[idx][0],m_texSizes[idx],posVertices);
		draw_edge(idx,!(idx % 2),posVertices);

		--idx;

		for(;idx >= 1;--idx)
		{
			size_downEvn(m_tmpTexs[idx + 1][0],m_tmpTexs[idx][0],m_texSizes[idx],posVertices);
			draw_edge(idx,!(idx % 2),posVertices);

			--idx;

			size_downOdd(m_tmpTexs[idx + 1][0],m_tmpTexs[idx][0],m_texSizes[idx],posVertices);
			draw_edge(idx,!(idx % 2),posVertices);
		}

		int si = 1;
		int di = 0;

		int i = 1;

		size_upOdd(m_tmpTexs[i-1][si],m_tmpTexs[i][di],m_texSizes[i],posVertices);

		++i;

		for(;i<m_texSizes.size();++i)
		{
			if(i%2)
			{
				size_upCompressOdd(m_tmpTexs[i-1][di],m_tmpTexs[i][si],m_tmpTexs[i][di],m_texSizes[i],posVertices);
			}
			else
			{
				size_upCompressEvn(m_tmpTexs[i-1][di],m_tmpTexs[i][si],m_tmpTexs[i][di],m_texSizes[i],posVertices);
			}
		}


		for(i = 0;i<m_texSizes.size();++i)
		{
			draw_reg(i,!(i % 2),posVertices);
		}
	}


	void CGEEdgeExtract::init_tmp_tex(CGESizei sz)
	{

		if(m_size.width != sz.width || m_size.height != sz.height)
		{
			m_size = sz;
			if(!m_tmpTexs.empty() )
			{
				for(int i=0;i<(int)m_tmpTexs.size();++i)
				{
					releas_tex(m_tmpTexs[i]);

				}
				m_tmpTexs.clear();
				m_texSizes.clear();
			}

			int ms = CGE_MAX(sz.width,sz.height);
			int level_n = int(log(ms / 16.0f) / log(2.0f));

			int mul = (int)pow(2.0f,level_n);

			int lw = sz.width / mul + 1;
			if(sz.width % mul) ++lw;

			int lh = sz.height / mul + 1;
			if(sz.height % mul) ++lh;

			m_wFac = lw / ((float)sz.width  / mul);
			m_hFac = lh / ((float)sz.height / mul);


			for(int i = 0;i<=level_n;++i)
			{
				int tm = (int)pow(2.0f,i);
				m_texSizes.push_back(CGESizei(lw * tm,lh * tm));
				m_texSizes.push_back(CGESizei(lw * tm,lh * tm));
			}

			m_tmpTexs = std::vector<std::vector<GLuint> >(m_texSizes.size());

			for(int i=0;i<m_texSizes.size();++i)
			{
				m_tmpTexs[i].push_back(create_tex(m_texSizes[i]));
				m_tmpTexs[i].push_back(create_tex(m_texSizes[i]));
				m_tmpTexs[i].push_back(create_tex(m_texSizes[i]));
			}
		}
	}



	void CGEEdgeExtract::make_small_map(CGESizei sz,GLuint srcTexture, const GLfloat* posVertices)
	{

		//if(flush_flag || sz.width != m_size.width || sz.height != m_size.height)
		//{
			init_tmp_tex(sz);
			draw_tmp_tex(srcTexture,posVertices);
		//	flush_flag = false;
		//}


	}

	void CGEEdgeExtract::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{

		handler->setAsTarget();
		
		make_small_map(handler->getOutputFBOSize(),srcTexture,posVertices);

		handler->setAsTarget();

		int si = 1;
		int di = 0;

		int level;
		
		if(m_level < 0)
		{
			level = (int)m_texSizes.size() + m_level;
			level = CGE_MAX(level,0);
		}
		else
		{
			level = m_level;
			level = CGE_MIN(level,int(m_texSizes.size() - 1));
		}

		int sourcIdx = si;

		int i = level + 1;

		if(i<m_texSizes.size())
		{


			if(i%2)
			{
				size_upOdd(m_tmpTexs[i-1][si],m_tmpTexs[i][di],m_texSizes[i],posVertices);
			}
			else
			{
				size_upEvn(m_tmpTexs[i-1][si],m_tmpTexs[i][di],m_texSizes[i],posVertices);
			}

			++i;

			for(;i<m_texSizes.size();++i)
			{
				if(i%2)
				{
					size_upOdd(m_tmpTexs[i-1][di],m_tmpTexs[i][di],m_texSizes[i],posVertices);
				}
				else
				{
					size_upEvn(m_tmpTexs[i-1][di],m_tmpTexs[i][di],m_texSizes[i],posVertices);
				}
			}
			sourcIdx = di;
		}

		//glBindTexture(GL_TEXTURE_2D, m_tmpTexs.back()[sourcIdx]);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		draw_tex(m_tmpTexs.back()[sourcIdx],handler->getTargetTextureID(),handler->getOutputFBOSize(),1.0 / m_wFac,1.0 / m_hFac,posVertices);

		//draw_tex(m_tmpTexs[level + 1][0],handler->getTargetTextureID(),handler->getOutputFBOSize(),1.0 / m_wFac,1.0 / m_hFac,posVertices);

		//glBindTexture(GL_TEXTURE_2D, m_tmpTexs.back()[sourcIdx]);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	CGEEdgeExtractInterface * getEdgeExtractFilter()
	{
		return CGEEdgeExtractInterface::Create();
	}
}
