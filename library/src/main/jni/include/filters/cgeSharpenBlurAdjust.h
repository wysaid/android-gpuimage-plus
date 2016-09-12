/*
 * cgeSharpenBlurAdjust.h
 *
 *  Created on: 2013-12-26
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
*/

#ifndef _CGESHARPENBLUR_ADJUST_H_
#define _CGESHARPENBLUR_ADJUST_H_

#include "cgeGLFunctions.h"
#include "CGEImageFilter.h"
#include "cgeImageHandler.h"

namespace CGE
{
	class CGESharpenBlurFilter : public CGEImageFilterInterface
	{
	public:
		CGESharpenBlurFilter() {}
		~CGESharpenBlurFilter(){}

		//<0: edge
		//0: the max blur
		//1: the original image
		//>1: sharpen
		virtual void setIntensity(float value);

		//the two methods below are only some wrapping from 'setIntensity' .
		virtual void setBlurIntensity(float value); //range: 0.0 ~ 1.0
		virtual void setSharpenIntensity(float value); //range: 0.0 ~ 10.0

		//////////////////////////////////////////////
		
		// Be sure to know: the filter would be slower as the samplerScale become greater. 
		//Please test the max-value before release.
		virtual void setSamplerScale(int value); //range 0.0 ~ 30.0
		
		virtual void setSamplerLimit(int value);
		
		virtual bool init();

		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

	protected:
		static CGEConstString paramIntensity;
		static CGEConstString paramSamplerRadius;
		static CGEConstString paramSamplerScale;
		static CGEConstString paramSamplerStepName;
		//The length of blurNorm must be 1.
		static CGEConstString paramBlurNormalName;

	protected:
		int m_samplerLimit;
		int m_samplerScale;
	};


	//Special usage. The "dstTexture" in function "render2Texture" is the texture to be rendered!
	//Don't use this class unless you know what I said.
	class CGEBlurFastFilter : public CGESharpenBlurFilter
	{
	public:
		CGEBlurFastFilter() : m_bUseFixedRadius(false) {}
		~CGEBlurFastFilter() {}

		bool initWithoutFixedRadius(bool noFixed = true);
		//Warning: see the description above the class.
		void render2Texture(CGEImageHandlerInterface* handler, GLuint dstTexture, GLuint vertexBufferID);

		void setSamplerScale(int value);

	protected:
		virtual bool init() {return false;}
		virtual void setIntensity(float) {}

	private:
		bool m_bUseFixedRadius;
	};

	//This filter would generate a cache of the nearly-processed blurred image.
	class CGESharpenBlurFastFilter : public CGEImageFilterInterface
	{
	public:
		CGESharpenBlurFastFilter() : m_texture(0), m_samplerScaleValue(0) {}
		~CGESharpenBlurFastFilter() { glDeleteTextures(1, &m_texture); }

		//<0: edge
		//0: the max blurring
		//1: the original image
		//>1: sharpen
		void setIntensity(float value);

		//the two methods below are only some wrapping from 'setIntensity' .
		void setBlurIntensity(float value); //range: 0.0 ~ 1.0
		void setSharpenIntensity(float value); //range: 0.0 ~ 10.0

		//////////////////////////////////////////////

		// Be sure to know: the filter would be slower as the samplerScale become greater. 
		//Please test the max-value before release.
		void setSamplerScale(int value); //range 0 ~ 30

		void setSamplerLimit(int value); //Less than (or equal to) sampler scale.

		virtual bool init();

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

		void flush();

	protected:
		static CGEConstString paramIntensity;
		static CGEConstString paramBlurredTexName;

	protected:
		GLuint m_texture;
		CGEBlurFastFilter m_blurProc;
		int m_samplerScaleValue;
	};

	class CGESharpenBlurFastWithFixedBlurRadiusFilter : public CGESharpenBlurFastFilter
	{
	public:
		bool init();
	};

	class CGESharpenBlurSimpleFilter : public CGEImageFilterInterface
	{
	public:
		CGESharpenBlurSimpleFilter() {}
		~CGESharpenBlurSimpleFilter() {}
		//<0: edge
		//0: the max blur
		//1: the original image
		//>1: sharpen
		void setIntensity(float value);

		//the two methods below are only some wrapping from 'setIntensity' .
		void setBlurIntensity(float value); //range: 0.0 ~ 1.0
		void setSharpenIntensity(float value); //range: 0.0 ~ 10.0

		//////////////////////////////////////////////

		// Be sure to know: the filter would be slower as the samplerScale become greater. 
		//Please test the max-value before release.
		void setSamplerScale(float value); //range 0.0 ~ 30.0

		bool init();

	protected:
		static CGEConstString paramIntensity;
		static CGEConstString paramSamplerScale;
		static CGEConstString paramSamplerStepName;

	};

	class CGESharpenBlurSimpleBetterFilter : public CGESharpenBlurSimpleFilter
	{
	public:

		bool init();

	};

}

#endif