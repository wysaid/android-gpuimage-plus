/*
 * cgeContrastAdjust.h
 *
 *  Created on: 2013-12-26
 *      Author: Wang Yang
 */

#ifndef _CGECONTRAST_ADJUST_H_
#define _CGECONTRAST_ADJUST_H_

#include "cgeGLFunctions.h"
#include "CGEImageFilter.h"
#include "cgeImageHandler.h"

namespace CGE
{
	class CGEContrastFilter : public CGEImageFilterInterface
	{
	public:
		CGEContrastFilter(){}
		~CGEContrastFilter(){}

		void setIntensity(float value); //range > 0, and 1 for origin

		bool init();

	protected:
		static CGEConstString paramName;
	};
}

#endif