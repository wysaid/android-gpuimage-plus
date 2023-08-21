/*
 * cgeMultipleEffectsCommon.cpp
 *
 *  Created on: 2014-1-2
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#include "cgeMultipleEffectsCommon.h"

//////////////////////////////////////////////////////////////////////////

static int s_colorScaleStatus = 1;

void clip(int (*rgb)[3], float dark, float light, float* l, float* u, int total)
{
    int dc = (int)(total * dark * 3);
    int lc = (int)(total * light * 3);
    int lbi = 0;
    int acm = 0;
    for (int i = 0; i < 256; ++i)
    {
        acm += rgb[i][0];
        acm += rgb[i][1];
        acm += rgb[i][2];

        if (acm > dc)
        {
            lbi = i;
            break;
        }
    }

    *l = lbi / 255.0f;

    int ubi = 0;

    acm = 0;

    for (int i = 255; i >= 0; --i)
    {
        acm += rgb[i][0];
        acm += rgb[i][1];
        acm += rgb[i][2];

        if (acm > lc)
        {
            ubi = i;
            break;
        }
    }

    *u = ubi / 255.0f;
}

// You should make sure that the right frame buffer is binded!
void getScale(GLfloat* colorScaleLow, GLfloat* colorScaleRange, GLfloat* sat, GLfloat dark, GLfloat light, GLint w, GLint h)
{
    if (s_colorScaleStatus <= 0)
    {
        *colorScaleLow = 0.0f;
        *colorScaleRange = 1.0f;
        return;
    }

    unsigned char *data = (unsigned char*)malloc(w * h * 4 * sizeof(GLubyte)), *tmpBuffer = data;
    if (data == nullptr)
    {
        return;
    }
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

    int rgb[256][3] = {};
    int total = w * h;

    for (int i = 0; i != total; ++i)
    {
        ++rgb[data[0]][0];
        ++rgb[data[1]][1];
        ++rgb[data[2]][2];
        data += 4;
    }

    float lb = 0.0f, ub = 0.0f;
    float dd = dark, ll = light;
    float s = 0.0f;
    while (s < 0.05f)
    {
        clip(rgb, dd, ll, &lb, &ub, total);
        s = ub - lb;
        dd -= 0.02f;
        ll += 0.02f;
        if (dd <= 0.f && ll >= 1.0f)
        {
            s = 1.0f;
            lb = 0.f;
            break;
        }
        if (dd <= 0.f)
        {
            dd = 0.f;
        }
        if (ll >= 1.0f)
        {
            ll = 1.0f;
        }
    }
    *sat *= s;
    *colorScaleLow = lb;
    *colorScaleRange = s;
    free(tmpBuffer);
}

// You should make sure that the right frame buffer is binded!
void getHalfToneLowFac(GLfloat* low, GLfloat* fac, GLint w, GLint h)
{
    if (s_colorScaleStatus <= 0)
    {
        *low = 0.2f;
        *fac = 1.4f;
        return;
    }
    unsigned char *data = (unsigned char*)malloc(w * h * 4 * sizeof(GLubyte)), *tmpBuffer = data;
    if (data == nullptr)
    {
        *low = 0.2f;
        *fac = 1.4f;
        return;
    }
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

    int lum[256] = { 0 };
    int total = w * h;

    for (int i = 0; i != total; ++i)
    {
        ++lum[(unsigned char)((data[0] * int(256 * 0.299) + data[1] * int(256 * 0.587) + data[2] * int(256 * 0.114)) >> 8)];
        data += 4;
    }

    for (int i = 1; i != 256; ++i)
    {
        lum[i] += lum[i - 1];
    }

    *low = 0.0f;
    for (int i = 0; i != 256; ++i)
    {
        if (lum[i] / (GLfloat)lum[255] > 0.1f)
        {
            *low = (float)i;
            break;
        }
    }
    int up = 255;
    for (int i = 255; i > 0; --i)
    {
        if ((GLfloat)(lum[255] - lum[i]) / lum[255] > 0.1)
        {
            up = i;
            break;
        }
    }
    *fac = 255.0f / (up - *low);
    *low /= 255.0f;
    free(tmpBuffer);
}

//////////////////////////////////////////////////////////////////////////

namespace CGE
{
void cgeEnableColorScale()
{
    ++s_colorScaleStatus;
}

void cgeDisableColorScale()
{
    --s_colorScaleStatus;
}
} // namespace CGE

//////////////////////////////////////////////////////////////////////////

static CGEConstString s_fshLomo = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform vec2 colorScale; // x for scale clip, y for scale range.
    uniform vec2 vignette;   // x for start, y for end.

    uniform float intensity;
    uniform float saturation;

    const vec2 vignetteCenter = vec2(0.5, 0.5);

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);

        float d = distance(textureCoordinate, vignetteCenter);
        float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
        float alpha = 1.0 - percent * percent;

        float lum = dot(src.rgb, vec3(0.299, 0.587, 0.114));
        vec3 dst = lum * (1.0 - saturation) + saturation * src.rgb;
        dst = (dst - colorScale.x) / colorScale.y * alpha;

        gl_FragColor = vec4(mix(src.rgb, dst, intensity), src.a);
    });

static CGEConstString s_fshLomoLinear = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform vec2 colorScale; // x for scale clip, y for scale range.
    uniform vec2 vignette;   // x for start, y for end.

    uniform float intensity;
    uniform float saturation;

    const vec2 vignetteCenter = vec2(0.5, 0.5);

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);

        float d = distance(textureCoordinate, vignetteCenter);
        float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
        float alpha = 1.0 - percent;

        float lum = dot(src.rgb, vec3(0.299, 0.587, 0.114));
        vec3 dst = lum * (1.0 - saturation) + saturation * src.rgb;
        dst = (dst - colorScale.x) / colorScale.y * alpha;

        gl_FragColor = vec4(mix(src.rgb, dst, intensity), src.a);
    });

//////////////////////////////////////////////////////////////////////////

static CGEConstString s_fshLomoWithCurve = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec3 curveArray[256];       // The array size is defined by "CURVE_PRECISION".
    const float curvePrecision = 255.0; // The same to above.

    uniform vec2 colorScale; // x for scale clip, y for scale range.
    uniform vec2 vignette;   // x for start, y for end.

    uniform float saturation;

    const vec2 vignetteCenter = vec2(0.5, 0.5);

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);

        float d = distance(textureCoordinate, vignetteCenter);
        float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
        float alpha = 1.0 - percent * percent;

        float lum = dot(src.rgb, vec3(0.299, 0.587, 0.114));
        vec3 dst = lum * (1.0 - saturation) + saturation * src.rgb;
        dst = (dst - colorScale.x) / colorScale.y * alpha;

        dst = clamp(dst, 0.0, 1.0);

        gl_FragColor = vec4(curveArray[int(dst.r * curvePrecision)].r,
                            curveArray[int(dst.g * curvePrecision)].g,
                            curveArray[int(dst.b * curvePrecision)].b,
                            src.a);
    });

static CGEConstString s_fshLomoWithCurveLinear = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec3 curveArray[256]; // The array size is defined by "CURVE_PRECISION".

    const float curvePrecision = 255.0; // The same to above.

    uniform vec2 colorScale; // x for scale clip, y for scale range.
    uniform vec2 vignette;   // x for start, y for end.

    uniform float saturation;

    const vec2 vignetteCenter = vec2(0.5, 0.5);

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);

        float d = distance(textureCoordinate, vignetteCenter);
        float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
        float alpha = 1.0 - percent;

        float lum = dot(src.rgb, vec3(0.299, 0.587, 0.114));
        vec3 dst = lum * (1.0 - saturation) + saturation * src.rgb;
        dst = (dst - colorScale.x) / colorScale.y * alpha;

        dst = clamp(dst, 0.0, 1.0);

        gl_FragColor = vec4(curveArray[int(dst.r * curvePrecision)].r,
                            curveArray[int(dst.g * curvePrecision)].g,
                            curveArray[int(dst.b * curvePrecision)].b,
                            src.a);
    });

//////////////////////////////////////////////////////////////////////////

static CGEConstString s_fshLomoWithCurveTex = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform sampler2D curveTexture;

    uniform vec2 colorScale; // x for scale clip, y for scale range.
    uniform vec2 vignette;   // x for start, y for end.

    uniform float saturation;

    const vec2 vignetteCenter = vec2(0.5, 0.5);

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);

        float d = distance(textureCoordinate, vignetteCenter);
        float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
        float alpha = 1.0 - percent * percent;

        float lum = dot(src.rgb, vec3(0.299, 0.587, 0.114));
        vec3 dst = lum * (1.0 - saturation) + saturation * src.rgb;
        dst = (dst - colorScale.x) / colorScale.y * alpha;

        dst = clamp(dst, 0.0, 1.0);

        gl_FragColor = vec4(texture2D(curveTexture, vec2(dst.r, 0.0)).r,
                            texture2D(curveTexture, vec2(dst.g, 0.0)).g,
                            texture2D(curveTexture, vec2(dst.b, 0.0)).b,
                            src.a);
    });

static CGEConstString s_fshLomoWithCurveTexLinear = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform sampler2D curveTexture;

    uniform vec2 colorScale; // x for scale clip, y for scale range.
    uniform vec2 vignette;   // x for start, y for end.

    uniform float saturation;

    const vec2 vignetteCenter = vec2(0.5, 0.5);

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);

        float d = distance(textureCoordinate, vignetteCenter);
        float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
        float alpha = 1.0 - percent;

        float lum = dot(src.rgb, vec3(0.299, 0.587, 0.114));
        vec3 dst = lum * (1.0 - saturation) + saturation * src.rgb;
        dst = (dst - colorScale.x) / colorScale.y * alpha;

        dst = clamp(dst, 0.0, 1.0);

        gl_FragColor = vec4(texture2D(curveTexture, vec2(dst.r, 0.0)).r,
                            texture2D(curveTexture, vec2(dst.g, 0.0)).g,
                            texture2D(curveTexture, vec2(dst.b, 0.0)).b,
                            src.a);
    });

//////////////////////////////////////////////////////////////////////////

namespace CGE
{
CGEConstString CGELomoFilter::paramColorScaleName = "colorScale";
CGEConstString CGELomoFilter::paramSaturationName = "saturation";
CGEConstString CGELomoFilter::paramVignetteName = "vignette";
CGEConstString CGELomoFilter::paramAspectRatio = "aspectRatio";
CGEConstString CGELomoFilter::paramIntensityName = "intensity";

bool CGELomoFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshLomo))
    {
        setIntensity(1.0f);
        setVignette(0.2f, 0.8f);
        return true;
    }
    return false;
}

void CGELomoFilter::setIntensity(float value)
{
    m_program.bind();
    m_program.sendUniformf(paramIntensityName, value);
}

void CGELomoFilter::setVignette(float start, float end)
{
    m_program.bind();
    m_program.sendUniformf(paramVignetteName, start, end);
}

void CGELomoFilter::setSaturation(float value)
{
    m_saturate = value;
}

void CGELomoFilter::setColorScale(float low, float range)
{
    m_scaleDark = low;
    m_scaleLight = range;
}

void CGELomoFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    if (m_uniformParam != nullptr)
        m_uniformParam->assignUniforms(handler, m_program.programID());
    // More initialization...
    {
        const CGESizei& sz = handler->getOutputFBOSize();
        GLfloat sat = m_saturate, colorScale[2] = { 0.0f, 1.0f };
        if (s_colorScaleStatus > 0 && m_scaleDark >= 0.0f && m_scaleLight > 0.0f)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTexture, 0);
            CGE_LOG_CODE(clock_t t = clock());
            getScale(colorScale, colorScale + 1, &sat, m_scaleDark, m_scaleLight, sz.width, sz.height);
            CGE_LOG_INFO("getscale用时%gs\n", (double)(clock() - t) / CLOCKS_PER_SEC);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handler->getTargetTextureID(), 0);
        }
        m_program.sendUniformf(paramColorScaleName, colorScale[0], colorScale[1]);
        m_program.sendUniformf(paramSaturationName, sat);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    cgeCheckGLError("glDrawArrays");
}

//////////////////////////////////////////////////////////////////////////

bool CGELomoLinearFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshLomoLinear))
    {
        setIntensity(1.0f);
        setVignette(0.2f, 0.8f);
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////

CGEConstString CGELomoWithCurveFilter::paramColorScaleName = "colorScale";
CGEConstString CGELomoWithCurveFilter::paramSaturationName = "saturation";
CGEConstString CGELomoWithCurveFilter::paramVignetteName = "vignette";
CGEConstString CGELomoWithCurveFilter::paramAspectRatio = "aspectRatio";

bool CGELomoWithCurveFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshLomoWithCurve))
    {
        setVignette(0.2f, 0.8f);
        resetCurve(m_curve, CGE_CURVE_PRECISION);
        return true;
    }
    return false;
}

void CGELomoWithCurveFilter::setVignette(float start, float end)
{
    m_program.bind();
    m_program.sendUniformf(paramVignetteName, start, end);
}

void CGELomoWithCurveFilter::setSaturation(float value)
{
    m_saturate = value;
}

void CGELomoWithCurveFilter::setColorScale(float low, float range)
{
    m_scaleDark = low;
    m_scaleLight = range;
}

void CGELomoWithCurveFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    if (m_uniformParam != nullptr)
        m_uniformParam->assignUniforms(handler, m_program.programID());
    // More initialization...
    {
        const CGESizei& sz = handler->getOutputFBOSize();
        GLfloat sat = m_saturate, colorScale[2] = { 0.0f, 1.0f };
        if (s_colorScaleStatus > 0 && m_scaleDark >= 0.0f && m_scaleLight > 0.0f)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTexture, 0);
            CGE_LOG_CODE(clock_t t = clock());
            getScale(colorScale, colorScale + 1, &sat, m_scaleDark, m_scaleLight, sz.width, sz.height);
            CGE_LOG_INFO("getscale用时%gs\n", (double)(clock() - t) / CLOCKS_PER_SEC);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handler->getTargetTextureID(), 0);
        }
        m_program.sendUniformf(paramColorScaleName, colorScale[0], colorScale[1]);
        m_program.sendUniformf(paramSaturationName, sat);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    cgeCheckGLError("glDrawArrays");
}

//////////////////////////////////////////////////////////////////////////

bool CGELomoWithCurveLinearFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshLomoWithCurveLinear))
    {
        setVignette(0.2f, 0.8f);
        resetCurve(m_curve, CGE_CURVE_PRECISION);
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////

bool CGELomoWithCurveTexFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshLomoWithCurveTex))
    {
        setVignette(0.2f, 0.8f);
        resetCurve(m_curve, CGE_CURVE_PRECISION);
        m_curveTexture = 0;
        initSampler();
        return true;
    }
    return false;
}

void CGELomoWithCurveTexFilter::initSampler()
{
    flush();
    UniformParameters* param;
    if (m_uniformParam == nullptr)
        param = new UniformParameters;
    else
        param = m_uniformParam;
    param->pushSampler2D(g_paramCurveMapTextureName, &m_curveTexture, 0);
    setAdditionalUniformParameter(param);
}

void CGELomoWithCurveTexFilter::flush()
{
    std::vector<float>::size_type sz = m_curve.size();
    if (sz == 0)
    {
        scaleCurve(m_curve, CGE_CURVE_PRECISION);
        sz = CGE_CURVE_PRECISION;
    }

#ifdef CGE_NOT_OPENGL_ES

    const GLenum dataType = GL_FLOAT;
    const void* samplerData = &m_curve[0][0];

#else

    const GLenum dataType = GL_UNSIGNED_BYTE;

    const CurveData* curveData = m_curve.data();
    unsigned char samplerData[CGE_CURVE_PRECISION * 3];
    for (int i = 0; i != CGE_CURVE_PRECISION; ++i)
    {
        const int index = i * 3;
        samplerData[index] = (unsigned char)(curveData[i][0] * 255.0f);
        samplerData[index + 1] = (unsigned char)(curveData[i][1] * 255.0f);
        samplerData[index + 2] = (unsigned char)(curveData[i][2] * 255.0f);
    }

#endif

    if (m_curveTexture == 0)
    {
        m_curveTexture = cgeGenTextureWithBuffer(samplerData, int(sz), 1, GL_RGB, dataType, 3, 0, GL_NEAREST, GL_CLAMP_TO_EDGE);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, m_curveTexture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (int)sz, 1, GL_RGB, dataType, samplerData);
    }
}

bool CGELomoWithCurveTexLinearFilter::init()
{
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshLomoWithCurveTexLinear))
    {
        setVignette(0.2f, 0.8f);
        resetCurve(m_curve, CGE_CURVE_PRECISION);
        initSampler();
        return true;
    }
    return false;
}

} // namespace CGE

//////////////////////////////////////////////////////////////////////////

static CGEConstString s_fshColorScale = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform vec2 colorScale; // x for scale clip, y for scale range.
    uniform float saturation;

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        float lum = dot(src.rgb, vec3(0.299, 0.587, 0.114));
        src.rgb = lum * (1.0 - saturation) + saturation * src.rgb;
        src.rgb = (src.rgb - colorScale.x) / colorScale.y;
        gl_FragColor = src;
    });

namespace CGE
{
CGEConstString CGEColorScaleFilter::paramColorScaleName = "colorScale";
CGEConstString CGEColorScaleFilter::paramSaturationName = "saturation";

bool CGEColorScaleFilter::init()
{
    return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshColorScale);
}

void CGEColorScaleFilter::setColorScale(float low, float range)
{
    m_scaleDark = low;
    m_scaleLight = range;
}

void CGEColorScaleFilter::setSaturation(float value)
{
    m_saturate = value;
}

void CGEColorScaleFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    if (m_uniformParam != nullptr)
        m_uniformParam->assignUniforms(handler, m_program.programID());
    // More initialization...
    {
        const CGESizei& sz = handler->getOutputFBOSize();
        GLfloat sat = m_saturate, colorScale[2] = { 0.0f, 1.0f };
        if (m_scaleDark >= 0.0f && m_scaleLight > 0.0f)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTexture, 0);
            CGE_LOG_CODE(clock_t t = clock());
            getScale(colorScale, colorScale + 1, &sat, m_scaleDark, m_scaleLight, sz.width, sz.height);
            CGE_LOG_INFO("getscale用时%gs\n", (double)(clock() - t) / CLOCKS_PER_SEC);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handler->getTargetTextureID(), 0);
        }
        if (sat >= 0.0f) m_program.sendUniformf(paramSaturationName, sat);
        m_program.sendUniformf(paramColorScaleName, colorScale[0], colorScale[1]);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    cgeCheckGLError("glDrawArrays");
}

} // namespace CGE

//////////////////////////////////////////////////////////////////////////

// clang-format off

static CGEConstString s_fshColorMul = CGE_SHADER_STRING_PRECISION_M(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform %s cmul;

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        src.rgb *= cmul;
        %s;
        gl_FragColor = vec4(src.rgb, src.a);
    });

// clang-format on

namespace CGE
{
CGEConstString CGEColorMulFilter::paramMulName = "cmul";

bool CGEColorMulFilter::initWithMode(MulMode mode)
{
    const char* pFLT = "float";
    const char* pVEC = "vec3";
    const char* pMAT = "mat3";
    const char *p1 = "", *p2 = "";
    char buffer[512];
    switch (mode)
    {
    case mulFLT:
        p1 = pFLT;
        p2 = "src.rgb += 1.0 - step(0.0, src.rgb)";
        break;
    case mulVEC:
        p1 = pVEC;
        p2 = "src.rgb += 1.0 - step(0.0, src.rgb)";
        break;
    case mulMAT:
        p1 = pMAT;
        break;
    default:
        CGE_LOG_ERROR("CGEColorMulFilter - Invalid mode name: %d\n", mode);
        return false;
    }
    sprintf(buffer, s_fshColorMul, p1, p2);
    return initShadersFromString(g_vshDefaultWithoutTexCoord, buffer);
}

void CGEColorMulFilter::setFLT(float value)
{
    m_program.bind();
    m_program.sendUniformf(paramMulName, value);
}

void CGEColorMulFilter::setVEC(float r, float g, float b)
{
    m_program.bind();
    m_program.sendUniformf(paramMulName, r, g, b);
}

void CGEColorMulFilter::setMAT(float* mat)
{
    m_program.bind();
    GLint index = glGetUniformLocation(m_program.programID(), paramMulName);
    if (index < 0)
    {
        CGE_LOG_ERROR("CGEColorMulFilter::setMAT - uniform name %s doesnot exist!\n", paramMulName);
        return;
    }
    glUniformMatrix3fv(index, 1, GL_FALSE, mat);
}

} // namespace CGE
