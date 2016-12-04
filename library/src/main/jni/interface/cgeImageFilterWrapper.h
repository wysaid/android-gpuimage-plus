/*
* cgeImageFilterWrapper.h
*
*  Created on: 2016-3-19
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _CGEIMAGEFILTERWRAPPER_H_
#define _CGEIMAGEFILTERWRAPPER_H_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageFilter_createNativeFilterByConfig
  (JNIEnv *, jclass, jstring, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_releaseNativeFilter
  (JNIEnv *, jclass, jlong);

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageFilter_createEnlarge2EyesFilter
  (JNIEnv *, jclass);

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageFilter_createEnlarge2EyesAndMouthFilter
  (JNIEnv *, jclass);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_setEnlarge2EyesFilterArgs
  (JNIEnv *, jclass, jlong, jfloat, jfloat, jfloat, jfloat, jfloat, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_setEnlarge2EyesFilterIntensity
  (JNIEnv *, jclass, jlong, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_setEnlarge2EyesAndMouthFilterArgs
  (JNIEnv *, jclass, jlong, jfloat, jfloat, jfloat);

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageFilter_createAlienLookFilter
  (JNIEnv *, jclass);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_setAlienLookFilterArgs
  (JNIEnv *, jclass, jlong, jfloat, jfloat, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_setAlienLookFilterPosition
  (JNIEnv *, jclass, jlong, jfloat, jfloat, jfloat, jfloat, jfloat, jfloat);


#ifdef __cplusplus
}
#endif


#endif