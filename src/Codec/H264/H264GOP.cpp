#include <iostream>
#include <algorithm>
#include <numeric>
#include <unordered_set>

#include "H264AccessUnit.h"
#include "H264Slice.h"
#include "H264SPS.h"

#include "H264GOP.h"

H264GOP::H264GOP():
    hasIDR(false), hasSlice(false)
{
}

H264GOP::~H264GOP(){
    accessUnits.clear();
}

uint32_t H264GOP::size() const {
    return accessUnits.size();
}

uint64_t H264GOP::byteSize() const {
    return std::accumulate(accessUnits.begin(), accessUnits.end(), 0, [](uint64_t acc, const std::unique_ptr<H264AccessUnit>& pAccessUnit){
        return acc + pAccessUnit->byteSize();
    });
}

std::vector<H264AccessUnit*> H264GOP::getAccessUnits() const {
    std::vector<H264AccessUnit*> pAccessUnits;
    std::transform(accessUnits.begin(), accessUnits.end(), std::back_inserter(pAccessUnits), [](const std::unique_ptr<H264AccessUnit>& pAccessUnit){
        return pAccessUnit.get();
    });
    return pAccessUnits;
}

std::vector<H264Slice*> H264GOP::slices() const {
    std::vector<H264Slice*> pSlices;
    for(auto& accessUnit : accessUnits){
        H264Slice* pSlice = accessUnit->slice();
        if(pSlice) pSlices.push_back(pSlice);
    }
    return pSlices;
}

void H264GOP::validate(){
    uint16_t prevFrameNumber = 0;
    bool encounteredIFrame = false;
    bool noSPSorPPS = true;
    uint16_t maxFrameNumber = 0;
    std::unordered_set<uint16_t> seenFrameNumbers;
    for(const std::unique_ptr<H264AccessUnit>& accessUnit : accessUnits){   
        accessUnit->validate();     
        if(accessUnit->empty() || !accessUnit->slice()) continue;
        const H264Slice* pSlice = accessUnit->slice();
        if(pSlice->slice_type == H264Slice::SliceType_I) encounteredIFrame = true;
        if(!pSlice->getPPS() || !pSlice->getSPS()) continue;
        noSPSorPPS = false;
        if(pSlice->frame_num > maxFrameNumber) maxFrameNumber = pSlice->frame_num;
        if(pSlice->frame_num < prevFrameNumber && (prevFrameNumber + 1)%pSlice->getSPS()->computeMaxFrameNumber() != pSlice->frame_num) {
            majorErrors.push_back("[GOP] Out of order frames detected");
        }
        prevFrameNumber = pSlice->frame_num;
        seenFrameNumbers.insert(pSlice->frame_num);
    }
    if(!noSPSorPPS && seenFrameNumbers.size() < maxFrameNumber+1) majorErrors.push_back("[GOP] Skipped frames detected");
    if(!encounteredIFrame) majorErrors.push_back("[GOP] No I-frame detected");
}

bool H264GOP::hasMajorErrors() const {
    return !majorErrors.empty() || std::accumulate(accessUnits.begin(), accessUnits.end(), false, [](bool acc, const std::unique_ptr<H264AccessUnit>& accessUnit){
        return acc || accessUnit->hasMajorErrors();
    });
}

bool H264GOP::hasMinorErrors() const {
    return !minorErrors.empty() || std::accumulate(accessUnits.begin(), accessUnits.end(), false, [](bool acc, const std::unique_ptr<H264AccessUnit>& accessUnit){
        return acc || accessUnit->hasMinorErrors();
    });
}

