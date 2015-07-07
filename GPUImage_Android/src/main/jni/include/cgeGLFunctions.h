/*
* cgeGLFunctions.h
*
*  Created on: 2013-12-5
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _CGEGLFUNCTIONS_H_
#define _CGEGLFUNCTIONS_H_

#include "cgeCommonDefine.h"
#include <cassert>

/*
为节省texture资源，对OpenGL 所有texture的使用约束如下:
0号和1号纹理在各种初始化中可能会被多次用到，如果需要绑定固定的纹理，
请在使用纹理时，从 TEXTURE_START 开始。
不排除这种需要会增加，所以，
请使用下面的宏进行加法运算, 以代替手写的 GL_TEXTURE*

*/

#define CGE_TEXTURE_START_INDEX 2
#define CGE_TEXTURE_START GL_TEXTURE2

#define CGE_TEXTURE_INPUT_IMAGE_INDEX 1
#define CGE_TEXTURE_INPUT_IMAGE GL_TEXTURE1

#define CGE_TEXTURE_OUTPUT_IMAGE_INDEX 0
#define CGE_TEXTURE_OUTPUT_IMAGE GL_TEXTURE0

//Mark if the texture is premultiplied with the alpha channel.
#ifndef CGE_TEXTURE_PREMULTIPLIED
#define CGE_TEXTURE_PREMULTIPLIED 0
#endif

#ifdef _CGE_SHADER_VERSION_

#define CGE_GLES_ATTACH_STRING_L "#version " CGE_GET_MACRO_STRING(_CGE_SHADER_VERSION_) "\n#ifdef GL_ES\nprecision lowp float;\n#endif\n"
#define CGE_GLES_ATTACH_STRING_M "#version " CGE_GET_MACRO_STRING(_CGE_SHADER_VERSION_) "\n#ifdef GL_ES\nprecision mediump float;\n#endif\n"
#define CGE_GLES_ATTACH_STRING_H "#version " CGE_GET_MACRO_STRING(_CGE_SHADER_VERSION_) "\n#ifdef GL_ES\nprecision highp float;\n#endif\n"

#else

#define CGE_GLES_ATTACH_STRING_L "#ifdef GL_ES\nprecision lowp float;\n#endif\n"
#define CGE_GLES_ATTACH_STRING_M "#ifdef GL_ES\nprecision mediump float;\n#endif\n"
#define CGE_GLES_ATTACH_STRING_H "#ifdef GL_ES\nprecision highp float;\n#endif\n"
#endif

//Do not add any precision conf within SHADER_STRING_PRECISION_* macro!
#if defined(_MSC_VER) && _MSC_VER < 1600
//If the m$ compiler is under 10.0, there mustn't be any ',' outside the "()" in the shader string !! 
//For exmaple: vec2(0.0, 0.0) --> YES. 
//             float a, b;  --> No!!!, you must do like this: float a; float b;
#define CGE_SHADER_STRING_PRECISION_L(string) CGE_GLES_ATTACH_STRING_L  #string
#define CGE_SHADER_STRING_PRECISION_M(string) CGE_GLES_ATTACH_STRING_M  #string
#define CGE_SHADER_STRING_PRECISION_H(string) CGE_GLES_ATTACH_STRING_H  #string
#ifndef CGE_SHADER_STRING
#define CGE_SHADER_STRING(string) #string
#endif
#else
#define CGE_SHADER_STRING_PRECISION_L(...) CGE_GLES_ATTACH_STRING_L  #__VA_ARGS__
#define CGE_SHADER_STRING_PRECISION_M(...) CGE_GLES_ATTACH_STRING_M  #__VA_ARGS__
#define CGE_SHADER_STRING_PRECISION_H(...) CGE_GLES_ATTACH_STRING_H  #__VA_ARGS__
#ifndef CGE_SHADER_STRING
#define CGE_SHADER_STRING(...) #__VA_ARGS__
#endif
#endif

namespace CGE
{
	typedef bool (*CGEEnableGLContextFunction)(void*);

	typedef bool (*CGEDisableGLContextFunction)(void*);
	typedef void* (*CGEBufferLoadFun)(const char* sourceName, void** bufferData, GLint* w, GLint* h, CGEBufferFormat* fmt, void* arg);
	typedef bool (*CGEBufferUnloadFun)(void* arg1, void* arg2);

	void cgeSetGLContextEnableFunction(CGEEnableGLContextFunction func, void* param);
	void cgeSetGLContextDisableFunction(CGEDisableGLContextFunction func, void* param);
	void* cgeGetGLEnableParam();
	void* cgeGetGLDisableParam();
	void cgeStopGlobalGLEnableFunction();
	void cgeRestoreGlobalGLEnableFunction();

