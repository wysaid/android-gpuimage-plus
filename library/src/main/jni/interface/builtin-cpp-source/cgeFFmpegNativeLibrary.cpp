/*
* cgeFFmpegNativeLibrary.cpp
*
*  Created on: 2015-11-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifdef _CGE_USE_FFMPEG_

#include "cgeFFmpegHeaders.h"
#include "cgeFFmpegNativeLibrary.h"
#include "cgeCommonDefine.h"

using namespace CGE;

extern "C"
{
	////////// ffmpeg ////////////////////

	JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFFmpegNativeLibrary_avRegisterAll(JNIEnv *, jclass)
	{
		CGE_LOG_INFO("registerFFmpeg...");
	    av_register_all();
	}
}


#endif







