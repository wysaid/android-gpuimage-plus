/*
* cgeMinValueFilter.h
*
*  Created on: 2015-9-24
*      Author: Wang Yang
* Description: 中值滤波
*/

#ifndef _CGE_MIDVALUE_FILTER_H_
#define _CGE_MIDVALUE_FILTER_H_

#include "cgeMinValueFilter.h"

namespace CGE
{
	class CGEMidValueFilter3x3 : public CGEMinValueFilter3x3
	{
	public:

		bool init();

	protected:


	};

}


#endif