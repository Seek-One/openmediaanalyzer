#include <iostream>
#include <algorithm>
#include <numeric>

#include "H264NAL.h"
#include "H264Slice.h"
#include "H264SEI.h"
#include "H264PPS.h"
#include "H264SPS.h"
#include "H264AUD.h"
#include "H264PPS.h"

#include "H264AccessUnit.h"

H264AccessUnit::H264AccessUnit(){

}

H264AccessUnit::~H264AccessUnit(){
    NALUnits.clear();
}

bool H264AccessUnit::empty() const{
    return NALUnits.empty();
}

void H264AccessUnit::addNALUnit(std::unique_ptr<H264NAL> NALUnit){
    NALUnits.push_back(std::move(NALUnit));
}

uint32_t H264AccessUnit::count() const{
    return NALUnits.size();
}

uint32_t H264AccessUnit::size() const{
    return std::accumulate(NALUnits.begin(), NALUnits.end(), 0, [](uint32_t acc, const std::unique_ptr<H264NAL>& unit){
        return acc+unit->nal_size;
    });
}

std::optional<uint16_t> H264AccessUnit::frameNumber() const{
    H264Slice* pSlice = slice();
    if(!pSlice || !pSlice->getPPS() || !pSlice->getSPS()) return std::nullopt;
    return pSlice->frame_num;
}

H264Slice* H264AccessUnit::slice() const{
    for(auto& NALUnit : NALUnits){
        if(H264Slice::isSlice(NALUnit.get())){
            return reinterpret_cast<H264Slice*>(NALUnit.get());
        }
    }
    return nullptr;
}

H264Slice* H264AccessUnit::primary_coded_slice() const{
    for(auto& NALUnit : NALUnits){
        if(H264Slice::isSlice(NALUnit.get())){
            H264Slice* slice = reinterpret_cast<H264Slice*>(NALUnit.get());
            if(slice->redundant_pic_cnt == 0) return slice;
        }
    }
    return nullptr;
}

H264NAL* H264AccessUnit::last() const{
    if(empty()) return nullptr;
    return NALUnits.back().get();
}

void H264AccessUnit::validate(){
    errors.clear();
    H264Slice* pPrimarySlice = primary_coded_slice();
    if(!pPrimarySlice) errors.push_back("[H264 Access Unit] No primary coded picture detected");

    int AUDs = 0;
    H264AUD* AUDUnit = nullptr;
    for(auto& NALUnit : NALUnits) if(NALUnit->nal_unit_type == H264NAL::UnitType_AUD) {
        if(!AUDUnit) AUDUnit = reinterpret_cast<H264AUD*>(NALUnit.get());
        ++AUDs;
    }
    if(AUDs > 0){
        if(NALUnits[0]->nal_unit_type != H264NAL::UnitType_AUD) errors.push_back("[H264 Access Unit] Access unit delimiter not in first position");
        if(AUDs > 1) errors.push_back("[H264 Access Unit] Multiple access unit delimiters detected");
        std::vector<uint8_t> allowedSliceTypes = H264AUD::slice_type_values[AUDUnit->primary_pic_type];
        for(auto& NALUnit : NALUnits){
            if(H264Slice::isSlice(NALUnit.get()) &&
            std::find(allowedSliceTypes.begin(), allowedSliceTypes.end(), reinterpret_cast<H264Slice*>(NALUnit.get())->slice_type-1) == allowedSliceTypes.end()){
                errors.push_back("[H264 Access Unit] Slice type not in values allowed by access unit delimiter");
            }
        }
    }

    for(int i = 0;i < NALUnits.size();++i){
        if(NALUnits[i]->nal_unit_type == H264NAL::UnitType_SEI){
            if(i+1 < NALUnits.size()){
                if(NALUnits[i+1]->nal_unit_type != H264NAL::UnitType_SEI && !H264Slice::isSlice(NALUnits[i+1].get())){
                    errors.push_back("[H264 Access Unit] SEI units block is not preceding the primary coded picture");
                }
            }
            H264SEI* SEIUnit = reinterpret_cast<H264SEI*>(NALUnits[i].get());
            for(int j = 0;j < SEIUnit->messages.size();++j){
                if(SEIUnit->messages[j]->payloadType == SEI_BUFFERING_PERIOD && j != 0){
                    errors.push_back("[H264 Access Unit] SEI buffering period message not leading SEI unit");
                }
            }
        }
    }
    int lastSliceRedundantPicCnt = -1;
    for(int i = 0;i < NALUnits.size();++i){
        if(H264Slice::isSlice(NALUnits[i].get())){
            H264Slice* pSlice = reinterpret_cast<H264Slice*>(NALUnits[i].get());
            if(pSlice->redundant_pic_cnt <= lastSliceRedundantPicCnt) errors.push_back("[H264 Access Unit] Pictures are not ordered in ascending order of redundant_pic_cnt");
            lastSliceRedundantPicCnt = pSlice->redundant_pic_cnt;
        }
    }
}