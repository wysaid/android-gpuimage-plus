/*
 * cgeGlobal.cpp
 *
 *  Created on: 2014-9-9
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#include "cgeGlobal.h"

#include "cgeCommonDefine.h"

#ifndef _CGE_ONLY_FILTERS_

#include "cgeAction.h"
#include "cgeSprite2d.h"

#endif

#include <cstring>

namespace CGE
{
bool g_isFastFilterImpossible = true;

#ifdef _CGE_USE_ES_API_3_0_
bool g_shouldUsePBO = true;
#endif

int CGEGlobalConfig::viewWidth = 1024;
int CGEGlobalConfig::viewHeight = 768;
GLuint CGEGlobalConfig::sVertexBufferCommon = 0;
float CGEGlobalConfig::sVertexDataCommon[8] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };

} // namespace CGE

extern "C"
{
void cgePrintGLInfo()
{
    CGE_LOG_KEEP("===== Here are some information of your device =====\n\n");

    cgePrintGLString("Vendor", GL_VENDOR);
    cgePrintGLString("Renderer", GL_RENDERER);
    cgePrintGLString("GL Version", GL_VERSION);
    cgePrintGLString("GL Extension", GL_EXTENSIONS);
    cgePrintGLString("Shading Language Version", GL_SHADING_LANGUAGE_VERSION);

    GLint iParam[2];

#ifdef GL_MAX_VERTEX_UNIFORM_VECTORS // GL ES
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, iParam);
#endif
#ifdef GL_MAX_VERTEX_UNIFORM_COMPONENTS // GL Desktop
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, iParam);
#endif
    CGE_LOG_KEEP("Max Vertex Uniform Vectors: %d\n", iParam[0]);

#ifdef GL_MAX_FRAGMENT_UNIFORM_VECTORS
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, iParam);
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, iParam);
#endif
    CGE_LOG_KEEP("Max Fragment Uniform Vectors : %d\n", iParam[0]);

#ifdef GL_MAX_VARYING_VECTORS
    glGetIntegerv(GL_MAX_VARYING_VECTORS, iParam);
#endif
#ifdef GL_MAX_VARYING_COMPONENTS
    glGetIntegerv(GL_MAX_VARYING_COMPONENTS, iParam);
#endif
    CGE_LOG_KEEP("Max Varying Vectors: %d\n", iParam[0]);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, iParam);
    CGE_LOG_KEEP("Max Texture Image Units : %d\n", iParam[0]);

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, iParam);
    CGE_LOG_KEEP("Max Combined Texture Image Units : %d\n", iParam[0]);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, iParam);
    CGE_LOG_KEEP("Max Texture Size : %d\n", iParam[0]);

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, iParam);
    CGE_LOG_KEEP("Max Vertex Attribs: %d\n", iParam[0]);

    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, iParam);
    CGE_LOG_KEEP("Max Viewport Dims : [%d, %d]\n", iParam[0], iParam[1]);

    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, iParam);
    CGE_LOG_KEEP("Max Render Buffer Size: %d\n", iParam[0]);

    glGetIntegerv(GL_STENCIL_BITS, iParam);
    CGE_LOG_KEEP("Stencil Buffer Bits : %d\n", iParam[0]);

    glGetIntegerv(GL_ALIASED_POINT_SIZE_RANGE, iParam);
    CGE_LOG_KEEP("Point Size Range: [%d, %d]\n", iParam[0], iParam[1]);

    glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, iParam);
    CGE_LOG_KEEP("Line Width Range: [%d, %d]\n", iParam[0], iParam[1]);

    glGetIntegerv(GL_DEPTH_BITS, iParam);
    CGE_LOG_KEEP("Depth Bits: %d\n", iParam[0]);

    cgeCheckGLError("cgePrintGLInfo");
    CGE_LOG_KEEP("\n===== Information end =====\n\n");
}

const char* cgeQueryGLExtensions()
{
    return (const char*)glGetString(GL_EXTENSIONS);
}

bool cgeCheckGLExtension(const char* ext)
{
    const char* extString = cgeQueryGLExtensions();
    return strstr(extString, ext) != nullptr;
}

GLuint cgeGenCommonQuadArrayBuffer()
{
    GLuint bufferID = 0;
    glGenBuffers(1, &bufferID);
    if (bufferID == 0)
        return 0;

    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CGE::CGEGlobalConfig::sVertexDataCommon), CGE::CGEGlobalConfig::sVertexDataCommon, GL_STATIC_DRAW);

    return bufferID;
}
}
