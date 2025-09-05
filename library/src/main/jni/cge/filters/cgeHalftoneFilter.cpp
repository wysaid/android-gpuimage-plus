/*
 * cgeHalftoneFilter.cpp
 *
 *  Created on: 2015-1-29
 *      Author: Wang Yang
 */

#include "cgeHalftoneFilter.h"

static CGEConstString s_fshHalftone = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform vec2 dotPercent;
    uniform float aspectRatio;

    // const vec3 weight = vec3(0.2125, 0.7154, 0.0721);

    void main() {
        vec2 samplePos = textureCoordinate - mod(textureCoordinate, dotPercent) + 0.5 * dotPercent;
        vec2 coordToUse = vec2(textureCoordinate.x, (textureCoordinate.y - 0.5) * aspectRatio + 0.5);
        vec2 adjustedPos = vec2(samplePos.x, (samplePos.y - 0.5) * aspectRatio + 0.5);

        float dis = distance(coordToUse, adjustedPos);

        vec4 color = texture2D(inputImageTexture, samplePos);
        vec3 dotScaling = 1.0 - color.rgb; // dot(color.rgb, weight);
        vec3 presenceDot = 1.0 - step(dis, dotPercent.x * dotScaling * 0.5);

        gl_FragColor = vec4(presenceDot, color.a);
    });

namespace CGE
{
CGEConstString CGEHalftoneFilter::paramAspectRatio = "aspectRatio";
CGEConstString CGEHalftoneFilter::paramDotPercent = "dotPercent";

bool CGEHalftoneFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshHalftone))
    {
        setDotSize(1.0f);
        return true;
    }
    return false;
}

void CGEHalftoneFilter::setDotSize(float value)
{
    m_dotSize = value;
}

void CGEHalftoneFilter::setIntensity(float value)
{
    setDotSize(value);
}

void CGEHalftoneFilter::render2Texture(CGE::CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    CGESizei sz = handler->getOutputFBOSize();
    float aspectRatio = sz.width / (float)sz.height;
    float dotPercent = m_dotSize / sz.width;
    m_program.sendUniformf(paramAspectRatio, aspectRatio);
    m_program.sendUniformf(paramDotPercent, dotPercent, dotPercent / aspectRatio);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    cgeCheckGLError("glDrawArrays");
}
} // namespace CGE