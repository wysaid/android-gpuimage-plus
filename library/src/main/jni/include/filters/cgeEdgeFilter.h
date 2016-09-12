/*
* cgeEdgeFilter.h
*
*  Created on: 2013-12-29
*      Author: Wang Yang
*/

#ifndef _CGEEDGE_H_
#define _CGEEDGE_H_

#include "cgeEmbossFilter.h"

namespace CGE
{
	class CGEEdgeFilter : public CGEEmbossFilter
	{
	public:
		bool init();

		//Intensity Range:[0, 1], 0 for origin, and 1 for the best effect
		//Stride: [0, 5]. Default: 2
	};

	class CGEEdgeSobelFilter : public CGEEmbossFilter
	{
	public:
		
		bool init();

	protected:
		//hide
		void setAngle(float value) {}
	};
}

#endif 