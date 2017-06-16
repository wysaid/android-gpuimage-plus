/*
 * cgePolkaDotFilter.h
 *
 *  Created on: 2015-2-1
 *      Author: Wang Yang
 */

#ifndef _CGE_POLKADOTFILTER_H_
#define _CGE_POLKADOTFILTER_H_

#include "cgeHalftoneFilter.h"

namespace CGE
{
	class CGEPolkaDotFilter : public CGEHalftoneFilter
	{
	public:
		bool init();

		//Range: (0, 1]
		void setDotScaling(float value);

	protected:
		static CGEConstString paramDotScaling;
	};
}

#endif