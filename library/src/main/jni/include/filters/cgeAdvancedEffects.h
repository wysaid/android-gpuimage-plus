/*
 * cgeAdvancedEffects.h
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#ifndef _CGEADVANCEDEFFECTS_H_
#define _CGEADVANCEDEFFECTS_H_

#include "cgeEmbossFilter.h"
#include "cgeEdgeFilter.h"
#include "cgeRandomBlurFilter.h"
#include "cgeBilateralBlurFilter.h"
#include "cgeMosaicBlurFilter.h"
#include "cgeEnlargeEyeFilter.h"
#include "cgeEyeTintFilter.h"
#include "cgeEyeBrightenFilter.h"
#include "cgeTeethWhitenFilter.h"
#include "cgeMarscaraFilter.h"
#include "cgeWrinkleRemoverFilter.h"
#include "cgeRedeyeRemoveFilter.h"
#include "cgeLiquidationFilter.h"
#include "cgeLightFilter.h"
#include "cgeHalftoneFilter.h"
#include "cgePolarPixellateFilter.h"
#include "cgePolkaDotFilter.h"
#include "cgeCrosshatchFilter.h"
#include "cgeHazeFilter.h"
#include "cgeLerpblurFilter.h"

#include "cgeSketchFilter.h"
#include "cgeBeautifyFilter.h"

namespace CGE
{
	CGEEmbossFilter* createEmbossFilter();
	CGEEdgeFilter* createEdgeFilter();
	CGEEdgeSobelFilter* createEdgeSobelFilter();
	CGERandomBlurFilter* createRandomBlurFilter();
	CGEBilateralBlurFilter* createBilateralBlurFilter();
    CGEBilateralBlurBetterFilter* createBilateralBlurBetterFilter();
	CGEMosaicBlurFilter* createMosaicBlurFilter();
	CGEEnlargeEyeFilter* createEnlargeEyeFilter();
	CGEEyeTintFilter* createEyeTintFilter();
	CGEEyeBrightenFilter* createEyeBrightenFilter();
	CGETeethWhitenFilter* createTeethWhitenFilter();
	CGEMarscaraFilter* createMarscaraFilter();
	CGEWrinkleRemoveFilter* createWrinkleRemoveFilter();
	CGERedeyeRemoveFilter* createRedeyeRemoveFilter();
	CGELightFilter* createLightFilter();
	CGELiquidationFilter* getLiquidationFilter(float ratio, float stride);
	CGELiquidationFilter* getLiquidationFilter(float width, float height , float stride);

	CGELiquidationNicerFilter* getLiquidationNicerFilter(float ratio, float stride);
	CGELiquidationNicerFilter* getLiquidationNicerFilter(float width, float height , float stride);

	CGEHalftoneFilter* createHalftoneFilter();
	CGEPolarPixellateFilter* createPolarPixellateFilter();
	CGEPolkaDotFilter* createPolkaDotFilter();
	CGECrosshatchFilter* createCrosshatchFilter();
	CGEHazeFilter* createHazeFilter();
	CGELerpblurFilter* createLerpblurFilter();

	CGESketchFilter* createSketchFilter();
    
    CGEBeautifyFilter* createBeautifyFilter();
}

#endif 
