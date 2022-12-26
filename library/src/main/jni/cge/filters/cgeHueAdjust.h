/*
 * cgeHueAdjust.h
 *
 *  Created on: 2015-1-29
 *      Author: Wang Yang
 */

#ifndef _CGEHUEADJUST_H_
#define _CGEHUEADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGEHueAdjustFilter : public CGEImageFilterInterface
	{
	public:

		//Range: [0, 2π]
		void setHue(float value);

		void setIntensity(float intensity);

		bool init();

	protected:
		static CGEConstString paramName;
	};

}

#endif