//
//  cgeMultiInputFilter.cpp
//
//  Created by wysaid on 19/4/16.
//  Copyright © 2019年 wysaid. All rights reserved.
//

#include "cgeMultiInputFilter.h"

namespace CGE
{
CGEMultiInputFilter::~CGEMultiInputFilter()
{
    CGE_LOG_INFO("CGEMultiInputFilter::~CGEMultiInputFilter : %p", this);
}

void CGEMultiInputFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    if (m_inputTextures.empty())
    {
        CGE_LOG_ERROR("CGEMultiInputFilter::render2Texture - input textures are empty!");
    }
    else
    {
        for (int i = 0; i != m_inputTextures.size(); ++i)
        {
            assert(m_inputTextures.size() == m_inputTextureLocations.size());

            glActiveTexture(GL_TEXTURE1 + i);
            glBindTexture(GL_TEXTURE_2D, m_inputTextures[i]);
            glUniform1i(m_inputTextureLocations[i], i + 1);
        }
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CGEMultiInputFilter::updateInputTextures(GLuint* textures, int count)
{
    m_inputTextures.resize(count);
    std::copy(textures, textures + count, m_inputTextures.begin());

    if (m_inputTextureLocations.size() != m_inputTextures.size())
    {
        m_inputTextureLocations.resize(m_inputTextures.size());
        char buffer[1024];
        for (int i = 0; i != m_inputTextureLocations.size(); ++i)
        {
            sprintf(buffer, "inputTexture%d", i);
            m_inputTextureLocations[i] = m_program.uniformLocation(buffer);
            if (m_inputTextureLocations[i] < 0)
            {
                CGE_LOG_ERROR("Invalid uniform name %s!!", buffer);
            }
        }
    }
}

//    void CGEMultiInputFilter::setInputCount(int count)
//    {
//
//    }
} // namespace CGE

extern "C"
{
JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEMultiInputFilterWrapper_nativeCreate(JNIEnv* env, jclass cls, jstring vsh, jstring fsh)
{
    const char* strVSH = env->GetStringUTFChars(vsh, 0);
    const char* strFSH = env->GetStringUTFChars(fsh, 0);
    auto* f = CGE::CGEMultiInputFilter::create(strVSH, strFSH);
    env->ReleaseStringUTFChars(vsh, strVSH);
    env->ReleaseStringUTFChars(fsh, strFSH);
    return (jlong)f;
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEMultiInputFilterWrapper_nativeRelease(JNIEnv* env, jclass cls, jlong addr)
{
    delete (CGE::CGEMultiInputFilter*)addr;
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEMultiInputFilterWrapper_nativeUpdateInputTextures(JNIEnv* env, jobject cls, jlong addr, jobject textures, int count)
{
    auto* f = (CGE::CGEMultiInputFilter*)addr;
    jint* p = (jint*)env->GetDirectBufferAddress(textures);
    static_assert(sizeof(jint) == sizeof(GLuint), "Size must match");
    f->updateInputTextures((GLuint*)p, count);
}
}