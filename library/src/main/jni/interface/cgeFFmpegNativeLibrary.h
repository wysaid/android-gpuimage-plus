/*
* cgeFFmpegNativeLibrary.h
*
*  Created on: 2015-11-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#if !defined(_CGEFFmpegNativeLibrary_H_) && defined(_CGE_USE_FFMPEG_)
#define _CGEFFmpegNativeLibrary_H_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/////////////////   ffmpeg   /////////////////////////

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFFmpegNativeLibrary_avRegisterAll
  (JNIEnv *, jclass); //注册 ffmpeg


//JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFFmpegNativeLibrary_nativeGenerateVideoWithFilter
//  (JNIEnv *, jclass, jstring, jstring, jstring, jfloat, jobject, jint, jfloat, jboolean);

#ifdef __cplusplus
}
#endif

#endif