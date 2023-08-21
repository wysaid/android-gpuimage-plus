/*
 * cgeDynamicWaveFilter.cpp
 *
 *  Created on: 2015-11-12
 *      Author: Wang Yang
 */

#include "cgeDynamicWaveFilter.h"

static CGEConstString s_fshWave = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform float motion;
    uniform float angle;
    uniform float strength;
    void main() {
        vec2 coord;
        coord.x = textureCoordinate.x + strength * sin(motion + textureCoordinate.x * angle);
        coord.y = textureCoordinate.y + strength * sin(motion + textureCoordinate.y * angle);
        gl_FragColor = texture2D(inputImageTexture, coord);
    });

namespace CGE
{
CGEConstString CGEDynamicWaveFilter::paramMotion = "motion";
CGEConstString CGEDynamicWaveFilter::paramAngle = "angle";
CGEConstString CGEDynamicWaveFilter::paramStrength = "strength";

bool CGEDynamicWaveFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshWave))
    {
        m_program.bind();
        m_motionLoc = m_program.uniformLocation(paramMotion);
        m_angleLoc = m_program.uniformLocation(paramAngle);
        m_strengthLoc = m_program.uniformLocation(paramStrength);
        setWaveAngle(20.0f);
        setStrength(0.01f);
        m_motion = 0.0f;
        m_autoMotion = false;
        return true;
    }

    return false;
}

void CGEDynamicWaveFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    if (m_autoMotion)
    {
        m_motion += m_motionSpeed;
        glUniform1f(m_motionLoc, m_motion);
        if (m_motion > 3.14159f * m_angle)
        {
            m_motion -= 3.14159f * m_angle;
        }
    }

    if (m_uniformParam != nullptr)
        m_uniformParam->assignUniforms(handler, m_program.programID());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    cgeCheckGLError("glDrawArrays");
}

void CGEDynamicWaveFilter::setIntensity(float value)
{
    setAutoMotionSpeed(value);
}

void CGEDynamicWaveFilter::setWaveMotion(float motion)
{
    m_motion = motion;
    m_program.bind();
    glUniform1f(m_motionLoc, motion);
}

void CGEDynamicWaveFilter::setWaveAngle(float angle)
{
    m_angle = angle;
    m_program.bind();
    glUniform1f(m_angleLoc, angle);
}

void CGEDynamicWaveFilter::setStrength(float strength)
{
    m_strength = strength;
    m_program.bind();
    glUniform1f(m_strengthLoc, strength);
}

void CGEDynamicWaveFilter::setAutoMotionSpeed(float speed)
{
    m_motionSpeed = speed;
    m_autoMotion = (speed > 0.0f);
}

} // namespace CGE