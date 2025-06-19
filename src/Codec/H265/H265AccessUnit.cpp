#include <iostream>
#include <algorithm>
#include <numeric>

#include "H265NAL.h"
#include "H265Slice.h"
#include "H265PPS.h"
#include "H265SPS.h"
#include "H265PPS.h"

#include "H265AccessUnit.h"

H265AccessUnit::H265AccessUnit():
    decodable(false)
{
}

H265AccessUnit::~H265AccessUnit(){
    NALUnits.clear();
}

H265Slice* H265AccessUnit::slice() const{
    auto sliceIt = std::find_if(NALUnits.begin(), NALUnits.end(), [](const std::unique_ptr<H265NAL>& NALUnit){ 
        return NALUnit->isSlice();
    });
    if(sliceIt != NALUnits.end()) return reinterpret_cast<H265Slice*>(sliceIt->get());
    return nullptr;
}

std::vector<H265Slice*> H265AccessUnit::slices() const{
    std::vector<H265Slice*> pSlices;
    for(const std::unique_ptr<H265NAL>& NALUnit : NALUnits){
        if(NALUnit->isSlice()) pSlices.push_back(reinterpret_cast<H265Slice*>(NALUnit.get()));
    }
    return pSlices;
}

bool H265AccessUnit::empty() const{
    return NALUnits.empty();
}

std::vector<H265NAL*> H265AccessUnit::getNALUnits() const{
    std::vector<H265NAL*> pNALUnits;
    std::transform(NALUnits.begin(), NALUnits.end(), std::back_inserter(pNALUnits), [](const std::unique_ptr<H265NAL>& pNALUnit){
        return pNALUnit.get();
    });
    return pNALUnits;
}

void H265AccessUnit::addNALUnit(std::unique_ptr<H265NAL> NALUnit){
    NALUnit->validate();
    NALUnits.push_back(std::move(NALUnit));
}

uint32_t H265AccessUnit::size() const{
    return NALUnits.size();
}

uint64_t H265AccessUnit::byteSize() const{
    return std::accumulate(NALUnits.begin(), NALUnits.end(), 0, [](uint64_t acc, const std::unique_ptr<H265NAL>& NALUnit){
        return acc + NALUnit->nal_size;
    });
}

std::optional<uint16_t> H265AccessUnit::frameNumber() const{
    H265Slice* pSlice = slice();
    if(!pSlice) return std::nullopt;
    return pSlice->slice_pic_order_cnt_lsb;
}

bool H265AccessUnit::isIRAP() const {
    H265Slice* pSlice = slice();
    if(!pSlice) return false;
    return pSlice->isIRAP();
}

bool H265AccessUnit::isIDR() const {
    H265Slice* pSlice = slice();
    if(!pSlice) return false;
    return pSlice->isIDR();
}

bool H265AccessUnit::isRASL() const {
    H265Slice* pSlice = slice();
    if(!pSlice) return false;
    return pSlice->nal_unit_type == H265NAL::UnitType_RASL_N || pSlice->nal_unit_type == H265NAL::UnitType_RASL_R;
}

bool H265AccessUnit::isRADL() const {
    H265Slice* pSlice = slice();
    if(!pSlice) return false;
    return pSlice->nal_unit_type == H265NAL::UnitType_RADL_N || pSlice->nal_unit_type == H265NAL::UnitType_RADL_R;
}

bool H265AccessUnit::isSLNR() const {
    H265Slice* pSlice = slice();
    if(!pSlice) return false;
    switch(pSlice->nal_unit_type){
        case H265NAL::UnitType_TRAIL_N:
        case H265NAL::UnitType_TSA_N:
        case H265NAL::UnitType_STSA_N:
        case H265NAL::UnitType_RADL_N:
        case H265NAL::UnitType_RASL_N:
            return true;
        default: return false;
    }
    return false;
}

void H265AccessUnit::validate(){
    minorErrors.clear();
    majorErrors.clear();
    std::vector<H265Slice*> pSlices = slices();
    if(pSlices.empty()) return;
    // header-related checks
    if(pSlices.size() > 1){
        if(std::any_of(pSlices.begin(), pSlices.end(), [pSlices](H265Slice* pSlice){
            return pSlice->nal_unit_type != pSlices.front()->nal_unit_type;
        })) majorErrors.push_back("Differing nal_unit_type values detected");
        if(std::any_of(pSlices.begin(), pSlices.end(), [pSlices](H265Slice* pSlice){
            return pSlice->nuh_layer_id != pSlices.front()->nuh_layer_id;
        })) majorErrors.push_back("Differing nuh_layer_id values detected");
    }
    for(auto& NALUnit : NALUnits){
        if(NALUnit->isSlice()) continue;
        switch(NALUnit->nal_unit_type){
            case H265NAL::UnitType_VPS:
            case H265NAL::UnitType_SPS:
                if(pSlices.front()->TemporalId != 0) minorErrors.push_back("TemporalId of access unit not equal to 0 as constrained by SPS/VPS unit");
                break;
            case H265NAL::UnitType_EOS_NUT:
            case H265NAL::UnitType_EOB_NUT:
                break;
            case H265NAL::UnitType_AUD:
            case H265NAL::UnitType_FD_NUT:
                if(NALUnit->TemporalId != pSlices.front()->TemporalId) minorErrors.push_back("TemporalId of AUD/FD unit not equal to TemporalId of access unit");
                break;
            default:
                if(NALUnit->TemporalId < pSlices.front()->TemporalId) minorErrors.push_back("TemporalId of non-VCL unit lesser than TemporalId of access unit");
                break;
        }
    }
    // order-related checks
    uint32_t lastVCLIndex = NALUnits.size();
    for(uint32_t i = 0;i < NALUnits.size();++i) if(NALUnits[i]->isSlice()) lastVCLIndex = i;
    for(uint32_t i = lastVCLIndex+1;i < NALUnits.size();++i){
        switch(NALUnits[i]->nal_unit_type){
            case H265NAL::UnitType_VPS:
            case H265NAL::UnitType_SPS:
            case H265NAL::UnitType_PPS:
            case H265NAL::UnitType_SEI_PREFIX:
                minorErrors.push_back(StringFormatter::formatString("NAL unit of type {} found after last VCL unit", NALUnits[i]->nal_unit_type));
            default: continue;
        }
    }
    // VCL units order-related checks
    if(!pSlices.front()->first_slice_segment_in_pic_flag) minorErrors.push_back("first_slice_segment_in_pic_flag not set for first VCL unit");
}

bool H265AccessUnit::isValid() const{
    return !hasMajorErrors() && !hasMinorErrors();
}

bool H265AccessUnit::hasMajorErrors() const{
    return !majorErrors.empty() || std::any_of(NALUnits.begin(), NALUnits.end(), [](const std::unique_ptr<H265NAL>& NALUnit){
        return !NALUnit->majorErrors.empty();
    });
}

bool H265AccessUnit::hasMinorErrors() const{
    return !minorErrors.empty() || std::any_of(NALUnits.begin(), NALUnits.end(), [](const std::unique_ptr<H265NAL>& NALUnit){
        return !NALUnit->minorErrors.empty();
    });
}
