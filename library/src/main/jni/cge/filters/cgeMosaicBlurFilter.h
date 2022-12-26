/*
* cgeMosaicBlur.h
*
*  Created on: 2014-4-10
*      Author: Wang Yang
*  Description: 马赛克
*/

#ifndef _CGE_MOSAICBLUR_H_
#define _CGE_MOSAICBLUR_H_

#include "cgeAdvancedEffectsCommon.h"

namespace CGE
{
	class CGEMosaicBlurFilter : public CGEAdvancedEffectOneStepFilterHelper
	{
	public:
		
		bool init();

		//Range: value >= 1.0, and 1.0(Default) for the origin. Value is better with integer.
		void setBlurPixels(float value);

	protected:
		static CGEConstString paramBlurPixelsName;
	};
}

#endif