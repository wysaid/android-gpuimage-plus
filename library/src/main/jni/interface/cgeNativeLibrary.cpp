/*
 * cgeNativeLibrary.cpp
 *
 *  Created on: 2015-7-9
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#include "cgeNativeLibrary.h"

#include "cgeFilters.h"
#include "cgeGLFunctions.h"
#include "cgeGlobal.h"
#include "cgeSharedGLContext.h"
#include "cgeUtilFunctions.h"

#include <android/bitmap.h>
#include <ctime>
#include <jni.h>
#include <unistd.h>
#include <sys/mman.h>
#include <android/api-level.h>

using namespace CGE;

// Function to check 16KB page size status
static void cgeCheck16kPageSize() {
    const char* status = "Unknown";
    const char* details = "";
    
    // 1. Check compile-time support
#ifdef ENABLE_16K_PAGE_SIZES
    bool compileTimeSupport = true;
#else
    bool compileTimeSupport = false;
#endif

    // 2. Check runtime Android version (16KB pages are supported from Android 15, API 35)
    int apiLevel = android_get_device_api_level();
    bool systemSupport = (apiLevel >= 35);
    
    // 3. Check actual page size being used
    long pageSize = sysconf(_SC_PAGESIZE);
    bool activelyUsing16k = (pageSize == 16384);
    
    // Determine status and details
    if (!compileTimeSupport) {
        status = "DISABLED";
        details = "16KB page size support not enabled at compile time";
    } else if (!systemSupport) {
        status = "COMPILE_READY";
        details = "16KB support compiled in, but Android version < 35 (Android 15)";
    } else if (activelyUsing16k) {
        status = "ACTIVE";
        details = "16KB page size is fully active and working";
    } else {
        status = "SUPPORTED_BUT_INACTIVE";
        details = "16KB support available but current page size is not 16KB";
    }
    
    // Log comprehensive status - use CGE_LOG_KEEP for important status info
    CGE_LOG_KEEP("=== CGE 16KB Page Size Status ===");
    CGE_LOG_KEEP("Status: %s", status);
    CGE_LOG_KEEP("Details: %s", details);
    CGE_LOG_INFO("Compile-time support: %s", compileTimeSupport ? "YES" : "NO");
    CGE_LOG_INFO("Android API Level: %d (16KB support needs API 35+)", apiLevel);
    CGE_LOG_INFO("Current page size: %ld bytes", pageSize);
    CGE_LOG_INFO("16KB pages active: %s", activelyUsing16k ? "YES" : "NO");
    CGE_LOG_KEEP("================================");
}

extern "C"
{
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    cgeGl3StubInit();
    cgeGl31StubInit();
    CGE_LOG_INFO("JNI_OnLoad called. cgeGl3StubInit called.\n");
    
    // Check and report 16KB page size status
    cgeCheck16kPageSize();
    
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_NativeLibraryLoader_onLoad(JNIEnv* env, jclass cls)
{
    cgeGl3StubInit();
    cgeGl31StubInit();
    CGE_LOG_INFO("NativeLibraryLoader.onload called. cgeGl3StubInit called.\n");
}

/*
 * Class:     org_wysaid_nativePort_CGENativeLibrary
 * Method:    filterImage_MultipleEffects
 * Signature: (Landroid/graphics/Bitmap;Ljava/lang/String;)Landroid/graphics/Bitmap;
 */
JNIEXPORT jobject JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeFilterImage_1MultipleEffects(JNIEnv* env, jclass cls, jobject bmp, jstring config, jfloat intensity)
{
    CGETexLoadArg texLoadArg;
    texLoadArg.env = env;
    texLoadArg.cls = cls;

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

    jobject newBitmap;
    jclass bitmapCls = env->GetObjectClass(bmp);

    ret = AndroidBitmap_lockPixels(env, bmp, (void**)&row);
    if (ret < 0)
    {
        CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return nullptr;
    }

    CGESharedGLContext* glContext = CGESharedGLContext::create();
    if (glContext == nullptr)
    {
        CGE_LOG_ERROR("Create Context Failed!");
        return bmp;
    }

    glContext->makecurrent();

    {
        CGEImageHandler handler;
        handler.initWithRawBufferData(row, w, h, CGE_FORMAT_RGBA_INT8, false);
        AndroidBitmap_unlockPixels(env, bmp);

        CGEMutipleEffectFilter* filter = new CGEMutipleEffectFilter;

        filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

        const char* strConfig = env->GetStringUTFChars(config, 0);
        filter->initWithEffectString(strConfig);
        env->ReleaseStringUTFChars(config, strConfig);

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

        ret = AndroidBitmap_lockPixels(env, newBitmap, (void**)&row);
        if (ret < 0)
        {
            CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            // AndroidBitmap_unlockPixels(env, bmp);
            return nullptr;
        }

        handler.getOutputBufferData(row, CGE_FORMAT_RGBA_INT8);
        AndroidBitmap_unlockPixels(env, newBitmap);
    }

    CGE_LOG_INFO("unlocked pixels, function totalTime: %g s", (clock() - tm) / (float)CLOCKS_PER_SEC);

    delete glContext;
    return newBitmap;
}

