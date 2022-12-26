/*
 * cgeExposureAdjust.h
 *
 *  Created on: 2015-1-29
 *      Author: Wang Yang
 */

#ifndef _CGEEXPOSUREADJUST_H_
#define _CGEEXPOSUREADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGEExposureFilter : public CGEImageFilterInterface
	{
	public:

		//Range: [-10, 10]
		void setIntensity(float value);

		bool init();

	protected:
		static CGEConstString paramName;

	};
}

#endif