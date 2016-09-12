/*
* cgeMarscara.h
*
*  Created on: 2014-5-5
*      Author: Wang Yang
*/

#ifndef _CGE_MARSCARA_H_
#define _CGE_MARSCARA_H_

#include "cgeAdvancedEffectsCommon.h"

namespace CGE
{
	class CGEMarscaraFilter : public CGEAdvancedEffectOneStepFilterHelper
	{
	public:

		bool init();

		//value >= 0
		void setSamplerScale(int value);

	protected:
		static CGEConstString paramSamplerScaleName;
	};
}



#endif