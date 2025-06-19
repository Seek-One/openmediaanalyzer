#include <iostream>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <sstream>

#include "H265AccessUnit.h"
#include "H265Slice.h"

#include "H265GOP.h"

H265GOP::H265GOP():
    hasIDR(false), hasSlice(false){
}

H265GOP::~H265GOP(){
    accessUnits.clear();
}

void H265GOP::setAccessUnitDecodability(){
    switch(accessUnits.back()->slice()->slice_type){
        case H265Slice::SliceType_I:
        case H265Slice::SliceType_P: {
            accessUnits.back()->decodable = true;
            uint32_t previousNonBFrameIndex = size()-1;
            for(int i = size()-2;i >= 0 && previousNonBFrameIndex == size()-1;--i){
                switch(accessUnits[i]->slice()->slice_type){
                    case H265Slice::SliceType_B:
                        accessUnits[i]->decodable = true;
                        break;
                    case H265Slice::SliceType_I:
                    case H265Slice::SliceType_P:
                        previousNonBFrameIndex = i;
                        break;
                    default:
                        break;
                }
            }
            if(previousNonBFrameIndex == size()-1) return; // No other frames;
            if(previousNonBFrameIndex == size()-2) return; // No B-frames;
            // std::sort(accessUnits.begin()+previousNonBFrameIndex, accessUnits.end()-1, [](const std::unique_ptr<H265AccessUnit>& lhs, const std::unique_ptr<H265AccessUnit>& rhs){
            //     return lhs->PicOrderCntVal < rhs->PicOrderCntVal;
            // });
            break;
        }
        case H265Slice::SliceType_B:
            break; // future frames required
        default:
            break;
    }
}

bool H265GOP::empty() const {
    return accessUnits.empty();
}

uint32_t H265GOP::size() const {
    return accessUnits.size();
}

uint64_t H265GOP::byteSize() const {
    return std::accumulate(accessUnits.begin(), accessUnits.end(), 0, [](uint64_t acc, const std::unique_ptr<H265AccessUnit>& accessUnit){
        return acc + accessUnit->byteSize();
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
    uint16_t maxFrameNumber = 0;
    uint16_t minFrameNumber = 0;
    if(accessUnits.front()->slice()) minFrameNumber = maxFrameNumber = accessUnits.front()->slice()->slice_pic_order_cnt_lsb;
    uint16_t lastNonBFrameNumber = 0;
    std::unordered_set<uint32_t> seenFrameNumbers;
    for(const std::unique_ptr<H265AccessUnit>& accessUnit : accessUnits){
        accessUnit->validate();
        if(accessUnit->empty() || !accessUnit->slice()) continue;
        const H265Slice* pSlice = accessUnit->slice();
        if(!pSlice->getPPS() || !pSlice->getSPS() || !pSlice->getVPS()) continue;
        if(pSlice->slice_pic_order_cnt_lsb > maxFrameNumber) maxFrameNumber = pSlice->slice_pic_order_cnt_lsb;
        if(pSlice->slice_pic_order_cnt_lsb < minFrameNumber) minFrameNumber = pSlice->slice_pic_order_cnt_lsb;
        if(pSlice->slice_type == H265Slice::SliceType_I) encounteredIFrame = true;
        if(!encounteredIFrame) accessUnit->majorErrors.push_back("No reference I-frame");
        if(pSlice->slice_type == H265Slice::SliceType_B){
            if(pSlice->slice_pic_order_cnt_lsb > lastNonBFrameNumber) majorErrors.push_back("[GOP] Out of order frames detected");
        } else {
            lastNonBFrameNumber = pSlice->slice_pic_order_cnt_lsb;
            if(pSlice->slice_pic_order_cnt_lsb < prevFrameNumber && (uint32_t)(prevFrameNumber + 1)%pSlice->getSPS()->computeMaxFrameNumber() != pSlice->slice_pic_order_cnt_lsb) {
                majorErrors.push_back("[GOP] Out of order frames detected");
            }
        }
        prevFrameNumber = pSlice->slice_pic_order_cnt_lsb;
        seenFrameNumbers.insert(pSlice->slice_pic_order_cnt_lsb);
    }
    std::unordered_set<uint32_t> missingFrameNumbers;
    for(const std::unique_ptr<H265AccessUnit>& accessUnit : accessUnits){
        for(const H265Slice* pSlice : accessUnit->slices()){
            for(uint32_t referencedFrameNumber : pSlice->PocStCurrAfter){
                if(referencedFrameNumber < minFrameNumber || referencedFrameNumber > maxFrameNumber) continue;
                if(seenFrameNumbers.find(referencedFrameNumber) == seenFrameNumbers.end()) missingFrameNumbers.insert(referencedFrameNumber);
            }
        }
    }
    if(!missingFrameNumbers.empty()){
        std::ostringstream skippedFramesStr;
        skippedFramesStr << "[GOP] Skipped frames detected : [" << (*missingFrameNumbers.begin());
        auto skippedFramesIt = missingFrameNumbers.begin();
        skippedFramesIt++;
        for(;skippedFramesIt != missingFrameNumbers.end();skippedFramesIt++) skippedFramesStr << (*skippedFramesIt) << ", ";
        skippedFramesStr << "]";
        majorErrors.push_back(skippedFramesStr.str());
    } 
    if(!encounteredIFrame) majorErrors.push_back("[GOP] No I-frame detected");
}

bool H265GOP::hasMajorErrors() const {
    return !majorErrors.empty() || std::any_of(accessUnits.begin(), accessUnits.end(), [](const std::unique_ptr<H265AccessUnit>& accessUnit){
        return accessUnit->hasMajorErrors();
    });
}

bool H265GOP::hasMinorErrors() const {
    return !minorErrors.empty() || std::any_of(accessUnits.begin(), accessUnits.end(), [](const std::unique_ptr<H265AccessUnit>& accessUnit){
        return accessUnit->hasMinorErrors();
    });
}