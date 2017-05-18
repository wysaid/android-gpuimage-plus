/* cgeBeautifyFilter.cpp
 *
 *  Created on: 2016-3-22
 *      Author: Wang Yang
 */


#include "cgeBeautifyFilter.h"
#include <cmath>

CGEConstString s_fshBeautify = CGE_SHADER_STRING_PRECISION_H
(
 uniform sampler2D inputImageTexture;
 varying vec2 textureCoordinate;
 
 uniform vec2 imageStep;
 uniform float intensity;
 
 void main()
{
    
    vec2 blurCoordinates[20];
    
    blurCoordinates[0] = textureCoordinate + vec2(0.0, -10.0) * imageStep;
    blurCoordinates[1] = textureCoordinate + vec2(5.0, -8.0) * imageStep;
    blurCoordinates[2] = textureCoordinate + vec2(8.0, -5.0) * imageStep;
    blurCoordinates[3] = textureCoordinate + vec2(10.0, 0.0) * imageStep;
    blurCoordinates[4] = textureCoordinate + vec2(8.0, 5.0) * imageStep;
    blurCoordinates[5] = textureCoordinate + vec2(5.0, 8.0) * imageStep;
    blurCoordinates[6] = textureCoordinate + vec2(0.0, 10.0) * imageStep;
    blurCoordinates[7] = textureCoordinate + vec2(-5.0, 8.0) * imageStep;
    blurCoordinates[8] = textureCoordinate + vec2(-8.0, 5.0) * imageStep;
    blurCoordinates[9] = textureCoordinate + vec2(-10.0, 0.0) * imageStep;
    blurCoordinates[10] = textureCoordinate + vec2(-8.0, -5.0) * imageStep;
    blurCoordinates[11] = textureCoordinate + vec2(-5.0, -8.0) * imageStep;
    blurCoordinates[12] = textureCoordinate + vec2(0.0, -6.0) * imageStep;
    blurCoordinates[13] = textureCoordinate + vec2(-4.0, -4.0) * imageStep;
    blurCoordinates[14] = textureCoordinate + vec2(-6.0, 0.0) * imageStep;
    blurCoordinates[15] = textureCoordinate + vec2(-4.0, 4.0) * imageStep;
    blurCoordinates[16] = textureCoordinate + vec2(0.0, 6.0) * imageStep;
    blurCoordinates[17] = textureCoordinate + vec2(4.0, 4.0) * imageStep;
    blurCoordinates[18] = textureCoordinate + vec2(6.0, 0.0) * imageStep;
    blurCoordinates[19] = textureCoordinate + vec2(4.0, -4.0) * imageStep;
    
    vec3 centralColor = texture2D(inputImageTexture, textureCoordinate).rgb;
    
    float sampleColor = centralColor.g * 24.0;
    
    sampleColor += texture2D(inputImageTexture, blurCoordinates[0]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[1]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[2]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[3]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[4]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[5]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[6]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[7]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[8]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[9]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[10]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[11]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[12]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[13]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[14]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[15]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[16]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[17]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[18]).g;
    sampleColor += texture2D(inputImageTexture, blurCoordinates[19]).g;
    
    sampleColor = sampleColor/44.0;
    
    float dis = centralColor.g - sampleColor + 0.5;
    
    if(dis <= 0.5)
    {
        dis = dis * dis * 2.0;
    }
    else
    {
        dis = 1.0 - ((1.0 - dis)*(1.0 - dis) * 2.0);
    }
    
    if(dis <= 0.5)
    {
        dis = dis * dis * 2.0;
    }
    else
    {
        dis = 1.0 - ((1.0 - dis)*(1.0 - dis) * 2.0);
    }
    
    if(dis <= 0.5)
    {
        dis = dis * dis * 2.0;
    }
    else
    {
        dis = 1.0 - ((1.0 - dis)*(1.0 - dis) * 2.0);
    }
    
    if(dis <= 0.5)
    {
        dis = dis * dis * 2.0;
    }
    else
    {
        dis = 1.0 - ((1.0 - dis)*(1.0 - dis) * 2.0);
    }
    
    if(dis <= 0.5)
    {
        dis = dis * dis * 2.0;
    }
    else
    {
        dis = 1.0 - ((1.0 - dis)*(1.0 - dis) * 2.0);
    }
    
    vec3 result = centralColor * 1.065 - dis * 0.065;
    
    float hue = dot(result, vec3(0.299,0.587,0.114)) - 0.3;
    
    hue = pow(clamp(hue, 0.0, 1.0), 0.3);
    
    result = centralColor * (1.0 - hue) + result * hue;
    result = (result - 0.8) * 1.06 + 0.8;
    
    result = pow(result, vec3(0.75));
    
    result = mix(centralColor, result, intensity);
    
    gl_FragColor = vec4(result, 1.0);
}
 );

namespace CGE
{
	bool CGEBeautifyFilter::init()
    {
        if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshBeautify))
        {
            setImageSize(720.0f, 1280.0f);
            setIntensity(1.0f);
            return true;
        }
        return false;
    }
    
    void CGEBeautifyFilter::setIntensity(float intensity)
    {
        m_program.bind();
        m_program.sendUniformf("intensity", intensity);
        m_intensity = fabs(intensity) < 0.05f ? 0.0f : intensity;
    }
    
    void CGEBeautifyFilter::setImageSize(float width, float height, float mul)
    {
        m_program.bind();
        m_program.sendUniformf("imageStep", mul / width, mul / height);
    }
    
    void CGEBeautifyFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
    {
        if(m_intensity != 0.0f)
        {
            CGEImageFilterInterface::render2Texture(handler, srcTexture, vertexBufferID);
        }
    }
}
