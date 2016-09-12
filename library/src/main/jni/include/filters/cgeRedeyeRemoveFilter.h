#ifndef _CGE_REDEYEREMOVE_H_
#define _CGE_REDEYEREMOVE_H_

#include "cgeAdvancedEffectsCommon.h"

namespace CGE
{
	class CGERedeyeRemoveFilter : public CGEAdvancedEffectOneStepFilterHelper
	{
	public:
		bool init();

		void setEyePostion(float x, float y);

		void setRadius(float radius);

		void setAlpha(float alpha);

	protected:
		static CGEConstString paramRadiusName;
		static CGEConstString paramEyePostionName;
		static CGEConstString  paramAlphaName;
	};
}

#endif