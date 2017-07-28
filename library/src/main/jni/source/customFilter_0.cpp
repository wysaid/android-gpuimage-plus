//
//  CustomFilter_0.h
//  filterLib
//
//  Created by wangyang on 16/8/2.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#include "customFilter_0.h"

using namespace CGE;

static CGEConstString s_fsh = CGE_SHADER_STRING_PRECISION_H
(
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 vSteps;
uniform float intensity;


void main(void)
{
    vec4 originalColor = texture2D(inputImageTexture, textureCoordinate);
    vec4 invertedColor = vec4(((intensity * 0.2 + 0.5) - originalColor.rgb), originalColor.w);
    gl_FragColor = invertedColor;
});

bool CustomFilter_0::init()
{
    if(m_program.initWithShaderStrings(g_vshDefaultWithoutTexCoord, s_fsh))
    {
        m_program.bind();
        mStepLoc = m_program.uniformLocation("vSteps");
        return true;
    }
    return false;
}

void CustomFilter_0::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();
    
    const CGESizei& sz = handler->getOutputFBOSize();
    
    glUniform2f(mStepLoc, 1.0f / sz.width, 1.0f / sz.height);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

CGEConstString CustomFilter_0::paramIntensity = "intensity";

void CustomFilter_0::setIntensity(float value)
{
    CGE_LOG_ERROR("setIntensity %g", value);
    m_program.bind();
    m_program.sendUniformf(paramIntensity, value);
}


