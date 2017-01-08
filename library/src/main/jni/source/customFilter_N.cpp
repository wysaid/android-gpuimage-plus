//
//  CustomFilter_N.h
//  filterLib
//
//  Created by wangyang on 16/8/2.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#include "CustomFilter_N.h"

using namespace CGE;

static CGEConstString s_fsh1 = CGE_SHADER_STRING_PRECISION_M
(
 varying vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;
 const vec3 vr = vec3(1.0, 0.835, 0.835);
 const vec3 vg = vec3(0.0, 0.588, 1.0);
 
 void main()
{
    vec4 src = texture2D(inputImageTexture, textureCoordinate);
    src.rgb = 1.0 - (1.0 - vr * src.r) * (1.0 - vg * src.g);
    gl_FragColor = src;
}
 );

bool CustomFilter_1::init()
{
    return m_program.initWithShaderStrings(g_vshDefaultWithoutTexCoord, s_fsh1);
}

/////////////////////////////////////////////

static CGEConstString s_fsh2 = CGE_SHADER_STRING_PRECISION_H
(
 varying vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;
 
 uniform vec2 vSteps;
 const float intensity = 0.45;
 
 float getLum(vec3 src)
{
    return dot(src, vec3(0.299, 0.587, 0.114));
}
 
 void main()
{
    mat3 m;
    vec4 src = texture2D(inputImageTexture, textureCoordinate);
    
    m[0][0] = getLum(texture2D(inputImageTexture, textureCoordinate - vSteps).rgb);
    m[0][1] = getLum(texture2D(inputImageTexture, textureCoordinate - vec2(0.0, vSteps.y)).rgb);
    m[0][2] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(vSteps.x, -vSteps.y)).rgb);
    m[1][0] = getLum(texture2D(inputImageTexture, textureCoordinate - vec2(vSteps.x, 0.0)).rgb);
    m[1][1] = getLum(src.rgb);
    m[1][2] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(vSteps.x, 0.0)).rgb);
    m[2][0] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(-vSteps.x, vSteps.y)).rgb);
    m[2][1] = getLum(texture2D(inputImageTexture, textureCoordinate + vec2(0.0, vSteps.y)).rgb);
    m[2][2] = getLum(texture2D(inputImageTexture, textureCoordinate + vSteps).rgb);
    
    float nx = m[0][0] + m[0][1] + m[0][2] - m[2][0] - m[2][1] - m[2][2];
    float ny = m[0][0] + m[1][0] + m[2][0] - m[0][2] - m[1][2] - m[2][2];
    float ndl = abs(nx + ny + intensity);
    float shade = 0.0;
    
    float norm = (nx * nx + ny * ny + intensity * intensity);
    shade = (ndl * 0.577) / sqrt(norm);
    
    gl_FragColor = vec4(src.rgb * shade, src.a);
}
 );

bool CustomFilter_2::init()
{
    if(m_program.initWithShaderStrings(g_vshDefaultWithoutTexCoord, s_fsh2))
    {
        m_program.bind();
        mStepLoc = m_program.uniformLocation("vSteps");
        return true;
    }
    return false;
}

/////////////////////////////////////////////

static CGEConstString s_fsh3 = CGE_SHADER_STRING_PRECISION_H
(
 varying vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;
 const vec2 vignette = vec2(0.1, 0.8);
 
 const vec3 c1 = vec3(0.992,0.137,0.314);
 const vec3 c2 = vec3(0.204,0.98,0.725);
 const vec2 vignetteCenter = vec2(0.5, 0.5);
 
 void main(void)
{
    vec4 src = texture2D(inputImageTexture, textureCoordinate);
    
    float d = distance(textureCoordinate, vec2(vignetteCenter.x, vignetteCenter.y));
    float percent = clamp((d-vignette.x)/vignette.y, 0.0, 1.0);
    float alpha = 1.0-percent;
    
    src.rgb = src.rgb * alpha;
    
    src.r = 1.0 - (1.0 - src.r*c1.r) * (1.0 - src.g*c2.r);
    src.g = 1.0 - (1.0 - src.r*c1.g) * (1.0 - src.g*c2.g);
    src.b = 1.0 - (1.0 - src.r*c1.b) * (1.0 - src.g*c2.b);
    
    gl_FragColor = src;
}
 );

bool CustomFilter_3::init()
{
    return m_program.initWithShaderStrings(g_vshDefaultWithoutTexCoord, s_fsh3);
}

/////////////////////////////////////////////

static CGEConstString s_fsh4 = CGE_SHADER_STRING_PRECISION_H
(
 varying vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;
 uniform vec3 colorGradient[5];
 //const float ratio = 1.25;
 
 vec3 soft_light_l3s(vec3 a, vec3 b)
{
    vec3 src;
    a = a * 2.0 - 32768.0;
    
    float tmpr = a.r > 0.0 ? sqrt(b.r)  - b.r : b.r - b.r * b.r;
    src.r = a.r * tmpr / 128.0 + b.r * 256.0;
    
    float tmpg = a.g > 0.0 ? sqrt(b.g)  - b.g : b.g - b.g * b.g;
    src.g = a.g * tmpg / 128.0 + b.g * 256.0;
    
    float tmpb = a.b > 0.0 ? sqrt(b.b)  - b.b : b.b - b.b * b.b;
    src.b = a.b * tmpb / 128.0 + b.b * 256.0;
    return src;
}
 
 void main()
{
    vec4 src = texture2D(inputImageTexture, textureCoordinate);
    vec2 tmpCoord = textureCoordinate * 256.0;// * ratio;
    float ps = tmpCoord.x + tmpCoord.y;
    int pi = int(ps / 128.0);
    float pr = mod(ps, 128.0) / 128.0;
    vec3 v1 = colorGradient[pi];
    vec3 v2 = colorGradient[pi + 1];
    vec3 tmp1 = v1 * (1.0 - pr) + v2 * pr;
    vec3 tmp2 = src.rgb * mat3(0.509, 0.4109, 0.07978,
                               0.209, 0.7109, 0.07978,
                               0.209, 0.4109, 0.3798);
    src.rgb = soft_light_l3s(tmp1, tmp2) / 255.0;
    gl_FragColor = src;
}
 );

bool CustomFilter_4::init()
{
    if(m_program.initWithShaderStrings(g_vshDefaultWithoutTexCoord, s_fsh4))
    {
        const GLfloat colorGradientValue[] =
        {
            0.0f, 0.0f, 32768.0f,
            8000.0f, 7000.0f, 24576.0f,
            16000.0f, 14000.0f, 16384.0f,
            24000.0f, 21000.0f, 8192.0f,
            32000.0f, 28000.0f, 0.0f 
        };
        m_program.bind();
        GLint loc = m_program.uniformLocation("colorGradient");
        if(loc < 0)
            return false;
        glUniform3fv(loc, 5, colorGradientValue);
        return true;
    }
    return false;
}