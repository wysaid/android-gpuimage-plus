/*
 * cgeImageHandlerWrapper.h
 *
 *  Created on: 2015-12-25
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#ifndef _CGE_IMAGEHANDLER_WRAPPER_H_
#define _CGE_IMAGEHANDLER_WRAPPER_H_

#include "jni.h"


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeCreateHandler
  (JNIEnv *, jobject);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeInitWithBitmap
  (JNIEnv *, jobject, jlong, jobject);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeInitWithSize
  (JNIEnv *, jobject, jlong, jint, jint);

JNIEXPORT jobject JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeGetResultBitmap
  (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetDrawerRotation
  (JNIEnv *, jobject, jlong, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetDrawerFlipScale
  (JNIEnv *, jobject, jlong, jfloat, jfloat);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterWithConfig
  (JNIEnv *, jobject, jlong, jstring, jboolean, jboolean);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterWithAddress
  (JNIEnv *, jobject, jlong, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterIntensity
  (JNIEnv *, jobject, jlong, jfloat, jboolean);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterIntensityAtIndex
  (JNIEnv *, jobject, jlong, jfloat, jint, jboolean);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeDrawResult
  (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeBindTargetFBO
  (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetAsTarget
  (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSwapBufferFBO
  (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeRevertImage
  (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeProcessingFilters
  (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeProcessWithFilter
  (JNIEnv *, jobject, jlong, jlong);

////////////////////////////////////

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeRelease
  (JNIEnv *, jobject, jlong);

///////////static methods ///////

#ifdef __cplusplus
}
#endif

#endif