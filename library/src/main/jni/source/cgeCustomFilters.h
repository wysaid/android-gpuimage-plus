//
//  cgeCustomFilters.h
//  filterLib
//
//  Created by wysaid on 16/1/11.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#ifndef cgeCustomFilters_h
#define cgeCustomFilters_h

typedef enum CustomFilterType
{
    CGE_CUSTOM_FILTER_0,
    CGE_CUSTOM_FILTER_1,
    CGE_CUSTOM_FILTER_2,
    CGE_CUSTOM_FILTER_3,
    CGE_CUSTOM_FILTER_4,
    CGE_CUSTOM_FILTER_TOTAL_NUMBER
} CustomFilterType;

#include <jni.h>
#include <android/bitmap.h>
#include "cgeSharedGLContext.h"
#include "cgeNativeLibrary.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    //intensity: 0 for origin, 1 for normal, below 0 for neg effect, above 1 for enhanced effect.
    //processingContext: nil for global context, otherwise use the context you provided.
    jobject cgeFilterImage_CustomFilters(JNIEnv *env, jobject bmp, CustomFilterType type, float intensity, jboolean hasContext);

    //processingContext: you can pass nullptr, so the previous binded context will be used. (may return nullptr if no context is binded)
    //other args meanings is the same to the above.
    //type "CGEMutipleEffectFilter" will be returned.
    void* cgeCreateCustomFilter(CustomFilterType type, float intensity);
    
    
#ifdef __cplusplus
}
#endif

#endif
