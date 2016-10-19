/*
* cgeVideoEncoder.h
*
*  Created on: 2015-7-30
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#if !defined(_VIDEO_ENCODER_H_) && defined(_CGE_USE_FFMPEG_)
#define _CGEVIDEO_ENCODER_H_

#include <string>
#include <fstream>
#include <mutex>

struct AVFrame;

namespace CGE
{
	struct CGEEncoderContextMP4;

	// 使用h264编码视频，FLTP浮点单声道音频的mp4 encoder
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
			int nbSamples[8]; //声音帧大小
			int channels; //声音包含几个通道
		};

		bool init(const char* filename, int fps, int width, int height, bool hasAudio = true, int bitRate = 1650000);

		void setRecordDataFormat(RecordDataFormat fmt);

		//两个record写入文件时将保证线程安全
		bool record(const ImageData& data);
		bool record(const AudioSampleData& data);

		bool recordAudioFrame(AVFrame*);
		bool recordVideoFrame(AVFrame*);

		double getVideoStreamTime();
		double getAudioStreamTime();

		//保存视频
		bool save();

		//丢弃视频
		void drop();

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

}



#endif
