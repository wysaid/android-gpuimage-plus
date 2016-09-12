/*
 * cgeColorBalanceAdjust.h
 *
 *  Created on: 2015-3-30
 *      Author: Wang Yang
 */

#ifndef _CGECOLORBALANCEADJUST_H_
#define _CGECOLORBALANCEADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGEColorBalanceFilter : public CGEImageFilterInterface
	{
	public:

		bool init();

		//Range[-1, 1], cyan to red
		void setRedShift(float value);

		//Range[-1, 1], magenta to green
		void setGreenShift(float value);

		//Range[-1, 1], yellow to blue
		void setBlueShift(float value);

	protected:
		static CGEConstString paramRedShiftName;
		static CGEConstString paramGreenShiftName;
		static CGEConstString paramBlueShiftName;

	};



}

#endif