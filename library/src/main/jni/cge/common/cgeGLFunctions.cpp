/*
 * cgeGLFunctions.cpp
 *
 *  Created on: 2013-12-5
 *      Author: Wang Yang
 */

#include "cgeGLFunctions.h"

#include <cmath>

CGE_LOG_CODE(
    static int sTextureCount;)

namespace CGE
{

#if !(defined(_CGE_DISABLE_GLOBALCONTEXT_) && _CGE_DISABLE_GLOBALCONTEXT_)

static CGEEnableGLContextFunction s_enableGLFunc = nullptr;
static CGEDisableGLContextFunction s_disableGLFunc = nullptr;
static void* s_enableGLParam;
static void* s_disableGLParam;
static bool s_stopGlobalGLEnableFunc = false;

void cgeSetGLContextEnableFunction(CGEEnableGLContextFunction func, void* param)
{
    s_enableGLFunc = func;
    s_enableGLParam = param;
}

void cgeSetGLContextDisableFunction(CGEDisableGLContextFunction func, void* param)
{
    s_disableGLFunc = func;
    s_disableGLParam = param;
}

void* cgeGetGLEnableParam()
{
    return s_enableGLParam;
}

void* cgeGetGLDisableParam()
{
    return s_disableGLParam;
}

void cgeStopGlobalGLEnableFunction()
{
    s_stopGlobalGLEnableFunc = true;
}

void cgeRestoreGlobalGLEnableFunction()
{
    s_stopGlobalGLEnableFunc = false;
}

void cgeEnableGlobalGLContext()
{
    if (s_enableGLFunc)
        s_enableGLFunc(s_enableGLParam);
}

void cgeDisableGlobalGLContext()
{
    if (s_disableGLFunc)
        s_disableGLFunc(s_disableGLParam);
}

#endif

//////////////////////////////////////////////////////////////////////////

static CGEBufferLoadFun s_loadDataWithSourceNameCommon = nullptr;
static CGEBufferUnloadFun s_unloadBufferDataCommon = nullptr;
static void* s_cgeLoadParam = nullptr;
static void* s_cgeUnloadParam = nullptr;

void cgeSetCommonLoadFunction(CGEBufferLoadFun fun, void* arg)
{
    s_loadDataWithSourceNameCommon = fun;
    s_cgeLoadParam = arg;
}
void cgeSetCommonUnloadFunction(CGEBufferUnloadFun fun, void* arg)
{
    s_unloadBufferDataCommon = fun;
    s_cgeUnloadParam = arg;
}

void* cgeLoadResourceCommon(const char* sourceName, void** bufferData, CGEBufferFormat* fmt, GLint* w, GLint* h)
{
    if (s_loadDataWithSourceNameCommon != nullptr)
        return s_loadDataWithSourceNameCommon(sourceName, bufferData, w, h, fmt, &s_cgeLoadParam);
    return nullptr;
}

bool cgeUnloadResourceCommon(void* bufferArg)
{
    if (s_unloadBufferDataCommon != nullptr)
        return s_unloadBufferDataCommon(bufferArg, s_cgeUnloadParam);
    return false;
}

CGEBufferLoadFun cgeGetCommonLoadFunc()
{
    return s_loadDataWithSourceNameCommon;
}

CGEBufferUnloadFun cgeGetCommonUnloadFunc()
{
    return s_unloadBufferDataCommon;
}

void* cgeGetCommonLoadArg()
{
    return s_cgeLoadParam;
}

void* cgeGetCommonUnloadArg()
{
    return s_cgeUnloadParam;
}

char* cgeGetScaledBufferOutofSize(const void* buffer, int& w, int& h, int channel, int minSizeX, int minSizeY)
{
    if ((minSizeX < w && minSizeY < h) || buffer == nullptr) return nullptr;
    char* tmpbuffer = nullptr;
    const char* data = (const char*)buffer;

    int width = w;
    double scale = CGE_MIN(w / (float)minSizeX, h / (float)minSizeY);
    w = ceilf(w / scale);
    h = ceilf(h / scale);

    int len = w * h;
    tmpbuffer = new char[len * channel];
    if (channel == 4)
    {
        for (int i = 0; i != h; ++i)
        {
            for (int j = 0; j != w; ++j)
            {
                const int L = (j + i * w) * channel;
                const int R = (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) * channel;
                tmpbuffer[L] = data[R];
                tmpbuffer[L + 1] = data[R + 1];
                tmpbuffer[L + 2] = data[R + 2];
                tmpbuffer[L + 3] = data[R + 3];
            }
        }
    }
    else if (channel == 3)
    {
        for (int i = 0; i != h; ++i)
        {
            for (int j = 0; j != w; ++j)
            {
                const int L = (j + i * w) * channel;
                const int R = (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) * channel;
                tmpbuffer[L] = data[R];
                tmpbuffer[L + 1] = data[R + 1];
                tmpbuffer[L + 2] = data[R + 2];
            }
        }
    }
    return tmpbuffer;
}

char* cgeGetScaledBufferInSize(const void* buffer, int& w, int& h, int channel, int maxSizeX, int maxSizeY)
{
    if ((maxSizeX > w && maxSizeY > h) || buffer == nullptr) return nullptr;
    char* tmpbuffer = nullptr;
    const char* data = (const char*)buffer;

    int width = w;
    double scale = CGE_MAX(w / (float)maxSizeX, h / (float)maxSizeY);
    w = w / scale;
    h = h / scale;
    int len = w * h;
    tmpbuffer = new char[len * channel];
    if (channel == 4)
    {
        for (int i = 0; i != h; ++i)
        {
            for (int j = 0; j != w; ++j)
            {
                const int L = (j + i * w) * channel;
                const int R = (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) * channel;
                tmpbuffer[L] = data[R];
                tmpbuffer[L + 1] = data[R + 1];
                tmpbuffer[L + 2] = data[R + 2];
                tmpbuffer[L + 3] = data[R + 3];
            }
        }
    }
    else if (channel == 3)
    {
        for (int i = 0; i != h; ++i)
        {
            for (int j = 0; j != w; ++j)
            {
                const int L = (j + i * w) * channel;
                const int R = (static_cast<int>(j * scale) + static_cast<int>(i * scale) * width) * channel;
                tmpbuffer[L] = data[R];
                tmpbuffer[L + 1] = data[R + 1];
                tmpbuffer[L + 2] = data[R + 2];
            }
        }
    }
    return tmpbuffer;
}

/////////////////////////////////////////////////

TextureObject::TextureObject(GLuint texture, const CGESizei& size) :
    m_texture(texture), m_size(size)
{
    if (texture == 0 && size.width != 0 && size.height != 0)
    {
        resize(size.width, size.height);
    }
}

TextureObject::TextureObject(TextureObject&& t) noexcept :
    m_texture(t.texture()), m_size(t.size())
{
    t.cleanup(false);
}

TextureObject::~TextureObject()
{
    if (m_texture != 0)
    {
        cleanup(true);
    }
}

TextureObject& TextureObject::operator=(TextureObject&& t) noexcept
{
    if (this == &t)
    {
        return *this;
    }
    m_texture = t.texture();
    m_size = t.size();
    t.cleanup(false);
    return *this;
}

TextureObject& TextureObject::operator=(TextureInfo&& t)
{
    m_texture = t.name;
    m_size = { t.width, t.height };
    t.name = 0;
    return *this;
}

void TextureObject::cleanup(bool deleteTexture)
{
    if (deleteTexture && m_texture != 0)
    {
        assert(glIsTexture(m_texture));
        CGE_DELETE_GL_OBJS(glDeleteTextures, m_texture);
    }
    m_texture = 0;
    m_size.set(0, 0);
}

bool TextureObject::resize(int w, int h, const void* buffer, GLenum format)
{
    if (m_texture == 0 || m_size.width != w || m_size.height != h || buffer != nullptr)
    {
        if (w == 0 || h == 0)
        {
            assert(0 && "TextureObject::resize must not be 0!");
            return false;
        }

        int channel;
        switch (format)
        {
        case GL_LUMINANCE:
            channel = 1;
            break;
        case GL_LUMINANCE_ALPHA:
            channel = 2;
            break;
        case GL_RGB:
            channel = 3;
            break;
        case GL_RGBA:
            channel = 4;
            break;
        default:
            assert(0);
            channel = 4;
            break;
        }

        if (m_texture == 0)
        {
            m_texture = cgeGenTextureWithBuffer(buffer, w, h, format, GL_UNSIGNED_BYTE, channel);
            m_size.set(w, h);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, m_texture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            if (m_size.width != w || m_size.height != h)
            {
                m_size.set(w, h);
                glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, buffer);
            }
            else
            {
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, format, GL_UNSIGNED_BYTE, buffer);
            }
        }

