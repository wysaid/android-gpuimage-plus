/*
 * cgeFilterBasic.h
 *
 *  Created on: 2013-12-25
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#ifndef _CGEBASICADJUST_H_
#define _CGEBASICADJUST_H_

#include "cgeBrightnessAdjust.h"
#include "cgeContrastAdjust.h"
#include "cgeSharpenBlurAdjust.h"
#include "cgeSaturationAdjust.h"
#include "cgeShadowHighlightAdjust.h"
#include "cgeWhiteBalanceAdjust.h"
#include "cgeMonochromeAdjust.h"
#include "cgeCurveAdjust.h"
#include "cgeColorLevelAdjust.h"
#include "cgeVignetteAdjust.h"
#include "cgeTiltshiftAdjust.h"
#include "cgeSharpenBlurAdjust.h"
#include "cgeSelectiveColorAdjust.h"
#include "cgeExposureAdjust.h"
#include "cgeHueAdjust.h"
#include "cgeColorBalanceAdjust.h"
#include "cgeLookupFilter.h"

namespace CGE
{
	CGEBrightnessFilter* createBrightnessFilter();
	CGEBrightnessFastFilter* createBrightnessFastFilter();
	CGEContrastFilter* createContrastFilter();
	CGESharpenBlurFilter* createSharpenBlurFilter();
	CGESharpenBlurFastFilter* createSharpenBlurFastFilter();
	CGESharpenBlurSimpleFilter* createSharpenBlurSimpleFilter();
	CGESharpenBlurSimpleBetterFilter* createSharpenBlurSimpleBetterFilter();
	CGESaturationHSLFilter* createSaturationHSLFilter();
	CGESaturationFilter* createSaturationFilter();
	CGEShadowHighlightFilter* createShadowHighlightFilter();
	CGEShadowHighlightFastFilter* createShadowHighlightFastFilter();
	CGEWhiteBalanceFilter* createWhiteBalanceFilter();
	CGEWhiteBalanceFastFilter* createWhiteBalanceFastFilter();
	CGEMonochromeFilter* createMonochromeFilter(); // 黑白
	CGECurveTexFilter* createCurveTexFilter();
	CGECurveFilter* createCurveFilter();
	CGEMoreCurveFilter* createMoreCurveFilter();
	CGEMoreCurveTexFilter* createMoreCurveTexFilter();
	CGEColorLevelFilter* createColorLevelFilter();
	CGEVignetteFilter* createVignetteFilter();
	CGEVignetteExtFilter* createVignetteExtFilter();
	CGETiltshiftVectorFilter* createTiltshiftVectorFilter();
	CGETiltshiftEllipseFilter* createTiltshiftEllipseFilter();
    CGETiltshiftVectorWithFixedBlurRadiusFilter* createFixedTiltshiftVectorFilter();
	CGETiltshiftEllipseWithFixedBlurRadiusFilter* createFixedTiltshiftEllipseFilter();
	CGESharpenBlurFastWithFixedBlurRadiusFilter* createSharpenBlurFastWithFixedBlurRadiusFilter();
	CGESelectiveColorFilter* createSelectiveColorFilter();
	CGEExposureFilter* createExposureFilter();
	CGEHueAdjustFilter* createHueAdjustFilter();
	CGEColorBalanceFilter* createColorBalanceFilter();
	CGELookupFilter* createLookupFilter();
}

#endif 
