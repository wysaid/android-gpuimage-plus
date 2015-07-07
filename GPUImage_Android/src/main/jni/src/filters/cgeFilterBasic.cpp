/*
* cgeFilterBasic.cpp
*
*  Created on: 2013-12-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeFilterBasic.h"

#define COMMON_FUNC(proc, type) \
type* proc = new type();\
if(!proc->init())\
{\
	delete proc;\
	proc = NULL;\
}\
return proc;\

namespace CGE
{
	CGEBrightnessFilter* getBrightnessFilter()
	{
		COMMON_FUNC(p, CGEBrightnessFilter);
	}

	CGEBrightnessFastFilter* getBrightnessFastFilter()
	{
		COMMON_FUNC(p, CGEBrightnessFastFilter);
	}

	CGEContrastFilter* getContrastFilter()
	{
		COMMON_FUNC(p, CGEContrastFilter);
	}

	CGESharpenBlurFilter* getSharpenBlurFilter()
	{
		COMMON_FUNC(p, CGESharpenBlurFilter);
	}

	CGESharpenBlurFastFilter* getSharpenBlurFastFilter()
	{
		COMMON_FUNC(p, CGESharpenBlurFastFilter);
	}

	CGESharpenBlurSimpleFilter* getSharpenBlurSimpleFilter()
	{
		COMMON_FUNC(p, CGESharpenBlurSimpleFilter);
	}

	CGESharpenBlurSimpleBetterFilter* getSharpenBlurSimpleBetterFilter()
	{
		COMMON_FUNC(p, CGESharpenBlurSimpleBetterFilter);
	}

	CGESaturationHSLFilter* getSaturationHSLFilter()
	{
		COMMON_FUNC(p, CGESaturationHSLFilter);
	}

	CGESaturationFilter* getSaturationFilter()
	{
		COMMON_FUNC(p, CGESaturationFilter);
	}
	
	CGEShadowHighlightFilter* getShadowHighlightFilter()
	{
		COMMON_FUNC(p, CGEShadowHighlightFilter);
	}

	CGEShadowHighlightFastFilter* getShadowHighlightFastFilter()
	{
		COMMON_FUNC(p, CGEShadowHighlightFastFilter);
	}

	CGEWhiteBalanceFilter* getWhiteBalanceFilter()
	{
		COMMON_FUNC(p, CGEWhiteBalanceFilter);
	}

	CGEWhiteBalanceFastFilter* getWhiteBalanceFastFilter()
	{
		COMMON_FUNC(p, CGEWhiteBalanceFastFilter);
	}

	CGEMonochromeFilter* getMonochromeFilter()
	{
		COMMON_FUNC(p, CGEMonochromeFilter);
	}

	CGECurveTexFilter* getCurveTexFilter()
	{
		COMMON_FUNC(p, CGECurveTexFilter);
	}

	CGECurveFilter* getCurveFilter()
	{
		COMMON_FUNC(p, CGECurveFilter);
	}

	CGEMoreCurveFilter* getMoreCurveFilter()
	{
		COMMON_FUNC(p, CGEMoreCurveFilter);
	}

	CGEMoreCurveTexFilter* getMoreCurveTexFilter()
	{
		COMMON_FUNC(p, CGEMoreCurveTexFilter);
	}

	CGEColorLevelFilter* getColorLevelFilter()
	{
		COMMON_FUNC(p, CGEColorLevelFilter);
	}
    
	CGEVignetteFilter* getVignetteFilter()
	{
		COMMON_FUNC(p, CGEVignetteFilter);
	}
	CGEVignetteExtFilter* getVignetteExtFilter()
	{
		COMMON_FUNC(p, CGEVignetteExtFilter);
	}
    
	CGETiltshiftVectorFilter* getTiltshiftVectorFilter()
	{
        
        COMMON_FUNC(p, CGETiltshiftVectorFilter);
	}
    
	CGETiltshiftEllipseFilter* getTiltshiftEllipseFilter()
	{
        COMMON_FUNC(p, CGETiltshiftEllipseFilter);
	}
    
    CGETiltshiftVectorWithFixedBlurRadiusFilter* getFixedTiltshiftVectorFilter()
    {
        COMMON_FUNC(p, CGETiltshiftVectorWithFixedBlurRadiusFilter);
    }
    
	CGETiltshiftEllipseWithFixedBlurRadiusFilter* getFixedTiltshiftEllipseFilter()
    {
        COMMON_FUNC(p, CGETiltshiftEllipseWithFixedBlurRadiusFilter);
    }

	CGESharpenBlurFastWithFixedBlurRadiusFilter* getSharpenBlurFastWithFixedBlurRadiusFilter()
	{
		 COMMON_FUNC(p, CGESharpenBlurFastWithFixedBlurRadiusFilter);
	}

	CGEZoomBlurFilter* getZoomBlurFilter()
	{
		COMMON_FUNC(p, CGEZoomBlurFilter);
	}

	CGEZoomBlur2Filter* getZoomBlur2Filter()
	{
		COMMON_FUNC(p, CGEZoomBlur2Filter);
	}

	CGEMotionBlurFilter *getMotionBlurFilter()
	{
		COMMON_FUNC(p, CGEMotionBlurFilter);
	}

	CGESelectiveColorFilter *getSelectiveColorFilter()
	{
		COMMON_FUNC(p, CGESelectiveColorFilter);
	}

	CGEExposureFilter* getExposureFilter()
	{
		COMMON_FUNC(p, CGEExposureFilter);
	}

	CGEHueAdjustFilter* getHueAdjustFilter()
	{
		COMMON_FUNC(p, CGEHueAdjustFilter);
	}

	CGEColorBalanceFilter* getColorBalanceFilter()
	{
		COMMON_FUNC(p, CGEColorBalanceFilter);
	}
}