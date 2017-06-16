/*
* cgeMaxValueFilter.h
*
*  Created on: 2015-3-20
*      Author: Wang Yang
* Description: 最大值滤波
*/

#ifndef _CGE_MAXVALUE_FILTER_H_
#define _CGE_MAXVALUE_FILTER_H_

#include "cgeMinValueFilter.h"

namespace CGE
{
	class CGEMaxValueFilter3x3 : public CGEMinValueFilter3x3
	{
	public:
		const char* getShaderCompFunc();

	};

	class CGEMaxValueFilter3x3Plus : public CGEMinValueFilter3x3Plus
	{
	public:
		const char* getShaderCompFunc();
	};
}

#endif