/*
 * Class:     org_wysaid_nativePort_CGENativeLibrary
 * Method:    filterImage_MultipleEffectsWriteBack
 * Signature: (Landroid/graphics/Bitmap;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeFilterImage_1MultipleEffectsWriteBack(JNIEnv* env, jclass cls, jobject bmp, jstring config, jfloat intensity)
{
    CGETexLoadArg texLoadArg;
    texLoadArg.env = env;
    texLoadArg.cls = cls;

    AndroidBitmapInfo info;
    int w, h, ret;
    char* row;

    CGE_LOG_CODE(clock_t tm = clock();)

    if ((ret = AndroidBitmap_getInfo(env, bmp, &info)) < 0)
    {
        CGE_LOG_ERROR("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    CGE_LOG_INFO("color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d", info.width, info.height, info.stride, info.format, info.flags);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        CGE_LOG_ERROR("Bitmap format is not RGBA_8888 !");
        return;
    }
    // get the basic information of the image
    w = info.width;
    h = info.height;

    ret = AndroidBitmap_lockPixels(env, bmp, (void**)&row);

    if (ret < 0)
    {
        CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return;
    }

    CGESharedGLContext* glContext = CGESharedGLContext::create();

    if (glContext == nullptr)
    {
        CGE_LOG_ERROR("Create Context Failed!");
        return;
    }

    glContext->makecurrent();

    {
        CGEImageHandler handler;
        handler.initWithRawBufferData(row, w, h, CGE_FORMAT_RGBA_INT8, false);

        CGEMutipleEffectFilter* filter = new CGEMutipleEffectFilter;

        filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

        const char* strConfig = env->GetStringUTFChars(config, 0);
        filter->initWithEffectString(strConfig);
        env->ReleaseStringUTFChars(config, strConfig);

        filter->setIntensity(intensity);

        handler.addImageFilter(filter);
        handler.processingFilters();

        CGE_LOG_INFO("Reading results....");
        handler.getOutputBufferData(row, CGE_FORMAT_RGBA_INT8);
        CGE_LOG_INFO("Reading results OK....");
        CGE_LOG_INFO("unlocking pixels....");
        AndroidBitmap_unlockPixels(env, bmp);
        CGE_LOG_INFO("unlocked pixels, function totalTime: %g s", (clock() - tm) / (float)CLOCKS_PER_SEC);
    }

    delete glContext;
}

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeCreateFilterWithConfig(JNIEnv* env, jclass, jstring config)
{
    CGETexLoadArg texLoadArg;
    texLoadArg.env = env;
    texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

    const char* configStr = env->GetStringUTFChars(config, 0);

    CGEMutipleEffectFilter* filter = new CGEMutipleEffectFilter;
    filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

    if (!filter->initWithEffectString(configStr))
    {
        delete filter;
        filter = nullptr;
    }

    env->ReleaseStringUTFChars(config, configStr);

    return (jlong)filter;
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeDeleteFilterWithAddress(JNIEnv* env, jclass, jlong addr)
{
    delete (CGEImageFilterInterfaceAbstract*)addr;
}

// filterType: normal, keep_ratio, tile

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeCreateBlendFilter(JNIEnv* env, jclass, jint blendMode, jint texID, jint texWidth, jint texHeight, jint filterType, jfloat intensity)
{
    CGEBlendWithResourceFilter* filter = nullptr;

    switch (filterType)
    {
    case 0:
        filter = new CGEBlendWithResourceFilter();
        CGE_LOG_INFO("Creating normal blend filter...");
        break;
    case 1:
        filter = new CGEBlendKeepRatioFilter();
        CGE_LOG_INFO("Creating keep-ratio blend filter...");
        break;
    case 2:
        filter = new CGEBlendTileFilter();
        CGE_LOG_INFO("Creating tile blend filter...");
        break;
    default:
        CGE_LOG_ERROR("Invalid filter type!\n");
        return 0;
    }

    if (filter->initWithMode((CGETextureBlendMode)blendMode))
    {
        filter->setSamplerID(texID);
        filter->setTexSize(texWidth, texHeight);
        filter->setIntensity(intensity);
    }
    else
    {
        delete filter;
        filter = nullptr;
    }

    return (jlong)filter;
}
}
