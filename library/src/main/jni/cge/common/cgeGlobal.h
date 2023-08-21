/*
 * cgeGlobal.h
 *
 *  Created on: 2014-9-9
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#ifndef _CGEGLOBAL_H_
#define _CGEGLOBAL_H_

#ifdef GLEW_USED
#include "cgePlatform_GLEW.h"
#elif defined(ANDROID_NDK)
#include "cgePlatform_ANDROID.h"
#elif defined(IOS_SDK)
#include "cgePlatform_iOS.h"
#elif defined(LIBCGE4QT_LIB)
#include "cgePlatform_QT.h"
#endif

#ifndef CGEAssert
#ifdef DEBUG
#include <assert.h>
#define CGEAssert(...) assert(__VA_ARGS__)
#else
#define CGEAssert(...)
#endif
#endif

#ifdef __cplusplus

namespace CGE
{
//辅助类，全局可用。
class CGEGlobalConfig
{
public:
    static int viewWidth, viewHeight;

    static GLuint sVertexBufferCommon;
    static float sVertexDataCommon[8];

    enum InitArguments
    {
        CGE_INIT_LEAST = 0,
        CGE_INIT_COMMONVERTEXBUFFER = 0x1,
        CGE_INIT_SPRITEBUILTIN = 0x3,
        CGE_INIT_DEFAULT = 0xffffffff,
    };

    static InitArguments sInitArugment;
};

} // namespace CGE

#endif

#ifdef __cplusplus
extern "C"
{
#endif

void cgePrintGLInfo();
const char* cgeQueryGLExtensions();
bool cgeCheckGLExtension(const char* ext);

GLuint cgeGenCommonQuadArrayBuffer();

#ifdef __cplusplus
}
#endif

#endif
