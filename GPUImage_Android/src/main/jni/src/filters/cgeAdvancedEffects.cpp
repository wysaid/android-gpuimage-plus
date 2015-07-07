/*
 * cgeAdvancedEffects.cpp
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#include "cgeAdvancedEffects.h"

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
	CGEEmbossFilter* getEmbossFilter()
	{
		COMMON_FUNC(p, CGEEmbossFilter);
	}

	cgeEdgeFilter* getEdgeFilter()
	{
		COMMON_FUNC(p, cgeEdgeFilter);
	}

	CGEEdgeSobelFilter* getEdgeSobelFilter()
	{
		COMMON_FUNC(p, CGEEdgeSobelFilter);
	}

	CGERandomBlurFilter* getRandomBlurFilter()
	{
		COMMON_FUNC(p, CGERandomBlurFilter);
	}
	
	CGEBilateralBlurFilter* getBilateralBlurFilter()
	{
		COMMON_FUNC(p, CGEBilateralBlurFilter);
	}

    CGEBilateralBlurBetterFilter* getBilateralBlurBetterFilter()
    {
        COMMON_FUNC(p, CGEBilateralBlurBetterFilter);
    }

	CGEMosaicBlurFilter* getMosaicBlurFilter()
	{
		COMMON_FUNC(p, CGEMosaicBlurFilter);
	}
	
	CGEEnlargeEyeFilter* getEnlargeEyeFilter()
	{
		COMMON_FUNC(p, CGEEnlargeEyeFilter);
	}
	
	CGEEyeTintFilter* getEyeTintFilter()
	{
		COMMON_FUNC(p, CGEEyeTintFilter);
	}

	CGEEyeBrightenFilter* getEyeBrightenFilter()
	{
		COMMON_FUNC(p, CGEEyeBrightenFilter);
	}
	
	CGETeethWhitenFilter* getTeethWhitenFilter()
	{
		COMMON_FUNC(p, CGETeethWhitenFilter);
	}

	CGEMarscaraFilter* getMarscaraFilter()
	{
		COMMON_FUNC(p, CGEMarscaraFilter);
	}

	CGEWrinkleRemoveFilter* getWrinkleRemoveFilter()
	{
		COMMON_FUNC(p, CGEWrinkleRemoveFilter);
	}
	
	CGERedeyeRemoveFilter* getRedeyeRemoveFilter()
	{
		COMMON_FUNC(p, CGERedeyeRemoveFilter);
	}
	
	CGELightFilter* getLightFilter()
	{
		COMMON_FUNC(p, CGELightFilter);
	}

	CGELiquidationFilter* getLiquidationFilter(float ratio, float stride)
	{
		CGELiquidationFilter* proc = new CGELiquidationFilter;
		if(!proc->initWithMesh(ratio, stride))
		{
			delete proc;
			return NULL;
		}
		return proc;
	}

	CGELiquidationFilter* getLiquidationFilter(float width, float height, float stride)
	{
		CGELiquidationFilter* proc = new CGELiquidationFilter;
		if(!proc->initWithMesh(width, height, stride))
		{
			delete proc;
			return NULL;
		}
		return proc;
	}

	CGELiquidationNicerFilter* getLiquidationNicerFilter(float ratio, float stride)
	{
		CGELiquidationNicerFilter* proc = new CGELiquidationNicerFilter;
		if(!proc->initWithMesh(ratio, stride))
		{
			delete proc;
			return NULL;
		}
		return proc;
	}

	CGELiquidationNicerFilter* getLiquidationNicerFilter(float width, float height, float stride)
	{
		CGELiquidationNicerFilter* proc = new CGELiquidationNicerFilter;
		if(!proc->initWithMesh(width, height, stride))
		{
			delete proc;
			return NULL;
		}
		return proc;
	}

	CGEHalftoneFilter* getHalftoneFilter()
	{
		COMMON_FUNC(p, CGEHalftoneFilter);
	}

	CGEPolarPixellateFilter* getPolarPixellateFilter()
	{
		COMMON_FUNC(p, CGEPolarPixellateFilter);
	}

	CGEPolkaDotFilter* getPolkaDotFilter()
	{
		COMMON_FUNC(p, CGEPolkaDotFilter);
	}

	CGECrosshatchFilter* getCrosshatchFilter()
	{
		COMMON_FUNC(p, CGECrosshatchFilter);
	}

	CGEHazeFilter* getHazeFilter()
	{
		COMMON_FUNC(p, CGEHazeFilter);
	}

	CGESketchFilter* getSketchFilter()
	{
		COMMON_FUNC(p, CGESketchFilter);
	}

 }
