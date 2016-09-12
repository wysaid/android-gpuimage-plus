/*
* cgeVideoPlayer.h
*
*  Created on: 2015-12-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#if !defined(_CGEVIDEOPLAYER_H_) && defined(_CGE_USE_FFMPEG_)
#define _CGEVIDEOPLAYER_H_

#include "cgeGLFunctions.h"
#include "cgeVideoDecoder.h"

namespace CGE
{
	class CGEVideoPlayerInterface
	{
	public:
		CGEVideoPlayerInterface() {}
		virtual ~CGEVideoPlayerInterface() {}

		virtual bool open(const char* filename, CGEVideoDecodeHandler::SamplingStyle s = CGEVideoDecodeHandler::ssFastBilinear) { return false; }
		virtual void close() {}

		virtual void play() {}
		virtual void stop() {}
		virtual void pause() {}
		virtual void resume() {}

		virtual void render() {}

		virtual bool update(double time) { return false;}

		virtual bool updateVideoFrame(CGEVideoFrameBufferData* data = nullptr) { return false; }
        virtual bool nextVideoFrame() { return false; }

		virtual bool seek(float position) { return false; }

	protected:

	};

	class CGEVideoPlayerYUV420P : public CGEVideoPlayerInterface
	{
	public:
		CGEVideoPlayerYUV420P();
		~CGEVideoPlayerYUV420P();

		bool open(const char* filename, CGEVideoDecodeHandler::SamplingStyle s = CGEVideoDecodeHandler::ssFastBilinear);

		//不同的初始化方式，不与 open 合用
		bool initWithDecodeHandler(CGEVideoDecodeHandler*);

		void close();

		void play();
		void stop();
		void pause();
		void resume();

		bool update(double time);

		void render();
		void setRotation(float rad);
		void setFlipScale(float x, float y);


		CGEFrameTypeNext queryNextFrame() { return m_decodeHandler->queryNextFrame(); }

		bool updateVideoFrame(const CGEVideoFrameBufferData* data = nullptr);
        bool nextVideoFrame();

		bool seek(float position);

		GLuint getTextureY() { return m_texYUV[0]; }
		GLuint getTextureU() { return m_texYUV[1]; }
		GLuint getTextureV() { return m_texYUV[2]; }

		CGEVideoDecodeHandler* getDecodeHandler() { return m_decodeHandler; }

		int getLinesize() { return m_linesize[0]; }
	protected:
		ProgramObject m_program;
		GLuint m_texYUV[3];
		GLint m_texYLoc, m_texULoc, m_texVLoc;
		GLuint m_posAttribLocation;
		GLuint m_rotLoc, m_flipScaleLoc;
		CGEVideoDecodeHandler* m_decodeHandler;

		GLuint m_vertexBuffer;
		int m_videoWidth, m_videoHeight;
		int m_linesize[3];
	};
}

#endif