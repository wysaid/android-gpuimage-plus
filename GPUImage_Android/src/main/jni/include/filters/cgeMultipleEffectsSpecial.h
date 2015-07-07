/*
* cgeMultipleEffectsSpecial.h
*
*  Created on: 2014-1-13
*      Author: Wang Yang
*/

#ifndef _CGEMUTIPLEEFFECTSSPECIAL_H_
#define _CGEMUTIPLEEFFECTSSPECIAL_H_

#include "cgeGLFunctions.h"
#include "cgeMultipleEffectsCommon.h"
#include "cgeMultipleEffects.h"

//All special effects were placed here.

namespace CGE
{
	class CGESpecialFilterE16 : public CGEImageFilterInterface
	{
	public:
		bool init();
	};

	class CGESpecialFilterE21 : public CGEImageFilterInterface
	{
	public:
		bool init();
	};

	class CGESpecialFilterE28 : public CGE::CGEColorScaleFilter
	{
	public:
		bool init();
	};

	class CGESpecialFilterE29 : public CGE::CGEColorScaleFilter
	{
	public:
		bool init();
	};

	class CGESpecialFilterE30To32 : public CGE::CGEColorScaleFilter
	{
	public:
		bool init();
	};

	class CGEFilterColorScaleAndTex1 : public CGE::CGEColorScaleFilter
	{
	public:
		CGEFilterColorScaleAndTex1() : m_texture(0) {}
		~CGEFilterColorScaleAndTex1() { glDeleteTextures(1, &m_texture); }
		virtual bool init() { return false; }
		
	protected:

		void initSampler();
		bool assignSamplerID(GLuint texID);
	private:
		GLuint m_texture;
	};

	class CGESpecialFilterE39 : public CGEFilterColorScaleAndTex1
	{
	public:
		bool initWithinCommonFilter(CGEMutipleEffectFilter* htProc);

	protected:
		static CGEConstString paramTextureNmae;
	};

	class CGESpecialFilterE73To80Help : public CGEImageFilterInterface
	{
	public:
		CGESpecialFilterE73To80Help() {}
		~CGESpecialFilterE73To80Help() { glDeleteTextures(1, &m_texture); }

	protected:
		static CGEConstString paramTextureName;
		void initSampler();
		bool assignSamplerID(GLuint texID);

	private:
		GLuint m_texture;
	};

	class CGESpecialFilterE73 : public CGESpecialFilterE73To80Help
	{
	public:
		bool initWithinCommonFilter(CGEMutipleEffectFilter* htProc);
	};

	class CGESpecialFilterE74 : public CGESpecialFilterE73To80Help
	{
	public:
		bool initWithinCommonFilter(CGEMutipleEffectFilter* htProc);
	};

	class CGESpecialFilterE75 : public CGESpecialFilterE73To80Help
	{
	public:
		bool initWithinCommonFilter(CGEMutipleEffectFilter* htProc);
	};

	class CGESpecialFilterE76 : public CGESpecialFilterE73To80Help
	{
	public:
		bool initWithinCommonFilter(CGEMutipleEffectFilter* htProc);
	};

	class CGESpecialFilterE77 : public CGESpecialFilterE73To80Help
	{
	public:
		bool initWithinCommonFilter(CGEMutipleEffectFilter* htProc);
	};

	class CGESpecialFilterE78 : public CGESpecialFilterE73To80Help
	{
	public:
		bool initWithinCommonFilter(CGEMutipleEffectFilter* htProc);
	};

	class CGESpecialFilterE79 : public CGESpecialFilterE73To80Help
	{
	public:
		bool initWithinCommonFilter(CGEMutipleEffectFilter* htProc);
	};

	class CGESpecialFilterE80 : public CGESpecialFilterE73To80Help
	{
	public:
		bool initWithinCommonFilter(CGEMutipleEffectFilter* htProc);
	};


	class CGESpecialFilterE87 : public CGEImageFilterInterface
	{
	public:
		bool init();
	};

	class CGESpecialFilterE88 : public CGEImageFilterInterface
	{
	public:
		bool init();
	};

	class CGESpecialFilterE89 : public CGEImageFilterInterface
	{
	public:
		bool init();
	};

	class CGESpecialFilterE90 : public CGEImageFilterInterface
	{
	public:
		bool init();
	protected:
		static const GLfloat paramColorGradientValue[15];
		static CGEConstString paramColorGradientName;
	};

	class CGESpecialFilterE96 : public CGESpecialFilterE90
	{
	public:
		bool init();
	protected:
		static const GLfloat paramColorGradientValue[15];
	};

	class CGESpecialFilterE91 : public CGEImageFilterInterface
	{
	public:
		bool init();
	};

	class CGESpecialFilterE92 : public CGEImageFilterInterface
	{
	public:
		bool init();
		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);
		static void getHalfToneLowFac(GLfloat* low, GLfloat* fac, GLint w, GLint h);
	protected:
		static CGEConstString paramHalfToneName;
	};

	class CGESpecialFilterE99 : public CGESpecialFilterE92
	{
	public:
		bool init();
	};

	class CGESpecialFilterE118 : public CGE::CGEColorScaleFilter
	{
	public:
		bool init();
	};

}

#endif