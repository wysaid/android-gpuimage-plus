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

#if defined(_CGE_DISABLE_GLOBALCONTEXT_) && _CGE_DISABLE_GLOBALCONTEXT_

#define CGE_ENABLE_GLOBAL_GLCONTEXT(...)
#define CGE_DISABLE_GLOBAL_GLCONTEXT(...)

#else

#define CGE_ENABLE_GLOBAL_GLCONTEXT(...) cgeEnableGlobalGLContext()
#define CGE_DISABLE_GLOBAL_GLCONTEXT(...) cgeDisableGlobalGLContext()

#endif

namespace CGE
{
#if !(defined(_CGE_DISABLE_GLOBALCONTEXT_) && _CGE_DISABLE_GLOBALCONTEXT_)

	typedef bool (*CGEEnableGLContextFunction)(void*);

	typedef bool (*CGEDisableGLContextFunction)(void*);

	void cgeSetGLContextEnableFunction(CGEEnableGLContextFunction func, void* param);
	void cgeSetGLContextDisableFunction(CGEDisableGLContextFunction func, void* param);
	void* cgeGetGLEnableParam();
	void* cgeGetGLDisableParam();
	void cgeStopGlobalGLEnableFunction();
	void cgeRestoreGlobalGLEnableFunction();

	void cgeEnableGlobalGLContext();
	void cgeDisableGlobalGLContext();

#endif

	// CGEBufferLoadFun 的返回值将作为 CGEBufferUnloadFun 的第一个参数
	// CGEBufferLoadFun 的参数 arg 将作为 CGEBufferUnloadFun 的第二个参数
	typedef void* (*CGEBufferLoadFun)(const char* sourceName, void** bufferData, GLint* w, GLint* h, CGEBufferFormat* fmt, void* arg);
	typedef bool (*CGEBufferUnloadFun)(void* arg1, void* arg2);

	//加载纹理回调， 注， 为了保持接口简洁性， 回调返回的纹理单元将由调用者负责释放
	//返回的纹理不应该为 glDeleteTextures 无法处理的特殊纹理类型.
	typedef GLuint (*CGETextureLoadFun)(const char* sourceName, GLint* w, GLint* h, void* arg);

	//You can set a common function for loading textures
	void cgeSetCommonLoadFunction(CGEBufferLoadFun fun, void* arg);
	void cgeSetCommonUnloadFunction(CGEBufferUnloadFun fun, void* arg);

	void* cgeLoadResourceCommon(const char* sourceName, void** bufferData, GLint* w, GLint* h, GLenum* format, GLenum* type);
	CGEBufferLoadFun cgeGetCommonLoadFunc();
	void* cgeGetCommonLoadArg();
	bool cgeUnloadResourceCommon(void* bufferArg);
	CGEBufferUnloadFun cgeGetCommonUnloadFunc();
	void* cgeGetCommonUnloadArg();

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
        SharedTexture() : m_textureID(0), m_refCount(nullptr), width(0), height(0) {}
        SharedTexture(GLuint textureID, int w, int h);
        
        SharedTexture(const SharedTexture& other) : m_textureID(0), m_refCount(nullptr)
		{
			*this = other;
		}

        ~SharedTexture();

		inline SharedTexture& operator =(const SharedTexture& other)
		{
			assert(this != &other && other.m_textureID != 0);

			if(m_refCount != nullptr && --*m_refCount <= 0)
			{
				clear();
			}

			m_textureID = other.m_textureID;
			m_refCount = other.m_refCount;
			if (m_refCount)
			{
				++*m_refCount;
				CGE_LOG_INFO("CGESharedTexture assgin: textureID %d, refCount: %d\n", m_textureID, *m_refCount);
			}
				
			width = other.width;
			height = other.height;
			return *this;
		}

		inline GLuint texID() const { return m_textureID; }

