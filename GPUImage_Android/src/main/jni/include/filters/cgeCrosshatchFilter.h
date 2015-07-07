/*
 * cgeCrosshatchFilter.h
 *
 *  Created on: 2015-2-1
 *      Author: Wang Yang
 */

#ifndef _CGE_CROSSHATCH_H_
#define _CGE_CROSSHATCH_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGECrosshatchFilter : public CGEImageFilterInterface
	{
	public:

		//Range: (0, 0.1], default: 0.03
		void setCrosshatchSpacing(float value);
		//Range: (0, 0.01], default: 0.003
		void setLineWidth(float value);

		bool init();

	protected:
		static CGEConstString paramCrosshatchSpacing;
		static CGEConstString paramLineWidth;
	};

}

#endif