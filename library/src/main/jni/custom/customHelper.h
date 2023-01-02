//
//  CustomHelper.h
//  filterLib
//
//  Created by wysaid on 16/1/11.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#ifndef CommonHelper_h
#define CommonHelper_h

#include "cgeCustomFilters.h"
#include "cgeGLFunctions.h"

namespace CGE
{
CGEImageFilterInterface* cgeCreateCustomFilterByType(CustomFilterType type);
}

#endif
