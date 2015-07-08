/*
* cgeImageHandler.cpp
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeImageHandler.h"
#include <assert.h> //xcode cannot find <cassert> sometime

namespace CGE
{
	CGEImageHandlerInterface::CGEImageHandlerInterface() : m_srcTexture(0), m_dstFrameBuffer(0)
	{
		m_dstImageSize.set(0, 0);
		m_bufferTextures[0] = 0;
		m_bufferTextures[1] = 0;
	}

	CGEImageHandlerInterface::~CGEImageHandlerInterface()
	{
		cgeEnableGlobalGLContext();
		glDeleteTextures(1, &m_srcTexture);
		clearImageFBO();
	}

	GLuint CGEImageHandlerInterface::getResultTextureAndClearHandler()
	{
		cgeEnableGlobalGLContext();
		glFinish();
		GLuint texID = m_bufferTextures[0];
		m_bufferTextures[0] = 0;
		clearImageFBO();
		glDeleteTextures(1, &m_srcTexture);
        m_srcTexture = 0;
		return texID;
	}

	size_t CGEImageHandlerInterface::getOutputBufferLen(size_t channel)
	{
		return m_dstImageSize.width * m_dstImageSize.height * channel;
	}

	size_t CGEImageHandlerInterface::getOutputBufferBytesPerRow(size_t channel)
	{
		return m_dstImageSize.width * channel;
	}

    void CGEImageHandlerInterface::copyTextureData(void* data, int w, int h, GLuint texID, GLenum dataFmt, GLenum channelFmt)
    {
        assert(texID != 0); //Invalid Texture ID

        cgeEnableGlobalGLContext();

        glBindFramebuffer(GL_FRAMEBUFFER, m_dstFrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
        glFinish();
		if(channelFmt != GL_RGBA)
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, w, h, channelFmt, dataFmt, data);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
        cgeCheckGLError("CGEImageHandlerInterface::copyTextureData");
    }

	bool CGEImageHandlerInterface::initImageFBO(const void* data, int w, int h, GLenum channelFmt, GLenum dataFmt, int channel)
	{
		clearImageFBO();
		glActiveTexture(CGE_TEXTURE_OUTPUT_IMAGE);

		//Set the swap buffer textures.
		m_bufferTextures[0] = cgeGenTextureWithBuffer(data, w, h, channelFmt, dataFmt, channel);
		m_bufferTextures[1] = cgeGenTextureWithBuffer(NULL, w, h, channelFmt, dataFmt, channel);

		CGE_LOG_INFO("FBO buffer texture id: %d and %d\n", m_bufferTextures[0], m_bufferTextures[1]);

		glGenFramebuffers(1, &m_dstFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_dstFrameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			clearImageFBO();
			CGE_LOG_ERROR("Image Handler initImageFBO failed! %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
			cgeCheckGLError("CGEImageHandlerInterface::initImageFBO");
			return false;
		}
		CGE_LOG_INFO("FBO Framebuffer id: %d\n", m_dstFrameBuffer);
		return true;
	}

	void CGEImageHandlerInterface::clearImageFBO()
	{
		cgeEnableGlobalGLContext();
		glActiveTexture(CGE_TEXTURE_OUTPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(2, m_bufferTextures);
		m_bufferTextures[0] = 0;
		m_bufferTextures[1] = 0;
		glDeleteFramebuffers(1, &m_dstFrameBuffer);
		m_dstFrameBuffer = 0;	
	}

	//////////////////////////////////////////////////////////////////////////

	CGEImageHandler::CGEImageHandler() : m_bRevertEnabled(false)
#ifdef _CGE_USE_ES_API_3_0_
		,m_pixelPackBuffer(0), m_pixelPackBufferSize(0)
#endif
	{}

	CGEImageHandler::~CGEImageHandler()
	{
		clearImageFilters();

#ifdef _CGE_USE_ES_API_3_0_
		clearPixelBuffer();
#endif
		CGE_LOG_INFO("Handler release\n");
	}

	bool CGEImageHandler::initWithRawBufferData(const void* imgData, GLint w, GLint h, CGEBufferFormat format, bool bEnableReversion)
	{
		cgeEnableGlobalGLContext();
		int channel;
		GLenum dataFmt, channelFmt;
		cgeGetDataAndChannelByFormat(format, &dataFmt, &channelFmt, &channel);
		if(channel == 0) return false;
		char *tmpbuffer = cgeGetScaledBufferInSize(imgData, w, h, channel, cgeGetMaxTextureSize(), cgeGetMaxTextureSize());
		const char* bufferdata = (tmpbuffer == NULL) ? (const char*)imgData : tmpbuffer;

		m_dstImageSize.set(w, h);
		CGE_LOG_INFO("Image Handler Init With RawBufferData %d x %d, %d channel\n", w, h, channel);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);

		glDeleteTextures(1, &m_srcTexture); //Delete last texture to avoid reinit error.
		m_bRevertEnabled = bEnableReversion;
		if(m_bRevertEnabled)
		{
			m_srcTexture = cgeGenTextureWithBuffer(bufferdata, w, h, channelFmt, dataFmt, channel);
			CGE_LOG_INFO("Input Image Texture id %d\n", m_srcTexture);
		}
		else m_srcTexture = 0;

		bool status = initImageFBO(bufferdata, w, h, channelFmt, dataFmt, channel);

		delete[] tmpbuffer;
		return status;
	}

	bool CGEImageHandler::updateData(const void* data, int w, int h, CGEBufferFormat format)
	{
		int channel;
		GLenum dataFmt, channelFmt;
		cgeGetDataAndChannelByFormat(format, &dataFmt, &channelFmt, &channel);
		if(!(w == m_dstImageSize.width && h == m_dstImageSize.height && channel == 4))
			return false;

		cgeEnableGlobalGLContext();

		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, m_bufferTextures[0]);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, channelFmt, dataFmt, data);
		return true;
	}

	bool CGEImageHandler::initWithTexture(GLuint textureID, GLint w, GLint h, CGEBufferFormat format, bool bEnableReversion)
	{
		if(textureID == 0 || w < 1 || h < 1)
			return false;

		m_srcTexture = textureID;
		m_dstImageSize.set(w, h);

		GLenum dataFmt, channelFmt;
		GLint channel;
		cgeGetDataAndChannelByFormat(format, &dataFmt, &channelFmt, &channel);

		initImageFBO(NULL, w, h, channelFmt, dataFmt, channel);
		
		m_bRevertEnabled = true;
		revertToKeptResult(false);
		m_bRevertEnabled = bEnableReversion;

		if(!m_bRevertEnabled)
			m_srcTexture = 0;
		return true;
	}

#ifdef _CGE_USE_ES_API_3_0_
	extern bool g_shouldUsePBO;

	bool CGEImageHandler::initPixelBuffer()
	{
		cgeCheckGLError("before CGEImageHandlerInterface::initPixelBuffer");

		bool ret = false;

		if(g_shouldUsePBO)
		{
			glDeleteBuffers(1, &m_pixelPackBuffer);
			glGenBuffers(1, &m_pixelPackBuffer);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pixelPackBuffer);

			GLenum err = glGetError();

			if(err == GL_FALSE && m_pixelPackBuffer != 0 && m_pixelPackBufferSize != 0)
			{
				glBufferData(GL_PIXEL_PACK_BUFFER, m_pixelPackBufferSize, 0, GL_DYNAMIC_READ);
				ret = true;
			}
			else
			{
				g_shouldUsePBO = false;
				glDeleteBuffers(1, &m_pixelPackBuffer);
				m_pixelPackBuffer = 0;
				m_pixelPackBuffer = 0;
				m_pixelPackBufferSize = 0;
				CGE_LOG_ERROR("GL_PIXEL_PACK_BUFFER - failed! Error code %x\n", err);
			}

			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		}

		return ret;
	}

	bool CGEImageHandler::initImageFBO(const void* data, int w, int h, GLenum channelFmt, GLenum dataFmt, int channel)
	{
		m_pixelPackBufferSize = m_dstImageSize.width * m_dstImageSize.height * channel;
		initPixelBuffer();
		return CGEImageHandlerInterface::initImageFBO(data, w, h, channelFmt, dataFmt, channel);
	}

	const void* CGEImageHandler::mapOutputBuffer(CGEBufferFormat fmt)
	{
		if(!g_shouldUsePBO || m_pixelPackBuffer == 0)
			return NULL;

		int channel;
		GLenum channelFmt, dataFmt;
		cgeGetDataAndChannelByFormat(fmt, &dataFmt, &channelFmt, &channel);

		if(m_pixelPackBufferSize != m_dstImageSize.width * m_dstImageSize.height * channel)
		{
			CGE_LOG_ERROR("Invalid format!\n");
			return NULL;
		}

		cgeEnableGlobalGLContext();
		useImageFBO();
		glFinish();

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pixelPackBuffer);
		glReadPixels(0, 0, m_dstImageSize.width, m_dstImageSize.height, channelFmt, dataFmt, 0);
		const void* ret = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, m_pixelPackBufferSize, GL_MAP_READ_BIT);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		return ret;
	}

	void CGEImageHandler::unmapOutputBuffer()
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pixelPackBuffer);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	}

#endif

	bool CGEImageHandler::getOutputBufferData(void* data, CGEBufferFormat fmt)
	{
		int channel;
		GLenum channelFmt, dataFmt;
		cgeGetDataAndChannelByFormat(fmt, &dataFmt, &channelFmt, &channel);
		size_t len = getOutputBufferLen(channel);
		if(NULL == data || len == 0 || channel != 4)
		{
			CGE_LOG_ERROR("%s\n", NULL == data ? "data is NULL" : (channel == 4 ? "Handler is not initialized!" : "Channel must be 4!") );
			return false;
		}
		cgeEnableGlobalGLContext();
		setAsTarget();
		glFinish();
		if(channelFmt != GL_RGBA)
			glPixelStorei(GL_PACK_ALIGNMENT, 1);

#ifdef _CGE_USE_ES_API_3_0_

		cgeCheckGLError("CGEImageHandlerInterface::CGEImageHandlerInterface");

		if(g_shouldUsePBO)
		{
			if(m_pixelPackBuffer == 0 || m_pixelPackBufferSize != m_dstImageSize.width * m_dstImageSize.height * channel)
			{
				m_pixelPackBufferSize = m_dstImageSize.width * m_dstImageSize.height * channel;
				initPixelBuffer();
			}

			if(m_pixelPackBuffer != 0)
			{
				glReadBuffer(GL_COLOR_ATTACHMENT0);
				glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pixelPackBuffer);
				glReadPixels(0, 0, m_dstImageSize.width, m_dstImageSize.height, channelFmt, dataFmt, 0);
				GLubyte* bytes =(GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, m_pixelPackBufferSize, GL_MAP_READ_BIT);

				if(bytes != NULL)
				{
					memcpy(data, bytes, m_pixelPackBufferSize);
				}
				else
				{
					CGE_LOG_ERROR("glMapBufferRange failed! Use normal read pixels instead...\n");
					glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
					glReadPixels(0, 0, m_dstImageSize.width, m_dstImageSize.height, channelFmt, dataFmt, data);
				}
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
				glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			}
		}
		else
		{
			glReadPixels(0, 0, m_dstImageSize.width, m_dstImageSize.height, channelFmt, dataFmt, data);
		}

#else
		glReadPixels(0, 0, m_dstImageSize.width, m_dstImageSize.height, channelFmt, dataFmt, data);
#endif		
		return true;
	}

#ifdef _CGE_USE_ES_API_3_0_

	void CGEImageHandler::clearPixelBuffer()
	{
		glDeleteBuffers(1, &m_pixelPackBuffer);
		m_pixelPackBuffer = 0;
		m_pixelPackBufferSize = 0;
	}

#endif

	void CGEImageHandler::setAsTarget()
	{
		cgeEnableGlobalGLContext();
		useImageFBO();
		glViewport(0, 0, m_dstImageSize.width, m_dstImageSize.height);
		CGE_LOG_CODE(if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
            CGE_LOG_ERROR("CGEImageHandler::setAsTarget failed!\n");
		});
	}

	void CGEImageHandler::useImageFBO()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_dstFrameBuffer);
	}

	size_t CGEImageHandler::getOutputBufferLen(size_t channel)
	{
		if(m_bufferTextures[0] == 0 || m_dstFrameBuffer == 0)
			return 0;
		return m_dstImageSize.width * m_dstImageSize.height * channel;
	}

	size_t CGEImageHandler::getOutputBufferBytesPerRow(size_t channel)
	{
		if(m_bufferTextures[0] == 0 || m_dstFrameBuffer == 0)
			return 0;
		return m_dstImageSize.width * channel;
	}

	void CGEImageHandler::swapBufferFBO()
	{
		useImageFBO();
		std::swap(m_bufferTextures[0], m_bufferTextures[1]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			CGE_LOG_ERROR("Image Handler swapBufferFBO failed!\n");
		}
		else
		{
			CGE_LOG_INFO("Swapping buffer FBO...\n");
		}
	}

	GLuint CGEImageHandler::copyLastResultTexture(void** )
	{
		if(m_bufferTextures[1] == 0 || m_dstFrameBuffer == 0)
			return 0;
		cgeEnableGlobalGLContext();
		useImageFBO();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[1], 0);
		GLuint tex = 0;
		glGenTextures(1, &tex);
		tex = cgeGenTextureWithBuffer(NULL, m_dstImageSize.width, m_dstImageSize.height, GL_RGBA, GL_UNSIGNED_BYTE);
		glActiveTexture(CGE_TEXTURE_OUTPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, tex);
        glFinish();
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_dstImageSize.width, m_dstImageSize.height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
		return tex;
	}

	GLuint CGEImageHandler::copyResultTexture(void** )
	{
		if(m_bufferTextures[0] == 0 || m_dstFrameBuffer == 0)
			return 0;
		cgeEnableGlobalGLContext();

		useImageFBO();
		GLuint tex;
		glActiveTexture(CGE_TEXTURE_OUTPUT_IMAGE);
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
        glFinish();
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, m_dstImageSize.width, m_dstImageSize.height, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		return tex;
	}

	void CGEImageHandler::delCopiedTexture(void** , GLuint texID)
	{
		cgeEnableGlobalGLContext();
		glDeleteTextures(1, &texID);
	}

	void CGEImageHandler::addImageFilter(CGEImageFilterInterfaceAbstract* proc)
	{
		if(proc == NULL)
		{
			CGE_LOG_ERROR("CGEImageHandler: a null filter is sended. Skipping...\n");
			return;
		}
		m_vecFilters.push_back(proc);
	}

	void CGEImageHandler::clearImageFilters(bool bDelMem)
	{	
		if(bDelMem)
		{
			cgeEnableGlobalGLContext();
			for(std::vector<CGEImageFilterInterfaceAbstract*>::iterator iter = m_vecFilters.begin();
				iter != m_vecFilters.end(); ++iter)
			{
				delete *iter;
			}
		}
		m_vecFilters.clear();
	}

	void CGEImageHandler::processingFilters()
	{
		if(m_vecFilters.empty() || m_bufferTextures[0] == 0)
		{
			CGE_LOG_INFO("No filter or image to handle\n");
			return;
		}
		cgeEnableGlobalGLContext();
        glDisable(GL_BLEND);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        CGE_LOG_CODE(int index = 0;);
        CGE_LOG_CODE(clock_t total = clock(););
        for(std::vector<CGEImageFilterInterfaceAbstract*>::iterator iter = m_vecFilters.begin();
            iter < m_vecFilters.end(); ++iter)
        {
            swapBufferFBO();
            CGE_LOG_CODE(clock_t t = clock();)
            CGE_LOG_INFO("####Start Processing step %d...\n", ++index);
            (*iter)->render2Texture(this, m_bufferTextures[1], getPosVertices());
            glFlush();
            CGE_LOG_INFO("####Processing step %d finished. Time: %gs .\n", index, float(clock() - t) / CLOCKS_PER_SEC);
        }
		glFinish();
        CGE_LOG_INFO("####Finished Processing All! Total time: %gs \n", float(clock() - total) / CLOCKS_PER_SEC);
    }

	bool CGEImageHandler::processingWithFilter(GLint index)
	{
		if(index == -1)
			index = (GLint)m_vecFilters.size() - 1;
		return processingWithFilter(getFilterByIndex(index));
	}

	bool CGEImageHandler::processingWithFilter(CGEImageFilterInterfaceAbstract* proc)
	{
		if(proc == NULL)
			return false;

		cgeEnableGlobalGLContext();
		glDisable(GL_BLEND);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		swapBufferFBO();

		CGE_LOG_CODE(clock_t t = clock());
		CGE_LOG_INFO("####Start Processing...");
		proc->render2Texture(this, m_bufferTextures[1], getPosVertices());
		glFlush();
		CGE_LOG_INFO("####Finished Processing! Time: %gs \n", float(clock() - t) / CLOCKS_PER_SEC);
		return true;
	}

	void CGEImageHandler::disableReversion()
	{
		cgeEnableGlobalGLContext();
		glDeleteTextures(1, &m_srcTexture);
		m_srcTexture = 0;
		m_bRevertEnabled = false;
		CGE_LOG_INFO("Reversion isdisabled");
	}

	bool CGEImageHandler::keepCurrentResult()
	{
		if(!m_bRevertEnabled ||	m_bufferTextures[0] == 0 || m_dstFrameBuffer == 0)
			return false;

		cgeEnableGlobalGLContext();
		useImageFBO();
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, m_srcTexture);
        glFinish();
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_dstImageSize.width, m_dstImageSize.height);
		return true;
	}

	// glCopyTexSubImage2D 会block cpu
	bool CGEImageHandler::revertToKeptResult(bool bRevert2Target)
	{
		if(!m_bRevertEnabled ||	m_bufferTextures[0] == 0 || m_dstFrameBuffer == 0)
			return false;

		cgeEnableGlobalGLContext();
		useImageFBO();
		glActiveTexture(CGE_TEXTURE_OUTPUT_IMAGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_srcTexture, 0);
        glFlush();
		if(bRevert2Target)
		{
			glBindTexture(GL_TEXTURE_2D, m_bufferTextures[1]);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_dstImageSize.width, m_dstImageSize.height);
            glFlush();
        }
		glBindTexture(GL_TEXTURE_2D, m_bufferTextures[0]);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_dstImageSize.width, m_dstImageSize.height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
		return true;
	}

	int CGEImageHandler::getFilterIndexByAddr(const void* addr)
	{
		int sz = (int)m_vecFilters.size();
		for(int i = 0; i != sz; ++i)
		{
			if(addr == m_vecFilters[i])
				return i;
		}
		return -1;
	}

	void CGEImageHandler::peekFilters(std::vector<CGEImageFilterInterfaceAbstract*>* vTrans)
	{
		*vTrans = m_vecFilters;
	}

	void CGEImageHandler::popImageFilter()
	{
		if(!m_vecFilters.empty())
		{
			cgeEnableGlobalGLContext();
			std::vector<CGEImageFilterInterfaceAbstract*>::iterator iter = m_vecFilters.end()-1;
			delete *iter;
			m_vecFilters.erase(iter);
		} 
	}

	bool CGEImageHandler::insertFilterAtIndex(CGEImageFilterInterfaceAbstract* proc, GLuint index)
	{
		if(index > m_vecFilters.size()) return false;
		m_vecFilters.insert(m_vecFilters.begin() + index, proc);
		return true;
	}

	bool CGEImageHandler::deleteFilterByAddr(const void* addr, bool bDelMem)
	{
		if(m_vecFilters.empty())
			return false;

		for(std::vector<CGEImageFilterInterfaceAbstract*>::iterator iter = m_vecFilters.begin();
			iter < m_vecFilters.end(); ++iter)
		{
			if(*iter == addr)
			{
				if(bDelMem)
				{
					cgeEnableGlobalGLContext();
					delete *iter;
				}
				m_vecFilters.erase(iter);
				return true;
			}
		}
		return false;
	}

	bool CGEImageHandler::deleteFilterByIndex(GLuint index, bool bDelMem)
	{
		if(index >= m_vecFilters.size())
			return false;
		if(bDelMem)
		{
			cgeEnableGlobalGLContext();
			delete m_vecFilters[index];
		}
		m_vecFilters.erase(m_vecFilters.begin() + index);
		return true;
	}

	bool CGEImageHandler::replaceFilterAtIndex(CGEImageFilterInterfaceAbstract* proc, GLuint index, bool bDelMem)
	{
		if(index >= m_vecFilters.size())
			return false;
		std::vector<CGEImageFilterInterfaceAbstract*>::iterator iter = m_vecFilters.begin() + index;
		if(bDelMem)
		{
			cgeEnableGlobalGLContext();
			delete *iter;
		}
		*iter = proc;
		return true;
	}

	bool CGEImageHandler::swapFilterByIndex(GLuint left, GLuint right)
	{
		if(left == right || left >= m_vecFilters.size() || right >= m_vecFilters.size())
			return false;
		std::swap(*(m_vecFilters.begin() + left), *(m_vecFilters.begin() + right));
		return true;
	}

	CGEVideoHandler::~CGEVideoHandler()
	{
		clearCache();
	}

	void CGEVideoHandler::drawResultToScreen(GLint x, GLint y, GLint w, GLint h)
	{
		glViewport(x, y, w, h);
		if(m_initDrawFunc != NULL)
			m_initDrawFunc();
		else initDrawStatusDefault();

		if(m_renderProgram == NULL && !initRenderProgram())
		{
			return;
		}
		m_renderProgram->bind();
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, getPosVertices());
		glEnableVertexAttribArray(0);
		glActiveTexture(CGE_TEXTURE_INPUT_IMAGE);
		glBindTexture(GL_TEXTURE_2D, getTargetTextureID());
		m_renderProgram->sendUniformi(CGEImageFilterInterface::paramInputImageName, CGE_TEXTURE_INPUT_IMAGE_INDEX);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		cgeCheckGLError("glDrawArrays");
	}

	bool CGEVideoHandler::initRenderProgram()
	{
		m_renderProgram = new ProgramObject;
		m_renderProgram->bindAttribLocation(CGEImageFilterInterface::paramPositionIndexName, 0);
		if(!m_renderProgram->initWithShaderStrings(g_vshDrawToScreen, g_fshDefault))
		{
			CGE_LOG_ERROR("Init Cache Program Failed! Check if the context is valid.");
			CGE_DELETE(m_renderProgram);
			return false;
		}

		return true;
	}
}
