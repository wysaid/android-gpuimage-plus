//
//  CustomHelper.h
//  filterLib
//
//  Created by wysaid on 16/1/11.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#ifndef CommonHelper_h
#define CommonHelper_h

#include "cgeGLFunctions.h"
#include "cgeCustomFilters.h"

namespace CGE
{
    CGEImageFilterInterface* cgeCreateCustomFilterByType(CustomFilterType type);
}

#endif
