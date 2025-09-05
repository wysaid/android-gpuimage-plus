/*
 * cgeDynamicWaveFilter.cpp
 *
 *  Created on: 2016-8-12
 *      Author: Wang Yang
 */

#include "cgeMotionFlowFilter.h"

#include "cgeTextureUtils.h"

#include <cmath>

CGEConstString s_fsh = CGE_SHADER_STRING_PRECISION_L(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform float alpha;
    void main() {
        gl_FragColor = texture2D(inputImageTexture, textureCoordinate) * alpha;
    });

namespace CGE
{
CGEConstString CGEMotionFlowFilter::paramAlphaName = "alpha";

CGEMotionFlowFilter::CGEMotionFlowFilter() :
    m_totalFrames(10), m_delayFrames(0), m_delayedFrames(0), m_dAlpha(0.0f)
{
}

CGEMotionFlowFilter::~CGEMotionFlowFilter()
{
    clear();
    delete m_drawer;
}

void CGEMotionFlowFilter::clear()
{
    if (m_frameTextures.size() != 0)
    {
        glDeleteTextures((int)m_totalFrameTextures.size(), m_totalFrameTextures.data());
        m_frameTextures.clear();
        m_totalFrameTextures.clear();
    }
}

bool CGEMotionFlowFilter::init()
{
    m_drawer = TextureDrawer::create();
    if (m_program.initWithShaderStrings(g_vshDefaultWithoutTexCoord, s_fsh) && m_drawer != nullptr)
    {
        setTotalFrames(m_totalFrames);
        m_program.bind();
        m_alphaLoc = m_program.uniformLocation(paramAlphaName);
        return true;
    }
    return false;
}

void CGEMotionFlowFilter::setTotalFrames(int frames)
{
    clear();

    assert(frames > 0);

    if (frames <= 0)
        return;

    m_totalFrames = frames;
    m_dAlpha = 2.0f / ((frames + 1) * (frames + 2));
}

void CGEMotionFlowFilter::setFrameDelay(int delayFrame)
{
    m_delayFrames = delayFrame;
}

void CGEMotionFlowFilter::pushFrame(GLuint texture)
{
    //        glViewport(0, 0, m_width, m_height);

    if (m_frameTextures.size() < m_totalFrames)
    {
        GLuint texID = cgeGenTextureWithBuffer(nullptr, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE);
        m_framebuffer.bindTexture2D(texID);
        m_drawer->drawTexture(texture);
        m_frameTextures.push_back(texID);
        m_totalFrameTextures.push_back(texID);
    }
    else
    {
        GLuint texID = m_frameTextures.front();
        m_frameTextures.pop_front();
        m_framebuffer.bindTexture2D(texID);
        m_drawer->drawTexture(texture);
        m_frameTextures.push_back(texID);
    }
}

void CGEMotionFlowFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    const CGESizei& sz = handler->getOutputFBOSize();
    float alpha = 0.0f;

    if (m_width != sz.width || m_height != sz.height)
    {
        clear();
        m_width = sz.width;
        m_height = sz.height;
    }

    handler->setAsTarget();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_program.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    for (GLuint texID : m_frameTextures)
    {
        glUniform1f(m_alphaLoc, alpha += m_dAlpha);
        glBindTexture(GL_TEXTURE_2D, texID);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glUniform1f(m_alphaLoc, alpha += m_dAlpha);
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisable(GL_BLEND);

    if (m_delayedFrames < m_delayFrames)
    {
        ++m_delayedFrames;
    }
    else
    {
        m_delayedFrames = 0;
        pushFrame(srcTexture);
    }
}

} // namespace CGE