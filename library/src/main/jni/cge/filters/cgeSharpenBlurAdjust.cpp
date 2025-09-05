/*
 * cgeSharpenBlurAdjust.cpp
 *
 *  Created on: 2013-12-26
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#include "cgeSharpenBlurAdjust.h"

#include <cmath>
#include <cstdlib>

#define TEXTURE_SHARPENBLUR_BIND_ID 0

const static char* const s_fshFastBlur = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec2 samplerSteps;
    uniform int blurSamplerScale;
    uniform vec2 blurNorm;
    uniform int samplerRadius;

    float random(vec2 seed) {
        return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
    }

    void main() {
        if (samplerRadius == 0)
        {
            gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
            return;
        }

        vec4 resultColor = vec4(0.0);
        float blurPixels = 0.0;
        float offset = random(textureCoordinate) - 0.5;

        for (int i = -samplerRadius; i <= samplerRadius; ++i)
        {
            float percent = (float(i) + offset) / float(samplerRadius);
            float weight = 1.0 - abs(percent);
            vec2 coord = textureCoordinate + samplerSteps * percent * blurNorm * float(blurSamplerScale);
            resultColor += texture2D(inputImageTexture, coord) * weight;
            blurPixels += weight;
        }
        gl_FragColor = resultColor / blurPixels;
    });

const static char* const s_fshFixedRadiusBlur = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec2 samplerSteps;
    uniform int blurSamplerScale;
    uniform vec2 blurNorm;
    const int samplerRadius = 4;

    float random(vec2 seed) {
        return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
    }

    void main() {
        vec4 resultColor = vec4(0.0);
        float blurPixels = 0.0;
        // float offset = random(textureCoordinate) - 0.5;
        float offset = random(textureCoordinate) - 0.5;

        for (int i = -samplerRadius; i <= samplerRadius; ++i)
        {
            float percent = (float(i) + offset) / float(samplerRadius);
            float weight = 1.0 - abs(percent);
            vec2 coord = textureCoordinate + samplerSteps * percent * blurNorm * float(blurSamplerScale);
            resultColor += texture2D(inputImageTexture, coord) * weight;
            blurPixels += weight;
        }
        gl_FragColor = resultColor / blurPixels;
    });

const static char* const s_fshSharpenBlur_TwoStep = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec2 samplerSteps;
    uniform int blurSamplerScale;
    uniform vec2 blurNorm;
    uniform float intensity;
    uniform int samplerRadius;

    float random(vec2 seed) {
        return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
    }

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        if (samplerRadius == 0)
        {
            gl_FragColor = src;
            return;
        }

        vec4 resultColor = vec4(0.0);
        float blurPixels = 0.0;
        float offset = random(textureCoordinate) - 0.5;

        for (int i = -samplerRadius; i <= samplerRadius; ++i)
        {
            float percent = (float(i) + offset) / float(samplerRadius);
            float weight = 1.0 - abs(percent);
            vec2 coord = textureCoordinate + samplerSteps * percent * blurNorm * float(blurSamplerScale);
            resultColor += texture2D(inputImageTexture, coord) * weight;
            blurPixels += weight;
        }

        gl_FragColor = mix(resultColor / blurPixels, src, intensity);
    });

static CGEConstString s_fshMixTex = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform sampler2D blurredImageTexture;
    uniform float intensity;

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        vec4 tex = texture2D(blurredImageTexture, textureCoordinate);
        gl_FragColor = mix(tex, src, intensity);
    });

static CGEConstString s_fshSharpenBlur_Simple = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec2 samplerSteps;
    uniform float blurSamplerScale;
    uniform float intensity;

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        vec4 tmp = src +
            texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * vec2(-samplerSteps.x, 0.0)) +
            texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * vec2(samplerSteps.x, 0.0)) +
            texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * vec2(0.0, -samplerSteps.y)) +
            texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * vec2(0.0, samplerSteps.y));
        gl_FragColor = mix(tmp / 5.0, src, intensity);
    });

static CGEConstString s_fshSharpenBlur_SimpleBetter = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec2 samplerSteps;
    uniform float blurSamplerScale;
    uniform float intensity;

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        vec4 tmp = src;
        tmp += texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * vec2(-samplerSteps.x, 0.0));
        tmp += texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * vec2(samplerSteps.x, 0.0));
        tmp += texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * vec2(0.0, -samplerSteps.y));
        tmp += texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * vec2(0.0, samplerSteps.y));

        tmp += texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * samplerSteps);
        tmp += texture2D(inputImageTexture, textureCoordinate - blurSamplerScale * samplerSteps);
        tmp += texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * vec2(-samplerSteps.x, samplerSteps.y));
        tmp += texture2D(inputImageTexture, textureCoordinate + blurSamplerScale * vec2(samplerSteps.x, -samplerSteps.y));
        gl_FragColor = mix(tmp / 9.0, src, intensity);
    });

namespace CGE
{
CGEConstString CGESharpenBlurFilter::paramIntensity = "intensity";
CGEConstString CGESharpenBlurFilter::paramSamplerScale = "blurSamplerScale";
CGEConstString CGESharpenBlurFilter::paramSamplerStepName = "samplerSteps";
CGEConstString CGESharpenBlurFilter::paramBlurNormalName = "blurNorm";
CGEConstString CGESharpenBlurFilter::paramSamplerRadius = "samplerRadius";

bool CGESharpenBlurFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshSharpenBlur_TwoStep))
    {
#ifdef GL_ES
        m_samplerLimit = 6;
#else
        m_samplerLimit = 30;
#endif // GL_ES
        m_samplerScale = 0;
        setSamplerScale(1);
        return true;
    }
    return false;
}

void CGESharpenBlurFilter::setIntensity(float value)
{
    m_program.bind();
    m_program.sendUniformf(paramIntensity, value);
}

void CGESharpenBlurFilter::setSamplerScale(int value)
{
    m_samplerScale = abs(value);
    m_program.bind();
    m_program.sendUniformi(paramSamplerScale, m_samplerScale);
    int radius = CGE_MIN(m_samplerLimit, m_samplerScale);
    m_program.sendUniformi(paramSamplerRadius, radius);
}

void CGESharpenBlurFilter::setSamplerLimit(int value)
{
    m_samplerLimit = abs(value);
}

void CGESharpenBlurFilter::setBlurIntensity(float value)
{
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    setIntensity(1.0f - value);
}

void CGESharpenBlurFilter::setSharpenIntensity(float value)
{
    if (value < 0.0f) value = 0.0f;
    setIntensity(value + 1.0f);
}

void CGESharpenBlurFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    const CGESizei& sz = handler->getOutputFBOSize();

    m_program.bind();
    m_program.sendUniformf(paramSamplerStepName, 1.0f / sz.width, 1.0f / sz.height);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glActiveTexture(GL_TEXTURE0);

    // Pass one
    handler->setAsTarget();
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    m_program.sendUniformf(paramBlurNormalName, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Pass Two
    handler->swapBufferFBO();
    glBindTexture(GL_TEXTURE_2D, handler->getBufferTextureID());
    m_program.sendUniformf(paramBlurNormalName, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

//////////////////////////////////////////////////////////////////////////

bool CGEBlurFastFilter::initWithoutFixedRadius(bool noFixed)
{
    if (noFixed && initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshFastBlur))
    {
#ifdef GL_ES
        setSamplerLimit(6);
#else
        setSamplerLimit(30);
#endif // GL_ES

        m_bUseFixedRadius = false;
        setSamplerScale(1);
        return true;
    }
    else if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshFixedRadiusBlur))
    {
        m_bUseFixedRadius = true;
        setSamplerScale(1);
        return true;
    }
    return false;
}

void CGEBlurFastFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint dstTexture, GLuint vertexBufferID)
{
    const CGESizei& sz = handler->getOutputFBOSize();

    m_program.bind();
    m_program.sendUniformf(paramSamplerStepName, 1.0f / sz.width, 1.0f / sz.height);

    glActiveTexture(GL_TEXTURE0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Pass one
    handler->setAsTarget();
    glBindTexture(GL_TEXTURE_2D, handler->getBufferTextureID());
    m_program.sendUniformf(paramBlurNormalName, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Pass Two
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexture, 0);
    glBindTexture(GL_TEXTURE_2D, handler->getTargetTextureID());
    m_program.sendUniformf(paramBlurNormalName, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CGEBlurFastFilter::setSamplerScale(int value)
{
    if (m_bUseFixedRadius)
    {
        m_program.bind();
        m_program.sendUniformi(paramSamplerScale, value);
    }
    else
    {
        CGESharpenBlurFilter::setSamplerScale(value);
    }
}

//////////////////////////////////////////////////////////////////////////

CGEConstString CGESharpenBlurFastFilter::paramIntensity = "intensity";
CGEConstString CGESharpenBlurFastFilter::paramBlurredTexName = "blurredImageTexture";

bool CGESharpenBlurFastFilter::init()
{
    if (m_blurProc.initWithoutFixedRadius() && initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshMixTex))
    {
        UniformParameters* param = new UniformParameters;
        param->pushSampler2D(paramBlurredTexName, &m_texture, TEXTURE_SHARPENBLUR_BIND_ID);
        setAdditionalUniformParameter(param);
        return true;
    }
    return false;
}

void CGESharpenBlurFastFilter::setIntensity(float value)
{
    m_program.bind();
    m_program.sendUniformf(paramIntensity, value);
}

void CGESharpenBlurFastFilter::setBlurIntensity(float value)
{
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    setIntensity(1.0f - value);
}

void CGESharpenBlurFastFilter::setSharpenIntensity(float value)
{
    if (value < 0.0f) value = 0.0f;
    setIntensity(value + 1.0f);
}

void CGESharpenBlurFastFilter::setSamplerScale(int value)
{
    if (m_samplerScaleValue == value)
        return;
    m_blurProc.setSamplerScale(value);
    flush();
    m_samplerScaleValue = value;
}

void CGESharpenBlurFastFilter::setSamplerLimit(int value)
{
    m_blurProc.setSamplerLimit(value);
}

void CGESharpenBlurFastFilter::flush()
{
    glDeleteTextures(1, &m_texture);
    m_texture = 0;
}

void CGESharpenBlurFastFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();
    const CGESizei& sz = handler->getOutputFBOSize();
    if (m_texture == 0)
    {
        m_texture = cgeGenTextureWithBuffer(nullptr, sz.width, sz.height, GL_RGBA, GL_UNSIGNED_BYTE);
        m_blurProc.render2Texture(handler, m_texture, vertexBufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handler->getTargetTextureID(), 0);
    }

    CGEImageFilterInterface::render2Texture(handler, srcTexture, vertexBufferID);
}

bool CGESharpenBlurFastWithFixedBlurRadiusFilter::init()
{
    if (m_blurProc.initWithoutFixedRadius(false) && initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshMixTex))
    {
        UniformParameters* param = new UniformParameters;
        param->pushSampler2D(paramBlurredTexName, &m_texture, TEXTURE_SHARPENBLUR_BIND_ID);
        setAdditionalUniformParameter(param);
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////

CGEConstString CGESharpenBlurSimpleFilter::paramIntensity = "intensity";
CGEConstString CGESharpenBlurSimpleFilter::paramSamplerScale = "blurSamplerScale";
CGEConstString CGESharpenBlurSimpleFilter::paramSamplerStepName = "samplerSteps";

bool CGESharpenBlurSimpleFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshSharpenBlur_Simple))
    {
        m_program.bind();
        m_program.sendUniformf(paramIntensity, 0.0f);
        m_program.sendUniformf(paramSamplerScale, 1.0f);
        UniformParameters* param = new UniformParameters;
        param->requireStepsFactor(paramSamplerStepName);
        setAdditionalUniformParameter(param);
        return true;
    }
    return false;
}

void CGESharpenBlurSimpleFilter::setIntensity(float value)
{
    m_program.bind();
    m_program.sendUniformf(paramIntensity, value);
}

void CGESharpenBlurSimpleFilter::setBlurIntensity(float value)
{
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    setIntensity(1.0f - value);
}

void CGESharpenBlurSimpleFilter::setSharpenIntensity(float value)
{
    if (value < 0.0f) value = 0.0f;
    setIntensity(value + 1.0f);
}

void CGESharpenBlurSimpleFilter::setSamplerScale(float value)
{
    m_program.bind();
    m_program.sendUniformf(paramSamplerScale, value);
}

bool CGESharpenBlurSimpleBetterFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshSharpenBlur_SimpleBetter))
    {
        m_program.bind();
        m_program.sendUniformf(paramIntensity, 0.0f);
        m_program.sendUniformf(paramSamplerScale, 1.0f);
        UniformParameters* param = new UniformParameters;
        param->requireStepsFactor(paramSamplerStepName);
        setAdditionalUniformParameter(param);
        return true;
    }
    return false;
}

} // namespace CGE
