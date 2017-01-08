//
//  cgeCustomFilters.mm
//  filterLib
//
//  Created by wysaid on 16/1/11.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#include "cgeCustomFilters.h"

#include "cgeUtilFunctions.h"
#include "cgeImageHandler.h"
#include "cgeMultipleEffects.h"
#include "customHelper.h"

using namespace CGE;

extern "C"
{
    void* cgeCreateCustomFilter(CustomFilterType type, float intensity)
    {
        if(type < 0 || type >= CGE_CUSTOM_FILTER_TOTAL_NUMBER)
            return nullptr;
        
        CGEMutipleEffectFilter* filter = nullptr;       
        CGEImageFilterInterface* customFilter = cgeCreateCustomFilterByType(type);
        
        if(customFilter == nullptr)
        {
            CGE_LOG_ERROR("create Custom filter failed!");;
            return nullptr;
        }
        
        filter = new CGEMutipleEffectFilter();
        filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, nullptr);
        filter->initCustomize();
        filter->addFilter(customFilter);
        filter->setIntensity(intensity);

        return filter;
    }
    

    jobject cgeFilterImage_CustomFilters(JNIEnv *env, jobject bmp, CustomFilterType type, float intensity, jboolean hasContext)
    {
        if(type < 0 || type >= CGE_CUSTOM_FILTER_TOTAL_NUMBER || intensity == 0.0f || bmp == nullptr)
            return bmp;
        
        CGESharedGLContext* ctx = nullptr;
        jobject newBitmap = nullptr;
        
        if(!hasContext)
        {
            ctx = CGESharedGLContext::create();

            if(ctx == nullptr)
            {
                CGE_LOG_ERROR("create context failed!");
                return nullptr;
            }

            ctx->makecurrent();
        }
        
        //similar with cgeNativeLibrary.cpp
        {
            AndroidBitmapInfo info;
            int w, h, ret;
            char* row;

            CGE_LOG_CODE(clock_t tm = clock();)

            if ((ret = AndroidBitmap_getInfo(env, bmp, &info)) < 0)
            {
                CGE_LOG_ERROR("AndroidBitmap_getInfo() failed ! error=%d", ret);
                return nullptr;
            }
            CGE_LOG_INFO("color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d", info.width, info.height, info.stride, info.format, info.flags);
            if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
            {
                CGE_LOG_ERROR("Bitmap format is not RGBA_8888 !");
                return nullptr;
            }

            // get the basic information of the image
            w = info.width;
            h = info.height;

            jclass bitmapCls = env->GetObjectClass(bmp);

            ret = AndroidBitmap_lockPixels(env, bmp, (void**) &row);
            if (ret < 0)
            {
                CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
                return nullptr;
            }

            CGEImageHandler handler;
            handler.initWithRawBufferData(row, w, h, CGE_FORMAT_RGBA_INT8, false);
            AndroidBitmap_unlockPixels(env, bmp);

            CGEImageFilterInterface* customFilter = cgeCreateCustomFilterByType(type);

            if(customFilter == nullptr)
            {
                CGE_LOG_ERROR("create Custom filter failed!");;
                return nullptr;
            }
            
            auto* filter = new CGEMutipleEffectFilter();
            filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, nullptr);
            filter->initCustomize();
            filter->addFilter(customFilter);
            filter->setIntensity(intensity);

            handler.addImageFilter(filter);
            handler.processingFilters();

            jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
            jstring configName = env->NewStringUTF("ARGB_8888");
            jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
            jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
            jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass, valueOfBitmapConfigFunction, configName);
            env->DeleteLocalRef(configName);
            newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, info.width, info.height, bitmapConfig);

            ret = AndroidBitmap_lockPixels(env, newBitmap, (void**) &row);
            if (ret < 0)
            {
                CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
                // AndroidBitmap_unlockPixels(env, bmp);
                return nullptr;
            }

            handler.getOutputBufferData(row, CGE_FORMAT_RGBA_INT8);
            AndroidBitmap_unlockPixels(env, newBitmap);
        }

        delete ctx;
        return newBitmap;
    }

}