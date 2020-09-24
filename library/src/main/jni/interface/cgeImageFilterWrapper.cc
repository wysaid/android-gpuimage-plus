/*
* cgeImageFilterWrapper.cpp
*
*  Created on: 2016-3-19
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeImageFilterWrapper.h"
#include "cgeEnlargeEyeFilter.h"
#include "cgeUtilFunctions.h"
#include "cgeMultipleEffects.h"
#include "cgeTextureUtils.h"
#include "cgeAdvancedEffects.h"

using namespace CGE;

extern "C"
{

	JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageFilter_createEnlarge2EyesFilter(JNIEnv *, jclass)
	{
		CGEEnlarge2EyesFilter* filter = new CGEEnlarge2EyesFilter();
		if(!filter->init())
		{
			delete filter;
			filter = nullptr;
		}
		return (jlong)filter;
	}

	JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageFilter_createEnlarge2EyesAndMouthFilter(JNIEnv *, jclass)
	{
		CGEEnlarge2EyesAndMouthFilter* filter = new CGEEnlarge2EyesAndMouthFilter();
		if(!filter->init())
		{
			delete filter;
			filter = nullptr;
		}
		return (jlong)filter;
	}

	JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_setEnlarge2EyesFilterArgs(JNIEnv *, jclass, jlong addr, jfloat leftEyeRadius, jfloat rightEyeRadius, jfloat leftEyePosX, jfloat leftEyePosY, jfloat rightEyePosX, jfloat rightEyePosY)
	{
		CGEEnlarge2EyesFilter* filter = (CGEEnlarge2EyesFilter*)addr;
		filter->setEyeEnlargeRadius(leftEyeRadius, rightEyeRadius);
		filter->setEyePos(Vec2f(leftEyePosX, leftEyePosY), Vec2f(rightEyePosX, rightEyePosY));
	}

	JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_setEnlarge2EyesFilterIntensity(JNIEnv *, jclass, jlong addr, jfloat intensity)
	{
		CGEEnlarge2EyesFilter* filter = (CGEEnlarge2EyesFilter*)addr;
		filter->setIntensity(intensity);
	}

	JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_setEnlarge2EyesAndMouthFilterArgs(JNIEnv *, jclass, jlong addr, jfloat mouthRadius, jfloat mouthPosX, jfloat mouthPosY)
	{
		CGEEnlarge2EyesAndMouthFilter* filter = (CGEEnlarge2EyesAndMouthFilter*)addr;
		filter->setMouthEnlargeRadius(mouthRadius);
		filter->setMouthPos(Vec2f(mouthPosX, mouthPosY));
	}

	JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageFilter_createNativeFilterByConfig
	  (JNIEnv *env, jclass, jstring config, jfloat intensity)
	{
		if(config == nullptr)
			return 0;

		CGEImageFilterInterfaceAbstract* f = nullptr;

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
				if(filter->isWrapper())
				{
					auto&& filters = filter->getFilters(true);
					if(!filters.empty())
					{
						f = filters[0];
					}
					delete filter;
				}
				else
				{
					f = filter;
					filter->setIntensity(intensity);
				}
			}
		}

		env->ReleaseStringUTFChars(config, configStr);

		return (jlong)f;
	}

	JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_releaseNativeFilter
	  (JNIEnv *, jclass, jlong filter)
	{
		auto* f = (CGEImageFilterInterfaceAbstract*)filter;
		delete f;
	}


	JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageFilter_createAlienLookFilter(JNIEnv *, jclass)
	{
		auto* filter = createAlienLookFilter();
		return (jlong)filter;
	}

	JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_setAlienLookFilterArgs(JNIEnv *, jclass, jlong addr, jfloat width, jfloat height, jfloat intensity)
	{
		auto* filter = (CGEAlienLookFilter*)addr;
		filter->setIntensity(intensity);
		filter->setImageSize(width, height);
	}


	JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageFilter_setAlienLookFilterPosition(JNIEnv *, jclass, jlong addr, jfloat leftEyePosX, jfloat leftEyePosY, jfloat rightEyePosX, jfloat rightEyePosY, jfloat mouthPosX, jfloat mouthPosY)
	{
		auto* filter = (CGEAlienLookFilter*)addr;
		filter->updateKeyPoints(leftEyePosX, leftEyePosY, rightEyePosX, rightEyePosY, mouthPosX, mouthPosY);
	}

}





