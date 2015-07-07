/*
 * cgePainterDataset.cpp
 *
 *  Created on: 2014-3-12
 *      Author: Wang Yang
 */

#include "cgePainter.h"

namespace CGE
{

	const char* getBrushTextureName(PathAttrib::BrushStyle style)
	{
		const static char* textureNameTable[] = 
		{
			"none",
			"brush_default.png",
			"brush_test1.png",
			"brush_test2.png",
			"brush_test3.png",
			"brush_test4.png",
			"brush_test5.png",
		};

		if(style < 0 || style >= sizeof(textureNameTable) / sizeof(*textureNameTable))
			return NULL;
		return textureNameTable[style];
	}

}