		inline void bindToIndex(GLint index) const
		{
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, m_textureID);
		}

        void forceRelease(bool bDelTexture);
        
        //特殊用法， 与 forceRelease 配对使用
        inline void forceAssignTextureID(GLuint texID)
        {
            m_textureID = texID;
        }

	public:
		int width;  //public, for easy accessing.
		int height;

	protected:
        void clear();

	private:
		GLuint m_textureID;
		mutable int* m_refCount;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer() { glGenFramebuffers(1, &m_framebuffer); }
		~FrameBuffer() { glDeleteFramebuffers(1, &m_framebuffer); }

		inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer); }

		inline void bindTexture2D(const SharedTexture& texture, GLenum attachment = GL_COLOR_ATTACHMENT0) const
		{
			bindTexture2D(texture.texID(), texture.width, texture.height, attachment);
		}

		inline void bindTexture2D(const SharedTexture& texture, GLsizei x, GLsizei y, GLsizei w, GLsizei h, GLenum attachment = GL_COLOR_ATTACHMENT0) const
		{
			bindTexture2D(texture.texID(), x, y, w, h, attachment);
		}

		inline void bindTexture2D(GLuint texID, GLsizei w, GLsizei h, GLenum attachment = GL_COLOR_ATTACHMENT0) const
		{
			bindTexture2D(texID, 0, 0, w, h, attachment);
		}

		inline void bindTexture2D(GLuint texID, GLenum attachment = GL_COLOR_ATTACHMENT0) const
		{
			bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texID, 0);
			CGE_LOG_CODE(
				if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{
					CGE_LOG_ERROR("CGE::FrameBuffer::bindTexture2D - Frame buffer is not valid!");
				}
			)
		}

		inline void bindTexture2D(GLuint texID, GLsizei x, GLsizei y, GLsizei w, GLsizei h, GLenum attachment = GL_COLOR_ATTACHMENT0) const
		{
			bindTexture2D(texID, attachment);
			glViewport(x, y, w, h);
		}

		inline GLuint getID() { return m_framebuffer; }

	private:
		GLuint m_framebuffer;
	};

	struct CGESizei
    {
        CGESizei(): width(0), height(0) {}
        CGESizei(int w, int h) : width(w), height(h) {}
        void set(int w, int h)
        {
            width = w;
            height = h;
        }
        bool operator ==(const CGESizei &other) const
        {
            return width == other.width && height == other.height;
        }
        bool operator !=(const CGESizei &other) const
        {
            return width != other.width || height != other.height;
        }
        GLint width;
        GLint height;
    };
    
    struct CGESizef
    {
        CGESizef() : width(0.0f), height(0.0f) {}
        CGESizef(float w, float h) : width(w), height(h) {}
        void set(float w, float h)
        {
            width = w;
            height = h;
        }
        GLfloat width;
        GLfloat height;
    };
    
#ifndef CGE_MIN
    
    template<typename Type>
    inline Type CGE_MIN(Type a, Type b)
    {
        return a < b ? a : b;
    }
    
#endif
    
#ifndef CGE_MAX
    
    template<typename Type>
    inline Type CGE_MAX(Type a, Type b)
    {
        return a > b ? a : b;
    }
    
#endif
    
#ifndef CGE_MID
    
    template<typename Type>
    inline Type CGE_MID(Type n, Type vMin, Type vMax)
    {
        if(n < vMin)
            n = vMin;
        else if(n > vMax)
            n = vMax;
        return n;
    }
    
#endif

#ifndef CGE_MIX

	template<typename OpType, typename MixType>
	inline auto CGE_MIX(OpType a, OpType b, MixType value) -> decltype(a - a * value + b * value)
	{
		return a - a * value + b * value;
	}

#endif
    
    struct CGELuminance
    {
        enum { CalcPrecision = 16 };
        enum { Weight = (1<<CalcPrecision) };
        enum { RWeight = int(0.299*Weight), GWeight = int(0.587*Weight), BWeight = int(0.114*Weight) };
        
        static inline int RGB888(int r, int g, int b)
        {			
            return (r * RWeight + g * GWeight + b * BWeight) >> CalcPrecision;
        }
        
        //color 从低位到高位的顺序为r-g-b, 传参时需要注意大小端问题
        static inline int RGB565(unsigned short color)
        {
            const int r = (color & 31) << 3;
            const int g = ((color >> 5) & 63) << 2;
            const int b = ((color >> 11) & 31) << 3;
            
            return RGB888(r, g, b);
        }
    };

}

//////////////////////////////////////////////////////////////////////////
#include <vector>
#include <ctime>
#include <memory>
#include "cgeShaderFunctions.h"
#include "cgeImageHandler.h"
#include "CGEImageFilter.h"

#endif /* _CGEGLFUNCTIONS_H_ */
