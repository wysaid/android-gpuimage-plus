/*
 * cgeFastTiltShift.cpp
 *
 *  Created on: 2015-01-12
 *      Author: Su Zhiyun
 */

#include "cgeFastTiltShift.h"
#include <cmath>


static CGEConstString s_fshDown = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 sft;


void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0,-1.0));
	vec4 dst =  src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0,-1.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0, 1.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 1.0));
	dst += src * src * src;

	gl_FragColor = pow(dst * vec4(0.25),vec4(1.0 / 3.0));
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


static CGEConstString s_fshB1 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 sft;


void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate);
	vec4 dst =  src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-2.0,-1.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-2.0, 0.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0,-1.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-1.0, 0.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 0.0,-1.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0, 0.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 1.0,-1.0));
	dst += src * src * src;

	gl_FragColor = pow(dst * vec4(0.125),vec4(1.0 / 3.0));
}
);


static CGEConstString s_fshB1_1 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 sft;


void main()
{
	vec4 src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-2.0,-1.0));
	vec4 dst =  src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-2.0, 1.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 2.0,-1.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 2.0, 1.0));
	dst += src * src * src;

	gl_FragColor = pow(dst * vec4(0.25),vec4(1.0 / 3.0));
}
);


static CGEConstString s_fshB2_P1 = CGE_SHADER_STRING
(

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-3.0,-3.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 2.0,-3.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-3.0, 2.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 2.0, 2.0));
	dst += src * src * src;

	dst *= vec4(0.125);

	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 0.0,-3.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2(-2.0,-1.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 2.0,-1.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2(-2.0, 1.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 2.0, 1.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 0.0, 3.0));
	dst += src * src * src;

	dst *= 1.0 / 6.5;

);


static CGEConstString s_fshB2_P2 = CGE_SHADER_STRING
(

	vec4 bs = vec4(1.0/4.0);
	vec4 p1 = vec4(-1.0,-1.0,0.0,0.0);

	if(p > 0.25)
	{

		bs = vec4(1.0 / 12.0);
		vec2 p2 = vec2(0.0,0.0);
		p1 = vec4(-1.0,-2.0,0.0,1.0);

		if(p > 0.5)
		{

			bs = vec4(1.0 / (52.0));
			p1 = vec4(-3.0,-3.0,2.0,2.0);
			p2 = vec2(0.0,-3.0);

			src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2(-2.0,-1.0));
			dst += src * src * src;
			src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 2.0,-1.0));
			dst += src * src * src;
			src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2(-2.0, 1.0));
			dst += src * src * src;
			src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 2.0, 1.0));
			dst += src * src * src;
			src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 0.0, 3.0));
			dst += src * src * src;
		}



		src = texture2D(inputImageTextureBlur, textureCoordinate + sft * p2);
		dst += src * src * src;

		dst *= vec4(8.0);
	}
	src = texture2D(inputImageTexture, textureCoordinate + sft * p1.xy);
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * p1.zy);
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * p1.xw);
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * p1.zw);
	dst += src * src * src;

	dst *= bs;

);

static CGEConstString s_fshB2 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D inputImageTextureBlur;
uniform vec2 sft;


void main()
{
	vec4 src;
	vec4 dst = vec4(0.0);

	%s\n

	gl_FragColor = pow(dst,vec4(1.0 / 3.0));
}
);


static CGEConstString s_fshB2_1 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D inputImageTextureBlur;
uniform sampler2D inputImageTextureUp;
uniform vec2 sft;
uniform vec2 upsft;
uniform mat2 trans;
uniform vec2 pos;
uniform float th;


void main()
{
	vec4 src;
	vec4 dst = vec4(0.0);

	vec2 ncodin = (textureCoordinate - pos) * trans;
	float p = sqrt(dot(ncodin,ncodin)) - th;

	if(p < 1.0)
	{
		%s\n
	}
	else
	{
		src = texture2D(inputImageTextureUp, textureCoordinate + upsft);
		dst += src * src * src;
	}


	gl_FragColor = pow(dst,vec4(1.0 / 3.0));
}
);


