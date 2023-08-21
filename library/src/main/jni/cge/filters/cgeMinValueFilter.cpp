/*
 * cgeMinValueFilter.cpp
 *
 *  Created on: 2015-3-20
 *      Author: Wang Yang
 */

#include "cgeMinValueFilter.h"

// clang-format off

static CGEConstString s_minValueFunc = CGE_SHADER_STRING(

    float lum(vec4 value) {
        return dot(value.rgb, vec3(0.299, 0.587, 0.114));
    } vec4 getValue(vec4 newValue, vec4 originValue) {
        return mix(newValue, originValue, step(lum(originValue), lum(newValue)));
    });

static CGEConstString s_vshMinValue3x3 = CGE_SHADER_STRING(

    varying vec2 texCoord[9];
    attribute vec2 vPosition;

    uniform vec2 samplerSteps;

    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);

        vec2 thisCoord = (vPosition.xy + 1.0) / 2.0;

        texCoord[0] = thisCoord - samplerSteps;
        texCoord[1] = thisCoord + vec2(0, -samplerSteps.y);
        texCoord[2] = thisCoord + vec2(samplerSteps.x, -samplerSteps.y);
        texCoord[3] = thisCoord + vec2(-samplerSteps.x, 0.0);
        texCoord[4] = thisCoord;
        texCoord[5] = thisCoord + vec2(samplerSteps.x, 0.0);
        texCoord[6] = thisCoord + vec2(-samplerSteps.x, samplerSteps.y);
        texCoord[7] = thisCoord + vec2(0.0, samplerSteps.y);
        texCoord[8] = thisCoord + samplerSteps;
    });

static CGEConstString s_vshMinValue3x3_2 = CGE_SHADER_STRING(

    //针对OpenGL ES 可能仅有8个varying的设备进行兼容
    varying vec4 texCoord0;
    varying vec4 texCoord1;
    varying vec4 texCoord2;
    varying vec4 texCoord3;
    varying vec2 texCoord4;
    attribute vec2 vPosition;

    uniform vec2 samplerSteps;

    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);

        vec2 thisCoord = (vPosition.xy + 1.0) / 2.0;

        texCoord0.xy = thisCoord - samplerSteps;
        texCoord1.xy = thisCoord + vec2(0, -samplerSteps.y);
        texCoord2.xy = thisCoord + vec2(samplerSteps.x, -samplerSteps.y);
        texCoord3.xy = thisCoord + vec2(-samplerSteps.x, 0.0);
        texCoord4 = thisCoord;
        texCoord0.zw = thisCoord + vec2(samplerSteps.x, 0.0);
        texCoord1.zw = thisCoord + vec2(-samplerSteps.x, samplerSteps.y);
        texCoord2.zw = thisCoord + vec2(0.0, samplerSteps.y);
        texCoord3.zw = thisCoord + samplerSteps;
    });

static CGEConstString s_fshMinValue3x3 = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 texCoord[9];

    uniform sampler2D inputImageTexture;

    %s\n

    void main() {
        vec4 vMin;

        {
            vec4 vTemp;

            vMin = texture2D(inputImageTexture, texCoord[0]);

            vTemp = texture2D(inputImageTexture, texCoord[1]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[2]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[3]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[4]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[5]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[6]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[7]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[8]);
            vMin = getValue(vTemp, vMin);
        }

        gl_FragColor = vMin;
    });

static CGEConstString s_fshMinValue3x3_2 = CGE_SHADER_STRING_PRECISION_M(
    varying vec4 texCoord0;
    varying vec4 texCoord1;
    varying vec4 texCoord2;
    varying vec4 texCoord3;
    varying vec2 texCoord4;

    uniform sampler2D inputImageTexture;

    %s\n

    void main() {
        vec4 vMin;

        {
            vec4 vTemp;

            vMin = texture2D(inputImageTexture, texCoord0.xy);

            vTemp = texture2D(inputImageTexture, texCoord1.xy);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord2.xy);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord3.xy);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord4);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord0.zw);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord1.zw);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord2.zw);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord3.zw);
            vMin = getValue(vTemp, vMin);
        }

        gl_FragColor = vMin;
    });

// clang-format on

namespace CGE
{
CGEConstString CGEMinValueFilter3x3::paramSamplerStepsName = "samplerSteps";

bool CGEMinValueFilter3x3::init()
{
    char buffer[4096];

    sprintf(buffer, s_fshMinValue3x3, getShaderCompFunc());
    if (!initShadersFromString(s_vshMinValue3x3, buffer))
    {
        sprintf(buffer, s_fshMinValue3x3_2, getShaderCompFunc());
        if (!initShadersFromString(s_vshMinValue3x3_2, buffer))
            return false;
    }

    initLocations();
    return true;
}

void CGEMinValueFilter3x3::initLocations()
{
    m_program.bind();
    m_samplerStepsLoc = m_program.uniformLocation(paramSamplerStepsName);
}

const char* CGEMinValueFilter3x3::getShaderCompFunc()
{
    return s_minValueFunc;
}

void CGEMinValueFilter3x3::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();

    const auto& sz = handler->getOutputFBOSize();

    glUniform2f(m_samplerStepsLoc, 1.0f / sz.width, 1.0f / sz.height);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    cgeCheckGLError("glDrawArrays");
}
} // namespace CGE

