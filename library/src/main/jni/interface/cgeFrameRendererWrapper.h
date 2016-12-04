/*
* cgeFrameRendererWrapper.h
*
*  Created on: 2015-11-26
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include <jni.h>
/* Header for class org_wysaid_nativePort_CGEFrameRenderer */

#ifndef _Included_org_wysaid_nativePort_CGEFrameRenderer
#define _Included_org_wysaid_nativePort_CGEFrameRenderer
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeCreate
 * Signature: ()Ljava/nio/ByteBuffer;
 */
JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeCreateRenderer
  (JNIEnv *, jobject);

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeInit
 * Signature: (Ljava/nio/ByteBuffer;IIII)Z
 */
JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeInit
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint);

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeUpdate
 * Signature: (Ljava/nio/ByteBuffer;I)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeUpdate
  (JNIEnv *, jobject, jlong, jint, jfloatArray);

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeRender
 * Signature: (Ljava/nio/ByteBuffer;IIII)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeRender
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint);

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetSrcRotation
 * Signature: (Ljava/nio/ByteBuffer;F)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetSrcRotation
  (JNIEnv *, jobject, jlong, jfloat);

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetSrcFlipScale
 * Signature: (Ljava/nio/ByteBuffer;FF)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetSrcFlipScale
  (JNIEnv *, jobject, jlong, jfloat, jfloat);

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetRenderRotation
 * Signature: (Ljava/nio/ByteBuffer;F)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetRenderRotation
  (JNIEnv *, jobject, jlong, jfloat);

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetRenderFlipScale
 * Signature: (Ljava/nio/ByteBuffer;FF)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetRenderFlipScale
  (JNIEnv *, jobject, jlong, jfloat, jfloat);

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetFilterWidthConfig
 * Signature: (Ljava/nio/ByteBuffer;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetFilterWidthConfig
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetFilterIntensity
 * Signature: (Ljava/nio/ByteBuffer;F)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetFilterIntensity
  (JNIEnv *, jobject, jlong, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSrcResize
  (JNIEnv *, jobject, jlong, jint, jint);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetMaskTexture
  (JNIEnv *, jobject, jlong, jint, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetMaskTextureRatio
  (JNIEnv *, jobject, jlong, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeRelease
  (JNIEnv *, jobject, jlong);

JNIEXPORT jint JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeQueryBufferTexture
  (JNIEnv *, jobject, jlong);

JNIEXPORT jint JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeQueryTargetTexture
  (JNIEnv *, jobject, jlong);


JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetMaskRotation
  (JNIEnv *, jobject, jlong, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetMaskFlipScale
  (JNIEnv *, jobject, jlong, jfloat, jfloat);

 JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeRunProc
   (JNIEnv *, jobject, jlong);

/////////////////  Render Utils  /////////////////////////////

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeDrawCache
  (JNIEnv *, jobject, jlong);

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeGetImageHandler
  (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeBindImageFBO
  (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSwapBufferFBO
  (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeProcessWithFilter
  (JNIEnv *, jobject, jlong, jlong);


#ifdef __cplusplus
}
#endif
#endif
