/*
* cgeCustomFilterWrapper.cpp
*
*  Created on: 2017-1-8
*      Author: Wang Yang
*        Mail: admin@wysaid.org
* Description: This demo teaches you how to create your own filters.
*/

#ifndef _CGECUSTOMIZEDFILTER_H_
#define _CGECUSTOMIZEDFILTER_H_

#include "cgeCustomFilters.h"
#include "cgeCommonDefine.h"

extern "C"
{
	JNIEXPORT jobject JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeFilterImageWithCustomFilter
	(JNIEnv *env, jclass cls, jobject bmp, jint index, jfloat intensity, jboolean hasContext, jboolean useWrapper)
	{
		if(index < 0 || index >= CGE_CUSTOM_FILTER_TOTAL_NUMBER)
		{
			CGE_LOG_ERROR("Invalid filter index!");
			return bmp;
		}

		return cgeFilterImage_CustomFilters(env, bmp, (CustomFilterType)index, intensity, hasContext, useWrapper);
	}

	JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeCreateCustomNativeFilter
	(JNIEnv *env, jclass cls, jint index, jfloat intensity, jboolean useWrapper)
	{
		if(index < 0 || index >= CGE_CUSTOM_FILTER_TOTAL_NUMBER)
		{
			CGE_LOG_ERROR("Invalid filter index!");
			return 0;
		}

		return (jlong)cgeCreateCustomFilter((CustomFilterType)index, intensity, useWrapper);
	}

	JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeGetCustomFilterNum
	(JNIEnv *env, jclass cls)
	{
		return CGE_CUSTOM_FILTER_TOTAL_NUMBER;
	}

}

#endif