	void cgeEnableGlobalGLContext();
	void cgeDisableGlobalGLContext();

	void cgeGetDataAndChannelByFormat(CGEBufferFormat fmt, GLenum* dataFmt, GLenum* channelFmt, GLint* channel);

	//You can set a common function for loading textures
	void cgeSetCommonLoadFunction(CGEBufferLoadFun fun, void* arg);
	void cgeSetCommonUnloadFunction(CGEBufferUnloadFun fun, void* arg);

	void* cgeLoadResourceCommon(const char* sourceName, void** bufferData, GLint* w, GLint* h, GLenum* format, GLenum* type);
	CGEBufferLoadFun cgeGetCommonLoadFunc();
	void* cgeGetCommonLoadArg();
	bool cgeUnloadResourceCommon(void* bufferArg);
	CGEBufferUnloadFun cgeGetCommonUnloadFunc();
	void* cgeGetCommonUnloadArg();

	GLuint cgeGenTextureWithBuffer(const void* bufferData, GLint w, GLint h, GLenum channelFmt, GLenum dataFmt, GLint channels = 4, GLint bindID = 0, GLenum texFilter = GL_LINEAR, GLenum texWrap = GL_CLAMP_TO_EDGE);

	char* cgeGetScaledBufferInSize(const void* buffer, int& w, int& h, int channel, int maxSizeX, int maxSizeY);
	char* cgeGetScaledBufferOutofSize(const void* buffer, int& w, int& h, int channel, int minSizeX, int minSizeY);
	inline GLint cgeGetMaxTextureSize()
	{
		GLint n;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &n);
		return n-1;
	}

	class SharedTexture
	{
	public:
        SharedTexture() : m_textureID(0), m_refCount(NULL), width(0), height(0) {}
		SharedTexture(GLuint textureID, int w, int h)
		{
			m_textureID = textureID;
			m_refCount = new int(1);
			width = w;
			height = h;
			CGE_LOG_CODE(
				if(m_textureID == 0)
					CGE_LOG_ERROR("CGESharedTexture : Invalid TextureID!");
				else
				{
					CGE_LOG_INFO("---CGESharedTexture creating, textureID %d, total : %d ###\n", textureID, ++sTextureCount);
				}
			);
		}

        SharedTexture(const SharedTexture& other) : m_textureID(0), m_refCount(NULL)
		{
			*this = other;
		}

		~SharedTexture()
		{
			if(m_refCount == NULL)
			{
				CGE_LOG_CODE(
					if(m_textureID != 0)
					{
						CGE_LOG_ERROR("SharedTexture : Error occurred!");
					});
				return;
			}

			--*m_refCount;
			if(*m_refCount <= 0)
			{
				clear();
			}
			CGE_LOG_CODE(
			else
			{
				CGE_LOG_INFO("@@@ Texture %d deRef count: %d\n", m_textureID, *m_refCount);
			})
		}

		inline SharedTexture& operator =(const SharedTexture& other)
		{
			assert(this != &other && other.m_textureID != 0);

			if(m_refCount != NULL && --*m_refCount <= 0)
			{
				clear();
			}

			m_textureID = other.m_textureID;
			m_refCount = other.m_refCount;
			if(m_refCount)
				++*m_refCount;
			width = other.width;
			height = other.height;

			CGE_LOG_INFO("CGESharedTexture assgin: textureID %d, refCount: %d\n", m_textureID, *m_refCount);
			return *this;
		}

		inline GLuint texID() const { return m_textureID; }

		inline void bindToIndex(GLint index) const
		{
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, m_textureID);
		}

		inline void forceRelease(bool bDelTexture)
		{
			assert(*m_refCount == 1); // 使用 forceRelease 时 SharedTexture 必须保证只存在一个实例
			if(bDelTexture)
				glDeleteTextures(1, &m_textureID);
			m_textureID = 0;
			CGE_DELETE(m_refCount);
			width = 0;
			height = 0;
		}

	public:
		int width;  //public, for easy accessing.
		int height;

	protected:
		inline void clear()
		{
			CGE_LOG_CODE(
				if(m_textureID == 0)
				{
					CGE_LOG_ERROR("!!!CGESharedTexture - Invalid TextureID To Release!\n");
				}
				else
				{
					CGE_LOG_INFO("###CGESharedTexture deleting, textureID %d, now total : %d ###\n", m_textureID, --sTextureCount);
			});

			assert(*m_refCount == 0); // 未知错误

			glDeleteTextures(1, &m_textureID);
			m_textureID = 0;

			CGE_DELETE(m_refCount);
			width = 0;
			height = 0;
		}

		CGE_LOG_CODE(
			static int sTextureCount;
		)

	private:
		GLuint m_textureID;
		mutable int* m_refCount;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer() { glGenFramebuffers(1, &m_framebuffer); }
		~FrameBuffer() { glDeleteFramebuffers(1, &m_framebuffer); }

		inline void bind() { glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer); }

		inline void bindTexture2D(const SharedTexture& texture, GLenum attachment = GL_COLOR_ATTACHMENT0)
		{
			bindTexture2D(texture.texID(), texture.width, texture.height, attachment);
		}

		inline void bindTexture2D(const SharedTexture& texture, GLsizei x, GLsizei y, GLsizei w, GLsizei h, GLenum attachment = GL_COLOR_ATTACHMENT0)
		{
			bindTexture2D(texture.texID(), x, y, w, h, attachment);
		}

		inline void bindTexture2D(GLuint texID, GLsizei w, GLsizei h, GLenum attachment = GL_COLOR_ATTACHMENT0)
		{
			bindTexture2D(texID, 0, 0, w, h, attachment);
		}

		inline void bindTexture2D(GLuint texID, GLsizei x, GLsizei y, GLsizei w, GLsizei h, GLenum attachment = GL_COLOR_ATTACHMENT0)
		{
			bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texID, 0);

			glViewport(x, y, w, h);

			CGE_LOG_CODE(
				if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{
					CGE_LOG_ERROR("CGE::FrameBuffer::bindTexture2D - Frame buffer is not valid!");
				}
			)
		}

	private:
		GLuint m_framebuffer;
	};

	enum CGEGlobalBlendMode
	{
		CGEGLOBAL_BLEND_NONE,
		CGEGLOBAL_BLEND_ALPHA,
		CGEGLOBAL_BLEND_ALPHA_SEPERATE,
		CGEGLOBAL_BLEND_ADD,
		CGEGLOBAL_BLEND_ADD_SEPARATE,
		CGEGLOBAL_BLEND_ADD_SEPARATE_EXT, //带EXT的忽略alpha是否预乘
		CGEGLOBAL_BLEND_MULTIPLY,
		CGEGLOBAL_BLEND_MULTIPLY_SEPERATE,
		CGEGLOBAL_BLEND_SCREEN,
		CGEGLOBAL_BLEND_SCREEN_EXT,
	};

	inline void cgeSetGlobalBlendMode(const CGEGlobalBlendMode mode)
	{
#if CGE_TEXTURE_PREMULTIPLIED

		const GLenum BLEND_SRC = GL_ONE;

#else

		const GLenum BLEND_SRC = GL_SRC_ALPHA;

#endif

		switch (mode)
		{
		case CGEGLOBAL_BLEND_ALPHA:
			glBlendFunc(BLEND_SRC, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case CGEGLOBAL_BLEND_ALPHA_SEPERATE:
			glBlendFuncSeparate(BLEND_SRC, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
			break;
		case CGEGLOBAL_BLEND_ADD:
			glBlendFunc(BLEND_SRC, GL_ONE);
			break;
		case CGEGLOBAL_BLEND_ADD_SEPARATE:
			glBlendFuncSeparate(BLEND_SRC, GL_ONE, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
			break;
		case CGEGLOBAL_BLEND_ADD_SEPARATE_EXT:
			glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
			break;
		case CGEGLOBAL_BLEND_MULTIPLY:
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);
			break;
		case CGEGLOBAL_BLEND_MULTIPLY_SEPERATE:
			glBlendFuncSeparate(BLEND_SRC, GL_SRC_COLOR, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
			break;
		case CGEGLOBAL_BLEND_SCREEN:
			glBlendFunc(BLEND_SRC, GL_ONE_MINUS_SRC_COLOR);
			break;
		case CGEGLOBAL_BLEND_SCREEN_EXT:
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			break;
		case CGEGLOBAL_BLEND_NONE:
			//fall through
		default:
			glDisable(GL_BLEND);
			return ;
		}

		glEnable(GL_BLEND);
	}

}

//////////////////////////////////////////////////////////////////////////
#include <vector>
#include <ctime>
#include <memory>
#include "cgeShaderFunctions.h"
#include "cgeImageHandler.h"
#include "CGEImageFilter.h"

#endif /* _CGEGLFUNCTIONS_H_ */
