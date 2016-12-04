/*
 * cgeImageHandlerWrapper.cpp
 *
 *  Created on: 2015-12-25
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#include "cgeImageHandlerWrapper.h"
#include "cgeImageHandlerAndroid.h"
#include "cgeTextureUtils.h"
#include "cgeUtilFunctions.h"
#include "cgeMultipleEffects.h"

using namespace CGE;

extern "C"
{


JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeCreateHandler
  (JNIEnv *env, jobject)
{
	CGEImageHandlerAndroid* handler = new CGEImageHandlerAndroid();
	return (jlong)handler;
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeInitWithBitmap
  (JNIEnv *env, jobject, jlong addr, jobject bmp)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	return handler->initWithBitmap(env, bmp, true); //use revision.
}

JNIEXPORT jobject JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeGetResultBitmap
  (JNIEnv *env, jobject, jlong addr)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	return handler->getResultBitmap(env);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetDrawerRotation
  (JNIEnv *env, jobject, jlong addr, jfloat rad)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;	
	auto* drawer = handler->getResultDrawer();
	drawer->setRotation(rad);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetDrawerFlipScale
  (JNIEnv *env, jobject, jlong addr, jfloat x, jfloat y)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	auto* drawer = handler->getResultDrawer();
	drawer->setFlipScale(x, y);
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterWithConfig
  (JNIEnv *env, jobject, jlong addr, jstring config, jboolean shouldCleanOlder, jboolean shouldProcess)
{	
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;

	if(shouldCleanOlder)
		handler->clearImageFilters(true);

	if(shouldProcess)
		handler->revertToKeptResult();

	if(config == nullptr)
	{
		return false;
	}

	const char* configStr = env->GetStringUTFChars(config, 0);	

	if(configStr == nullptr || *configStr == '\0')
	{
		CGE_LOG_INFO("Using empty filter config.");
	}
	else
	{
		CGETexLoadArg texLoadArg;
		
		texLoadArg.env = env;
		texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

		CGEMutipleEffectFilter* filter = new CGEMutipleEffectFilter();
		filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

		if(!filter->initWithEffectString(configStr))
		{
			delete filter;
		}
		else
		{
			handler->addImageFilter(filter);

			if(shouldProcess && handler->getTargetTextureID() != 0)
			{
				handler->processingFilters();
			}
		}
	}

	env->ReleaseStringUTFChars(config, configStr);
	return true;
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterIntensity
  (JNIEnv *env, jobject, jlong addr, jfloat intensity, jboolean shouldProcess)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;

	auto&& filters = handler->peekFilters();

	for(auto* filter : filters)
	{
		filter->setIntensity(intensity);
	}

	if(shouldProcess && !filters.empty() && handler->getTargetTextureID() != 0)
	{
		handler->revertToKeptResult();
		handler->processingFilters();
	}
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeDrawResult
  (JNIEnv *env, jobject, jlong addr)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	handler->drawResult();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeBindTargetFBO
  (JNIEnv *env, jobject, jlong addr)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	handler->useImageFBO();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetAsTarget
  (JNIEnv *env, jobject, jlong addr)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	handler->setAsTarget();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSwapBufferFBO
  (JNIEnv *env, jobject, jlong addr)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	handler->swapBufferFBO();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeRevertImage
  (JNIEnv *env, jobject, jlong addr)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	handler->revertToKeptResult();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeProcessingFilters
  (JNIEnv *env, jobject, jlong addr)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	handler->processingFilters();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeRelease
  (JNIEnv *env, jobject, jlong addr)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	delete handler;
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeProcessWithFilter
  (JNIEnv *env, jobject, jlong addr, jlong filterAddr)
{
	CGEImageHandlerAndroid* handler = (CGEImageHandlerAndroid*)addr;
	handler->processingWithFilter((CGEImageFilterInterfaceAbstract*)filterAddr);
}

}