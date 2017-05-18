/*
* cgeFilterBasic.cpp
*
*  Created on: 2013-12-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeFilterBasic.h"

#define COMMON_FUNC(type) \
type* proc = new type();\
if(!proc->init())\
{\
	delete proc;\
	proc = NULL;\
}\
return proc;\

namespace CGE
{
	CGEBrightnessFilter* createBrightnessFilter()
	{
		COMMON_FUNC(CGEBrightnessFilter);
	}

	CGEBrightnessFastFilter* createBrightnessFastFilter()
	{
		COMMON_FUNC(CGEBrightnessFastFilter);
	}

	CGEContrastFilter* createContrastFilter()
	{
		COMMON_FUNC(CGEContrastFilter);
	}

	CGESharpenBlurFilter* createSharpenBlurFilter()
	{
		COMMON_FUNC(CGESharpenBlurFilter);
	}

	CGESharpenBlurFastFilter* createSharpenBlurFastFilter()
	{
		COMMON_FUNC(CGESharpenBlurFastFilter);
	}

	CGESharpenBlurSimpleFilter* createSharpenBlurSimpleFilter()
	{
		COMMON_FUNC(CGESharpenBlurSimpleFilter);
	}

	CGESharpenBlurSimpleBetterFilter* createSharpenBlurSimpleBetterFilter()
	{
		COMMON_FUNC(CGESharpenBlurSimpleBetterFilter);
	}

	CGESaturationHSLFilter* createSaturationHSLFilter()
	{
		COMMON_FUNC(CGESaturationHSLFilter);
	}

	CGESaturationFilter* createSaturationFilter()
	{
		COMMON_FUNC(CGESaturationFilter);
	}
	
	CGEShadowHighlightFilter* createShadowHighlightFilter()
	{
		COMMON_FUNC(CGEShadowHighlightFilter);
	}

	CGEShadowHighlightFastFilter* createShadowHighlightFastFilter()
	{
		COMMON_FUNC(CGEShadowHighlightFastFilter);
	}

	CGEWhiteBalanceFilter* createWhiteBalanceFilter()
	{
		COMMON_FUNC(CGEWhiteBalanceFilter);
	}

	CGEWhiteBalanceFastFilter* createWhiteBalanceFastFilter()
	{
		COMMON_FUNC(CGEWhiteBalanceFastFilter);
	}

	CGEMonochromeFilter* createMonochromeFilter()
	{
		COMMON_FUNC(CGEMonochromeFilter);
	}

	CGECurveTexFilter* createCurveTexFilter()
	{
		COMMON_FUNC(CGECurveTexFilter);
	}

	CGECurveFilter* createCurveFilter()
	{
		COMMON_FUNC(CGECurveFilter);
	}

	CGEMoreCurveFilter* createMoreCurveFilter()
	{
		COMMON_FUNC(CGEMoreCurveFilter);
	}

	CGEMoreCurveTexFilter* createMoreCurveTexFilter()
	{
		COMMON_FUNC(CGEMoreCurveTexFilter);
	}

	CGEColorLevelFilter* createColorLevelFilter()
	{
		COMMON_FUNC(CGEColorLevelFilter);
	}
    
	CGEVignetteFilter* createVignetteFilter()
	{
		COMMON_FUNC(CGEVignetteFilter);
	}
	CGEVignetteExtFilter* createVignetteExtFilter()
	{
		COMMON_FUNC(CGEVignetteExtFilter);
	}
    
	CGETiltshiftVectorFilter* createTiltshiftVectorFilter()
	{
        
        COMMON_FUNC(CGETiltshiftVectorFilter);
	}
    
	CGETiltshiftEllipseFilter* createTiltshiftEllipseFilter()
	{
        COMMON_FUNC(CGETiltshiftEllipseFilter);
	}
    
    CGETiltshiftVectorWithFixedBlurRadiusFilter* createFixedTiltshiftVectorFilter()
    {
        COMMON_FUNC(CGETiltshiftVectorWithFixedBlurRadiusFilter);
    }
    
	CGETiltshiftEllipseWithFixedBlurRadiusFilter* createFixedTiltshiftEllipseFilter()
    {
        COMMON_FUNC(CGETiltshiftEllipseWithFixedBlurRadiusFilter);
    }

	CGESharpenBlurFastWithFixedBlurRadiusFilter* createSharpenBlurFastWithFixedBlurRadiusFilter()
	{
		 COMMON_FUNC(CGESharpenBlurFastWithFixedBlurRadiusFilter);
	}

	CGESelectiveColorFilter *createSelectiveColorFilter()
	{
		COMMON_FUNC(CGESelectiveColorFilter);
	}

	CGEExposureFilter* createExposureFilter()
	{
		COMMON_FUNC(CGEExposureFilter);
	}

	CGEHueAdjustFilter* createHueAdjustFilter()
	{
		COMMON_FUNC(CGEHueAdjustFilter);
	}

	CGEColorBalanceFilter* createColorBalanceFilter()
	{
		COMMON_FUNC(CGEColorBalanceFilter);
	}

	CGELookupFilter* createLookupFilter()
	{
		COMMON_FUNC(CGELookupFilter);
	}
}
