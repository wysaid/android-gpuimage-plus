/*
* cgeVideoEncoder.cpp
*
*  Created on: 2015-7-30
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifdef _CGE_USE_FFMPEG_

#include <cassert>
#include "cgeGLFunctions.h"
#include "cgeVideoEncoder.h"
#include "cgeUtilFunctions.h"
#include "cgeFFmpegHeaders.h"


static AVStream *addStream(AVFormatContext *oc, AVCodec **codec,
                            enum AVCodecID codec_id, int frameRate, int width = -1, int height = -1, int bitRate = 1650000)
{
    AVCodecContext *c;
    AVStream *st;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        CGE_LOG_ERROR("Could not find encoder for '%s'\n", avcodec_get_name(codec_id));
        return nullptr;
    }

    st = avformat_new_stream(oc, *codec);
    if (!st) {
        CGE_LOG_ERROR("Could not allocate stream\n");
        return nullptr;
    }
    st->id = oc->nb_streams-1;
    c = st->codec;

    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        c->sample_fmt  = AV_SAMPLE_FMT_FLTP;
        c->bit_rate    = 64000;
        c->sample_rate = 44100;
        c->channels    = 1;
        c->flags      |= CODEC_FLAG_GLOBAL_HEADER;
        c->strict_std_compliance = -2;
        break;

    case AVMEDIA_TYPE_VIDEO:
        c->codec_id = codec_id;

        c->bit_rate = bitRate;
        /* Resolution must be a multiple of two. */
        c->width    = width;
        c->height   = height;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
        c->time_base.den = frameRate;
        c->time_base.num = 1;
        c->gop_size      = 12; /* emit one intra frame every twelve frames at most */
        c->pix_fmt       = AV_PIX_FMT_YUV420P;

        av_opt_set(c->priv_data, "preset", "veryfast", 0);
        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            /* just for testing, we also add B frames */
            c->max_b_frames = 2;
        }
        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
            /* Needed to avoid using macroblocks in which some coeffs overflow.
             * This does not happen with normal video, it just happens here as
             * the motion of the chroma plane does not match the luma plane. */
            c->mb_decision = 2;
        }
    break;

    default:
        break;
    }

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return st;
}

//////////////////////////////////////////////////////////////////////////

namespace CGE
{
	struct CGEEncoderContextMP4
	{
		CGEEncoderContextMP4() : pOutputFmt(nullptr), pFormatCtx(nullptr), pVideoStream(nullptr), pAudioStream(nullptr), pVideoCodec(nullptr), pAudioCodec(nullptr), pVideoFrame(nullptr), pAudioFrame(nullptr), pSwsCtx(nullptr), pSwrCtx(nullptr), dstSampleData(nullptr), dstSamplesSize(0), dstSampleDataIndex(0)
		{
			memset(&videoPacket, 0, sizeof(videoPacket));
			memset(&dstPicture, 0, sizeof(dstPicture));
		}
		~CGEEncoderContextMP4()
		{
			cleanup();
		}

		inline void cleanup()
		{
			if(pVideoStream)
			{
				avcodec_close(pVideoStream->codec);
				pVideoStream = nullptr;
			}

			if(pVideoFrame)
			{
				av_free(dstPicture.data[0]);
				av_free(pVideoFrame);
				pVideoFrame = nullptr;
			}

			if(pAudioFrame)
			{
				avcodec_free_frame(&pAudioFrame);
				pAudioFrame = nullptr;
			}

			if(pAudioStream)
			{
				avcodec_close(pAudioStream->codec);
				pAudioStream = nullptr;
			}

			if(pSwrCtx != nullptr)
			{
				av_free(dstSampleData[0]);
				dstSampleData = nullptr;
				dstSamplesLinesize = 0;
				dstSamplesSize = 0;
				swr_free(&pSwrCtx);
				pSwrCtx = nullptr;
			}

			if(pOutputFmt && pFormatCtx && !(pOutputFmt->flags & AVFMT_NOFILE))
			{
				avio_close(pFormatCtx->pb);
			}

			if(pFormatCtx)
			{
				avformat_free_context(pFormatCtx);
				pFormatCtx = nullptr;
			}

			pOutputFmt = nullptr;

			//av_free_packet(&audioPacket);

			memset(&videoPacket, 0, sizeof(videoPacket));
			memset(&dstPicture, 0, sizeof(dstPicture));

			if(pSwsCtx != nullptr)
			{
				sws_freeContext(pSwsCtx);
				pSwsCtx = nullptr;
			}

			pVideoCodec = nullptr;
			pAudioCodec = nullptr;
			dstSampleDataIndex = 0;
		}

