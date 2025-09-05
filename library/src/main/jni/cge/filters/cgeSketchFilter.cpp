/*
 * cgeSketchFilter.cpp
 *
 *  Created on: 2015-3-20
 *      Author: Wang Yang
 */

#include "cgeSketchFilter.h"

static CGEConstString s_fshSketch = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform sampler2D maxValueTex;

    uniform float intensity;

    float lum(vec4 v) {
        return dot(v.rgb, vec3(0.299, 0.587, 0.114));
    }

    void main() {
        vec4 origin = texture2D(inputImageTexture, textureCoordinate);
        vec4 maxValue = texture2D(maxValueTex, textureCoordinate);

        float lumOrigin = lum(origin);
        float lumMax = lum(maxValue) + 0.001;

        float blendColor = min(lumOrigin / lumMax, 1.0);

        // float blendColor = min(luminance + (luminance * revMaxValue) / (1.0 - revMaxValue), 1.0);

        gl_FragColor = vec4(mix(origin.rgb, vec3(blendColor), intensity), origin.a);
    });

namespace CGE
{
CGEConstString CGESketchFilter::paramCacheTextureName = "maxValueTex";
CGEConstString CGESketchFilter::paramIntensityName = "intensity";

CGESketchFilter::CGESketchFilter() :
    m_textureCache(0)
{
    m_cacheSize.set(-1, -1);
}

CGESketchFilter::~CGESketchFilter()
{
    glDeleteTextures(1, &m_textureCache);
}

bool CGESketchFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshSketch) && m_maxValueFilter.init())
    {
        setIntensity(1.0f);
        m_program.bind();
        m_program.sendUniformi(paramCacheTextureName, 1);
        return true;
    }
    return false;
}

void CGESketchFilter::setIntensity(float intensity)
{
    m_program.bind();
    m_program.sendUniformf(paramIntensityName, intensity);
}

void CGESketchFilter::render2Texture(CGE::CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    CGESizei sz = handler->getOutputFBOSize();

    if (m_textureCache == 0 || sz != m_cacheSize)
    {
        glDeleteTextures(1, &m_textureCache);
        m_textureCache = cgeGenTextureWithBuffer(nullptr, sz.width, sz.height, GL_RGBA, GL_UNSIGNED_BYTE);
        m_cacheSize = sz;
    }

    handler->setAsTarget();

    glActiveTexture(GL_TEXTURE0);

    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureCache, 0);

        ProgramObject& programMaxValue = m_maxValueFilter.getProgram();
        programMaxValue.bind();

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindTexture(GL_TEXTURE_2D, srcTexture);
        glUniform2f(m_maxValueFilter.getStepsLocation(), 1.0f / sz.width, 1.0f / sz.height);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //
        //  			glBindTexture(GL_TEXTURE_2D, handler->getTargetTextureID());
        //  			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handler->getTargetTextureID(), 0);

    m_program.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindTexture(GL_TEXTURE_2D, srcTexture);
    //		m_program.sendUniformi(paramInputImageName, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textureCache);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    cgeCheckGLError("glDrawArrays");
}

void CGESketchFilter::flush()
{
    glDeleteTextures(1, &m_textureCache);
    m_textureCache = 0;
}

} // namespace CGE