/*
* cgeCustomFilterWrapper.h
*
*  Created on: 2017-1-8
*      Author: Wang Yang
*        Mail: admin@wysaid.org
* Description: This demo teaches you how to create your own filters.
*/

#ifndef _CGECUSTOMIZEDFILTER_H_
#define _CGECUSTOMIZEDFILTER_H_

#include <jni.h>

extern "C"
{
	JNIEXPORT jobject JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeFilterImageWithCustomFilter
  (JNIEnv *env, jclass cls, jobject bmp, jint index, jfloat intensity, jboolean hasContext);

  	JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeCreateCustomNativeFilter
  (JNIEnv *env, jclass cls, jint index, jfloat intensity);

  	JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeGetCustomFilterNum
  (JNIEnv *env, jclass cls);

}

#endif