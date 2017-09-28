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
#include "cgeTextureUtils.h"

#define USE_GPU_I420_ENCODING 0 //Maybe faster when set to 1

extern "C"
{
    JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFFmpegNativeLibrary_nativeGenerateVideoWithFilter(JNIEnv *env, jclass cls, jstring outputFilename, jstring inputFilename, jstring filterConfig, jfloat filterIntensity, jobject blendImage, jint blendMode, jfloat blendIntensity, jboolean mute)
    {
        CGE_LOG_INFO("##### nativeGenerateVideoWithFilter!!!");
        
        if(outputFilename == nullptr || inputFilename == nullptr)
            return false;
        
        CGESharedGLContext* glContext = CGESharedGLContext::create(2048, 2048); //Max video resolution size of 2k.
        
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
        
        bool retStatus = CGE::cgeGenerateVideoWithFilter(outFilenameStr, inFilenameStr, configStr, filterIntensity, texResult.texID, (CGETextureBlendMode)blendMode, blendIntensity, mute, &texLoadArg);
        
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

    //A wrapper that Disables All Error Checking.
    class FastFrameHandler : public CGEImageHandler
    {
    public:

        void processingFilters()
        {
            if(m_vecFilters.empty() || m_bufferTextures[0] == 0)
            {
                glFlush();
                return;
            }

            glDisable(GL_BLEND);
            assert(m_vertexArrayBuffer != 0);

            glViewport(0, 0, m_dstImageSize.width, m_dstImageSize.height);

            for(auto& filter : m_vecFilters)
            {
                swapBufferFBO();
                glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffer);
                filter->render2Texture(this, m_bufferTextures[1], m_vertexArrayBuffer);
                glFlush();
            }

            glFinish();
        }

        void swapBufferFBO()
        {
            useImageFBO();
            std::swap(m_bufferTextures[0], m_bufferTextures[1]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
        }

    };
    
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
        
        int audioSampleRate = decodeHandler->getAudioSampleRate();

        CGE_LOG_INFO("The input audio sample-rate: %d", audioSampleRate);

#if USE_GPU_I420_ENCODING

        //The video height must be mutiple of 8 when using the gpu encoding.
        if(videoHeight % 8 != 0)
        {
            videoHeight = (videoHeight & ~0x7u) + 8;
        }

        TextureDrawerRGB2YUV420P* gpuEncoder = TextureDrawerRGB2YUV420P::create();
        gpuEncoder->setOutputSize(videoWidth, videoHeight);

        cgeMakeBlockLimit([&](){

            CGE_LOG_INFO("delete I420 gpu encoder");
            delete gpuEncoder;
        });

        mp4Encoder.setRecordDataFormat(CGEVideoEncoderMP4::FMT_YUV420P);

#else

        mp4Encoder.setRecordDataFormat(CGEVideoEncoderMP4::FMT_RGBA8888);

#endif

#if 0
        AVDictionary* metaData = decodeHandler->getOptions(); //Something wrong, fixing...
#else
        AVDictionary* metaData = nullptr;
#endif
        if(!mp4Encoder.init(outputFilename, ENCODE_FPS, videoWidth, videoHeight, !mute, 1650000, audioSampleRate, metaData, decodeHandler->getRotation()))
        {
            CGE_LOG_ERROR("CGEVideoEncoderMP4 - start recording failed!");
            return false;
        }
        
        CGE_LOG_INFO("encoder created!");
        
        FastFrameHandler handler;
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
        CGEVideoEncoderMP4::ImageData imageData = {0};
        imageData.width = videoWidth;
        imageData.height = videoHeight;

#if USE_GPU_I420_ENCODING

        imageData.linesize[0] = videoWidth * videoHeight;
        imageData.linesize[1] = videoWidth * videoHeight >> 2;
        imageData.linesize[2] = videoWidth * videoHeight >> 2;

        imageData.data[0] = cacheBuffer;
        imageData.data[1] = cacheBuffer + imageData.linesize[0];
        imageData.data[2] = imageData.data[1] + imageData.linesize[1];

#else
        imageData.linesize[0] = videoWidth * 4;
        imageData.data[0] = cacheBuffer;
#endif

        CGE_LOG_INFO("Enter loop...\n");
        
        while(1)
        {
            CGEFrameTypeNext nextFrameType = videoPlayer.queryNextFrame();
            
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
                    videoPTS = newPTS;
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
                    
#if USE_GPU_I420_ENCODING

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glViewport(0, 0, videoWidth, videoHeight * 3 / 8);
                    gpuEncoder->drawTexture(handler.getTargetTextureID());
                    glFinish();

                    glReadPixels(0, 0, videoWidth, videoHeight * 3 / 8, GL_RGBA, GL_UNSIGNED_BYTE, cacheBuffer);

#elif 1 //Maybe faster than calling 'handler.getOutputBufferData'

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glViewport(0, 0, videoWidth, videoHeight);
                    handler.drawResult();
                    glFinish();
                    
                    glReadPixels(0, 0, videoWidth, videoHeight, GL_RGBA, GL_UNSIGNED_BYTE, cacheBuffer);
#else
                    
                    handler.getOutputBufferData(cacheBuffer, CGE_FORMAT_RGBA_INT8);
#endif

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
                    //maybe wrong for some audio, replace with the code below.
                    mp4Encoder.recordVideoFrame(frame);
                }
            }
            else if(nextFrameType == FrameType_AudioFrame)
            {
                if(!mute)
                {
#if 1  //Set this to 0 when you need to convert more audio formats and this function can not give a right result.
                    AVFrame* pAudioFrame = decodeHandler->getCurrentAudioAVFrame();
                    if(pAudioFrame == nullptr)
                        continue;
                    
                    mp4Encoder.recordAudioFrame(pAudioFrame);
#else

                    auto* decodeData = decodeHandler->getCurrentAudioFrame();
                    CGEVideoEncoderMP4::AudioSampleData encodeData;
                    encodeData.data[0] = (const unsigned short*)decodeData->data;
                    encodeData.nbSamples[0] = decodeData->nbSamples;
                    encodeData.channels = decodeData->channels;

                    CGE_LOG_INFO("ts: %g, nbSamples: %d, bps: %d, channels: %d, linesize: %d, format: %d", decodeData->timestamp, decodeData->nbSamples, decodeData->bytesPerSample, decodeData->channels, decodeData->linesize, decodeData->format);

                    mp4Encoder.record(encodeData);

#endif
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
