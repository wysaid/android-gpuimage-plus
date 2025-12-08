/*
 * cgeVideoEncoder.h
 *
 *  Created on: 2015-7-30
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#if !defined(_VIDEO_ENCODER_H_) && defined(CGE_USE_FFMPEG)
#define _CGEVIDEO_ENCODER_H_

#include <fstream>
#include <mutex>
#include <string>

struct AVFrame;
struct AVDictionary;

namespace CGE
{
struct CGEEncoderContextMP4;

// h264 video encoding, aac-FLTP single channel audio encoding
// MP4 encoder using h264 for video and FLTP floating point mono audio
class CGEVideoEncoderMP4
{
public:
    CGEVideoEncoderMP4();
    ~CGEVideoEncoderMP4();

    enum RecordDataFormat
    {
        FMT_RGBA8888,
        FMT_RGB565,
        FMT_BGR24,
        FMT_GRAY8,
        FMT_NV21,
        FMT_YUV420P
    };

    struct ImageData
    {
        const unsigned char* data[8];
        int width, height;
        int linesize[8];
        long pts;
    };

    struct AudioSampleData
    {
        const unsigned short* data[8];
        int nbSamples[8]; // Audio sample size
        int channels;     // Audio channel
    };

    bool init(const char* filename, int fps, int width, int height, bool hasAudio = true, int bitRate = 1650000, int audioSampleRate = 44100, AVDictionary* options = nullptr, const char* rotation = nullptr);

    void setRecordDataFormat(RecordDataFormat fmt);

    // thread safe
    bool record(const ImageData& data);
    bool record(const AudioSampleData& data);

    bool recordAudioFrame(AVFrame*);
    bool recordVideoFrame(AVFrame*);

    double getVideoStreamTime();
    double getAudioStreamTime();

    // Save video
    bool save();

    // Discard video
    void drop();

    int getAudioSampleRate();

protected:
    bool _openVideo();
    bool _openAudio();
    int _queryDataFormat(RecordDataFormat fmt);

private:
    CGEEncoderContextMP4* m_context;
    std::string m_filename;
    int m_recordDataFmt;
    std::mutex m_mutex;

    unsigned char* m_videoPacketBuffer;
    int m_videoPacketBufferSize;
    unsigned char* m_audioPacketBuffer;
    int m_audioPacketBufferSize;

    bool m_hasAudio;
};

} // namespace CGE

#endif
