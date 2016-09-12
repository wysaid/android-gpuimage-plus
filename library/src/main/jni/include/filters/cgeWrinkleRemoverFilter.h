/*
* cgeWrinkleRemoverFilter.h
*
*  Created on: 2014-5-5
*      Author: Wang Yang
*/

#ifndef _CGE_WRINKLE_REMOVER_H_
#define _CGE_WRINKLE_REMOVER_H_

#include "cgeAdvancedEffectsCommon.h"

namespace CGE
{
	class CGEWrinkleRemoveFilter : public CGEAdvancedEffectTwoStepFilterHelper
	{
	public:
		bool init();

		//value >= 0
		void setBlurScale(float value);
		//0 < low < high < 0.1
		void setNoiseRange(float low, float high);

		//range: [0, 1]
		void setNoiseIntensity(float value);

	protected:
		static CGEConstString paramSamplerScaleName;
		static CGEConstString paramNoiseRangeName;
		static CGEConstString paramNoiseIntensityName;

	};

}

#endif