		AVOutputFormat  *pOutputFmt;
		AVFormatContext *pFormatCtx;
		AVStream        *pVideoStream;
		AVStream        *pAudioStream;

		AVCodec         *pVideoCodec;
		AVCodec         *pAudioCodec;

		AVFrame         *pVideoFrame;
		AVFrame         *pAudioFrame;
		SwsContext      *pSwsCtx;
		SwrContext      *pSwrCtx;

		AVPacket        videoPacket;
		AVPacket        audioPacket;

		AVPicture       dstPicture;
		uint8_t         **dstSampleData;
		int             dstSampleDataIndex;
		int             dstSamplesLinesize;
		int             dstSamplesSize;

		int             maxDstNbSamples;
	};

	CGEVideoEncoderMP4::CGEVideoEncoderMP4() : m_videoPacketBuffer(nullptr), m_audioPacketBuffer(nullptr)
	{
		m_context = new CGEEncoderContextMP4;
	}

	CGEVideoEncoderMP4::~CGEVideoEncoderMP4()
	{
		drop();
		delete m_context;

		if(m_videoPacketBuffer != nullptr)
			av_free(m_videoPacketBuffer);

		if(m_audioPacketBuffer != nullptr)
			av_free(m_audioPacketBuffer);
	}

	bool CGEVideoEncoderMP4::init(const char* filename, int fps, int width, int height, bool hasAudio, int bitRate)
	{
		m_hasAudio = hasAudio;

		avformat_alloc_output_context2(&m_context->pFormatCtx, nullptr, nullptr, filename);
		if(!m_context->pFormatCtx)
		{
			//CGE_LOG_INFO("Could not deduce output format from file extension: using MPEG.\n");
			avformat_alloc_output_context2(&m_context->pFormatCtx, NULL, "mp4", filename);
		}

		m_filename = filename;

		// auto ret = avformat_alloc_output_context2(&m_context->pFormatCtx, nullptr, nullptr, filename);

		// CGE_LOG_ERROR("ret num %x, filename %s", ret, filename);

		if(!m_context->pFormatCtx)
		{
			//CGE_LOG_ERROR("avformat_alloc_output_context2 failed...");
			return false;
		}

		m_context->pOutputFmt = m_context->pFormatCtx->oformat;
		m_context->pVideoStream = nullptr;

		if(m_context->pOutputFmt->video_codec != AV_CODEC_ID_NONE)
		{
			m_context->pVideoStream = addStream(m_context->pFormatCtx, &m_context->pVideoCodec, m_context->pOutputFmt->video_codec, fps, width, height, bitRate);
		}

		if(m_hasAudio && m_context->pOutputFmt->audio_codec != AV_CODEC_ID_NONE)
		{
			m_context->pAudioStream = addStream(m_context->pFormatCtx, &m_context->pAudioCodec, m_context->pOutputFmt->audio_codec, fps, width, height, bitRate);
		}

		if(m_videoPacketBuffer != nullptr)
			av_free(m_videoPacketBuffer);

		if(m_audioPacketBuffer != nullptr)
		{
			av_free(m_audioPacketBuffer);
			m_audioPacketBuffer = nullptr;
		}

		m_videoPacketBufferSize = CGE::CGE_MAX(1024 * 256, width * height * 8);
		m_videoPacketBuffer = (unsigned char*)av_malloc(m_videoPacketBufferSize);

		if(m_hasAudio)
		{
			m_audioPacketBufferSize = 256 * 1024;
			m_audioPacketBuffer = (unsigned char*)av_malloc(m_audioPacketBufferSize);
		}

		//CGE_LOG_INFO("addStream OK!");

		if(!m_context->pVideoStream || !_openVideo())
		{
			CGE_LOG_ERROR("_openVideo failed!\n");
			return false;
		}

		CGE_LOG_INFO("_openVideo OK!");

		if(m_hasAudio)
		{
			if(!m_context->pAudioStream || !_openAudio())
			{
				CGE_LOG_ERROR("_openAudio failed!\n");
				return false;
			}
		}		

		if(!(m_context->pOutputFmt->flags & AVFMT_NOFILE))
		{
			if(0 > avio_open(&m_context->pFormatCtx->pb, filename, AVIO_FLAG_WRITE))
			{
				CGE_LOG_ERROR("could not open file.");
				return false;
			}
		}

		CGE_LOG_INFO("avio_open OK!");

		if(0 > avformat_write_header(m_context->pFormatCtx, nullptr))
		{
			CGE_LOG_ERROR("avformat_write_header failed...");
			return false;
		}

		CGE_LOG_INFO("avformat_write_header OK!");

		if(m_context->pVideoFrame)
			m_context->pVideoFrame->pts = 0;

		return true;
	}

