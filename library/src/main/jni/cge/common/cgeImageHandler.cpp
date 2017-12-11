/*
* cgeImageHandler.cpp
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeImageHandler.h"
#include "cgeTextureUtils.h"

#include <cassert>

CGE_UNEXPECTED_ERR_MSG
(
 static int sHandlerCount = 0;
 )

namespace CGE
{
	CGEImageHandlerInterface::CGEImageHandlerInterface() : m_srcTexture(0), m_dstFrameBuffer(0), m_vertexArrayBuffer(0)
	{
		m_dstImageSize.set(0, 0);
		m_bufferTextures[0] = 0;
		m_bufferTextures[1] = 0;
        
        CGE_UNEXPECTED_ERR_MSG
        (
         CGE_LOG_KEEP("Handler create, total: %d\n", ++sHandlerCount);
         )
	}

	CGEImageHandlerInterface::~CGEImageHandlerInterface()
	{
		CGE_ENABLE_GLOBAL_GLCONTEXT();
		glDeleteTextures(1, &m_srcTexture);
		clearImageFBO();
		glDeleteBuffers(1, &m_vertexArrayBuffer);
        m_vertexArrayBuffer = 0;
        
        CGE_UNEXPECTED_ERR_MSG
        (
         CGE_LOG_KEEP("Handler release, remain: %d\n", --sHandlerCount);
         )
	}

	GLuint CGEImageHandlerInterface::getResultTextureAndClearHandler()
	{
		CGE_ENABLE_GLOBAL_GLCONTEXT();
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

        CGE_ENABLE_GLOBAL_GLCONTEXT();

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

		if(m_vertexArrayBuffer == 0)
			m_vertexArrayBuffer = cgeGenCommonQuadArrayBuffer();
		CGE_LOG_INFO("Vertex Array Buffer id: %d\n", m_vertexArrayBuffer);

		//Set the swap buffer textures.
		m_bufferTextures[0] = cgeGenTextureWithBuffer(data, w, h, channelFmt, dataFmt, channel);
		m_bufferTextures[1] = cgeGenTextureWithBuffer(nullptr, w, h, channelFmt, dataFmt, channel);

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
		CGE_ENABLE_GLOBAL_GLCONTEXT();
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(2, m_bufferTextures);
		m_bufferTextures[0] = 0;
		m_bufferTextures[1] = 0;
		glDeleteFramebuffers(1, &m_dstFrameBuffer);
		m_dstFrameBuffer = 0;	
	}

	//////////////////////////////////////////////////////////////////////////

	CGEImageHandler::CGEImageHandler() : m_bRevertEnabled(false), m_drawer(nullptr), m_resultDrawer(nullptr)
#ifdef _CGE_USE_ES_API_3_0_
		,m_pixelPackBuffer(0), m_pixelPackBufferSize(0)
#endif
	{}

	CGEImageHandler::~CGEImageHandler()
	{
		CGE_ENABLE_GLOBAL_GLCONTEXT();
		clearImageFilters();
		delete m_drawer;
		delete m_resultDrawer;

#ifdef _CGE_USE_ES_API_3_0_
		clearPixelBuffer();
#endif

	}

	bool CGEImageHandler::initWithRawBufferData(const void* imgData, GLint w, GLint h, CGEBufferFormat format, bool bEnableReversion)
	{
		CGE_ENABLE_GLOBAL_GLCONTEXT();
		int channel;
		GLenum dataFmt, channelFmt;
		cgeGetDataAndChannelByFormat(format, &dataFmt, &channelFmt, &channel);
		if(channel == 0) return false;
		char *tmpbuffer = cgeGetScaledBufferInSize(imgData, w, h, channel, cgeGetMaxTextureSize(), cgeGetMaxTextureSize());
		const char* bufferdata = (tmpbuffer == nullptr) ? (const char*)imgData : tmpbuffer;

		m_dstImageSize.set(w, h);
		CGE_LOG_INFO("Image Handler Init With RawBufferData %d x %d, %d channel\n", w, h, channel);

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

		cgeCheckGLError("CGEImageHandler::initWithRawBufferData");
		return status;
	}

	bool CGEImageHandler::updateData(const void* data, int w, int h, CGEBufferFormat format)
	{
		int channel;
		GLenum dataFmt, channelFmt;
		cgeGetDataAndChannelByFormat(format, &dataFmt, &channelFmt, &channel);
		if(!(w == m_dstImageSize.width && h == m_dstImageSize.height && channel == 4))
			return false;

		CGE_ENABLE_GLOBAL_GLCONTEXT();

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

		initImageFBO(nullptr, w, h, channelFmt, dataFmt, channel);
		
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
			return nullptr;

		int channel;
		GLenum channelFmt, dataFmt;
		cgeGetDataAndChannelByFormat(fmt, &dataFmt, &channelFmt, &channel);

		if(m_pixelPackBufferSize != m_dstImageSize.width * m_dstImageSize.height * channel)
		{
			CGE_LOG_ERROR("Invalid format!\n");
			return nullptr;
		}

		CGE_ENABLE_GLOBAL_GLCONTEXT();
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
		if(nullptr == data || len == 0 || channel != 4)
		{
			CGE_LOG_ERROR("%s\n", nullptr == data ? "data is NULL" : (channel == 4 ? "Handler is not initialized!" : "Channel must be 4!") );
			return false;
		}
		CGE_ENABLE_GLOBAL_GLCONTEXT();
		setAsTarget();
		glFinish();
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

				if(bytes != nullptr)
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
		CGE_ENABLE_GLOBAL_GLCONTEXT();
        glBindFramebuffer(GL_FRAMEBUFFER, m_dstFrameBuffer);
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
        
        CGE_LOG_CODE
        (
         if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
         {
             CGE_LOG_ERROR("Image Handler swapBufferFBO failed!\n");
         }
         else
         {
             CGE_LOG_INFO("Swapping buffer FBO...\n");
         }
        )
	}

	bool CGEImageHandler::copyTexture(GLuint dst, GLuint src)
	{
		if(m_drawer == nullptr)
		{
			m_drawer = TextureDrawer::create();
			if(m_drawer == nullptr)
			{
				CGE_LOG_ERROR("Texture Drawer create failed!");
				return false;
			}
		}

		GLboolean hasBlending = glIsEnabled(GL_BLEND);
		GLboolean hasDepth = glIsEnabled(GL_DEPTH_TEST);

		if(hasBlending)
			glDisable(GL_BLEND);
		if(hasDepth)
			glDisable(GL_DEPTH_TEST);

		useImageFBO();
		glFlush();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst, 0);
		glViewport(0, 0, m_dstImageSize.width, m_dstImageSize.height);
		glClear(GL_COLOR_BUFFER_BIT);
		m_drawer->drawTexture(src);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
		
		if(hasBlending)
			glEnable(GL_BLEND);
		if(hasDepth)
			glEnable(GL_DEPTH_TEST);

		return true;
	}

	bool CGEImageHandler::copyTexture(GLuint dst, GLuint src, int x, int y, int w, int h)
	{
		return copyTexture(dst, src, 0, 0, x, y, w, h);
	}

	bool CGEImageHandler::copyTexture(GLuint dst, GLuint src, int xOffset, int yOffset, int x, int y, int w, int h)
	{
		assert(dst != 0 && src != 0);
		useImageFBO();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src, 0);
		glBindTexture(GL_TEXTURE_2D, dst);
		glFinish();
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, x, y, w, h);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
		return true; // Always success.
	}

	void CGEImageHandler::drawResult()
	{
		if(m_resultDrawer == nullptr)
		{
			m_resultDrawer = TextureDrawer::create();
			if(m_resultDrawer == nullptr)
			{
				CGE_LOG_ERROR("Create Texture Drawer Failed!\n");
				return ;
			}
		}
		m_resultDrawer->drawTexture(m_bufferTextures[0]);
	}

	TextureDrawer* CGEImageHandler::getResultDrawer()
	{
		if(m_resultDrawer == nullptr)
			m_resultDrawer = TextureDrawer::create();
		return m_resultDrawer;
	}

	void CGEImageHandler::setResultDrawer(TextureDrawer* drawer)
	{
		if(m_resultDrawer != nullptr)
			delete m_resultDrawer;
		m_resultDrawer = drawer;
	}

	GLuint CGEImageHandler::copyLastResultTexture(GLuint texID)
	{
		if(m_bufferTextures[1] == 0 || m_dstFrameBuffer == 0)
			return texID;
		CGE_ENABLE_GLOBAL_GLCONTEXT();

		if(texID == 0)
			texID = cgeGenTextureWithBuffer(nullptr, m_dstImageSize.width, m_dstImageSize.height, GL_RGBA, GL_UNSIGNED_BYTE);

		if(!copyTexture(texID, m_bufferTextures[1]))
		{
			useImageFBO();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[1], 0);
			glBindTexture(GL_TEXTURE_2D, texID);
			glFinish();
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_dstImageSize.width, m_dstImageSize.height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
		}

		return texID;
	}

	GLuint CGEImageHandler::copyResultTexture(GLuint texID)
	{
		if(m_bufferTextures[1] == 0 || m_dstFrameBuffer == 0)
			return texID;
		CGE_ENABLE_GLOBAL_GLCONTEXT();

		if(texID == 0)
			texID = cgeGenTextureWithBuffer(nullptr, m_dstImageSize.width, m_dstImageSize.height, GL_RGBA, GL_UNSIGNED_BYTE);

		if(!copyTexture(texID, m_bufferTextures[0]))
		{
			useImageFBO();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
			glBindTexture(GL_TEXTURE_2D, texID);
			glFinish();
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_dstImageSize.width, m_dstImageSize.height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
		}

		return texID;
	}

	void CGEImageHandler::addImageFilter(CGEImageFilterInterfaceAbstract* proc)
	{
		if(proc == nullptr)
		{
			CGE_LOG_ERROR("CGEImageHandler: a null filter is sent. Skipping...\n");
			return;
		}
		if(!proc->isWrapper())
		{
			m_vecFilters.push_back(proc);
			return;
		}
		
		auto&& filters = proc->getFilters(true);
		for(auto filter : filters)
		{
			m_vecFilters.push_back(filter);
		}
		delete proc;
	}

	void CGEImageHandler::clearImageFilters(bool bDelMem)
	{	
		if(bDelMem)
		{
			CGE_ENABLE_GLOBAL_GLCONTEXT();
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

		CGE_ENABLE_GLOBAL_GLCONTEXT();
		assert(m_vertexArrayBuffer != 0);

        glDisable(GL_BLEND);

        CGE_LOG_CODE(int index = 0;);
        CGE_LOG_CODE(clock_t total = clock(););
        for(std::vector<CGEImageFilterInterfaceAbstract*>::iterator iter = m_vecFilters.begin();
            iter < m_vecFilters.end(); ++iter)
        {
            swapBufferFBO();
            CGE_LOG_CODE(clock_t t = clock();)
            CGE_LOG_INFO("####Start Processing step %d...\n", ++index);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffer);
            (*iter)->render2Texture(this, m_bufferTextures[1], m_vertexArrayBuffer);
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
		if(proc == nullptr)
			return false;

		CGE_ENABLE_GLOBAL_GLCONTEXT();
		assert(m_vertexArrayBuffer != 0);

		glDisable(GL_BLEND);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffer);
		swapBufferFBO();

//		CGE_LOG_CODE(clock_t t = clock());
//		CGE_LOG_INFO("####Start Processing...");
		proc->render2Texture(this, m_bufferTextures[1], m_vertexArrayBuffer);
		glFlush();
//		CGE_LOG_INFO("####Finished Processing! Time: %gs \n", float(clock() - t) / CLOCKS_PER_SEC);
		return true;
	}

	void CGEImageHandler::disableReversion()
	{
		CGE_ENABLE_GLOBAL_GLCONTEXT();
		glDeleteTextures(1, &m_srcTexture);
		m_srcTexture = 0;
		m_bRevertEnabled = false;
		CGE_LOG_INFO("Reversion isdisabled");
	}

	bool CGEImageHandler::keepCurrentResult()
	{
		if(!m_bRevertEnabled ||	m_bufferTextures[0] == 0 || m_dstFrameBuffer == 0)
			return false;

		CGE_ENABLE_GLOBAL_GLCONTEXT();
		useImageFBO();

		glBindTexture(GL_TEXTURE_2D, m_srcTexture);
        glFinish();
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_dstImageSize.width, m_dstImageSize.height);
		return true;
	}

	bool CGEImageHandler::revertToKeptResult(bool bRevert2Target)
	{
		if(!m_bRevertEnabled ||	m_bufferTextures[0] == 0 || m_dstFrameBuffer == 0)
			return false;

		CGE_ENABLE_GLOBAL_GLCONTEXT();
		useImageFBO();

		if(m_drawer == nullptr)
		{
			m_drawer = TextureDrawer::create();
		}

		if(m_drawer == nullptr)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_srcTexture, 0);
			if(bRevert2Target)
			{
				glBindTexture(GL_TEXTURE_2D, m_bufferTextures[1]);

				// glCopyTexSubImage2D 会block cpu
				glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_dstImageSize.width, m_dstImageSize.height);
				glFlush();
			}
			glBindTexture(GL_TEXTURE_2D, m_bufferTextures[0]);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_dstImageSize.width, m_dstImageSize.height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
		}
		else
		{
			glViewport(0, 0, m_dstImageSize.width, m_dstImageSize.height);
			if(bRevert2Target)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[1], 0);
				m_drawer->drawTexture(m_srcTexture);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bufferTextures[0], 0);
			m_drawer->drawTexture(m_srcTexture);
		}

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
			CGE_ENABLE_GLOBAL_GLCONTEXT();
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
					CGE_ENABLE_GLOBAL_GLCONTEXT();
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
			CGE_ENABLE_GLOBAL_GLCONTEXT();
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
			CGE_ENABLE_GLOBAL_GLCONTEXT();
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

}
