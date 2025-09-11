/*
 * cgeImageProcessing.cpp
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#include "cgeImageFilter.h"

CGE_UNEXPECTED_ERR_MSG(
    static int sFilterCount = 0;)

namespace CGE
{

CGEConstString g_vshDefault = CGE_SHADER_STRING(
    attribute vec2 vPosition;
    attribute vec2 aTexCoord;
    varying vec2 textureCoordinate;
    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);
        textureCoordinate = aTexCoord;
    });

CGEConstString g_vshDefaultWithoutTexCoord = CGE_SHADER_STRING(
    attribute vec2 vPosition;
    varying vec2 textureCoordinate;
    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);
        // An opportunism code. Do not use it unless you know what it means.
        textureCoordinate = (vPosition.xy + 1.0) / 2.0;
    });

CGEConstString g_vshDrawToScreen = CGE_SHADER_STRING(
    attribute vec2 vPosition;
    varying vec2 textureCoordinate;
    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);
        textureCoordinate = (vec2(vPosition.x, -vPosition.y) + 1.0) / 2.0;
    });

CGEConstString g_vshDrawToScreenRot90 = CGE_SHADER_STRING(
    attribute vec2 vPosition;
    varying vec2 textureCoordinate;
    const mat2 mRot = mat2(0.0, 1.0, -1.0, 0.0);
    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);
        textureCoordinate = vec2(vPosition.x, -vPosition.y) / 2.0 * mRot + 0.5;
    });

CGEConstString g_fshDefault = CGE_SHADER_STRING_PRECISION_L(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    void main() {
        gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    });

//////////////////////////////////////////////////////////////////////////

static CGEConstString s_fshFastAdjust = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec3 curveArray[% d]; // The array size is defined by "CURVE_PRECISION".

    const float curvePrecision = % .1f; // The same to above.

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        gl_FragColor = vec4(curveArray[int(src.r * curvePrecision)].r,
                            curveArray[int(src.g * curvePrecision)].g,
                            curveArray[int(src.b * curvePrecision)].b,
                            src.a);
    });

static char s_fshFastAdjustBuffer[512];
CGEConstString g_fshFastAdjust = s_fshFastAdjustBuffer;

CGEConstString g_paramFastAdjustArrayName = "curveArray";

//////////////////////////////////////////////////////////////////////////

static CGEConstString s_fshFastAdjustRGB = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform float curveArrayRGB[% d]; // The array size is defined by "CURVE_PRECISION".

    const float curvePrecision = % .1f; // The same to above.

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        gl_FragColor = vec4(curveArrayRGB[int(src.r * curvePrecision)],
                            curveArrayRGB[int(src.g * curvePrecision)],
                            curveArrayRGB[int(src.b * curvePrecision)],
                            src.a);
    });

static char s_fshFastAdjustRGBBuffer[512];
CGEConstString g_fshFastAdjustRGB = s_fshFastAdjustRGBBuffer;

CGEConstString g_paramFastAdjustRGBArrayName = "curveArrayRGB";

//////////////////////////////////////////////////////////////////////////

static inline int s_genFastAdjustShader(char* dst, const char* src, int precision)
{
    return sprintf(dst, src, precision, precision - 1.0f);
}

//程序启动时对 CGE_CURVE_PRECISION 作预处理
static bool _dummy = s_genFastAdjustShader(s_fshFastAdjustBuffer, s_fshFastAdjust, CGE_CURVE_PRECISION) && s_genFastAdjustShader(s_fshFastAdjustRGBBuffer, s_fshFastAdjustRGB, CGE_CURVE_PRECISION);

//////////////////////////////////////////////////////////////////////////

CGEConstString g_fshCurveMapNoIntensity = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform sampler2D curveTexture; // We do not use sampler1D because GLES dosenot support that.

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        gl_FragColor = vec4(texture2D(curveTexture, vec2(src.r, 0.0)).r,
                            texture2D(curveTexture, vec2(src.g, 0.0)).g,
                            texture2D(curveTexture, vec2(src.b, 0.0)).b,
                            src.a);
    });

CGEConstString g_paramCurveMapTextureName = "curveTexture";

//////////////////////////////////////////////////////////////////////////

CGEImageFilterInterfaceAbstract::CGEImageFilterInterfaceAbstract()
{
    CGE_ENABLE_GLOBAL_GLCONTEXT();
    CGE_UNEXPECTED_ERR_MSG(
        CGE_LOG_KEEP("Filter create, total: %d\n", ++sFilterCount);)
}

CGEImageFilterInterfaceAbstract::~CGEImageFilterInterfaceAbstract()
{
    CGE_UNEXPECTED_ERR_MSG(
        CGE_LOG_KEEP("Filter release, remain: %d\n", --sFilterCount);)
}

//////////////////////////////////////////////////////////////////////////

CGEConstString CGEImageFilterInterface::paramInputImageName = "inputImageTexture";
CGEConstString CGEImageFilterInterface::paramPositionIndexName = "vPosition";

CGEImageFilterInterface::CGEImageFilterInterface() :
    m_uniformParam(nullptr)
{
    //将所有filter的 paramPositionIndexName 绑定到 0
    m_program.bindAttribLocation(paramPositionIndexName, 0);
}

CGEImageFilterInterface::~CGEImageFilterInterface()
{
    delete m_uniformParam;
}

//////////////////////////////////////////////////////////////////////////
bool CGEImageFilterInterface::initShadersFromString(const char* vsh, const char* fsh)
{
    return m_program.initWithShaderStrings(vsh, fsh);
}

void CGEImageFilterInterface::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    if (m_uniformParam != nullptr)
        m_uniformParam->assignUniforms(handler, m_program.programID());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    cgeCheckGLError("glDrawArrays");
}

void CGEImageFilterInterface::setAdditionalUniformParameter(UniformParameters* param)
{
    if (m_uniformParam == param) return;
    delete m_uniformParam;
    m_uniformParam = param;
}

//////////////////////////////////////////////////////////////////////////

CGEConstString CGEFastAdjustFilter::paramArray = g_paramFastAdjustArrayName;

bool CGEFastAdjustFilter::init()
{
    initCurveArrays();
    return initShadersFromString(CGE::g_vshDefaultWithoutTexCoord, g_fshFastAdjust);
}

void CGEFastAdjustFilter::assignCurveArrays()
{
    m_program.bind();
    GLint index = glGetUniformLocation(m_program.programID(), paramArray);
    if (index < 0)
    {
        CGE_LOG_ERROR("CGEFastAdjustFilter: Failed when assignCurveArray();\n");
        return;
    }
    glUniform3fv(index, (GLsizei)m_curve.size(), &m_curve[0][0]);
}

void CGEFastAdjustFilter::initCurveArrays()
{
    m_curve.resize(CGE_CURVE_PRECISION);

    for (std::vector<float>::size_type t = 0; t != CGE_CURVE_PRECISION; ++t)
    {
        m_curve[t][0] = float(t) / (CGE_CURVE_PRECISION - 1.0f);
        m_curve[t][1] = float(t) / (CGE_CURVE_PRECISION - 1.0f);
        m_curve[t][2] = float(t) / (CGE_CURVE_PRECISION - 1.0f);
    }
}

CGEConstString CGEFastAdjustRGBFilter::paramArrayRGB = g_paramFastAdjustRGBArrayName;

bool CGEFastAdjustRGBFilter::init()
{
    initCurveArray();
    return initShadersFromString(CGE::g_vshDefaultWithoutTexCoord, g_fshFastAdjustRGB);
}

void CGEFastAdjustRGBFilter::assignCurveArray()
{
    m_program.bind();
    GLint index = glGetUniformLocation(m_program.programID(), paramArrayRGB);
    if (index < 0)
    {
        CGE_LOG_ERROR("CGEFastAdjustRGBFilter: Failed when assignCurveArray();\n");
        return;
    }
    glUniform1fv(index, (GLsizei)m_curveRGB.size(), m_curveRGB.data());
}

void CGEFastAdjustRGBFilter::initCurveArray()
{
    m_curveRGB.resize(CGE_CURVE_PRECISION);
    //	m_curveRGB.shrink_to_fit();
    for (std::vector<float>::size_type t = 0; t != CGE_CURVE_PRECISION; ++t)
    {
        m_curveRGB[t] = float(t) / (CGE_CURVE_PRECISION - 1.0f);
    }
}

} // namespace CGE