	bool CGEVideoEncoderMP4::_openVideo(/*AVFormatContext *oc, AVCodec *codec, AVStream *st*/)
	{
		int ret;
		AVCodecContext *c = m_context->pVideoStream->codec;
		AVFormatContext *oc = m_context->pFormatCtx;
		AVCodec *codec = m_context->pVideoCodec;

		/* open the codec */
		ret = avcodec_open2(c, codec, nullptr);
		if (ret < 0) {
			//fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
			return false;
		}

		/* allocate and init a re-usable frame */
		m_context->pVideoFrame = avcodec_alloc_frame();
		if (!m_context->pVideoFrame) {
			//fprintf(stderr, "Could not allocate video frame\n");
			return false;
		}

		/* Allocate the encoded raw picture. */

		ret = avpicture_alloc(&m_context->dstPicture, c->pix_fmt, c->width, c->height);
		if (ret < 0)
		{
			// fprintf(stderr, "Could not allocate picture: %s\n", av_err2str(ret));
			return false;
		}

		/* copy data and linesize picture pointers to frame */
		*((AVPicture *)m_context->pVideoFrame) = m_context->dstPicture;
		return true;
	}

	bool CGEVideoEncoderMP4::_openAudio()
	{
		int ret;
		AVCodecContext *c = m_context->pAudioStream->codec;
		AVFormatContext *oc = m_context->pFormatCtx;
		AVCodec *codec = m_context->pAudioCodec;

		AVDictionary *opts = NULL;

		av_dict_set(&opts, "strict", "experimental", 0);

		/* open it */
		ret = avcodec_open2(c, codec, &opts);
		av_dict_free(&opts);

		if (ret < 0)
		{
			CGE_LOG_ERROR("Could not open audio codec: %s\n", av_err2str(ret));
			return false;
		}

		m_context->maxDstNbSamples = c->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE ?
			10000 : c->frame_size;

		/* create resampler context */
		if (c->sample_fmt != AV_SAMPLE_FMT_S16)
		{
			auto swr_ctx = swr_alloc();
			m_context->pSwrCtx = swr_ctx;
			if (!m_context->pSwrCtx)
			{
				CGE_LOG_ERROR("Could not allocate resampler context\n");
				return false;
			}

			/* set options */
			av_opt_set_int       (swr_ctx, "in_channel_count",   c->channels,       0);
			av_opt_set_int       (swr_ctx, "in_sample_rate",     c->sample_rate,    0);
			av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
			av_opt_set_int       (swr_ctx, "out_channel_count",  c->channels,       0);
			av_opt_set_int       (swr_ctx, "out_sample_rate",    c->sample_rate,    0);
			av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt",     c->sample_fmt,     0);

			/* initialize the resampling context */
			if ((ret = swr_init(swr_ctx)) < 0)
			{
				CGE_LOG_ERROR("Failed to initialize the resampling context\n");
				return false;
			}

			/* compute the number of converted samples: buffering is avoided
			* ensuring that the output buffer will contain at least all the
			* converted input samples */
			ret = av_samples_alloc_array_and_samples(&m_context->dstSampleData, &m_context->dstSamplesLinesize, c->channels, m_context->maxDstNbSamples, c->sample_fmt, 0);
			if (ret < 0)
			{
				CGE_LOG_ERROR("Could not allocate destination samples\n");
				return false;
			}

			m_context->dstSamplesSize = av_samples_get_buffer_size(NULL, c->channels, m_context->maxDstNbSamples, c->sample_fmt, 0);
		}

		return true;
	}