//////////////////////////////////////////////////////////////////////////

static CGEConstString s_vshMinValue3x3Plus = CGE_SHADER_STRING(
    varying vec2 texCoord[13];

    attribute vec2 vPosition;

    uniform vec2 samplerSteps;

    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);

        vec2 thisCoord = (vPosition.xy + 1.0) / 2.0;

        texCoord[0] = thisCoord + vec2(0, -samplerSteps.y * 2.0);
        texCoord[1] = thisCoord - samplerSteps;
        texCoord[2] = thisCoord + vec2(0, -samplerSteps.y);
        texCoord[3] = thisCoord + vec2(samplerSteps.x, -samplerSteps.y);
        texCoord[4] = thisCoord + vec2(-samplerSteps.x * 2.0, 0.0);
        texCoord[5] = thisCoord + vec2(-samplerSteps.x, 0.0);
        texCoord[6] = thisCoord;
        texCoord[7] = thisCoord + vec2(samplerSteps.x, 0.0);
        texCoord[8] = thisCoord + vec2(samplerSteps.x * 2.0, 0.0);
        texCoord[9] = thisCoord + vec2(-samplerSteps.x, samplerSteps.y);
        texCoord[10] = thisCoord + vec2(0.0, samplerSteps.y);
        texCoord[11] = thisCoord + samplerSteps;
        texCoord[12] = thisCoord + vec2(0.0, 2.0 * samplerSteps.y);
    });

static CGEConstString s_vshMinValue3x3Plus_2 = CGE_SHADER_STRING(
    varying vec4 texCoord0;
    varying vec4 texCoord1;
    varying vec4 texCoord2;
    varying vec4 texCoord3;
    varying vec4 texCoord4;
    varying vec4 texCoord5;
    varying vec2 texCoord6;

    attribute vec2 vPosition;

    uniform vec2 samplerSteps;

    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);

        vec2 thisCoord = (vPosition.xy + 1.0) / 2.0;

        texCoord0.xy = thisCoord + vec2(0, -samplerSteps.y * 2.0);
        texCoord1.xy = thisCoord - samplerSteps;
        texCoord2.xy = thisCoord + vec2(0, -samplerSteps.y);
        texCoord3.xy = thisCoord + vec2(samplerSteps.x, -samplerSteps.y);
        texCoord4.xy = thisCoord + vec2(-samplerSteps.x * 2.0, 0.0);
        texCoord5.xy = thisCoord + vec2(-samplerSteps.x, 0.0);
        texCoord6 = thisCoord;
        texCoord0.zw = thisCoord + vec2(samplerSteps.x, 0.0);
        texCoord1.zw = thisCoord + vec2(samplerSteps.x * 2.0, 0.0);
        texCoord2.zw = thisCoord + vec2(-samplerSteps.x, samplerSteps.y);
        texCoord3.zw = thisCoord + vec2(0.0, samplerSteps.y);
        texCoord4.zw = thisCoord + samplerSteps;
        texCoord5.zw = thisCoord + vec2(0.0, 2.0 * samplerSteps.y);
    });

static CGEConstString s_fshMinValue3x3Plus = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 texCoord[13];

    uniform sampler2D inputImageTexture;

    %s\n

    void main() {
        vec4 vMin;

        {
            vec4 vTemp;

            vMin = texture2D(inputImageTexture, texCoord[0]);

            vTemp = texture2D(inputImageTexture, texCoord[1]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[2]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[3]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[4]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[5]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[6]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[7]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[8]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[9]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[10]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[11]);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord[12]);
            vMin = getValue(vTemp, vMin);
        }

        gl_FragColor = vMin;
    });

static CGEConstString s_fshMinValue3x3Plus_2 = CGE_SHADER_STRING_PRECISION_M(
    varying vec4 texCoord0;
    varying vec4 texCoord1;
    varying vec4 texCoord2;
    varying vec4 texCoord3;
    varying vec4 texCoord4;
    varying vec4 texCoord5;
    varying vec2 texCoord6;

    uniform sampler2D inputImageTexture;

    %s\n

    void main() {
        vec4 vMin;

        {
            vec4 vTemp;

            vMin = texture2D(inputImageTexture, texCoord0.xy);

            vTemp = texture2D(inputImageTexture, texCoord1.xy);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord2.xy);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord3.xy);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord4.xy);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord5.xy);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord6);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord0.zw);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord1.zw);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord2.zw);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord3.zw);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord4.zw);
            vMin = getValue(vTemp, vMin);

            vTemp = texture2D(inputImageTexture, texCoord5.zw);
            vMin = getValue(vTemp, vMin);
        }

        gl_FragColor = vMin;
    });

namespace CGE
{
bool CGEMinValueFilter3x3Plus::init()
{
    char buffer[4096];

    sprintf(buffer, s_fshMinValue3x3Plus, getShaderCompFunc());

    if (!initShadersFromString(s_vshMinValue3x3Plus, buffer))
    {
        sprintf(buffer, s_fshMinValue3x3Plus_2, getShaderCompFunc());

        if (!initShadersFromString(s_vshMinValue3x3Plus_2, buffer))
            return false;
    }

    initLocations();
    return true;
}
} // namespace CGE
