/*
 * cgeCardEffects.h
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#ifndef _CGECARDEFFECTS_H_
#define _CGECARDEFFECTS_H_

#include "cgeBordersCommon.h"

namespace CGE
{
	class CGEBorderCardFilter : public CGEBorderFilterInterface
	{
	public:
		bool init();
		virtual void setThickness(float thickness) {}
		
		//Sets the picture-rect. range: [0, 1]
		//The parameters stand for the left, top, width, height
		void setPictureZone(float x, float y, float w, float h);


		//Sets the center to clamp. range: [0, 2], while 1 means align-center.
		void setAlign(float align);

	protected:
		static CGEConstString paramTextureName;
		static CGEConstString paramOriginAspectRatioName;
		static CGEConstString paramZoneStartName;
		static CGEConstString paramZoneRangeName;
		static CGEConstString paramZoneEndName;
		static CGEConstString paramAlignName;
	};

}

#endif /* _CGECARDEFFECTS_H_ */
