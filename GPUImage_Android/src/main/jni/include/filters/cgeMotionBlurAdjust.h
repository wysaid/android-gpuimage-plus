/*
 * cgeMotionBlurAdjust.h
 *
 *  Created on: 2014-9-25
 *      Author: wxfred
 */

#ifndef _CGEMOTIONBLURADJUST_H
#define _CGEMOTIONBLURADJUST_H

#include "cgeGLFunctions.h"
#include "cgeSharpenBlurAdjust.h"
#include "cgeVec.h"

namespace CGE
{
	class CGEMotionBlurFilter : public CGEImageFilterInterface
	{
	public:
		CGEMotionBlurFilter(){}
		~CGEMotionBlurFilter(){}

		bool init();

		// range: radius >= 0.0
		void setSamplerRadius(float radius);

		void setAngle(float angle);
		void setRadians(float radians);

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, const GLfloat* posVertices);

	protected:
		static CGEConstString paramSamplerRadiusName;
		static CGEConstString paramSamplerStepName;
		static CGEConstString paramBlurNormName;

		float m_samplerRadius;
		Vec2f m_blurNorm;
	};
}

#endif