	bool CGEVideoEncoderMP4::record(const ImageData& data)
	{
		AVPixelFormat pixFmt = (AVPixelFormat)m_recordDataFmt;
		AVCodecContext *codecCtx = m_context->pVideoStream->codec;

		// auto tm = getCurrentTimeMillis(), tm2 = tm;

		if(data.data[0] != nullptr)
		{
			if(pixFmt != codecCtx->pix_fmt || data.width != codecCtx->width || data.height != codecCtx->height)
			{
				if(!m_context->pSwsCtx)
				{
					m_context->pSwsCtx = sws_getContext(data.width, data.height, pixFmt, 
						codecCtx->width, codecCtx->height, codecCtx->pix_fmt, SWS_POINT, nullptr, nullptr, nullptr);
					if(!m_context->pSwsCtx)
					{
						CGE_LOG_ERROR("create sws context failed!");
						return false;
					}
					CGE_LOG_INFO("create sws context success!");
				}

				sws_scale(m_context->pSwsCtx, data.data, data.linesize, 0, codecCtx->height, m_context->dstPicture.data, m_context->dstPicture.linesize);
			}
			else
			{
				const int sz1 = sizeof(m_context->dstPicture.data) < sizeof(data.data) ? sizeof(m_context->dstPicture.data) : sizeof(data.data);
				const int sz2 = sizeof(m_context->dstPicture.linesize) < sizeof(data.linesize) ? sizeof(m_context->dstPicture.linesize) : sizeof(data.linesize);

				memcpy(m_context->dstPicture.data, data.data, sz1);
				memcpy(m_context->dstPicture.linesize, data.linesize, sz2);
			}

			m_context->pVideoFrame->pts = data.pts;
		}
		else
		{
			//++m_context->pVideoFrame->pts;
		}

		// tm2 = getCurrentTimeMillis();
		// CGE_LOG_ERROR("转换格式花费时间: %g", (tm2 - tm));
		// tm = tm2;

		if(m_context->pFormatCtx->oformat->flags & AVFMT_RAWPICTURE)
		{
			AVPacket& pkt = m_context->videoPacket;
			av_init_packet(&pkt);

			pkt.flags |= AV_PKT_FLAG_KEY;
			pkt.stream_index = m_context->pVideoStream->index;
			pkt.data = m_context->dstPicture.data[0];
			pkt.size = sizeof(AVPicture);

			m_mutex.lock();
			auto ret =  av_interleaved_write_frame(m_context->pFormatCtx, &pkt);
			m_mutex.unlock();

			if(0 > ret)
			{
				CGE_LOG_ERROR("av_interleaved_write_frame error1...");
				return false;
			}
			av_free_packet(&pkt);
		}
		else
		{
			AVPacket& pkt = m_context->videoPacket;
			int gotPacket;
			av_init_packet(&pkt);
			pkt.data = m_videoPacketBuffer;
			pkt.size = m_videoPacketBufferSize;

			//avcodec_encode_video2 为慢速操作
			if(0 > avcodec_encode_video2(codecCtx, &pkt, data.data[0] == nullptr ? nullptr : m_context->pVideoFrame, &gotPacket))
			{
				CGE_LOG_ERROR("avcodec_encode_video2 error...\n");
				return false;
			}

			// tm2 = getCurrentTimeMillis();
			// CGE_LOG_ERROR("encode 花费时间: %g", (tm2 - tm));
			// tm = tm2;

			if(gotPacket && pkt.size)
			{
				// CGE_LOG_ERROR("PTS %d, %d\n", data.pts, pkt.pts);

				if(pkt.pts != AV_NOPTS_VALUE)
				{
					pkt.pts = av_rescale_q(pkt.pts, codecCtx->time_base, m_context->pVideoStream->time_base);
				}
				if(pkt.dts != AV_NOPTS_VALUE)
				{
					pkt.dts = av_rescale_q(pkt.dts, codecCtx->time_base, m_context->pVideoStream->time_base);
				}

				pkt.stream_index = m_context->pVideoStream->index;

				m_mutex.lock();
				auto ret = av_interleaved_write_frame(m_context->pFormatCtx, &pkt);
				m_mutex.unlock();

				// av_free_packet(&pkt);

				// tm2 = getCurrentTimeMillis();
				// CGE_LOG_ERROR("av_interleaved_write_frame 花费时间: %g", (tm2 - tm));
				// tm = tm2;	

				if(0 > ret)
				{
					CGE_LOG_ERROR("av_interleaved_write_frame error2... 0x%x\n", ret);
					return false;
				}

			}
			else if(data.data[0] == nullptr)
			{
				return false;
			}
		}

		return m_context->pVideoFrame->key_frame != 0;
	}

