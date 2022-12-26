/*
 * cgePolarPixellateFilter.h
 *
 *  Created on: 2015-2-1
 *      Author: Wang Yang
 */

#ifndef _CGE_POLARPIXELLATEFILTER_H_
#define _CGE_POLARPIXELLATEFILTER_H_

#include "cgeGLFunctions.h"

namespace CGE
{
	class CGEPolarPixellateFilter : public CGEImageFilterInterface
	{
	public:

		//Range: [0, 1]
		void setCenter(float x, float y);
		//Range: [0, 0.2]
		void setPixelSize(float x, float y);

		bool init();

	protected:
		static CGEConstString paramCenter;
		static CGEConstString paramPixelSize;
	};
}

#endif