static CGEConstString s_fshB2_a = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D inputImageTextureBlur;
uniform vec2 sft;
uniform mat2 trans;
uniform vec2 pos;
uniform float th;


void main()
{
	vec4 src;
	vec4 dst = vec4(0.0);

	vec2 ncodin = (textureCoordinate - pos) * trans;
    float p = length(ncodin) - th;

	%s\n

	gl_FragColor = pow(dst,vec4(1.0 / 3.0));
}
);

static CGEConstString s_fshB3 = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D inputImageTextureBlur;
uniform sampler2D inputImageTextureBlur2;
uniform vec2 sft;


void main()
{
	vec4 src;

	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-5.0,-7.0));
	vec4 dst = src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 4.0,-7.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-7.0,-5.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 6.0,-5.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-7.0, 4.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 6.0, 4.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2(-5.0, 6.0));
	dst += src * src * src;
	src = texture2D(inputImageTexture, textureCoordinate + sft * vec2( 4.0, 6.0));
	dst += src * src * src;

	dst *= vec4(0.125);

	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2(-2.0,-7.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 2.0,-7.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2(-4.0,-5.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 0.0,-5.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 4.0,-5.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2(-4.0, 5.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 0.0, 5.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 4.0, 5.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2(-2.0, 7.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur, textureCoordinate + sft * vec2( 2.0, 7.0));
	dst += src * src * src;

	dst *= vec4(0.25);


	src = texture2D(inputImageTextureBlur2, textureCoordinate + sft * vec2(-4.0,-2.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur2, textureCoordinate + sft * vec2( 4.0,-2.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur2, textureCoordinate + sft * vec2(-4.0, 2.0));
	dst += src * src * src;
	src = texture2D(inputImageTextureBlur2, textureCoordinate + sft * vec2( 4.0, 2.0));
	dst += src * src * src;

	gl_FragColor = pow(dst * vec4(1.0 / 6.75),vec4(1.0 / 3.0));
}
);

static CGEConstString s_fshUp = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D inputImageTextureS1;
uniform sampler2D inputImageTextureS2;
uniform vec4 m1;
uniform vec4 m2;
uniform mat2 trans;
uniform vec2 pos;
uniform float th;
uniform float bsth;


void main()
{
	vec4 src;

	vec2 ncodin = (textureCoordinate - pos) * trans;
	float p = sqrt(dot(ncodin,ncodin)) - th;

	vec4 c1 = texture2D(inputImageTexture,textureCoordinate);
	vec4 c2 = texture2D(inputImageTextureS1, textureCoordinate * m1.xy + m1.zw);
	vec4 c3 = texture2D(inputImageTextureS2, textureCoordinate * m2.xy + m2.zw);

	//c1 = vec4(1.0);

	float fac = fract(max(log(max(p,bsth)) * -1.44269504,0.0));
	fac = min((1.0 - fac),fac) * 2.0;

	src = mix(c2,c3,vec4(clamp(fac * 2.0 - 0.5,0.0,1.0)));

	src = mix(c1,src,clamp(p / bsth,0.0,1.0));

	gl_FragColor = src;
}
);

namespace CGE
{


	class CGEFastTiltShift : public CGEFastTiltShiftInterface
	{
	public:

		CGEFastTiltShift(GradientMode mode);
		~CGEFastTiltShift();

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

		bool init();

		//Both the real size about the image.
		//eg: 100, 100. This means blurring from "start" and gradual changing in "gradient".
		void setBlurGradient(GLfloat start, GLfloat gradient);

		//Real pixels. This means the position the blur line would pass by.
		void setBlurCentralPos(GLfloat x, GLfloat y);

		//Real Pixels.
		//The "radiusX" and "randiusY" sets the radius of the ellipse.
		void setEllipseFactor(GLfloat fac);

		//range: both [0, 1]. This means the direction of the blur line.
		void setBlurNormal(GLfloat x, GLfloat y);

		//range: [0, π/2]. This means the angle of the blur normal.
		//Note: You can choose one of "setBlurNormal" and "setRotation" as you like. The two functions did the same thing.
		void setRotation(GLfloat angle);

		void setRadius(float radius); // range: [0.0f,0.5f]

		void flush();

		void setGradientMode(GradientMode mode);

	private:

		void init_tmp_tex(CGESizei sz);

		void size_down(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices);


		void draw_tmp_tex(GLuint srcTexture,const GLfloat* posVertices);

		void draw_tex(GLuint srcTexture,GLuint disTexture,CGESizei disSize,float wf,float hf,const GLfloat* posVertices);

		void blur_tex(CGE::ProgramObject &shader,GLuint srcTexture,GLuint disTexture,CGESizei size,const GLfloat* posVertices);

		void make_small_map(CGESizei sz,GLuint srcTexture, const GLfloat* posVertices);

		void draw_level_start(int level,const GLfloat* posVertices);

		void draw_level(int level,bool sf,float fac, const GLfloat* posVertices);
		
		void cal_fac(float m[][2],float &px,float &py,float &th);

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

		CGE::ProgramObject m_upShader;
		CGE::ProgramObject m_downShader;
		CGE::ProgramObject m_resizeShader;
		CGE::ProgramObject m_b1Shader;
		CGE::ProgramObject m_b1_1Shader;
		CGE::ProgramObject m_b2Shader;
		CGE::ProgramObject m_b2_1Shader;
		CGE::ProgramObject m_b2_aShader;
		CGE::ProgramObject m_b2_1aShader;
		CGE::ProgramObject m_b3Shader;
		std::vector<std::vector<GLuint> > m_tmpTexs;
		std::vector<CGESizei> m_texSizes;
		CGESizei m_size;
//		GLuint m_srcTex;
		int m_texIdx;
		int m_baseFlag;
		float m_wFac[2],m_hFac[2];
		float m_bsth;

		float m_radius;

		float m_start,m_gradient,m_path_x,m_path_y,m_norm_x,m_norm_y,m_ellipse_fac;

		bool flush_flag;

		GradientMode m_mode;

	};
	CGEFastTiltShift::CGEFastTiltShift(GradientMode mode):
		m_size(0,0),
//		m_srcTex(-1),
		m_radius(0.07f * 1.0f),
		m_start(0),
		m_gradient(500),
		m_path_x(0),
		m_path_y(0),
		m_norm_x(1),
		m_norm_y(-1),
		flush_flag(true),
		m_ellipse_fac(1),
		m_mode(mode)
	{
		m_wFac[0] = 1.0f;
		m_wFac[1] = 1.0f;
		m_hFac[0] = 1.0f;
		m_hFac[1] = 1.0f;
	}

	CGEFastTiltShiftInterface * CGEFastTiltShiftInterface::Create(GradientMode mode)
	{
		CGEFastTiltShiftInterface* filter = new CGEFastTiltShift(mode);
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

	CGEFastTiltShift::~CGEFastTiltShift()
	{
		for(int i=0;i<(int)m_tmpTexs.size();++i)
		{
			releas_tex(m_tmpTexs[i]);

		}
	}


	void CGEFastTiltShift::setBlurGradient(GLfloat start, GLfloat gradient)
	{
		m_start = start;
		m_gradient = gradient;
	}

	void CGEFastTiltShift::setBlurCentralPos(GLfloat x, GLfloat y)
	{
		m_path_x = x;
		m_path_y = y;
	}

	void CGEFastTiltShift::setBlurNormal(GLfloat x, GLfloat y)
	{
		m_norm_x = x;
		m_norm_y = y;
	}

	void CGEFastTiltShift::setRotation(GLfloat angle)
	{
        m_norm_x = cosf(angle);
        m_norm_y = sinf(angle);
	}

	void CGEFastTiltShift::setRadius(float radius)
	{
		m_radius = CGE_MID(radius,0.0f, 0.5f);
	}


	void CGEFastTiltShift::setEllipseFactor(float fac)
	{
		m_ellipse_fac = fac;
	}

	inline bool initShader(CGE::ProgramObject &shader,CGEConstString s)
	{
		return shader.initWithShaderStrings(g_vshDefaultWithoutTexCoord, s);
	}

	inline bool initShader(CGE::ProgramObject &shader,CGEConstString s,CGEConstString p)
	{
		int size = (int)(strlen(s) + strlen(p) + 10);
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

	void CGEFastTiltShift::setGradientMode(GradientMode mode)
	{
		m_mode = mode;
	}

	bool CGEFastTiltShift::init()
	{
		if(!initShader(m_upShader,s_fshUp))
			return false;

		if(!initShader(m_downShader,s_fshDown))
			return false;

		if(!initShader(m_resizeShader,s_fshResize))
			return false;

		if(!initShader(m_b1Shader,s_fshB1))
			return false;

		if(!initShader(m_b1_1Shader,s_fshB1_1))
			return false;

		if(!initShader(m_b2Shader,s_fshB2,s_fshB2_P1))
			return false;

		if(!initShader(m_b2_1Shader,s_fshB2_1,s_fshB2_P1))
			return false;

		if(!initShader(m_b2_1aShader,s_fshB2_1,s_fshB2_P2))
			return false;

		if(!initShader(m_b2_aShader,s_fshB2_a,s_fshB2_P2))
			return false;

		if(!initShader(m_b3Shader,s_fshB3))
			return false;

		return true;
	}


	void CGEFastTiltShift::size_down(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,const GLfloat* posVertices)
	{
		glViewport(0, 0, dstSize.width,dstSize.height);

		start_shader(m_downShader,posVertices);

		m_downShader.sendUniformf("sft",0.25f / dstSize.width,0.25f / dstSize.width);

		add_tex(m_downShader,paramInputImageName,srcTexture);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void CGEFastTiltShift::draw_tex(GLuint srcTexture,GLuint dstTexture,CGESizei dstSize,float wf,float hf,const GLfloat* posVertices)
	{
		glViewport(0, 0, dstSize.width, dstSize.height);

		start_shader(m_resizeShader,posVertices);

		m_resizeShader.sendUniformf("fac",wf,hf);

		add_tex(m_resizeShader,paramInputImageName,srcTexture);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	inline GLuint create_tex(CGESizei & size,bool nearest = true)
	{
		return cgeGenTextureWithBuffer(NULL , size.width, size.height, GL_RGBA, GL_UNSIGNED_BYTE, 4, 0, nearest ? GL_NEAREST : GL_LINEAR);
	}

	void CGEFastTiltShift::draw_tmp_tex(GLuint srcTexture,const GLfloat* posVertices)
	{
		int idx = (int)m_tmpTexs.size() - 1;
		m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx]));
		m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx]));
		m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx],false));

		draw_tex(srcTexture,m_tmpTexs[idx][0],m_texSizes[idx],m_wFac[idx % 2],m_hFac[idx % 2],posVertices);
		blur_tex(m_b1Shader,m_tmpTexs[idx][0],m_tmpTexs[idx][1],m_texSizes[idx],posVertices);

		--idx;
		m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx]));
		m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx]));
		m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx],false));

		glBindTexture(GL_TEXTURE_2D, m_tmpTexs[idx + 1][0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		draw_tex(m_tmpTexs[idx + 1][0],m_tmpTexs[idx][0],m_texSizes[idx],m_wFac[idx % 2] / m_wFac[(idx + 1) % 2],m_hFac[idx % 2] / m_hFac[(idx + 1) % 2],posVertices);
		blur_tex(m_b1Shader,m_tmpTexs[idx][0],m_tmpTexs[idx][1],m_texSizes[idx],posVertices);
		glBindTexture(GL_TEXTURE_2D, m_tmpTexs[idx + 1][0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		--idx;
		for(;idx >= 2;--idx)
		{
			m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx]));
			m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx]));
			m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx],false));
			size_down(m_tmpTexs[idx + 2][0],m_tmpTexs[idx][0],m_texSizes[idx],posVertices);
			blur_tex(m_b1Shader,m_tmpTexs[idx][0],m_tmpTexs[idx][1],m_texSizes[idx],posVertices);
		}


		m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx]));
		m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx]));
		blur_tex(m_b1_1Shader,m_tmpTexs[idx+2][1],m_tmpTexs[idx][0],m_texSizes[idx],posVertices);
		draw_level_start(idx,posVertices);

		--idx;
		m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx]));
		m_tmpTexs[idx].push_back(create_tex(m_texSizes[idx]));
		blur_tex(m_b1_1Shader,m_tmpTexs[idx+2][1],m_tmpTexs[idx][0],m_texSizes[idx],posVertices);
		draw_level_start(idx,posVertices);
	}


	void CGEFastTiltShift::init_tmp_tex(CGESizei sz)
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

			int ms,lw1,lh1,lw2,lh2;

			if(sz.width > sz.height)
			{
				lw1 = 240 + 1;
				lh1 = sz.height * 240 / sz.width + 1;
				if((sz.height * 240) % sz.width) ++lh1;

				lw2 = 340 + 1;
				lh2 = sz.height * 340 / sz.width + 1;
				if((sz.height * 340) % sz.width) ++lh2;

				ms = sz.width;

				m_wFac[0] = lw1 / 240;
				m_hFac[0] = lh1 / ((float)sz.height * 240 / sz.width);

				m_wFac[1] = lw2 / 340;
				m_hFac[1] = lh2 / ((float)sz.height * 340 / sz.width); 
			}
			else
			{
				lh1 = 240 + 1;
				lw1 = sz.width * 240 / sz.height + 1;
				if((sz.width * 240) % sz.height) ++lw1;

				lh2 = 340 + 1;
				lw2 = sz.width * 340 / sz.height + 1;
				if((sz.width * 340) % sz.height) ++lw2;

				ms = sz.height;

				m_wFac[0] = lw1 / ((float)sz.width * 240 / sz.height);
				m_hFac[0] = lh1 / 240;

				m_wFac[1] = lw2 / ((float)sz.width * 340 / sz.height);
				m_hFac[1] = lh2 / 340; 
			}

			int level= 4;

			m_texSizes.push_back(CGESizei(lw1,lh1));
			m_texSizes.push_back(CGESizei(lw2,lh2));
			m_texSizes.push_back(CGESizei(lw1,lh1));
			m_texSizes.push_back(CGESizei(lw2,lh2));

			if(ms > 480)
			{
				++level;
				m_texSizes.push_back(CGESizei(lw1 * 2,lh1 * 2));
			}
			if(ms > 680)
			{
				++level;
				m_texSizes.push_back(CGESizei(lw2 * 2,lh2 * 2));
			}
			if(ms > 960)
			{
				++level;
				m_texSizes.push_back(CGESizei(lw1 * 4,lh1 * 4));
			}
			if(ms > 1920)
			{
				++level;
				m_texSizes.push_back(CGESizei(lw2 * 4,lh2 * 4));
			}

			m_tmpTexs = std::vector<std::vector<GLuint> >(level);
		}

	}

	inline void init_tex(std::vector<GLuint> &tex,CGESizei size)
	{
		releas_tex(tex);
		for(int i=0;i<3;++i)
		{
			GLuint texID = cgeGenTextureWithBuffer(NULL , size.width, size.height, GL_RGBA, GL_UNSIGNED_BYTE, 4, 0, GL_NEAREST);
			tex.push_back(texID);
		}
	}
	
	void CGEFastTiltShift::blur_tex(CGE::ProgramObject &shader,GLuint srcTexture,GLuint dstTexture,CGESizei size,const GLfloat* posVertices)
	{
		float sx = 1.0f / size.width;
		float sy = 1.0f / size.height;

		glViewport(0, 0, size.width, size.height);

		start_shader(shader,posVertices);

		shader.sendUniformf("sft",sx,sy);

		add_tex(shader,paramInputImageName,srcTexture);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
	
	inline CGESizei cal_size(CGESizei os,float radius)
	{
		int ls = (int)(8.0f / radius + 1.5f);

		if(os.width > os.height)return CGESizei(ls,os.height * ls / os.width);
		else return CGESizei(ls * os.width /os.height,ls);
	}

	void CGEFastTiltShift::flush()
	{
		flush_flag = true;
	}


	void CGEFastTiltShift::draw_level_start(int level,const GLfloat* posVertices)
	{
		if(level >= 2)return;

		CGESizei size = m_texSizes[level];

		float sx = 1.0f / size.width;
		float sy = 1.0f / size.height;

		glViewport(0, 0, size.width, size.height);

		start_shader(m_b3Shader,posVertices);

		m_b3Shader.sendUniformf("sft",sx,sy);

		add_tex(m_b3Shader,paramInputImageName,m_tmpTexs[level + 2][0]);
		add_tex(m_b3Shader,"inputImageTextureBlur",m_tmpTexs[level + 2][1]);
		add_tex(m_b3Shader,"inputImageTextureBlur2",m_tmpTexs[level][0]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tmpTexs[level][1], 0);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void CGEFastTiltShift::draw_level(int level,bool sf,float fac,const GLfloat* posVertices)
	{
		if(level < 2)return;

		CGESizei size = m_texSizes[level];

		float m[2][2];
		float px,py,th;

		cal_fac(m,px,py,th);

		m[0][0] /= fac;
		m[0][1] /= fac;
		m[1][0] /= fac;
		m[1][1] /= fac;
		th /= fac;

		float sx = 1.0f / size.width;
		float sy = 1.0f / size.height;

		float wf = m_wFac[level & 1];
		float hf = m_hFac[level & 1];

		m[0][0] *= wf;
		m[0][1] *= hf;
		m[1][0] *= wf;
		m[1][1] *= hf;

		px = px / wf;
		py = py / hf;
		px += 0.5f * sx;
		py += 0.5f * sy; 


		glViewport(0, 0, size.width, size.height);

		if(sf)
		{
			if(m_tmpTexs.size() - level < 3)
			{

				start_shader(m_b2_aShader,posVertices);

				m_b2_aShader.sendUniformf("sft",sx,sy);
				m_b2_aShader.sendUniformMat2("trans",1,false,m[0]);
				m_b2_aShader.sendUniformf("pos",px,py);
				m_b2_aShader.sendUniformf("th",th);


				add_tex(m_b2_aShader,paramInputImageName,m_tmpTexs[level][0]);
				add_tex(m_b2_aShader,"inputImageTextureBlur",m_tmpTexs[level][1]);
			}
			else
			{
				start_shader(m_b2Shader,posVertices);

				m_b2Shader.sendUniformf("sft",sx,sy);

				add_tex(m_b2Shader,paramInputImageName,m_tmpTexs[level][0]);
				add_tex(m_b2Shader,"inputImageTextureBlur",m_tmpTexs[level][1]);
			}

		}
		else
		{
			CGE::ProgramObject  *shader; 
			if(m_tmpTexs.size() - level < 3)
			{
				shader = &m_b2_1aShader;
			}
			else
			{
				shader = &m_b2_1Shader;
			}
			start_shader(*shader,posVertices);

			shader->sendUniformf("sft",sx,sy);
			shader->sendUniformMat2("trans",1,false,m[0]);
			shader->sendUniformf("pos",px,py);
			shader->sendUniformf("th",th);


			add_tex(*shader,paramInputImageName,m_tmpTexs[level][0]);
			add_tex(*shader,"inputImageTextureBlur",m_tmpTexs[level][1]);

			if(level < 4)
			{
				add_tex(*shader,"inputImageTextureUp",m_tmpTexs[level - 2][1]);
				shader->sendUniformf("upsft",0.0f,0.0f);
			}
			else
			{
				add_tex(*shader,"inputImageTextureUp",m_tmpTexs[level - 2][2]);
				shader->sendUniformf("upsft",sx * 0.5f,sy * 0.5f);
			}
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tmpTexs[level][2], 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}


	void CGEFastTiltShift::make_small_map(CGESizei sz,GLuint srcTexture, const GLfloat* posVertices)
	{

		if(flush_flag || sz.width != m_size.width || sz.height != m_size.height)
		{
			init_tmp_tex(sz);
			draw_tmp_tex(srcTexture,posVertices);
			flush_flag = false;
		}

		int baseLevel = (int)((log(m_radius / (0.07f / 8)) / log(2.0f) * 2.0f) + 0.5f);

		baseLevel = CGE_MID(6 - baseLevel, 0, (int)m_tmpTexs.size() - 2);

		int level = baseLevel;

		draw_level(level,true,pow(0.5f,(level - baseLevel - 1.0f) * 0.5f),posVertices);
		++level;
		draw_level(level,true,pow(0.5f,(level - baseLevel - 1.0f) * 0.5f),posVertices);
		++level;

		for(;level<(int)m_tmpTexs.size();++level)
		{
			draw_level(level,false,pow(0.5f,(level - baseLevel - 1.0f) * 0.5f),posVertices);
		}

		m_bsth = pow(0.5f,(level - baseLevel - 1.0f) * 0.5f) * 0.25f;
		m_baseFlag = baseLevel & 1;
	}


	void CGEFastTiltShift::cal_fac(float m[][2],float &px,float &py,float &th)
	{

		float nx = m_norm_y;
		float ny = m_norm_x;

		float bs = 1.0f / sqrt(nx * nx + ny * ny);

		nx *= bs;
		ny *= bs;


		m[0][0] = -ny;
		m[0][1] = nx;
		m[1][0] = nx;
		m[1][1] = ny;

		float inv_fac = 1.0f / m_ellipse_fac;

		if(VECTOR == m_mode)
		{
			 inv_fac = 0.0f;
		}

		float c[2][2] = {
			1.0f * inv_fac / m_gradient, 0.0f             ,
			0.0f                             , 1.0f / m_gradient,
		};


		float k[2][2] = {
            (float)m_size.width, 0.0f         ,
            0.0f        , (float)m_size.height,
		};

		// m = c * m * k

		m[0][0] *= c[0][0] * k[0][0];
		m[0][1] *= c[0][0] * k[1][1];
		m[1][0] *= c[1][1] * k[0][0];
		m[1][1] *= c[1][1] * k[1][1];

		px = m_path_x / m_size.width;
		py = m_path_y / m_size.height;

		th = m_start / m_gradient;
	}

	void CGEFastTiltShift::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices)
	{

		handler->setAsTarget();
		
		make_small_map(handler->getOutputFBOSize(),srcTexture,posVertices);

		handler->setAsTarget();

		int idx1 = (int)m_texSizes.size() - 2;
		int idx2 = (int)m_texSizes.size() - 1;

		if(m_baseFlag ^ (m_texSizes.size() & 1))std::swap(idx1,idx2);

		CGESizei sz1 = m_texSizes[idx1];
		CGESizei sz2 = m_texSizes[idx2];

		start_shader(m_upShader,posVertices);

		add_tex(m_upShader,paramInputImageName,srcTexture);
		add_tex(m_upShader,"inputImageTextureS1",m_tmpTexs[idx1][2]);
		add_tex(m_upShader,"inputImageTextureS2",m_tmpTexs[idx2][2]);

		m_upShader.sendUniformf("m1",
			1.0f / m_wFac[idx1 & 1],
			1.0f / m_hFac[idx1 & 1],
			0.5f / sz1.width,
			0.5f / sz1.height);

		m_upShader.sendUniformf("m2",
			1.0f / m_wFac[idx2 & 1],
			1.0f / m_hFac[idx2 & 1],
			0.5f / sz2.width,
			0.5f / sz2.height);

		float m[2][2];
		float px,py,th;

		cal_fac(m,px,py,th);

		m_upShader.sendUniformMat2("trans",1,false,m[0]);
		m_upShader.sendUniformf("pos",px,py);
		m_upShader.sendUniformf("th",th);
		m_upShader.sendUniformf("bsth",m_bsth);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handler->getTargetTextureID(), 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);		
	}

	CGEFastTiltShiftInterface * getFastVectorTiltShiftFilter()
	{
		return CGEFastTiltShiftInterface::Create(CGEFastTiltShiftInterface::VECTOR);
	}


	CGEFastTiltShiftInterface * getFastEllipseTiltShiftFilter()
	{
		return CGEFastTiltShiftInterface::Create(CGEFastTiltShiftInterface::ELLIPSE);
	}

}
