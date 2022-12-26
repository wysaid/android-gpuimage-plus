/*
* cgeUtilFunctions.h
*
*  Created on: 2015-12-15
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#if !defined(_CGE_VIDEOUTILS_H_) && defined(_CGE_USE_FFMPEG_)
#define _CGE_VIDEOUTILS_H_

#include "cgeGLFunctions.h"
#include "cgeUtilFunctions.h"

#include <jni.h>

namespace CGE
{
	//This teaches you how to implement function above CGE.
	bool cgeGenerateVideoWithFilter(const char* outputFilename, const char* inputFilename, const char* filterConfig, float filterIntensity, GLuint texID, CGETextureBlendMode blendMode, float blendIntensity, bool mute, CGETexLoadArg* loadArg);
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFFmpegNativeLibrary_nativeGenerateVideoWithFilter
  (JNIEnv *, jclass, jstring, jstring, jstring, jfloat, jobject, jint, jfloat, jboolean);

#ifdef __cplusplus
}
#endif

#endif