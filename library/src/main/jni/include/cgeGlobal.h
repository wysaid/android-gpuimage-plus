/*
* cgeGlobal.h
*
*  Created on: 2014-9-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _CGEGLOBAL_H_
#define _CGEGLOBAL_H_

#ifdef GLEW_USED
#include "cgePlatform_GLEW.h"
#elif defined(ANDROID_NDK)
#include "cgePlatform_ANDROID.h"
#elif defined(IOS_SDK)
#include "cgePlatform_IOS.h"
#elif defined(LIBCGE4QT_LIB)
#include "cgePlatform_QT.h"
#endif


#ifdef __cplusplus

namespace CGE
{
	//辅助类，全局可用。
	class CGEGlobalConfig
	{
	public:
		static int viewWidth, viewHeight;

		static GLuint sVertexBufferCommon;
		static float sVertexDataCommon[8];

		enum InitArguments
		{
			CGE_INIT_LEAST = 0,
			CGE_INIT_COMMONVERTEXBUFFER = 0x1,
			CGE_INIT_SPRITEBUILTIN = 0x3,
			CGE_INIT_DEFAULT = 0xffffffff,
		};

		static InitArguments sInitArugment;
	};

	//CGE 全局初始化函数。
	bool cgeInitialize(int w = CGEGlobalConfig::viewWidth, int h = CGEGlobalConfig::viewHeight, CGEGlobalConfig::InitArguments arg = CGEGlobalConfig::CGE_INIT_DEFAULT);

	inline bool cgeInitialize(int w, int h, GLenum arg)
	{
		return cgeInitialize(w, h, CGEGlobalConfig::InitArguments(arg));
	}

	void cgeInitFilterStatus();

	//CGE 全局清除函数
	void cgeCleanup();

	//设置画面显示尺寸
	void cgeSetGlobalViewSize(int width, int height);

}

#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    void cgePrintGLInfo();
    const char* cgeQueryGLExtensions();
    bool cgeCheckGLExtension(const char* ext);

	GLuint cgeGenCommonQuadArrayBuffer();
    
#ifdef __cplusplus
}
#endif

#endif