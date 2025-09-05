/*
 * cgeAdvancedEffectsCommon.cpp
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#include "cgeAdvancedEffectsCommon.h"

namespace CGE
{
CGEConstString CGEAdvancedEffectOneStepFilterHelper::paramStepsName = "samplerSteps";

void CGEAdvancedEffectOneStepFilterHelper::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    if (m_uniformParam != nullptr)
        m_uniformParam->assignUniforms(handler, m_program.programID());

    // Additional functions for new effects.
    {
        CGESizei sz = handler->getOutputFBOSize();
        m_program.sendUniformf(paramStepsName, 1.0f / sz.width, 1.0f / sz.height);
    }
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    cgeCheckGLError("glDrawArrays");
}

//////////////////////////////////////////////////////////////////////////

CGEConstString CGEAdvancedEffectTwoStepFilterHelper::paramStepsName = "samplerSteps";

void CGEAdvancedEffectTwoStepFilterHelper::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    CGESizei sz = handler->getOutputFBOSize();
    m_program.bind();

    glActiveTexture(GL_TEXTURE0);

    // Pass one
    handler->setAsTarget();
    {
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glBindTexture(GL_TEXTURE_2D, srcTexture);

        m_program.sendUniformf(paramStepsName, 0.0f, 1.0f / sz.height);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    // Pass Two
    handler->swapBufferFBO();
    handler->setAsTarget();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindTexture(GL_TEXTURE_2D, handler->getBufferTextureID());

    m_program.sendUniformf(paramStepsName, 1.0f / sz.width, 0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

} // namespace CGE