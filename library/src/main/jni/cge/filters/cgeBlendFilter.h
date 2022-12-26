/*
 * cgeBlendFilter.h
 *
 *  Created on: 2015-2-13
 *      Author: Wang Yang
 */

#ifndef _CGEBLENDFILTER_H_
#define _CGEBLENDFILTER_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	//"CGEBlendInterface" should not be used as instances.
	class CGEBlendInterface : public CGEImageFilterInterface
	{
	public:
		virtual ~CGEBlendInterface() {}
		virtual bool initWithMode(CGETextureBlendMode mode) = 0;
		virtual bool initWithMode(const char* modeName) = 0;
		virtual void setIntensity(float value);

		static CGETextureBlendMode getBlendModeByName(const char* modeName);
		static const char* getShaderFuncByBlendMode(const char* modeName);
		static const char* getShaderFuncByBlendMode(CGETextureBlendMode mode);
		static const char* getBlendWrapper();
		static const char* getBlendKrWrapper();
		static const char* getBlendPixWrapper();
		static const char* getBlendSelfWrapper();

		static bool initWithModeName(const char* modeName, CGEBlendInterface* blendIns);		
		static CGEConstString paramIntensityName;
	};

	class CGEBlendFilter : public CGEBlendInterface
	{
	public:
		CGEBlendFilter() : m_blendTexture(0) {}
		~CGEBlendFilter() { glDeleteTextures(1, &m_blendTexture); }

		virtual bool initWithMode(CGETextureBlendMode mode);
		virtual bool initWithMode(const char* modeName);

		void setSamplerID(GLuint texID, bool shouldDelete = true);

	protected:
		static CGEConstString paramBlendTextureName;
		void initSampler();

	protected:
		GLuint m_blendTexture; //The texture would be deleted by this filter;
	};

	class CGEBlendWithResourceFilter : public CGEBlendFilter
	{
	public:
		CGEBlendWithResourceFilter() {}
		~CGEBlendWithResourceFilter() { }

		virtual void setTexSize(int w, int h);
		CGESizei& getTexSize() { return m_blendTextureSize; }

	protected:		

		CGESizei m_blendTextureSize;
	};

	class CGEBlendKeepRatioFilter : public CGEBlendWithResourceFilter
	{
	public:
		bool initWithMode(CGETextureBlendMode mode);
		bool initWithMode(const char* modeName);

		void setTexSize(int w, int h);

		void flushTexSize();
	protected:
		static CGEConstString paramAspectRatioName;
	};

	class CGEBlendTileFilter : public CGEBlendWithResourceFilter
	{
	public:
		bool initWithMode(CGETextureBlendMode mode);
		bool initWithMode(const char* modeName);
		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);
	protected:
		static CGEConstString paramScalingRatioName;
	};

	class CGEPixblendFilter : public CGEBlendInterface
	{
	public:
		CGEPixblendFilter() {}
		~CGEPixblendFilter() {}

		virtual bool initWithMode(CGETextureBlendMode mode);
		virtual bool initWithMode(const char* modeName);

		void setBlendColor(float r, float g, float b, float a = 1.0f);

	protected:
		static CGEConstString paramBlendColorName;
	};

	class CGEBlendWithSelfFilter : public CGEBlendInterface
	{
	public:
		CGEBlendWithSelfFilter() {}
		~CGEBlendWithSelfFilter() {}

		bool initWithMode(CGETextureBlendMode mode);
		bool initWithMode(const char* modeName);

	};

	class CGEBlendVignetteFilter : public CGEPixblendFilter
	{
	public:
		virtual bool initWithMode(CGETextureBlendMode mode);

		void setVignetteCenter(float x, float y);  //Range: [0, 1], and 0.5 for the center.
		void setVignette(float start, float range); //Range: [0, 1]

	protected:
		static CGEConstString paramVignetteCenterName;
		static CGEConstString paramVignetteName;
	};

	class CGEBlendVignetteNoAlphaFilter : public CGEBlendVignetteFilter
	{
		virtual bool initWithMode(CGETextureBlendMode mode);
	};

	class CGEBlendVignette2Filter : public CGEBlendVignetteFilter
	{
	public:
		virtual bool initWithMode(CGETextureBlendMode mode);
	};

	class CGEBlendVignette2NoAlphaFilter : public CGEBlendVignetteFilter
	{
	public:
		virtual bool initWithMode(CGETextureBlendMode mode);
	};
}


#endif