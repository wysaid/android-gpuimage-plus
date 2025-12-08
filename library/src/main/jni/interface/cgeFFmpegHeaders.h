/*
 * cgeFFmpegHeaders.h
 *
 *  Created on: 2015-12-10
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

// Should not be directly referenced in header files, otherwise it will create heavy dependencies on ffmpeg headers

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
}
