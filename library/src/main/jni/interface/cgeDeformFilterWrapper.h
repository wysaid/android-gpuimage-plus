/*
* cgeDeformFilterWrapper.h
*
*  Created on: 2018-12-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef CGE_DEFORM_FILTER_WRAPPER_H_
#define CGE_DEFORM_FILTER_WRAPPER_H_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeCreate(JNIEnv *, jobject, jint, jint, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeRelease(JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeRestore(JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeRestoreWithIntensity(JNIEnv *, jobject, jlong, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeForwardDeform(JNIEnv *, jobject, jlong, jfloat, jfloat, jfloat, jfloat, jfloat, jfloat, jfloat, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeRestoreWithPoint(JNIEnv *, jobject, jlong, jfloat, jfloat, jfloat, jfloat, jfloat, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeBloatDeform(JNIEnv *, jobject, jlong, jfloat, jfloat, jfloat, jfloat, jfloat, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeWrinkleDeform(JNIEnv *, jobject, jlong, jfloat, jfloat, jfloat, jfloat, jfloat, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeSetUndoSteps(JNIEnv *, jobject, jlong, jint);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeCanUndo(JNIEnv *, jobject, jlong);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeCanRedo(JNIEnv *, jobject, jlong);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeUndo(JNIEnv *, jobject, jlong);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeRedo(JNIEnv *, jobject, jlong);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativePushDeformStep(JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeShowMesh(JNIEnv *, jobject, jlong, jboolean);



#ifdef __cplusplus
}
#endif

#endif