	//将缓存音频数据， 一次只能存储1024整数倍
	bool CGEVideoEncoderMP4::record(const AudioSampleData& data)
	{
		assert(m_hasAudio);

		if(data.data[0] != nullptr)
		{
			assert(m_context->pSwrCtx != nullptr); // 必然resample

			AVCodecContext* audioCodec = m_context->pAudioStream->codec;

			if(m_context->pAudioFrame == nullptr)
			{
				m_context->pAudioFrame = avcodec_alloc_frame();
			}

			AVFrame* pAudioFrame = m_context->pAudioFrame;
			int srcNbSamples = data.nbSamples[0];

			while(1)
			{
				int dstNbSamples = m_context->maxDstNbSamples - m_context->dstSampleDataIndex;

				// 4 stands for the output channels.
				auto convertData = m_context->dstSampleData[0] + 4 * m_context->dstSampleDataIndex;

				int ret = swr_convert(m_context->pSwrCtx, &convertData, dstNbSamples, (const uint8_t**)data.data, srcNbSamples);

				if(ret == 0)
				{
					// CGE_LOG_ERROR("ret == 0");
					break;
				}
				else if(ret < 0)
				{
					CGE_LOG_ERROR("Error while converting...\n");
					return false;
				}

				m_context->dstSampleDataIndex += ret;
				srcNbSamples = 0;

				if(m_context->dstSampleDataIndex >= m_context->maxDstNbSamples)
				{
					m_context->dstSampleDataIndex = 0;
					// CGE_LOG_ERROR("Recording...");
					pAudioFrame->nb_samples = m_context->maxDstNbSamples;
					pAudioFrame->quality = audioCodec->global_quality;
					avcodec_fill_audio_frame(pAudioFrame, audioCodec->channels, audioCodec->sample_fmt, m_context->dstSampleData[0], m_context->dstSamplesSize, 0);
					pAudioFrame->data[0] = m_context->dstSampleData[0];
					pAudioFrame->linesize[0] = m_context->dstSamplesSize;
					recordAudioFrame(pAudioFrame);
				}
			}

			return pAudioFrame->key_frame != 0;
		}

		return recordAudioFrame(nullptr);
	}

	bool CGEVideoEncoderMP4::recordAudioFrame(AVFrame* pAudioFrame)
	{
		assert(m_hasAudio);

		AVPacket pkt = { 0 };
		av_init_packet(&pkt);
		pkt.data = m_audioPacketBuffer;
		pkt.size = m_audioPacketBufferSize;

		int gotPacket;

		int ret = avcodec_encode_audio2(m_context->pAudioStream->codec, &pkt, pAudioFrame, &gotPacket);

		if(ret < 0)
		{
			CGE_LOG_ERROR("Error encoding audio frame: %s\n", av_err2str(ret));
			return false;
		}

		if(!gotPacket || pkt.size == 0)
			return false;

		pkt.stream_index = m_context->pAudioStream->index;

		m_mutex.lock();
		ret = av_interleaved_write_frame(m_context->pFormatCtx, &pkt);
		m_mutex.unlock();

		// av_free_packet(&pkt);

		if(ret != 0)
		{
			CGE_LOG_ERROR("Error while writing audio frame: %s\n", av_err2str(ret));
			return false;
		}

		return true;
	}

