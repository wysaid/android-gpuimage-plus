//
//  CustomFilter_0.h
//  filterLib
//
//  Created by wangyang on 16/8/2.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#include "customFilter_0.h"

using namespace CGE;

static CGEConstString s_fsh = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec2 vSteps;

    const vec2 vignetteCenter = vec2(0.5, 0.5);

    vec3 hardLight(vec3 src1, vec3 src2, float alpha) {
        if (src2.r < 0.5)
            src2.r = (src1.r * src2.r) * 2.0;
        else
            src2.r = (src1.r + src2.r) * 2.0 - (src1.r * src2.r) * 2.0 - 1.0;

        if (src2.g < 0.5)
            src2.g = (src1.g * src2.g) * 2.0;
        else
            src2.g = (src1.g + src2.g) * 2.0 - (src1.g * src2.g) * 2.0 - 1.0;

        if (src2.b < 0.5)
            src2.b = (src1.b * src2.b) * 2.0;
        else
            src2.b = (src1.b + src2.b) * 2.0 - (src1.b * src2.b) * 2.0 - 1.0;

        return mix(src1, src2, alpha);
    }

    void main(void) {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        float max_r = 0.87055 / min(vSteps.x, vSteps.y);
        float lens_fac = max_r * max_r;
        float lens_facinv = 1073741824.0 / lens_fac;
        float lens_amount = 102.0;
        float dis = length(textureCoordinate - vignetteCenter);
        float bd = (lens_fac + dis) * lens_facinv / 262144.0;
        bd = bd * bd / 1024.0;
        bd = 536870912.0 / bd;
        float alpha = 1.0 + lens_amount * (bd - 32768.0) / 256.0 / 32768.0;
        alpha = clamp(alpha, 0.0, 1.0);
        vec3 dst = src.rgb * alpha;

        float lum = dot(dst, vec3(0.299, 0.587, 0.114));
        vec3 tmp = vec3(lum, lum, 1.0 - lum);
        dst = hardLight(dst, tmp, 0.5);
        gl_FragColor = vec4(dst, src.a);
    });

bool CustomFilter_0::init()
{
    if (m_program.initWithShaderStrings(g_vshDefaultWithoutTexCoord, s_fsh))
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

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
