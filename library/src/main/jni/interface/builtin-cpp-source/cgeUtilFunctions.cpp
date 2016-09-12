/*
* cgeUtilFunctions.cpp
*
*  Created on: 2015-11-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include <sys/time.h>

#include "cgeUtilFunctions.h"

using namespace CGE;

extern "C"
{
	//获取当前的毫秒级时间
	double getCurrentTimeMillis()
	{
		struct timeval tv;
		gettimeofday(&tv, nullptr);
		return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
	}

	GLuint cgeGlobalTextureLoadFunc(const char* sourceName, GLint* w, GLint* h, void* arg)
	{
		CGETexLoadArg* texLoadArg = (CGETexLoadArg*)arg;
		JNIEnv *env = texLoadArg->env;
		jclass cls = texLoadArg->cls;

		jmethodID loadTextureMethod = env->GetStaticMethodID(cls, "loadTextureByName", "(Ljava/lang/String;)Lorg/wysaid/nativePort/CGENativeLibrary$TextureResult;");

		if(loadTextureMethod == nullptr)
		{
			CGE_LOG_ERROR("Fatal error: find method failed!\n");
			return 0;
		}

		jclass texResult = env->FindClass("org/wysaid/nativePort/CGENativeLibrary$TextureResult");
		
		jfieldID textureFieldID = env->GetFieldID(texResult,"texID","I");
		jfieldID texWidthFieldID = env->GetFieldID(texResult,"width","I");
		jfieldID texHeightFieldID = env->GetFieldID(texResult,"height","I");

		jstring srcName = env->NewStringUTF(sourceName);
		jobject result = env->CallStaticObjectMethod(cls, loadTextureMethod, srcName);
		env->DeleteLocalRef(srcName);

		if(result == nullptr)
		{
			return 0;
		}

		if(w != nullptr)
			*w = env->GetIntField(result, texWidthFieldID);
		if(h != nullptr)
			*h = env->GetIntField(result, texHeightFieldID);
		jint texID = env->GetIntField(result, textureFieldID);

		CGE_LOG_INFO("cgeGlobalTextureLoadFunc loadImage success - srcName: %s, texID: %d, texWidth: %d, texHeight: %d", sourceName, texID, *w, *h);

		return texID;
	};

	CGETextureResult cgeLoadTexFromBitmap_JNI(JNIEnv* env, jclass cls, jobject bitmap)
	{
		CGETextureResult result = {0};

		jmethodID loadTextureMethod = env->GetStaticMethodID(cls, "loadTextureByBitmap", "(Landroid/graphics/Bitmap;)Lorg/wysaid/nativePort/CGENativeLibrary$TextureResult;");

		if(loadTextureMethod == nullptr)
		{
			CGE_LOG_ERROR("Fatal error: find method failed!\n");
			return result;
		}

		jclass texResult = env->FindClass("org/wysaid/nativePort/CGENativeLibrary$TextureResult");
		
		jfieldID textureFieldID = env->GetFieldID(texResult,"texID","I");
		jfieldID texWidthFieldID = env->GetFieldID(texResult,"width","I");
		jfieldID texHeightFieldID = env->GetFieldID(texResult,"height","I");

		jobject loadResult = env->CallStaticObjectMethod(cls, loadTextureMethod, bitmap);

		if(loadResult == nullptr)
		{
			return result;
		}

		result.width = env->GetIntField(loadResult, texWidthFieldID);
		result.height = env->GetIntField(loadResult, texHeightFieldID);
		result.texID = env->GetIntField(loadResult, textureFieldID);

		return result;
	}

	CGETextureResult cgeLoadTexFromFile_JNI(JNIEnv* env, jclass cls, jstring imgFile)
	{
		CGETextureResult result = {0};

		jmethodID loadTextureMethod = env->GetStaticMethodID(cls, "loadTextureByFile", "(Ljava/lang/String;)Lorg/wysaid/nativePort/CGENativeLibrary$TextureResult;");

		if(loadTextureMethod == nullptr)
		{
			CGE_LOG_ERROR("Fatal error: find method failed!\n");
			return result;
		}

		jclass texResult = env->FindClass("org/wysaid/nativePort/CGENativeLibrary$TextureResult");
		
		jfieldID textureFieldID = env->GetFieldID(texResult,"texID","I");
		jfieldID texWidthFieldID = env->GetFieldID(texResult,"width","I");
		jfieldID texHeightFieldID = env->GetFieldID(texResult,"height","I");

		jobject loadResult = env->CallStaticObjectMethod(cls, loadTextureMethod, imgFile);

		if(loadResult == nullptr)
		{
			return result;
		}

		result.width = env->GetIntField(loadResult, texWidthFieldID);
		result.height = env->GetIntField(loadResult, texHeightFieldID);
		result.texID = env->GetIntField(loadResult, textureFieldID);

		return result;
	}
}























