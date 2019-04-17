/*
 * cgeImageHandlerAndroid.cpp
 *
 *  Created on: 2015-12-20
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#include "cgeImageHandlerAndroid.h"
 #include <android/bitmap.h>

namespace CGE
{
	CGEImageHandlerAndroid::CGEImageHandlerAndroid()
	{
		CGE_LOG_INFO("CGEImageHandlerAndroid created!\n");
	}

	CGEImageHandlerAndroid::~CGEImageHandlerAndroid()
	{
		CGE_LOG_INFO("CGEImageHandlerAndroid released!\n");
	}

	bool CGEImageHandlerAndroid::initWithBitmap(JNIEnv* env, jobject bmp, bool enableReversion)
	{
		AndroidBitmapInfo info;

		int ret = AndroidBitmap_getInfo(env, bmp, &info);

		if(ret < 0)
		{
			CGE_LOG_ERROR("AndroidBitmap_getInfo() failed ! error=%d", ret);
			return false;
		}

		CGE_LOG_INFO("color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d", info.width, info.height, info.stride, info.format, info.flags);

		if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
		{
			CGE_LOG_ERROR("Bitmap format is not RGBA_8888 !");
			return false;
		}

		char* row;

		ret = AndroidBitmap_lockPixels(env, bmp, (void**) &row);

		if(ret < 0)
		{
			CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
			return false;
		}

		bool flag = initWithRawBufferData(row, info.width, info.height, CGE_FORMAT_RGBA_INT8, enableReversion);

		AndroidBitmap_unlockPixels(env, bmp);

		return flag;
	}

	jobject CGEImageHandlerAndroid::getResultBitmap(JNIEnv* env)
	{

		jclass bitmapCls = env->FindClass("android/graphics/Bitmap");

		jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
        jstring configName = env->NewStringUTF("ARGB_8888");
        jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
        jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
        jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass, valueOfBitmapConfigFunction, configName);
        env->DeleteLocalRef(configName);
        jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, m_dstImageSize.width, m_dstImageSize.height, bitmapConfig);

        char* row;

        int ret = AndroidBitmap_lockPixels(env, newBitmap, (void**) &row);

        if (ret < 0)
        {
            CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            return nullptr;
        }

        getOutputBufferData(row, CGE_FORMAT_RGBA_INT8);
        AndroidBitmap_unlockPixels(env, newBitmap);

        return newBitmap;
	}


}












