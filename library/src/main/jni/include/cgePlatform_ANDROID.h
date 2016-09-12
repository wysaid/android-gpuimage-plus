/*
 * cgePlatforms.h
 *
 *  Created on: 2013-12-31
 *      Author: Wang Yang
 *  Description: load some library and do some essential initialization before compiling.
 */

#ifndef CGEPLATFORMS_H_
#define CGEPLATFORMS_H_

#include <cstdio>

#ifdef _CGE_USE_ES_API_3_0_
	 #include <GLES3/gl3.h>
	 #include <GLES3/gl3ext.h>
#else
	#include <GLES2/gl2.h>
	
#endif

 #include <GLES2/gl2ext.h>

//#include <EGL/egl.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <jni.h>
#include <cstdlib>


#ifndef CGE_LOG_TAG
#define CGE_LOG_TAG "CGE"
#endif

#if defined(DEBUG) || defined(_DEBUG) || defined(_CGE_LOGS_)
#define  CGE_LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO, CGE_LOG_TAG, __VA_ARGS__)
#define  CGE_LOG_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR, CGE_LOG_TAG, __VA_ARGS__)
#else
#define  CGE_LOG_INFO(...)
#define  CGE_LOG_ERROR(...)
#endif

#if defined(DEBUG) || defined(_DEBUG) || defined(_CGE_LOGS_)
#define CGE_LOG_CODE(...) __VA_ARGS__
#else
#define CGE_LOG_CODE(...)
#endif

#if defined(_CGE_MEMORY_LEAK_TEST_) && _CGE_MEMORY_LEAK_TEST_
#define CGE_UNEXPECTED_ERR_MSG(...) __VA_ARGS__
//for important log msg
#define CGE_LOG_KEEP(...) __android_log_print(ANDROID_LOG_WARN, CGE_LOG_TAG, __VA_ARGS__)
#else
#define CGE_UNEXPECTED_ERR_MSG(...)
#endif


#endif /* HTPLATFORMS_H_ */
