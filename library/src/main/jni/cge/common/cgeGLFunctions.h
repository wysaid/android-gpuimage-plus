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

// You can set a common function for loading textures
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
    return n - 1;
}

class FrameBuffer
{
public:
    FrameBuffer() { glGenFramebuffers(1, &m_framebuffer); }
    ~FrameBuffer() { glDeleteFramebuffers(1, &m_framebuffer); }

    inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer); }

    inline void bindTexture2D(GLuint texID, GLsizei w, GLsizei h, GLenum attachment = GL_COLOR_ATTACHMENT0) const
    {
        bindTexture2D(texID, 0, 0, w, h, attachment);
    }

    inline void bindTexture2D(GLuint texID, GLenum attachment = GL_COLOR_ATTACHMENT0) const
    {
        bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texID, 0);
        CGE_LOG_CODE(
            GLenum code = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (code != GL_FRAMEBUFFER_COMPLETE) {
                CGE_LOG_ERROR("CGE::FrameBuffer::bindTexture2D - Frame buffer is not valid: %x\n", code);
            })
    }

    inline void bindTexture2D(GLuint texID, GLsizei x, GLsizei y, GLsizei w, GLsizei h, GLenum attachment = GL_COLOR_ATTACHMENT0) const
    {
        bindTexture2D(texID, attachment);
        glViewport(x, y, w, h);
    }

    inline GLuint fbo() { return m_framebuffer; }

protected:
    GLuint m_framebuffer;
};

struct CGESizei
{
    CGESizei() :
        width(0), height(0) {}
    CGESizei(int w, int h) :
        width(w), height(h) {}
    void set(int w, int h)
    {
        width = w;
        height = h;
    }
    bool operator==(const CGESizei& other) const
    {
        return width == other.width && height == other.height;
    }
    bool operator!=(const CGESizei& other) const
    {
        return width != other.width || height != other.height;
    }
    GLint width;
    GLint height;
};

struct CGESizef
{
    CGESizef() :
        width(0.0f), height(0.0f) {}
    CGESizef(float w, float h) :
        width(w), height(h) {}
    void set(float w, float h)
    {
        width = w;
        height = h;
    }
    GLfloat width;
    GLfloat height;
};

struct TextureInfo
{
    GLuint name{};
    int width{}, height{};
};

class TextureObject
{
public:
    virtual ~TextureObject();
    explicit TextureObject(GLuint texture = 0, const CGESizei& size = CGESizei());
    explicit TextureObject(const TextureObject&) = delete;
    TextureObject(TextureObject&&) noexcept;
    explicit TextureObject(TextureInfo&& t) :
        m_texture(t.name), m_size(t.width, t.height) { t.name = 0; }

    GLuint texture() const { return m_texture; }
    GLint width() const { return m_size.width; }
    GLint height() const { return m_size.height; }
    CGESizei size() const { return m_size; }

    void cleanup(bool deleteTexture = true);

    // 注意, format 不支持改变, 如果有相关需求需要自行添加
    bool resize(int w, int h, const void* buffer = nullptr, GLenum format = GL_RGBA);
    inline bool updateTextureData(int w, int h, const void* buffer = nullptr, GLenum format = GL_RGBA)
    {
        return resize(w, h, buffer, format);
    }

    TextureObject& operator=(TextureObject&& t) noexcept;
    TextureObject& operator=(TextureInfo&& t);

protected:
    GLuint m_texture = 0;
    CGESizei m_size;
};

class FrameBufferWithTexture : protected FrameBuffer, public TextureObject
{
public:
    using FrameBuffer::FrameBuffer;
    ~FrameBufferWithTexture() override;

    GLuint renderbuffer() const { return m_renderBuffer; }

    // act like resize.
    // it will resize renderbuffer if exist.
    void bindTexture2D(GLsizei w, GLsizei h, const void* buffer = nullptr);

    void attachDepthBuffer();

    bool checkStatus();

    using FrameBuffer::bind;
    using FrameBuffer::fbo;

private:
    using TextureObject::resize;
    GLuint m_renderBuffer = 0;
};

struct CGELuminance
{
    enum
    {
        CalcPrecision = 16
    };
    enum
    {
        Weight = (1 << CalcPrecision)
    };
    enum
    {
        RWeight = int(0.299 * Weight),
        GWeight = int(0.587 * Weight),
        BWeight = int(0.114 * Weight)
    };

    static inline int RGB888(int r, int g, int b)
    {
        return (r * RWeight + g * GWeight + b * BWeight) >> CalcPrecision;
    }

    // color 从低位到高位的顺序为r-g-b, 传参时需要注意大小端问题
    static inline int RGB565(unsigned short color)
    {
        const int r = (color & 31) << 3;
        const int g = ((color >> 5) & 63) << 2;
        const int b = ((color >> 11) & 31) << 3;

        return RGB888(r, g, b);
    }
};

} // namespace CGE

//////////////////////////////////////////////////////////////////////////

// clang-format off
#include <ctime>
#include <memory>
#include <vector>

#include "cgeShaderFunctions.h"
#include "cgeImageFilter.h"
#include "cgeImageHandler.h"
// clang-format on

#endif /* _CGEGLFUNCTIONS_H_ */
