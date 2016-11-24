/*
* cgeFFmpegHeaders.h
*
*  Created on: 2015-12-10
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

//不可直接在头文件中引用, 否则将对ffmpeg头文件产生较大依赖

extern "C"
{
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavformat/avformat.h"
#include "../ffmpeg/libavutil/avutil.h"
#include "../ffmpeg/libswresample/swresample.h"
#include "../ffmpeg/libswscale/swscale.h"

#include "../ffmpeg/libavutil/opt.h"
#include "../ffmpeg/libavutil/imgutils.h"

}