/*
 * cgeColorLevelAdjust.h
 *
 *  Created on: 2014-1-20
 *      Author: Wang Yang
 */

#ifndef _CGECOLORLEVELADJUST_H_
#define _CGECOLORLEVELADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGEColorLevelFilter : public CGEImageFilterInterface
	{
	public:
		CGEColorLevelFilter() {}
		~CGEColorLevelFilter() {}

		bool init();

		void setLevel(float dark, float light); // range [0, 1], dark < light
		void setGamma(float value); // range [0, 3], default: 1 (origin)

	protected: 
		static CGEConstString paramLevelName;
		static CGEConstString paramGammaName;
	};
}

#endif