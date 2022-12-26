/*
 * cgeShadowHighlightAdjust.h
 *
 *  Created on: 2013-12-26
 *      Author: Wang Yang
 */

#ifndef _CGESHADOWHIGHLIGHT_H_
#define _CGESHADOWHIGHLIGHT_H_

#include "cgeGLFunctions.h"
#include "cgeImageHandler.h"

namespace CGE
{
	class CGEShadowHighlightFilter : public CGEImageFilterInterface
	{
	public:
		CGEShadowHighlightFilter(){}
		~CGEShadowHighlightFilter(){}

		void setShadow(float value); // range [-200, 100]
		void setHighlight(float value); // range [-100, 200]

		bool init();

	protected:
		static CGEConstString paramShadowName;
		static CGEConstString paramHighlightName;

	private:
	};

	class CGEShadowHighlightFastFilter : public CGEFastAdjustRGBFilter
	{
	public:
		CGEShadowHighlightFastFilter() : m_shadow(0.0f), m_highlight(0.0f) {}
		~CGEShadowHighlightFastFilter() {}

		void setShadowAndHighlight(float shadow, float highlight); //the same to above.

		bool init();

		float getShadow() { return m_shadow; }
		float getHighlight() { return m_highlight; }

	private:
		float m_shadow, m_highlight;
	};

}

#endif