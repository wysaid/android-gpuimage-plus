//
//  CustomHelper.cpp
//  filterLib
//
//  Created by wysaid on 16/1/11.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#include "customHelper.h"
#include "customFilter_0.h"

#define CREATE_FILTER(var, Type) \
do{\
var = new Type(); \
if(!var->init()) \
{\
    delete var; \
    var = nullptr; \
}}while(0);

namespace CGE
{
    CGEImageFilterInterface* cgeCreateCustomFilterByType(CustomFilterType type)
    {
        CGEImageFilterInterface* resultFilter = nullptr;
        
        switch (type)
        {
            case CGE_CUSTOM_FILTER_0:
                CREATE_FILTER(resultFilter, CustomFilter_0);
                break;
            default:
                return nullptr;
        }
        
        return resultFilter;
    }
    
}