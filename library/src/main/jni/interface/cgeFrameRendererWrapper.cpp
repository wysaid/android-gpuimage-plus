/*
* cgeFrameRendererWrapper.h
*
*  Created on: 2015-11-26
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeFrameRendererWrapper.h"
#include "cgeUtilFunctions.h"
#include "cgeFrameRenderer.h"

using namespace CGE;


extern "C" {

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeCreate
 * Signature: ()Ljava/nio/ByteBuffer;
 */
JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeCreateRenderer
  (JNIEnv *env, jobject)
{
	cgePrintGLInfo();
	CGEFrameRenderer* renderer = new CGEFrameRenderer();
	return (jlong)renderer;
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeInit
 * Signature: (Ljava/nio/ByteBuffer;IIII)Z
 */
JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeInit
  (JNIEnv *env, jobject, jlong addr, jint srcW, jint srcH, jint dstW, jint dstH)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	CGE_LOG_INFO("native instance address: %p", renderer);
	return renderer->init(srcW, srcH, dstW, dstH);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeUpdate
 * Signature: (Ljava/nio/ByteBuffer;I)V
 */
 JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeUpdate
  (JNIEnv *env, jobject, jlong addr, jint extTex, jfloatArray matrix)
{
	jfloat* mat = env->GetFloatArrayElements(matrix, nullptr);
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->update(extTex, mat);
	env->ReleaseFloatArrayElements(matrix, mat, 0);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeRender
 * Signature: (Ljava/nio/ByteBuffer;IIII)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeRender
  (JNIEnv *env, jobject, jlong addr, jint x, jint y, jint width, jint height)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->render(x, y, width, height);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetSrcRotation
 * Signature: (Ljava/nio/ByteBuffer;F)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetSrcRotation
  (JNIEnv *env, jobject, jlong addr, jfloat rad)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->setSrcRotation(rad);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetSrcFlipScale
 * Signature: (Ljava/nio/ByteBuffer;FF)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetSrcFlipScale
  (JNIEnv *env, jobject, jlong addr, jfloat x, jfloat y)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->setSrcFlipScale(x, y);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetRenderRotation
 * Signature: (Ljava/nio/ByteBuffer;F)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetRenderRotation
  (JNIEnv *env, jobject, jlong addr, jfloat rad)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->setRenderRotation(rad);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetRenderFlipScale
 * Signature: (Ljava/nio/ByteBuffer;FF)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetRenderFlipScale
  (JNIEnv *env, jobject, jlong addr, jfloat x, jfloat y)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->setRenderFlipScale(x, y);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetFilterWidthConfig
 * Signature: (Ljava/nio/ByteBuffer;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetFilterWidthConfig
  (JNIEnv *env, jobject obj, jlong addr, jstring config)
{
	static CGETexLoadArg texLoadArg;
	texLoadArg.env = env;
	texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	const char* configStr = env->GetStringUTFChars(config, 0);
	renderer->setFilterWithConfig(configStr, cgeGlobalTextureLoadFunc, &texLoadArg);
	env->ReleaseStringUTFChars(config, configStr);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetFilterIntensity
 * Signature: (Ljava/nio/ByteBuffer;F)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetFilterIntensity
  (JNIEnv *env, jobject, jlong addr, jfloat intensity)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->setFilterIntensity(intensity);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSrcResize
  (JNIEnv *env, jobject, jlong addr, jint width, jint height)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->srcResize(width, height);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetMaskTexture
  (JNIEnv *env, jobject, jlong addr, jint texID, jfloat aspectRatio)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->setMaskTexture(texID, aspectRatio);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetMaskTextureRatio
  (JNIEnv *env, jobject, jlong addr, jfloat aspectRatio)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->setMaskTextureRatio(aspectRatio);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeRelease
  (JNIEnv *env, jobject, jlong addr)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	delete renderer;
}

JNIEXPORT jint JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeQueryBufferTexture
  (JNIEnv *env, jobject, jlong addr)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	return (jint)renderer->getBufferTexture();
}

JNIEXPORT jint JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeQueryTargetTexture
  (JNIEnv *env, jobject, jlong addr)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	return (jint)renderer->getTargetTexture();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetMaskRotation
  (JNIEnv *env, jobject, jlong addr, jfloat rad)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->setMaskRotation(rad);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSetMaskFlipScale
  (JNIEnv *env, jobject, jlong addr, jfloat x, jfloat y)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->setMaskFlipScale(x, y);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeRunProc
   (JNIEnv *env, jobject, jlong addr)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->runProc();
}

/////////////////  Render Utils  /////////////////////////////

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeDrawCache
  (JNIEnv *env, jobject, jlong addr)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->drawCache();
}

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeGetImageHandler
  (JNIEnv *env, jobject, jlong addr)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	return (jlong)renderer->getImageHandler();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeBindImageFBO
  (JNIEnv *env, jobject, jlong addr)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->bindImageFBO();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeSwapBufferFBO
  (JNIEnv *, jobject, jlong addr)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;
	renderer->swapBufferFBO();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeProcessWithFilter
  (JNIEnv *, jobject, jlong addr, jlong filter)
{
	CGEFrameRenderer* renderer = (CGEFrameRenderer*)addr;

	//较危险操作， 慎用.
	renderer->getImageHandler()->processingWithFilter((CGEImageFilterInterfaceAbstract*)filter);
}


}







