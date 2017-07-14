//
// Created by Xue Weining on 2017/6/27.
//
#include "cgeFaceDetectFilter.h"
static CGEConstString s_fshFaceDetect = CGE_SHADER_STRING_PRECISION_H
(
        varying vec2 textureCoordinate;
        uniform sampler2D inputImageTexture;
        uniform float points[212];
        uniform bool isFaceDetect;

        void main() {
            gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
            if (isFaceDetect) {
                for (int i = 0; i < 106; i++) {
                    if (pow(abs(textureCoordinate.x - points[2*i]), 2.0f) +
                        pow(abs(textureCoordinate.y - points[2*i+1]), 2.0f) < 0.00001f) {
                        gl_FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
                    }
                }
            }
        }
);

namespace CGE
{
    bool CGEFaceDetectFilter::init()
    {
        if(initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshFaceDetect))
        {
            return true;
        }
        return false;
    }

    void CGEFaceDetectFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
    {
        handler->setAsTarget();
        m_program.bind();

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, srcTexture);
        if(m_uniformParam != nullptr)
            m_uniformParam->assignUniforms(handler, m_program.programID());

        float* keyPoints = handler->getFaceDetectKeyPoint();
        if (keyPoints != nullptr) {
            GLint isFaceDetectUniform = glGetUniformLocation(m_program.programID(), "isFaceDetect");
            glUniform1i(isFaceDetectUniform, GL_TRUE);
            GLint pointsUniform = glGetUniformLocation(m_program.programID(), "points");
            glUniform1fv(pointsUniform, 212, keyPoints);
        }
        else {
            GLint isFaceDetectUniform = glGetUniformLocation(m_program.programID(), "isFaceDetect");
            glUniform1i(isFaceDetectUniform, GL_FALSE);
        }

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        cgeCheckGLError("glDrawArrays");
    }
}
