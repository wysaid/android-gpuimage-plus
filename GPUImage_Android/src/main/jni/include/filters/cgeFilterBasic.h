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
#include "cgeZoomBlurAdjust.h"
#include "cgeMotionBlurAdjust.h"
#include "cgeSelectiveColorAdjust.h"
#include "cgeExposureAdjust.h"
#include "cgeHueAdjust.h"
#include "cgeFastTiltShift.h"
#include "cgeEdgeExtract.h"
#include "cgeColorBalanceAdjust.h"

namespace CGE
{
	CGEBrightnessFilter* getBrightnessFilter();
	CGEBrightnessFastFilter* getBrightnessFastFilter();
	CGEContrastFilter* getContrastFilter();
	CGESharpenBlurFilter* getSharpenBlurFilter();
	CGESharpenBlurFastFilter* getSharpenBlurFastFilter();
	CGESharpenBlurSimpleFilter* getSharpenBlurSimpleFilter();
	CGESharpenBlurSimpleBetterFilter* getSharpenBlurSimpleBetterFilter();
	CGESaturationHSLFilter* getSaturationHSLFilter();
	CGESaturationFilter* getSaturationFilter();
	CGEShadowHighlightFilter* getShadowHighlightFilter();
	CGEShadowHighlightFastFilter* getShadowHighlightFastFilter();
	CGEWhiteBalanceFilter* getWhiteBalanceFilter();
	CGEWhiteBalanceFastFilter* getWhiteBalanceFastFilter();
	CGEMonochromeFilter* getMonochromeFilter(); // 黑白
	CGECurveTexFilter* getCurveTexFilter();
	CGECurveFilter* getCurveFilter();
	CGEMoreCurveFilter* getMoreCurveFilter();
	CGEMoreCurveTexFilter* getMoreCurveTexFilter();
	CGEColorLevelFilter* getColorLevelFilter();
	CGEVignetteFilter* getVignetteFilter();
	CGEVignetteExtFilter* getVignetteExtFilter();
	CGETiltshiftVectorFilter* getTiltshiftVectorFilter();
	CGETiltshiftEllipseFilter* getTiltshiftEllipseFilter();
    CGETiltshiftVectorWithFixedBlurRadiusFilter* getFixedTiltshiftVectorFilter();
	CGETiltshiftEllipseWithFixedBlurRadiusFilter* getFixedTiltshiftEllipseFilter();
	CGESharpenBlurFastWithFixedBlurRadiusFilter* getSharpenBlurFastWithFixedBlurRadiusFilter();
	CGEZoomBlurFilter* getZoomBlurFilter();
	CGEZoomBlur2Filter* getZoomBlur2Filter();
	CGEMotionBlurFilter* getMotionBlurFilter();
	CGESelectiveColorFilter* getSelectiveColorFilter();
	CGEExposureFilter* getExposureFilter();
	CGEHueAdjustFilter* getHueAdjustFilter();
	CGEColorBalanceFilter* getColorBalanceFilter();
}

#endif 
