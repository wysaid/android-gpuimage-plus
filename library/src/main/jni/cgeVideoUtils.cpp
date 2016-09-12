/*
 * cgeUtilFunctions.cpp
 *
 *  Created on: 2015-12-15
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#ifdef _CGE_USE_FFMPEG_

#include "cgeVideoUtils.h"
#include "cgeFFmpegHeaders.h"
#include "cgeVideoPlayer.h"
#include "cgeVideoEncoder.h"
#include "cgeSharedGLContext.h"
#include "cgeMultipleEffects.h"
#include "cgeBlendFilter.h"
#include "cgeFrameRenderer.h"

extern "C"
{
    JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFFmpegNativeLibrary_nativeGenerateVideoWithFilter(JNIEnv *env, jclass cls, jstring outputFilename, jstring inputFilename, jstring filterConfig, jfloat filterIntensity, jobject blendImage, jint blendMode, jfloat blendIntensity, jboolean mute)
    {
        CGE_LOG_INFO("##### nativeGenerateVideoWithFilter!!!");
        
        if(outputFilename == nullptr || inputFilename == nullptr)
            return false;
        
        CGESharedGLContext* glContext = CGESharedGLContext::create(2048, 2048); //保证录制分辨率足够大 (2k)
        
        if(glContext == nullptr)
        {
            CGE_LOG_ERROR("Create GL Context Failed!");
            return false;
        }
        
        glContext->makecurrent();
        
        CGETextureResult texResult = {0};
        
        jclass nativeLibraryClass = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");
        
        if(blendImage != nullptr)
            texResult = cgeLoadTexFromBitmap_JNI(env, nativeLibraryClass, blendImage);
        
        const char* outFilenameStr = env->GetStringUTFChars(outputFilename, 0);
        const char* inFilenameStr = env->GetStringUTFChars(inputFilename, 0);
        const char* configStr = filterConfig == nullptr ? nullptr : env->GetStringUTFChars(filterConfig, 0);

        CGETexLoadArg texLoadArg;
        texLoadArg.env = env;
        texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");
        
        bool retStatus = cgeGenerateVideoWithFilter(outFilenameStr, inFilenameStr, configStr, filterIntensity, texResult.texID, (CGETextureBlendMode)blendMode, blendIntensity, mute, &texLoadArg);
        
        env->ReleaseStringUTFChars(outputFilename, outFilenameStr);
        env->ReleaseStringUTFChars(inputFilename, inFilenameStr);
        
        if(configStr != nullptr)
            env->ReleaseStringUTFChars(filterConfig, configStr);
        
        CGE_LOG_INFO("generate over!\n");
        
        delete glContext;
        
        return retStatus;
    }
    
}

namespace CGE
{
    
    // A simple-slow offscreen video rendering function.
    bool cgeGenerateVideoWithFilter(const char* outputFilename, const char* inputFilename, const char* filterConfig, float filterIntensity, GLuint texID, CGETextureBlendMode blendMode, float blendIntensity, bool mute, CGETexLoadArg* loadArg)
    {
        static const int ENCODE_FPS = 30;
        
        CGEVideoDecodeHandler* decodeHandler = new CGEVideoDecodeHandler();
        
        if(!decodeHandler->open(inputFilename))
        {
            CGE_LOG_ERROR("Open %s failed!\n", inputFilename);
            delete decodeHandler;
            return false;
        }
        
        //		decodeHandler->setSamplingStyle(CGEVideoDecodeHandler::ssFastBilinear); //already default
        
        int videoWidth = decodeHandler->getWidth();
        int videoHeight = decodeHandler->getHeight();
        unsigned char* cacheBuffer = nullptr;
        int cacheBufferSize = 0;
        
        CGEVideoPlayerYUV420P videoPlayer;
        videoPlayer.initWithDecodeHandler(decodeHandler);
        
        CGEVideoEncoderMP4 mp4Encoder;
        
        mp4Encoder.setRecordDataFormat(CGEVideoEncoderMP4::FMT_RGBA8888);
        if(!mp4Encoder.init(outputFilename, ENCODE_FPS, videoWidth, videoHeight, !mute))
        {
            CGE_LOG_ERROR("CGEVideoEncoderMP4 - start recording failed!");
            return false;
        }
        
        CGE_LOG_INFO("encoder created!");
        
        CGEFastFrameHandler handler;
        CGEBlendFilter* blendFilter = nullptr;
        
        if(texID != 0 && blendIntensity != 0.0f)
        {
            blendFilter = new CGEBlendFilter();
            
            if(blendFilter->initWithMode(blendMode))
            {
                blendFilter->setSamplerID(texID);
                blendFilter->setIntensity(blendIntensity);
            }
            else
            {
                delete blendFilter;
                blendFilter = nullptr;
            }
        }
        
        bool hasFilter = blendFilter != nullptr || (filterConfig != nullptr && *filterConfig != '\0' && filterIntensity != 0.0f);
        
        CGE_LOG_INFO("Has filter: %d\n", (int)hasFilter);
        
        if(hasFilter)
        {
            handler.initWithRawBufferData(nullptr, videoWidth, videoHeight, CGE_FORMAT_RGBA_INT8, false);
            // handler.getResultDrawer()->setFlipScale(1.0f, -1.0f);
            if(filterConfig != nullptr && *filterConfig != '\0' && filterIntensity != 0.0f)
            {
                CGEMutipleEffectFilter* filter = new CGEMutipleEffectFilter;
                filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, loadArg);
                filter->initWithEffectString(filterConfig);
                filter->setIntensity(filterIntensity);
                handler.addImageFilter(filter);
            }
            
            if(blendFilter != nullptr)
            {
                handler.addImageFilter(blendFilter);
            }
            
            cacheBufferSize = videoWidth * videoHeight * 4;
            cacheBuffer = new unsigned char[cacheBufferSize];
        }
        
        int videoPTS = -1;
        
        CGE_LOG_INFO("Enter loop...\n");
        
        while(1)
        {
            CGEFrameTypeNext nextFrameType = videoPlayer.queryNextFrame();
            
            // CGE_LOG_INFO("next frame: %d\n", (int)nextFrameType);
            
            if(nextFrameType == FrameType_VideoFrame)
            {
                if(!videoPlayer.updateVideoFrame())
                    continue;
                
                int newPTS = round(decodeHandler->getCurrentTimestamp() / 1000.0 * ENCODE_FPS);
                
                CGE_LOG_INFO("last pts: %d, new pts; %d\n", videoPTS, newPTS);
                
                if(videoPTS < 0)
                {
                    videoPTS = 0;
                }
                else if(videoPTS < newPTS)
                {
                    //避免掉帧
                    // if(videoPTS + 3 > newPTS)
                    // {
                    //     ++videoPTS;
                    // }
                    // else
                    // {
                    videoPTS = newPTS;
                    // }
                }
                else
                {
                    CGE_LOG_ERROR("drop frame...\n");
                    continue;
                }
                
                if(hasFilter)
                {
                    handler.setAsTarget();
                    glViewport(0, 0, videoPlayer.getLinesize(), videoHeight);
                    videoPlayer.render();
                    handler.processingFilters();
                    
#if 1
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glViewport(0, 0, videoWidth, videoHeight);
                    handler.drawResult();
                    glFinish();
                    
                    glReadPixels(0, 0, videoWidth, videoHeight, GL_RGBA, GL_UNSIGNED_BYTE, cacheBuffer);
#else
                    
                    handler.getOutputBufferData(cacheBuffer, CGE_FORMAT_RGBA_INT8);
#endif
                    
                    CGEVideoEncoderMP4::ImageData imageData;
                    imageData.width = videoWidth;
                    imageData.height = videoHeight;
                    imageData.linesize[0] = videoWidth * 4;
                    imageData.data[0] = cacheBuffer;
                    imageData.pts = videoPTS;
                    
                    if(!mp4Encoder.record(imageData))
                    {
                        CGE_LOG_ERROR("record frame failed!");
                    }
                }
                else
                {
                    AVFrame* frame = decodeHandler->getCurrentVideoAVFrame();
                    
                    frame->pts = videoPTS;
                    if(frame->data[0] == nullptr)
                        continue;
                    mp4Encoder.recordVideoFrame(frame);
                }
            }
            else if(nextFrameType == FrameType_AudioFrame)
            {
                if(!mute)
                {
                    AVFrame* pAudioFrame = decodeHandler->getCurrentAudioAVFrame();
                    //const CGEAudioFrameBufferData* pAudioData = decodeHandler->getCurrentAudioFrame();
                    
                    if(pAudioFrame == nullptr)
                        continue;
                    
                    mp4Encoder.recordAudioFrame(pAudioFrame);
                }
            }
            else
            {
                break;
            }
        }
        
        mp4Encoder.save();			
        delete[] cacheBuffer;
        
        return true;
    }
    
}


#endif