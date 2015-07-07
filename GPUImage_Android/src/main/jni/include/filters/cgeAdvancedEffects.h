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

#include "cgeSketchFilter.h"

namespace CGE
{
	CGEEmbossFilter* getEmbossFilter();
	cgeEdgeFilter* getEdgeFilter();
	CGEEdgeSobelFilter* getEdgeSobelFilter();
	CGERandomBlurFilter* getRandomBlurFilter();
	CGEBilateralBlurFilter* getBilateralBlurFilter();
    CGEBilateralBlurBetterFilter* getBilateralBlurBetterFilter();
	CGEMosaicBlurFilter* getMosaicBlurFilter();
	CGEEnlargeEyeFilter* getEnlargeEyeFilter();
	CGEEyeTintFilter* getEyeTintFilter();
	CGEEyeBrightenFilter* getEyeBrightenFilter();
	CGETeethWhitenFilter* getTeethWhitenFilter();
	CGEMarscaraFilter* getMarscaraFilter();
	CGEWrinkleRemoveFilter* getWrinkleRemoveFilter();
	CGERedeyeRemoveFilter* getRedeyeRemoveFilter();
	CGELightFilter* getLightFilter();
	CGELiquidationFilter* getLiquidationFilter(float ratio, float stride);
	CGELiquidationFilter* getLiquidationFilter(float width, float height , float stride);

	CGELiquidationNicerFilter* getLiquidationNicerFilter(float ratio, float stride);
	CGELiquidationNicerFilter* getLiquidationNicerFilter(float width, float height , float stride);

	CGEHalftoneFilter* getHalftoneFilter();
	CGEPolarPixellateFilter* getPolarPixellateFilter();
	CGEPolkaDotFilter* getPolkaDotFilter();
	CGECrosshatchFilter* getCrosshatchFilter();
	CGEHazeFilter* getHazeFilter();

	CGESketchFilter* getSketchFilter();
}

#endif 
