/*
* cgeFFmpegNativeLibrary.cpp
*
*  Created on: 2015-11-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifdef _CGE_USE_FFMPEG_

#include "cgeFFmpegHeaders.h"
#include "cgeFFmpegNativeLibrary.h"
#include "cgeCommonDefine.h"

#include "cgeVideoDecoder.h"
#include "cgeVideoPlayer.h"
#include "cgeSharedGLContext.h"
#include "cgeUtilFunctions.h"
#include "cgeVideoUtils.h"

using namespace CGE;

extern "C"
{
	////////// ffmpeg ////////////////////

	JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFFmpegNativeLibrary_avRegisterAll(JNIEnv *, jclass)
	{
		CGE_LOG_INFO("registerFFmpeg...");
	    av_register_all();
	}

	// JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFFmpegNativeLibrary_nativeGenerateVideoWithFilter(JNIEnv *env, jclass cls, jstring outputFilename, jstring inputFilename, jstring filterConfig, jfloat filterIntensity, jobject blendImage, jint blendMode, jfloat blendIntensity, jboolean mute)
	// {
	// 	CGE_LOG_INFO("##### nativeGenerateVideoWithFilter!!!");

	// 	if(outputFilename == nullptr || inputFilename == nullptr)
	// 		return false;

	// 	CGESharedGLContext* glContext = CGESharedGLContext::create(2048, 2048); //保证录制分辨率足够大 (2k)

	// 	if(glContext == nullptr)
	// 	{
	// 		CGE_LOG_ERROR("Create GL Context Failed!");
	// 		return false;
	// 	}

	// 	glContext->makecurrent();

	// 	CGETextureResult texResult = {0};

	// 	jclass nativeLibraryClass = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

	// 	if(blendImage != nullptr)
	// 		texResult = cgeLoadTexFromBitmap_JNI(env, nativeLibraryClass, blendImage);

	// 	const char* outFilenameStr = env->GetStringUTFChars(outputFilename, 0);
	// 	const char* inFilenameStr = env->GetStringUTFChars(inputFilename, 0);
	// 	const char* configStr = filterConfig == nullptr ? nullptr : env->GetStringUTFChars(filterConfig, 0);

	// 	bool retStatus = cgeGenerateVideoWithFilter(outFilenameStr, inFilenameStr, configStr, filterIntensity, texResult.texID, (CGETextureBlendMode)blendMode, blendIntensity, mute);

	// 	env->ReleaseStringUTFChars(outputFilename, outFilenameStr);
	// 	env->ReleaseStringUTFChars(inputFilename, inFilenameStr);

	// 	if(configStr != nullptr)
	// 		env->ReleaseStringUTFChars(filterConfig, configStr);

	// 	CGE_LOG_INFO("generate over!\n");

	// 	delete glContext;

	// 	return retStatus;
	// }

}


#endif







