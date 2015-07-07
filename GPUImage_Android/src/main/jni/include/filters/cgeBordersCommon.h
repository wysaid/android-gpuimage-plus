/*
* cgeBordersCommon.h
*
*  Created on: 2014-1-23
*      Author: Wang Yang
*/

#ifndef _CGEBORDERSCOMMON_H_
#define _CGEBORDERSCOMMON_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	typedef struct CGEBorderBlockArea
	{
		GLfloat x, y;
		GLfloat w, h;
	}CGEBorderBlockArea;

	enum CGEBorderTexAlign{ LB, CB, RB, LT, CT, RT, CX, XC, CC, XX };

	class CGEBorderTexAlignHelper
	{
	public:
		void setTexAlign(CGEBorderTexAlign align, GLfloat w, GLfloat h);
		void setTexAlign(CGEBorderTexAlign align, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
		void setTexAlign(const char* align, GLfloat w, GLfloat h);
		void setTexAlign(const char* align, GLfloat x, GLfloat y, GLfloat w, GLfloat h);

	protected:
		GLfloat m_texX, m_texY, m_texTWidth, m_texTHeight;
		CGEBorderTexAlign m_align;
	};

	//A helper class that provides "setThickness" function to all borders.
	//You should not create instances of "CGEBorderFilterInterface".
	class CGEBorderFilterInterface : protected CGEImageFilterInterface
	{
		friend class CGEDataParsingEngine;

	public:
		CGEBorderFilterInterface() : m_texture(0) { assignBorderSampler(); }
		~CGEBorderFilterInterface() { glDeleteTextures(1, &m_texture); }

		virtual void setThickness(float thickness) {};
		virtual void setAlign(float align) {};
		using CGEImageFilterInterface::render2Texture;

	protected:
		static CGEConstString paramThicknessName;
		static CGEConstString paramTextureName;
		void assignBorderSampler();
		void setSamplerID(GLuint samplerID);

	protected:
		GLuint m_texture;
	};

	class CGEBorderSFilter : public CGEBorderFilterInterface
	{
	public:		
		bool init();
	};

	class CGEBorderMFilter : public CGEBorderFilterInterface
	{
	public:
        CGEBorderMFilter() : m_thickness(0.2f), m_lengthVParam(), m_lengthHParam(), m_cornerRatio(1.0f) {}
		~CGEBorderMFilter() {}
		virtual bool init();

		virtual void setBlocks(GLfloat* block, float texRatio = 1.0f); //The array size must be 32!
		virtual void setThickness(float thickness);

		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

	protected:
		static CGEConstString paramBorderBlockName;
		static CGEConstString paramBorderLengthHVName;

	protected:
		float m_thickness;
		float m_lengthVParam, m_lengthHParam;
		float m_cornerRatio;
	};

	class CGEBorderCoverSKRFilter : public CGEBorderFilterInterface
	{
		friend class CGEDataParsingEngine;
	public:
		virtual bool init();

	protected:
		static CGEConstString paramStepsRatioName;
	};

	//Cover Only four corners.
	class CGEBorderCoverCornersFilter : public CGEBorderFilterInterface
	{
	public:
		CGEBorderCoverCornersFilter() {}
		~CGEBorderCoverCornersFilter() {}

		virtual bool init();

		virtual void setBlocks(GLfloat*); //The array size must be 16!
		virtual void setThickness(float thickness);

		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

	protected:
		static CGEConstString paramBorderBlockName;

	private:
		float m_thickness;
	};

	//Cover the up、 down side and put an image on to the photo.
	class CGEBorderCoverUDTFilter : public CGEBorderFilterInterface, public CGEBorderTexAlignHelper
	{
	public:
		virtual bool init();

		virtual void setBlocks(GLfloat* blocks, GLfloat w, GLfloat h); //The array size of "blocks" must be 12! w&h stands for the real size of the block-texture.
		virtual void setThickness(GLfloat thickness);

		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

	protected:
		static CGEConstString paramBorderBlockName;
		static CGEConstString paramTexAreaName;
		static CGEConstString paramLengthHName;

	private:
		GLfloat m_thickness;
		GLfloat m_texAspectRatio, m_TexUDAspectRatio;
	};

	class CGEBorderSWithTexFilter : public CGEBorderFilterInterface, public CGEBorderTexAlignHelper
	{
	public:
		virtual bool init();

		virtual void setBorderSize(GLfloat);
		virtual void setBlocks(GLfloat* blocks, GLfloat w, GLfloat h); //The array size must be 8! w&h stands for the real size of the block-texture.

		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

	protected:
		static CGEConstString paramTexAreaName;
		static CGEConstString paramBorderBlockName;

	private:
		
		GLfloat m_texAspectRatio;
		GLfloat m_borderSize;

	};

	class CGEBorderSWithTexKRFilter : public CGEBorderSWithTexFilter
	{
	public:	
		virtual bool init();

	protected:
		static CGEConstString paramAspectRatioName;

	};

	class CGEBorderMWithTexFilter : public CGEBorderMFilter, public CGEBorderTexAlignHelper
	{
	public:
		virtual bool init();
		virtual void setTexBlock(GLfloat* block, float texRatio = 1.0f); //The array size must be 8! The 4 elements in front stand for the TexBlock and the remain 4 elements stand for the TexArea. w&h stands for the real size of the block-texture.
		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);
		virtual void setBorderSize(GLfloat borderX, GLfloat borderY) { m_borderSizeX = borderX; m_borderSizeY = borderY; }

	protected:
		static CGEConstString paramTexAreaName;
		static CGEConstString paramTexBlockName;

	protected:
		GLfloat m_texAspectRatio;
		GLfloat m_borderSizeX, m_borderSizeY;

	};

	class CGEBorderCoverRTAndLBWithTexFilter : public CGEBorderFilterInterface, public CGEBorderTexAlignHelper
	{
	public:
		virtual bool init();
		virtual void setBlocks(GLfloat* blocks); //The array size must be 12! w&h stands for the real size of the block-texture.
		virtual void setBorderSize(GLfloat value) { m_borderSize = value; }
		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

	protected:
		static CGEConstString paramTexAreaName;
		static CGEConstString paramBlockTexName;
		static CGEConstString paramBlockRTName;
		static CGEConstString paramBlockLBName;
		static CGEConstString paramAreaRTName;
		static CGEConstString paramAreaLBName;

	protected:
		GLfloat m_blocks[12];
		GLfloat m_borderSize;
	};

	class CGEBorderCoverBottomKRFilter : public CGEBorderFilterInterface
	{
	public:
		virtual bool init();
		virtual void setBlockShape(GLfloat w, GLfloat h);
		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

	protected:
		static CGEConstString paramTexCoordName;

	protected: 
		GLfloat m_thickness;
	};

	class CGEBorderCoverUpDownFilter : public CGEBorderFilterInterface
	{
	public:
		bool init();
		void setThickness(float thickness);
		void setShapeRatio(float shapeRatio); // Texture's (width / height)
		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

	protected:
		static CGEConstString paramLengthHName;

		float m_shapeRatio;
		float m_thickness;
	};

	class CGEBorderCoverLeftRightFilter : public CGEBorderCoverUpDownFilter
	{
	public:
		bool init();
		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

	protected:
		static CGEConstString paramLengthVName;
	};

}

#endif
