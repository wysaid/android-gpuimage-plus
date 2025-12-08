/*
 * cgeFrameRecorder.h
 *
 *  Created on: 2015-7-29
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#ifdef CGE_USE_FFMPEG

#if !defined(_CGEFRAMERECORDER_H_) && defined(CGE_USE_FFMPEG)
#define _CGEFRAMERECORDER_H_

#include "cgeFrameRenderer.h"
#include "cgeImageFilter.h"
#include "cgeVideoEncoder.h"

#include <queue>

// cgeFrameRecorder is suitable for real-time video recording, not for video conversion.

namespace CGE
{

class ImageDataWriteThread : public CGEThreadPreemptive
{
public:
    ImageDataWriteThread();
    ~ImageDataWriteThread();

    // Use maximum N buffers
    enum
    {
        MAX_DATA_CACHE_LEN = 3
    };

    struct DataCache
    {
        // DataCache() : buffer(nullptr), pts(-1) {}
        unsigned char* buffer;
        long pts;
    };

    DataCache getData4Read();
    void putData4Read(const DataCache& tex);

    DataCache getData4Write();
    void putData4Write(const DataCache& tex);

    bool hasData4Read();
    bool hasData4Write();

    // Set required buffer size
    void setBufferAllocSize(int bufferSize)
    {
        m_bufferSize = bufferSize;
    }

    void clearBuffers();

    void setTask(std::function<void(const DataCache&)> task)
    {
        m_task = task;
    }

protected:
    void runTask();

private:
    // Dynamic size data cache
    std::vector<DataCache> m_totalCaches;
    std::queue<DataCache> m_data4Read;
    std::queue<DataCache> m_data4Write;

    std::mutex m_readMutex;
    std::mutex m_writeMutex;

    int m_bufferSize;

    std::function<void(const DataCache&)> m_task;
};

///////////////////////////////////////

class CGEFrameRecorder : public CGEFrameRenderer
{
public:
    CGEFrameRecorder();
    ~CGEFrameRecorder();

    // Will affect framebuffer settings.
    // void update(GLuint externalTexture, float* transformMatrix);

    void runProc();

    ////////////////////////////////////////////////////////////
    // Video recording related
    // During actual recording, the frame rate may not always be met. When frame rate is insufficient,
    // frames will be dropped appropriately through calculation to maintain audio-video sync
    // If recording frame rate is too fast, some frames will also be discarded to achieve sync
    ////////////////////////////////////////////////////////////

    typedef CGEVideoEncoderMP4::AudioSampleData AudioSampleData;

    // Record current image frame.
    void recordImageFrame();

    // Record audio frame.
    void recordAudioFrame(const AudioSampleData& data);

    // Start recording video, parameter is frame rate (>0)
    // Returns true if recording started successfully, otherwise false
    // filename must be an absolute file path
    bool startRecording(int fps, const char* filename, int bitRate = 1650000);

    // Check if video recording has started (paused state is also considered started)
    bool isRecordingStarted();

    // End video recording
    // If not saving, parameter should be false, otherwise true
    // Return value indicates whether saving was successful
    bool endRecording(bool shouldSave);

    // Pause video recording (will cause audio/video threads to suspend)
    void pauseRecording();

    // Check if video recording is paused (not started state is considered not paused)
    bool isRecordingPaused();

    // Resume video recording
    // Return value indicates whether resume was successful
    bool resumeRecording();

    // Get current recording video duration
    double getRecordingTimestamp();

    double getVideoStreamTime();
    double getAudioStreamTime();

    // Beauty/enhancement related

    void setGlobalFilter(const char* config, CGETextureLoadFun texLoadFunc, void* loadArg);
    void setGlobalFilterIntensity(float intensity);
    inline CGEImageFilterInterfaceAbstract* getGlobalFilter() { return m_globalFilter; }

protected:
    void _createOffscreenContext();

    // Recording related...

    // Thread pool containing only one thread, used as recording thread
    CGEThreadPool* m_recordThread;
    CGESharedGLContext* m_offscreenContext; // Offscreen context used by recording thread

    std::function<void(void*)> m_recordingWork;

    ImageDataWriteThread* m_recordImageThread;

    double m_recordingTimestamp, m_lastRecordingTime;
    CGEVideoEncoderMP4* m_encoder;

    int m_currentPTS;
    int m_recordFPS;
    bool m_isRecording, m_isRecordingPaused;

    // Face tracking related
    CGEImageFilterInterfaceAbstract* m_globalFilter;
};

} // namespace CGE

#endif

#endif