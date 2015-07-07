/*
 * cgeZoomBlurAdjust.h
 *
 *  Created on: 2014-9-17
 *      Author: wxfred
 */

#ifndef _CGEZOOMBLURADJUST_H
#define _CGEZOOMBLURADJUST_H

#include "cgeGLFunctions.h"

namespace CGE
{
	//适合向外扩散的情况
	class CGEZoomBlurFilter : public CGEImageFilterInterface
	{
	public:
		CGEZoomBlurFilter(){}
		~CGEZoomBlurFilter(){}

		void setCenter(float x, float y); // texture coordinate
		void setIntensity(float strength); // range: [0, 1]

		bool init();

	protected:
		static CGEConstString paramCenterName;
		static CGEConstString paramIntensityName;
	};

	//折中方案， 兼顾向外与向内
	class CGEZoomBlur2Filter : public CGEZoomBlurFilter
	{
	public:
		CGEZoomBlur2Filter() {}
		~CGEZoomBlur2Filter() {}

		bool init();

	protected:
		static CGEConstString paramStepsName;

	};

}

#endif