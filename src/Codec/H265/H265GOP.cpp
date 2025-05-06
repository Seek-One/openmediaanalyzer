#include <iostream>
#include <algorithm>
#include <numeric>

#include "H265AccessUnit.h"
#include "H265Slice.h"

#include "H265GOP.h"

H265GOP::H265GOP():
    hasIDR(false), hasSlice(false){
}

H265GOP::~H265GOP(){
    accessUnits.clear();
}

bool H265GOP::empty() const {
    return accessUnits.empty();
}

uint32_t H265GOP::count() const {
    return accessUnits.size();
}

uint64_t H265GOP::size() const {
    return std::accumulate(accessUnits.begin(), accessUnits.end(), 0, [](uint64_t acc, const std::unique_ptr<H265AccessUnit>& accessUnit){
        return acc + accessUnit->size();
    });
}

std::vector<H265AccessUnit*> H265GOP::getAccessUnits() const {
    std::vector<H265AccessUnit*> pAccessUnits;
    std::transform(accessUnits.begin(), accessUnits.end(), std::back_inserter(pAccessUnits), [](const std::unique_ptr<H265AccessUnit>& pAccessUnit){
        return pAccessUnit.get();
    });
    return pAccessUnits;
}

void H265GOP::validate(){
    uint16_t prevFrameNumber = 0;
    bool encounteredIFrame = false;
    bool noSPSorPPS = true;
    uint16_t maxFrameNumber = 0;
    for(const std::unique_ptr<H265AccessUnit>& accessUnit : accessUnits){
        accessUnit->validate();
        if(accessUnit->empty() || !accessUnit->slice()) continue;
        const H265Slice* pSlice = accessUnit->slice();
        if(!pSlice->getPPS() || !pSlice->getSPS()) continue;
        noSPSorPPS = false;
        if(pSlice->slice_pic_order_cnt_lsb > maxFrameNumber) maxFrameNumber = pSlice->slice_pic_order_cnt_lsb;
        if(pSlice->slice_type == H265Slice::SliceType_I) encounteredIFrame = true;
        else if(!encounteredIFrame) accessUnit->errors.push_back("No reference I-frame");
        prevFrameNumber = pSlice->slice_pic_order_cnt_lsb;
    }
    if(maxFrameNumber+1 != count() && !noSPSorPPS) errors.push_back("[GOP] Skipped frames detected");
    if(!encounteredIFrame) errors.push_back("[GOP] No I-frame detected");
}