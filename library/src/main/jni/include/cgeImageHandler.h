/*
* cgeImageHandler.h
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*      Mail: admin@wysaid.org
*/

#ifndef _CGEIMAGEHANDLER_H_
#define _CGEIMAGEHANDLER_H_

#include "cgeGLFunctions.h"
#include "cgeImageFilter.h"
#include "cgeGlobal.h"

namespace CGE
{
	class CGEImageFilterInterfaceAbstract;
	class CGEImageFilterInterface;
	class TextureDrawer;

	class CGEImageHandlerInterface
	{
	public:
		CGEImageHandlerInterface();
		virtual ~CGEImageHandlerInterface();

		inline void setAsTarget()
        {
            CGEAssert(m_dstFrameBuffer[0] != 0);
            glBindFramebuffer(GL_FRAMEBUFFER, m_dstFrameBuffer[0]);
            glViewport(0, 0, m_dstImageSize.width, m_dstImageSize.height);
        }
        
        //This should only be called by the image processing classes. Do check that carefully.
		inline void swapBufferFBO()
        {
            std::swap(m_bufferTextures[0], m_bufferTextures[1]);
            std::swap(m_dstFrameBuffer[0], m_dstFrameBuffer[1]);
        }

        virtual void processingFilters() = 0;

		// The handler will be invalid, you must init it after this call
		virtual GLuint getResultTextureAndClearHandler();
        virtual size_t getOutputBufferLen(size_t channel = 4); //if 0 is returned , it means the handler is not successfully initialized.
        virtual size_t getOutputBufferBytesPerRow(size_t channel = 4);
        
		//The return value would be a new texture if 0 is passed in.
		virtual GLuint copyLastResultTexture(GLuint dstTex = 0) { return 0; }
		virtual GLuint copyResultTexture(GLuint dstTex = 0) { return 0; }

		//You should not modify the textures given below outside the handler.(Read-Only)
		//But once if you need to do that, remember to change their values.
		inline GLuint getSourceTextureID() { return m_srcTexture; }
		inline GLuint getTargetTextureID() { return m_bufferTextures[0]; }
		inline GLuint getBufferTextureID() { return m_bufferTextures[1]; }
		inline const CGESizei& getOutputFBOSize() const { return m_dstImageSize; }
		inline GLuint getOutputFrameBufferID() { return m_dstFrameBuffer[0]; }
        inline GLuint getInputFrameBufferID() { return m_dstFrameBuffer[1]; }

        void copyTextureData(void* data, int w, int h, GLuint texID, GLenum dataFmt, GLenum channelFmt);

	protected:
		virtual bool initImageFBO(const void* data, int w, int h, GLenum channelFmt, GLenum dataFmt, int channel);
		virtual void clearImageFBO();

	protected:
		GLuint m_srcTexture = 0;
		CGESizei m_dstImageSize = {0, 0};
		GLuint m_bufferTextures[2] = {0, 0};
		GLuint m_dstFrameBuffer[2] = {0, 0};
		GLuint m_vertexArrayBuffer = 0;
	};


	class CGEImageHandler : public CGEImageHandlerInterface
	{
	private:
		explicit CGEImageHandler(const CGEImageHandler&) {}
	public:
		CGEImageHandler();
		~CGEImageHandler() override;

		bool initWithRawBufferData(const void* data, GLint w, GLint h, CGEBufferFormat format, bool bEnableReversion = true);
		bool updateData(const void* data, int w, int h, CGEBufferFormat format);

        // The input 'textureID' will be used & released by the handler if 'shouldKeepInput == true'
		bool initWithTexture(GLuint textureID, GLint w, GLint h, CGEBufferFormat format, bool shouldKeepInput = false);
        bool updateTexture(GLuint textureID, int w, int h);

		bool getOutputBufferData(void* data, CGEBufferFormat format);

		void addImageFilter(CGEImageFilterInterfaceAbstract* proc);
		void popImageFilter();
		void clearImageFilters(bool bDelMem = true);
		inline size_t getFilterNum() { return m_vecFilters.size(); }
		inline CGEImageFilterInterfaceAbstract* getFilterByIndex(GLuint index)
		{ return index >= m_vecFilters.size() ? nullptr : m_vecFilters[index]; }
		int getFilterIndexByAddr(const void* addr);
		bool insertFilterAtIndex(CGEImageFilterInterfaceAbstract* proc, GLuint index);
		bool deleteFilterByAddr(const void* addr, bool bDelMem = true);
		bool deleteFilterByIndex(GLuint index, bool bDelMem = true);
		bool replaceFilterAtIndex(CGEImageFilterInterfaceAbstract* proc, GLuint index, bool bDelMem = true);
		bool swapFilterByIndex(GLuint left, GLuint right);
		void peekFilters(std::vector<CGEImageFilterInterfaceAbstract*>* vTrans);
		std::vector<CGEImageFilterInterfaceAbstract*>& peekFilters() { return m_vecFilters; }

		void processingFilters() override;

		//The last filter of all would be used if the index is -1
		bool processingWithFilter(GLint index);

		//Process with specified filter.
		bool processingWithFilter(CGEImageFilterInterfaceAbstract* proc);

		virtual void disableReversion();
		bool keepCurrentResult();

        // Both buffers will be revert if pass true
		virtual bool revertToKeptResult(bool bRevertAllBuffer = false);
		virtual GLuint copyLastResultTexture(GLuint dstTex = 0) override;
		virtual GLuint copyResultTexture(GLuint dstTex = 0) override;
        
#ifdef _CGE_USE_ES_API_3_0_

		const void* mapOutputBuffer(CGEBufferFormat fmt);
		void unmapOutputBuffer();

#endif

		bool copyTexture(GLuint dst, GLuint src);
		bool copyTexture(GLuint dst, GLuint src, int x, int y, int w, int h);
		bool copyTexture(GLuint dst, GLuint src, int xOffset, int yOffset, int x, int y, int w, int h);

		void drawResult();
		TextureDrawer* getResultDrawer();
		void setResultDrawer(TextureDrawer* drawer);

        inline void useImageFBO() { glBindFramebuffer(GL_FRAMEBUFFER, m_dstFrameBuffer[0]); }

	protected:
		std::vector<CGEImageFilterInterfaceAbstract*> m_vecFilters;

		TextureDrawer *m_drawer, *m_resultDrawer;

#ifdef _CGE_USE_ES_API_3_0_
		GLuint m_pixelPackBuffer;
		GLsizei m_pixelPackBufferSize;

		void clearPixelBuffer();
		bool initPixelBuffer();
		bool initImageFBO(const void* data, int w, int h, GLenum channelFmt, GLenum dataFmt, int channel);
#endif
	};
}
#endif
