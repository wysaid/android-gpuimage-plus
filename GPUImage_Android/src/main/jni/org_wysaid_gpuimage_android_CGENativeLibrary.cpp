
#include "org_wysaid_gpuimage_android_CGENativeLibrary.h"

#include "cgeCommonDefine.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     org_wysaid_gpuimage_android_CGENativeLibrary
 * Method:    _globalInit
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_org_wysaid_gpuimage_1android_CGENativeLibrary__1globalInit
  (JNIEnv *, jclass)
{
    CGE_LOG_INFO("jni test");
    return true;
}

/*
 * Class:     org_wysaid_gpuimage_android_CGENativeLibrary
 * Method:    _globalRelease
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_wysaid_gpuimage_1android_CGENativeLibrary__1globalRelease
  (JNIEnv *, jclass)
{

}


#ifdef __cplusplus
}
#endif