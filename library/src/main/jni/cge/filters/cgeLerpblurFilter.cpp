/*
 * cgeLerpblurFilter.cpp
 *
 *  Created on: 2015-8-3
 *      Author: Wang Yang
 */

#include "cgeLerpblurFilter.h"

#include <cmath>

static CGEConstString s_fshScale = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    void main() {
        gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    });

namespace CGE
{
CGELerpblurFilter::~CGELerpblurFilter()
{
    _clearMipmaps();
}

bool CGELerpblurFilter::init()
{
    memset(m_texCache, 0, sizeof(m_texCache));
    m_intensity = 0;
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshScale))
    {
        m_mipmapBase = 1.0f;
        m_isBaseChanged = true;
        return true;
    }
    return false;
}

void CGELerpblurFilter::setBlurLevel(int value)
{
    m_intensity = value;
    if (m_intensity > MAX_LERP_BLUR_INTENSITY)
        m_intensity = MAX_LERP_BLUR_INTENSITY;
}

void CGELerpblurFilter::setIntensity(float value)
{
    if (value <= 0.5f)
    {
        setBlurLevel(int(value * (2 * MAX_LERP_BLUR_INTENSITY)));
        if (m_mipmapBase != 1.0f)
            setMipmapBase(1.0f);
    }
    else
    {
        setBlurLevel(MAX_LERP_BLUR_INTENSITY);
        setMipmapBase((value - 0.5f) * 4.0f + 1.0f);
    }
}

void CGELerpblurFilter::_genMipmaps(int width, int height)
{
    _clearMipmaps();
    GLuint texIDs[MAX_LERP_BLUR_INTENSITY];

#ifdef IOS_SDK

    static GLenum textureType = GL_FALSE;

    if (textureType == GL_FALSE)
    {
        textureType = GL_UNSIGNED_BYTE;

        // Apple A9 (iphone 6s+) GL_UNSIGNED_BYTE纹理精度较低， 需要使用 GL_HALF_FLOAT_OES
        //为了增强兼容性， 对于支持 EXT_color_buffer_half_float 扩展的设备， 通通使用 GL_HALF_FLOAT_OES
#ifdef GL_HALF_FLOAT_OES

        if (cgeCheckGLExtension("EXT_color_buffer_half_float"))
        {
            textureType = GL_HALF_FLOAT_OES;
            CGE_LOG_INFO("###Lerp blur: EXT_color_buffer_half_float used!!\n");
        }
#endif
    }

#else

    static const GLenum textureType = GL_UNSIGNED_BYTE;

#endif

    glGenTextures(MAX_LERP_BLUR_INTENSITY, texIDs);
    for (int i = 0; i != MAX_LERP_BLUR_INTENSITY; ++i)
    {
        CGESizei sz(_calcLevel(width, i + 2), _calcLevel(height, i + 2));
        if (sz.width < 1)
            sz.width = 1;
        if (sz.height < 1)
            sz.height = 1;
        glBindTexture(GL_TEXTURE_2D, texIDs[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sz.width, sz.height, 0, GL_RGBA, textureType, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        m_texCache[i].texID = texIDs[i];
        m_texCache[i].size = sz;
    }
}

void CGELerpblurFilter::_clearMipmaps()
{
    GLuint texIDs[MAX_LERP_BLUR_INTENSITY];
    for (int i = 0; i != MAX_LERP_BLUR_INTENSITY; ++i)
        texIDs[i] = m_texCache[i].texID;
    glDeleteTextures(MAX_LERP_BLUR_INTENSITY, texIDs);
    memset(m_texCache, 0, sizeof(m_texCache));
    m_cacheTargetSize.set(0, 0);
}

void CGELerpblurFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    if (m_intensity <= 0)
    {
        handler->swapBufferFBO();
        return;
    }

    // TODO: Useless code to avoid some strange error on some devices: mx4&mx5(powerVR g6200)
    handler->setAsTarget();

    m_program.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glActiveTexture(GL_TEXTURE0);

    const CGESizei& sz = handler->getOutputFBOSize();

    if (m_texCache[0].texID == 0 || m_cacheTargetSize.width != sz.width || m_cacheTargetSize.height != sz.height || m_isBaseChanged)
    {
        _genMipmaps(sz.width, sz.height);
        m_cacheTargetSize = sz;
        m_isBaseChanged = false;
        CGE_LOG_INFO("CGELerpblurFilter::render2Texture - Base Changing!\n");
    }

    m_framebuffer.bindTexture2D(m_texCache[0].texID);
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    glViewport(0, 0, m_texCache[0].size.width, m_texCache[0].size.height);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();

    // down scale
    for (int i = 1; i < m_intensity; ++i)
    {
        TextureCache& texCache = m_texCache[i];
        m_framebuffer.bindTexture2D(texCache.texID);
        glViewport(0, 0, texCache.size.width, texCache.size.height);

        glBindTexture(GL_TEXTURE_2D, m_texCache[i - 1].texID);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFlush();
    }

    // up scale
    for (int i = m_intensity - 1; i > 0; --i)
    {
        TextureCache& texCache = m_texCache[i - 1];
        m_framebuffer.bindTexture2D(texCache.texID);
        glViewport(0, 0, texCache.size.width, texCache.size.height);

        glBindTexture(GL_TEXTURE_2D, m_texCache[i].texID);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFlush();
    }

    handler->setAsTarget();
    glBindTexture(GL_TEXTURE_2D, m_texCache[0].texID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CGELerpblurFilter::setMipmapBase(float value)
{
    m_mipmapBase = value;
    if (m_mipmapBase < 0.6f)
        m_mipmapBase = 0.6f;
    m_isBaseChanged = true;
}

int CGELerpblurFilter::_calcLevel(int len, int level)
{
    // return len / powf(m_mipmapBase, level);
    return len / (level * m_mipmapBase);
}
} // namespace CGE