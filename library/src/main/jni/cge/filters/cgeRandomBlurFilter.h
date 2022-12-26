/*
 * cgeRandomBlurFilter.h
 *
 *  Created on: 2013-12-29
 *      Author: Wang Yang
 */

#ifndef _CGERANDOMBLUR_H_
#define _CGERANDOMBLUR_H_

#include "cgeAdvancedEffectsCommon.h"

namespace CGE
{
	class CGERandomBlurFilter : public CGEAdvancedEffectOneStepFilterHelper
	{
	public:
		CGERandomBlurFilter(){}
		~CGERandomBlurFilter(){}

		void setIntensity(float value);
		void setSamplerScale(float value);

		bool init();

	protected:
		static CGEConstString paramIntensity;
		static CGEConstString paramSamplerScale;
		static CGEConstString paramSamplerRadius;
	};
}

#endif 