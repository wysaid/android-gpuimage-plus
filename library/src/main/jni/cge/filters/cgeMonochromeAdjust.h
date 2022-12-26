/*
 * cgeMonochromeAdjust.h
 *
 *  Created on: 2013-12-29
 *      Author: Wang Yang
 */

#ifndef _CGEMONOCHROME_ADJUST_H_
#define _CGEMONOCHROME_ADJUST_H_

#include "cgeGLFunctions.h"
#include "cgeImageFilter.h"
#include "cgeImageHandler.h"

namespace CGE
{
	class CGEMonochromeFilter : public CGEImageFilterInterface
	{
	public:
		CGEMonochromeFilter(){}
		~CGEMonochromeFilter(){}

		bool init();

		void setRed(float value);
		void setGreen(float value);
		void setBlue(float value);
		void setCyan(float value);
		void setMagenta(float value);
		void setYellow(float value);

	protected:
		static CGEConstString paramRed;
		static CGEConstString paramGreen;
		static CGEConstString paramBlue;
		static CGEConstString paramCyan;
		static CGEConstString paramMagenta;
		static CGEConstString paramYellow;
	};
}

#endif 
