/*
 * cgeSelectiveColorAdjust.h
 *
 *  Created on: 2014-11-18
 */

#ifndef _CGESELECTIVECOLORADJUST_H_
#define _CGESELECTIVECOLORADJUST_H_

#include "cgeGLFunctions.h"
#include "cgeImageFilter.h"

namespace CGE
{
	class CGESelectiveColorFilter : public CGEImageFilterInterface
	{
	public:

		CGESelectiveColorFilter():m_curveTexture(0){}
		~CGESelectiveColorFilter(){}

		void setRed    (float cyan,float magenta,float yellow,float key); // range: [-1, 1]
		void setGreen  (float cyan,float magenta,float yellow,float key); // range: [-1, 1]
		void setBlue   (float cyan,float magenta,float yellow,float key); // range: [-1, 1]
		void setCyan   (float cyan,float magenta,float yellow,float key); // range: [-1, 1]
		void setMagenta(float cyan,float magenta,float yellow,float key); // range: [-1, 1]
		void setYellow (float cyan,float magenta,float yellow,float key); // range: [-1, 1]
		void setWhite  (float cyan,float magenta,float yellow,float key); // range: [-1, 1]
		void setGray   (float cyan,float magenta,float yellow,float key); // range: [-1, 1]
		void setBlack  (float cyan,float magenta,float yellow,float key); // range: [-1, 1]

		bool init();

	protected:
		unsigned int m_curveTexture;
		static CGEConstString paramName;
	};

}

#endif
