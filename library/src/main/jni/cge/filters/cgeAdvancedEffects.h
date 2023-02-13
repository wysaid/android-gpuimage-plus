/*
 * cgeAdvancedEffects.h
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#ifndef _CGEADVANCEDEFFECTS_H_
#define _CGEADVANCEDEFFECTS_H_

#include "cgeBeautifyFilter.h"
#include "cgeBilateralBlurFilter.h"
#include "cgeCrosshatchFilter.h"
#include "cgeEdgeFilter.h"
#include "cgeEmbossFilter.h"
#include "cgeHalftoneFilter.h"
#include "cgeHazeFilter.h"
#include "cgeLerpblurFilter.h"
#include "cgeLiquifyFilter.h"
#include "cgeMosaicBlurFilter.h"
#include "cgePolarPixellateFilter.h"
#include "cgePolkaDotFilter.h"
#include "cgeRandomBlurFilter.h"
#include "cgeSketchFilter.h"
#include "cgeWaveformFilter.h"

namespace CGE
{
CGEEmbossFilter* createEmbossFilter();
CGEEdgeFilter* createEdgeFilter();
CGEEdgeSobelFilter* createEdgeSobelFilter();
CGERandomBlurFilter* createRandomBlurFilter();
CGEBilateralBlurFilter* createBilateralBlurFilter();
CGEBilateralBlurBetterFilter* createBilateralBlurBetterFilter();
CGEMosaicBlurFilter* createMosaicBlurFilter();
CGELiquifyFilter* getLiquidationFilter(float ratio, float stride);
CGELiquifyFilter* getLiquidationFilter(float width, float height, float stride);

CGELiquidationNicerFilter* getLiquidationNicerFilter(float ratio, float stride);
CGELiquidationNicerFilter* getLiquidationNicerFilter(float width, float height, float stride);

CGEHalftoneFilter* createHalftoneFilter();
CGEPolarPixellateFilter* createPolarPixellateFilter();
CGEPolkaDotFilter* createPolkaDotFilter();
CGECrosshatchFilter* createCrosshatchFilter();
CGEHazeFilter* createHazeFilter();
CGELerpblurFilter* createLerpblurFilter();

CGESketchFilter* createSketchFilter();

CGEBeautifyFilter* createBeautifyFilter();

CGEWaveformFilter* createWaveformFilter();
} // namespace CGE

#endif
