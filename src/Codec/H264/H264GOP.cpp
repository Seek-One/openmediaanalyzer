#include <iostream>
#include <algorithm>
#include <numeric>

#include "H264AccessUnit.h"
#include "H264Slice.h"

#include "H264GOP.h"

H264GOP::H264GOP():
    hasIDR(false), hasSlice(false)
{
}

H264GOP::~H264GOP(){
    accessUnits.clear();
}

uint32_t H264GOP::count(){
    return accessUnits.size();
}

std::vector<H264AccessUnit*> H264GOP::getAccessUnits(){
    std::vector<H264AccessUnit*> pAccessUnits;
    std::transform(accessUnits.begin(), accessUnits.end(), std::back_inserter(pAccessUnits), [](const std::unique_ptr<H264AccessUnit>& pAccessUnit){
        return pAccessUnit.get();
    });
    return pAccessUnits;
}

std::vector<H264Slice*> H264GOP::slices(){
    std::vector<H264Slice*> pSlices;
    for(auto& access_unit : accessUnits){
        H264Slice* pSlice = access_unit->slice();
        if(pSlice) pSlices.push_back(pSlice);
    }
    return pSlices;
}