        return true;
    }
    return false;
}

//////////////

FrameBufferWithTexture::~FrameBufferWithTexture()
{
    if (m_renderBuffer != 0)
    {
        CGE_DELETE_GL_OBJS(glDeleteRenderbuffers, m_renderBuffer);
    }
}

void FrameBufferWithTexture::bindTexture2D(GLsizei w, GLsizei h, const void* buffer)
{
    if (resize(w, h, buffer))
    {
        FrameBuffer::bindTexture2D(m_texture);

        // auto resize renderbuffer if exist.
        if (m_renderBuffer != 0)
        {
            attachDepthBuffer();
        }
        assert(checkStatus());
    }
    else
    {
        FrameBuffer::bind();
    }
}

void FrameBufferWithTexture::attachDepthBuffer()
{
    bool shouldCreate = false;

    if (m_renderBuffer == 0)
    {
        shouldCreate = true;
    }
    else
    {
        GLint param[2] = { 0, 0 };
        glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, param);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, param + 1);
        shouldCreate = (param[0] != m_size.width || param[1] != m_size.height);
    }

    if (shouldCreate)
    {
        if (m_renderBuffer == 0)
            glGenRenderbuffers(1, &m_renderBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_size.width, m_size.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer);
    }
}

bool FrameBufferWithTexture::checkStatus()
{
    GLenum ret = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (ret != GL_FRAMEBUFFER_COMPLETE)
    {
        CGE_LOG_ERROR("Frame buffer incomplete: %x!\n", ret);
    }
    return ret == GL_FRAMEBUFFER_COMPLETE;
}

} // namespace CGE
