/*
 * cgeAdvancedEffects.cpp
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 */

#include "cgeAdvancedEffects.h"

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
	CGEEmbossFilter* createEmbossFilter()
	{
		COMMON_FUNC(CGEEmbossFilter);
	}

	CGEEdgeFilter* createEdgeFilter()
	{
		COMMON_FUNC(CGEEdgeFilter);
	}

	CGEEdgeSobelFilter* createEdgeSobelFilter()
	{
		COMMON_FUNC(CGEEdgeSobelFilter);
	}

	CGERandomBlurFilter* createRandomBlurFilter()
	{
		COMMON_FUNC(CGERandomBlurFilter);
	}
	
	CGEBilateralBlurFilter* createBilateralBlurFilter()
	{
		COMMON_FUNC(CGEBilateralBlurFilter);
	}

    CGEBilateralBlurBetterFilter* createBilateralBlurBetterFilter()
    {
        COMMON_FUNC(CGEBilateralBlurBetterFilter);
    }

	CGEMosaicBlurFilter* createMosaicBlurFilter()
	{
		COMMON_FUNC(CGEMosaicBlurFilter);
	}
	
	CGELiquidationFilter* getLiquidationFilter(float ratio, float stride)
	{
		CGELiquidationFilter* proc = new CGELiquidationFilter;
		if(!proc->initWithMesh(ratio, stride))
		{
			delete proc;
			return nullptr;
		}
		return proc;
	}

	CGELiquidationFilter* getLiquidationFilter(float width, float height, float stride)
	{
		CGELiquidationFilter* proc = new CGELiquidationFilter;
		if(!proc->initWithMesh(width, height, stride))
		{
			delete proc;
			return nullptr;
		}
		return proc;
	}

	CGELiquidationNicerFilter* getLiquidationNicerFilter(float ratio, float stride)
	{
		CGELiquidationNicerFilter* proc = new CGELiquidationNicerFilter;
		if(!proc->initWithMesh(ratio, stride))
		{
			delete proc;
			return nullptr;
		}
		return proc;
	}

	CGELiquidationNicerFilter* getLiquidationNicerFilter(float width, float height, float stride)
	{
		CGELiquidationNicerFilter* proc = new CGELiquidationNicerFilter;
		if(!proc->initWithMesh(width, height, stride))
		{
			delete proc;
			return nullptr;
		}
		return proc;
	}

	CGEHalftoneFilter* createHalftoneFilter()
	{
		COMMON_FUNC(CGEHalftoneFilter);
	}

	CGEPolarPixellateFilter* createPolarPixellateFilter()
	{
		COMMON_FUNC(CGEPolarPixellateFilter);
	}

	CGEPolkaDotFilter* createPolkaDotFilter()
	{
		COMMON_FUNC(CGEPolkaDotFilter);
	}

	CGECrosshatchFilter* createCrosshatchFilter()
	{
		COMMON_FUNC(CGECrosshatchFilter);
	}

	CGEHazeFilter* createHazeFilter()
	{
		COMMON_FUNC(CGEHazeFilter);
	}

	CGELerpblurFilter* createLerpblurFilter()
	{
		COMMON_FUNC(CGELerpblurFilter);
	}

	CGESketchFilter* createSketchFilter()
	{
		COMMON_FUNC(CGESketchFilter);
	}

    CGEBeautifyFilter* createBeautifyFilter()
    {
        COMMON_FUNC(CGEBeautifyFilter);
    }
    
 }
