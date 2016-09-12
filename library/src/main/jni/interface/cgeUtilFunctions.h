/*
* cgeUtilFunctions.h
*
*  Created on: 2015-11-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _CGEUTILFUNCTIONS_H_
#define _CGEUTILFUNCTIONS_H_

#include "cgeGLFunctions.h"

#ifdef __cplusplus

extern "C"{

#endif



struct CGETexLoadArg
{
	JNIEnv* env;
	jclass cls;
};

struct CGETextureResult
{
	GLuint texID;
	int width, height;
};

double getCurrentTimeMillis();

GLuint cgeGlobalTextureLoadFunc(const char* sourceName, GLint* w, GLint* h, void* arg);

CGETextureResult cgeLoadTexFromBitmap_JNI(JNIEnv* env, jclass cls, jobject bitmap);
CGETextureResult cgeLoadTexFromFile_JNI(JNIEnv* env, jclass cls, jstring imgFile);

#ifdef __cplusplus
}
#endif

#endif