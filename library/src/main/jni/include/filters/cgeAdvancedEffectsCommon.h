/*
 * cgeAdvancedEffectsCommon.h
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#ifndef _CGEADVANCEDEFFECTSCOMMON_H_
#define _CGEADVANCEDEFFECTSCOMMON_H_

#include "cgeGLFunctions.h"
#include "cgeImageFilter.h"
#include "cgeImageHandler.h"

namespace CGE
{
	class CGEAdvancedEffectOneStepFilterHelper : public CGEImageFilterInterface
	{
	public:
		CGEAdvancedEffectOneStepFilterHelper(){}
		~CGEAdvancedEffectOneStepFilterHelper(){}
		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

	protected:
		static CGEConstString paramStepsName;
	};

	class CGEAdvancedEffectTwoStepFilterHelper : public CGEImageFilterInterface
	{
	public:
		CGEAdvancedEffectTwoStepFilterHelper() {}
		~CGEAdvancedEffectTwoStepFilterHelper() {}

		virtual void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

	protected:
		static CGEConstString paramStepsName;
	};

}

#endif
