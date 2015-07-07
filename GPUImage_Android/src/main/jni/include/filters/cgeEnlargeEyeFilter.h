/*
* cgeEnlargeEyeFilter.h
*
*  Created on: 2014-4-23
*      Author: Wang Yang
*/

#ifndef _CGE_ENLARGEEYE_H_
#define _CGE_ENLARGEEYE_H_

#include "cgeAdvancedEffectsCommon.h"

namespace CGE
{
	class CGEEnlargeEyeFilter : public CGEAdvancedEffectOneStepFilterHelper
	{
	public:
		
		bool init();

		void setEnlargeRadius(float radius); //Real radius counts by pixels.
		void setIntensity(float value); //Range: [-1.0, 1.0]
		void setCentralPosition(float x, float y); //Real position counts by pixels.

	protected:
		static CGEConstString paramRadiusName;
		static CGEConstString paramIntensityName;
		static CGEConstString paramCentralPosName;
	};

}

#endif