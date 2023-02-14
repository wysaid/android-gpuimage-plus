/*
 * cgeFrameRecorderWrapper.cpp
 *
 *  Created on: 2015-7-9
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#include "cgeFrameRecorderWrapper.h"

#include "cgeUtilFunctions.h"

#ifdef CGE_USE_FFMPEG
#include "cgeFrameRecorder.h"
#else
#include "cgeFrameRenderer.h"
namespace CGE
{
typedef CGEFrameRenderer CGEFrameRecorder;
}
#endif

using namespace CGE;

extern "C"
{
/*
 * Class:     org_wysaid_nativePort_CGEFrameRecorder
 * Method:    nativeCreate
 * Signature: ()Ljava/nio/ByteBuffer;
 */
JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeCreateRecorder(JNIEnv* env, jobject)
{
    cgePrintGLInfo();
    CGEFrameRecorder* recorder = new CGEFrameRecorder();
    return (jlong)recorder;
}

//视频录制相关 API

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeStartRecording(JNIEnv* env, jobject, jlong addr, jint fps, jstring filename, jint bitRate)
{
#ifdef CGE_USE_FFMPEG
    const char* path = env->GetStringUTFChars(filename, 0);
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    bool ret = recorder->startRecording(fps, path, bitRate);
    env->ReleaseStringUTFChars(filename, path);
    return ret;
#else
    return {};
#endif
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeIsRecordingStarted(JNIEnv* env, jobject, jlong addr)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    return recorder->isRecordingStarted();
#else
    return {};
#endif
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeEndRecording(JNIEnv* env, jobject, jlong addr, jboolean shouldSave)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    return recorder->endRecording(shouldSave);
#else
    return {};
#endif
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativePauseRecording(JNIEnv* env, jobject, jlong addr)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    recorder->pauseRecording();
#endif
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeIsRecordingPaused(JNIEnv* env, jobject, jlong addr)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    return recorder->isRecordingPaused();
#else
    return {};
#endif
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeResumeRecording(JNIEnv* env, jobject, jlong addr)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    return recorder->resumeRecording();
#else
    return {};
#endif
}

JNIEXPORT jdouble JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeGetTimestamp(JNIEnv* env, jobject, jlong addr)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    return recorder->getRecordingTimestamp();
#else
    return {};
#endif
}

JNIEXPORT jdouble JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeGetVideoStreamtime(JNIEnv* env, jobject, jlong addr)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    return recorder->getVideoStreamTime();
#else
    return {};
#endif
}

JNIEXPORT jdouble JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeGetAudioStreamtime(JNIEnv* env, jobject, jlong addr)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    return recorder->getAudioStreamTime();
#else
    return {};
#endif
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeRecordImageFrame(JNIEnv* env, jobject, jlong addr)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    recorder->recordImageFrame();
#endif
}

// static inline bool isBigEndian()
// {
// 	unsigned short n = 0xabcd;
// 	unsigned char* c = (unsigned char*)&n;
// 	CGE_LOG_INFO("N: %x, c[0]: %x, c[1]: %x", n , c[0], c[1]);
// 	return c[0] == 0xab;
// }

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeRecordAudioFrame(JNIEnv* env, jobject, jlong addr, jobject audioBuffer, jint bufferLen)
{
#ifdef CGE_USE_FFMPEG
    unsigned short* buffer = (unsigned short*)env->GetDirectBufferAddress(audioBuffer);

    if (buffer != nullptr)
    {
        CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
        CGEVideoEncoderMP4::AudioSampleData audioData;
        audioData.channels = 1;
        audioData.data[0] = buffer;
        audioData.nbSamples[0] = bufferLen;
        recorder->recordAudioFrame(audioData);
    }
    else
    {
        CGE_LOG_ERROR("Record Audio Frame failed!\n");
    }
#endif
}

/////////////////  Face Detection  /////////////////////////////

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeSetGlobalFilter(JNIEnv* env, jobject, jlong addr, jstring config)
{
#ifdef CGE_USE_FFMPEG
    static CGETexLoadArg texLoadArg;
    texLoadArg.env = env;
    texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    const char* configStr = env->GetStringUTFChars(config, 0);
    recorder->setGlobalFilter(configStr, cgeGlobalTextureLoadFunc, &texLoadArg);
    env->ReleaseStringUTFChars(config, configStr);
#endif
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeSetBeautifyFilter(JNIEnv*, jobject, jlong addr)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    auto* handler = recorder->getImageHandler();
    int w = -1, h = -1;

    char buffer[512];
    const char* config = "#unpack @beautify face 1.0";

    if (handler != nullptr)
    {
        const auto& sz = handler->getOutputFBOSize();
        w = sz.width;
        h = sz.height;
    }

    if (w > 0 && h > 0)
    {
        sprintf(buffer, "%s %d %d", config, w, h);
        config = buffer;
    }

    recorder->setGlobalFilter(config, nullptr, nullptr);
    CGE_LOG_INFO("启用美化效果!");
#endif
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeSetGlobalFilterIntensity(JNIEnv* env, jobject, jlong addr, jfloat intensity)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    recorder->setGlobalFilterIntensity(intensity);
#endif
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFrameRecorder_nativeIsGlobalFilterEnabled(JNIEnv*, jobject, jlong addr)
{
#ifdef CGE_USE_FFMPEG
    CGEFrameRecorder* recorder = (CGEFrameRecorder*)addr;
    return recorder->getGlobalFilter() != nullptr;
#else
    return {};
#endif
}
}