	bool CGEVideoEncoderMP4::recordVideoFrame(AVFrame* pVideoFrame)
	{
		AVCodecContext *codecCtx = m_context->pVideoStream->codec;

		AVPacket& pkt = m_context->videoPacket;
		int gotPacket;
		av_init_packet(&pkt);
		pkt.data = m_videoPacketBuffer;
		pkt.size = m_videoPacketBufferSize;

		//avcodec_encode_video2 为慢速操作
		if(0 > avcodec_encode_video2(codecCtx, &pkt, pVideoFrame, &gotPacket))
		{
			CGE_LOG_ERROR("avcodec_encode_video2 error...\n");
			return false;
		}

		// tm2 = getCurrentTimeMillis();
		// CGE_LOG_ERROR("encode 花费时间: %g", (tm2 - tm));
		// tm = tm2;

		if(gotPacket && pkt.size)
		{
			// CGE_LOG_ERROR("PTS %d, %d\n", data.pts, pkt.pts);

			if(pkt.pts != AV_NOPTS_VALUE)
			{
				pkt.pts = av_rescale_q(pkt.pts, codecCtx->time_base, m_context->pVideoStream->time_base);
			}
			if(pkt.dts != AV_NOPTS_VALUE)
			{
				pkt.dts = av_rescale_q(pkt.dts, codecCtx->time_base, m_context->pVideoStream->time_base);
			}

			pkt.stream_index = m_context->pVideoStream->index;

			m_mutex.lock();
			auto ret = av_interleaved_write_frame(m_context->pFormatCtx, &pkt);
			m_mutex.unlock();

			// av_free_packet(&pkt);

			// tm2 = getCurrentTimeMillis();
			// CGE_LOG_ERROR("av_interleaved_write_frame 花费时间: %g", (tm2 - tm));
			// tm = tm2;	

			if(0 > ret)
			{
				CGE_LOG_ERROR("av_interleaved_write_frame error2... 0x%x\n", ret);
				return false;
			}
		}

		return true;;
	}

	bool CGEVideoEncoderMP4::save()
	{
		{
			ImageData videoData = {0};
			while(record(videoData));
		}
		
		if(m_hasAudio)
		{
			AudioSampleData audioData = {0};
			while(record(audioData));
		}

		if(0 != av_write_trailer(m_context->pFormatCtx))
			return false;
		m_context->cleanup();
		return true;
	}

	void CGEVideoEncoderMP4::drop()
	{
		m_context->cleanup();
	}

	int CGEVideoEncoderMP4::_queryDataFormat(RecordDataFormat fmt)
	{
		AVPixelFormat result = AV_PIX_FMT_NONE;
		switch (fmt)
		{
		case FMT_RGBA8888:
			result = AV_PIX_FMT_RGBA;
			break;
		case FMT_RGB565:
			result = AV_PIX_FMT_RGB565;
			break;
		case FMT_BGR24:
			result = AV_PIX_FMT_BGR24;
			break;
		case FMT_GRAY8:
			result = AV_PIX_FMT_GRAY8;
			break;
		case FMT_NV21:
			result = AV_PIX_FMT_NV21;
			break;
		case FMT_YUV420P:
			result = AV_PIX_FMT_YUV420P;
			break;
		default:
			break;
		}
		return result;
	}

	void CGEVideoEncoderMP4::setRecordDataFormat(RecordDataFormat fmt)
	{
		m_recordDataFmt = _queryDataFormat(fmt);
	}

	double CGEVideoEncoderMP4::getVideoStreamTime()
	{
		return (m_context && m_context->pVideoStream) ? m_context->pVideoStream->pts.val * av_q2d(m_context->pVideoStream->time_base) : 0.0;
	}

	double CGEVideoEncoderMP4::getAudioStreamTime()
	{
		return (m_context && m_context->pAudioStream) ? m_context->pAudioStream->pts.val * av_q2d(m_context->pAudioStream->time_base) : 0.0;
	}
}


#endif











