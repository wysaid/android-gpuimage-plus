/*
* cgeVideoPlayer.cpp
*
*  Created on: 2015-12-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifdef _CGE_USE_FFMPEG_

#include "cgeVideoPlayer.h"
#include "cgeGlobal.h"
#include <cmath>

static const char* const s_vshYUV420p = CGE_SHADER_STRING
(
attribute vec4 vPosition;
varying vec2 vTexCoord;
uniform mat2 rotation;
uniform vec2 flipScale;

void main()
{
	gl_Position = vPosition;
	// vTexCoord = vec2(1.0 + vPosition.x, 1.0 - vPosition.y) / 2.0;
	vTexCoord = flipScale * (vPosition.xy / 2.0 * rotation) + 0.5;
}
);

static CGEConstString s_fshYUV420p = CGE_SHADER_STRING_PRECISION_M
(
varying vec2 vTexCoord;

uniform sampler2D textureY;
uniform sampler2D textureU;
uniform sampler2D textureV;
uniform mat3 m3ColorConversion;

void main()
{
	vec3 yuv;
	yuv.x = texture2D(textureY, vTexCoord).r; //fix: use full range
	yuv.y = texture2D(textureU, vTexCoord).r - 0.5;
	yuv.z = texture2D(textureV, vTexCoord).r - 0.5;

	vec3 videoColor = m3ColorConversion * yuv;

	gl_FragColor = vec4(videoColor, 1.0);
}
);

//The two matrix is from: https://developer.apple.com/library/content/samplecode/GLCameraRipple/Listings/GLCameraRipple_Shaders_Shader_fsh.html#//apple_ref/doc/uid/DTS40011222-GLCameraRipple_Shaders_Shader_fsh-DontLinkElementID_9

// BT.601, which is the standard for SDTV.
static const GLfloat s_colorConversion601[] = {
    1, 1, 1,
    0, -0.34413, 1.772,
    1.402, -.71414, 0
};

// BT.709, which is the standard for HDTV.
static const GLfloat s_colorConversion709[] = {
	1, 1, 1,
	0, -0.18732, 1.8556,
	1.57481, -0.46813, 0.0,
};

namespace CGE
{
	CGEVideoPlayerYUV420P::CGEVideoPlayerYUV420P() : m_posAttribLocation(0), m_decodeHandler(nullptr), m_vertexBuffer(0)
	{
		m_program.bindAttribLocation(CGEImageFilterInterface::paramPositionIndexName, m_posAttribLocation);

		if(!m_program.initWithShaderStrings(s_vshYUV420p, s_fshYUV420p))
		{
			CGE_LOG_ERROR("cgeVideoPlayerYUV420P program init failed!");
			return;
		}

		m_program.bind();
		m_program.sendUniformMat3("m3ColorConversion", 1, GL_FALSE, s_colorConversion601);

		//textureY

		m_texYLoc = m_program.uniformLocation("textureY");
		m_texULoc = m_program.uniformLocation("textureU");
		m_texVLoc = m_program.uniformLocation("textureV");

		glUniform1i(m_texYLoc, 1);
		glUniform1i(m_texULoc, 2);
		glUniform1i(m_texVLoc, 3);

		if(m_texYLoc < 0 || m_texULoc < 0 || m_texVLoc < 0)
		{
			CGE_LOG_ERROR("Invalid YUV Texture Uniforms\n");
		}

		memset(m_texYUV, 0, sizeof(m_texYUV));

		m_rotLoc = m_program.uniformLocation("rotation");
		m_flipScaleLoc = m_program.uniformLocation("flipScale");

		setRotation(0.0f);
		setFlipScale(1.0f, 1.0f);
		cgeCheckGLError("cgeVideoPlayerYUV420P");
	}

	CGEVideoPlayerYUV420P::~CGEVideoPlayerYUV420P()
	{
		close();
	}

	bool CGEVideoPlayerYUV420P::open(const char* filename, CGEVideoDecodeHandler::SamplingStyle s)
	{
		if(m_decodeHandler != nullptr)
			close();

		m_decodeHandler = new CGEVideoDecodeHandler();
		if(!m_decodeHandler->open(filename))
		{
			CGE_LOG_ERROR("Open %s failed!\n", filename);
			return false;
		}

		m_decodeHandler->setSamplingStyle(s);

		return initWithDecodeHandler(m_decodeHandler);
	}

	bool CGEVideoPlayerYUV420P::initWithDecodeHandler(CGEVideoDecodeHandler* handler)
	{
		assert(handler != nullptr); //handler == nullptr

		if(m_decodeHandler != handler && m_decodeHandler != nullptr)
			delete m_decodeHandler;

		m_decodeHandler = handler;

		m_linesize[0] = m_videoWidth = m_decodeHandler->getWidth();
		m_linesize[2] = m_linesize[1] = m_linesize[0] / 2;
		m_videoHeight = m_decodeHandler->getHeight();

		m_texYUV[0] = cgeGenTextureWithBuffer(nullptr, m_linesize[0], m_videoHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, 1, 1);

		m_texYUV[1] = cgeGenTextureWithBuffer(nullptr, m_linesize[1], m_videoHeight / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, 1, 2);

		m_texYUV[2] = cgeGenTextureWithBuffer(nullptr, m_linesize[2], m_videoHeight / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, 1, 3);

		if(m_vertexBuffer == 0)
			m_vertexBuffer = cgeGenCommonQuadArrayBuffer();

		CGE_LOG_INFO("CGEVideoPlayerYUV420P vertex buffer id: %d", m_vertexBuffer);
		return m_vertexBuffer != 0;
	}

	void CGEVideoPlayerYUV420P::close()
	{
		glDeleteTextures(3, m_texYUV);
		memset(m_texYUV, 0, sizeof(m_texYUV));
		delete m_decodeHandler;
		m_decodeHandler = nullptr;

		glDeleteBuffers(1, &m_vertexBuffer);
		m_vertexBuffer = 0;
	}

	void CGEVideoPlayerYUV420P::play()
	{

	}

	void CGEVideoPlayerYUV420P::stop()
	{

	}

	void CGEVideoPlayerYUV420P::pause()
	{

	}

	void CGEVideoPlayerYUV420P::resume()
	{

	}

	bool CGEVideoPlayerYUV420P::seek(float position)
	{
		return true;
	}

	void CGEVideoPlayerYUV420P::render()
	{
		CGE_LOG_CODE(
		if(m_texYUV[0] == 0)
		{
			CGE_LOG_ERROR("You should get a frame first!\n");
			return ;
		}
		);

		m_program.bind();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_texYUV[0]);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_texYUV[1]);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_texYUV[2]);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glEnableVertexAttribArray(m_posAttribLocation);
		glVertexAttribPointer(m_posAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void CGEVideoPlayerYUV420P::setRotation(float rad)
	{
		float cosRad = cosf(rad);
		float sinRad = sinf(rad);
		float mat2[] = {
			cosRad, sinRad,
			-sinRad, cosRad
		};

		m_program.bind();
		glUniformMatrix2fv(m_rotLoc, 1, GL_FALSE, mat2);
	}

	void CGEVideoPlayerYUV420P::setFlipScale(float x, float y)
	{
		m_program.bind();
		glUniform2f(m_flipScaleLoc, x, y);
	}

	bool CGEVideoPlayerYUV420P::updateVideoFrame(const CGEVideoFrameBufferData* data)
	{
		const CGEVideoFrameBufferData* pFramebuffer = data == nullptr ? m_decodeHandler->getCurrentVideoFrame() : data;

		if(pFramebuffer == nullptr)
		{
			return false;
		}

		const CGEVideoFrameBufferData& framebuffer = *pFramebuffer;

		m_program.bind();

		if(m_linesize[0] != framebuffer.linesize[0])
		{
			m_linesize[0] = framebuffer.linesize[0];
			m_linesize[1] = framebuffer.linesize[1];
			m_linesize[2] = framebuffer.linesize[2];

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_texYUV[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_linesize[0], m_videoHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, framebuffer.data[0]);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_texYUV[1]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_linesize[1], m_videoHeight / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, framebuffer.data[1]);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, m_texYUV[2]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_linesize[2], m_videoHeight / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, framebuffer.data[2]);
		}
		else
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_texYUV[0]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_linesize[0], m_videoHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, framebuffer.data[0]);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_texYUV[1]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_linesize[1], m_videoHeight / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, framebuffer.data[1]);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, m_texYUV[2]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_linesize[2], m_videoHeight / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, framebuffer.data[2]);
		}

		cgeCheckGLError("cgeVideoPlayerYUV420P::updateVideoFrame");
		return true;
	}

    bool CGEVideoPlayerYUV420P::nextVideoFrame()
    {
        const CGEVideoFrameBufferData* data = m_decodeHandler->getNextVideoFrame();
        if(data == nullptr)
            return false;

        return CGEVideoPlayerYUV420P::updateVideoFrame(data);
    }

	bool CGEVideoPlayerYUV420P::update(double time)
	{
		double ts = m_decodeHandler->getCurrentTimestamp();
		if(time < ts)
			return true;
// 		else if(time > m_decodeHandler->getTotalTime())
// 			return false;
		return nextVideoFrame();
	}

}

#endif