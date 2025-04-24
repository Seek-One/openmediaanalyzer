#include <iostream>
#include <algorithm>
#include <numeric>

#include "H265NAL.h"
#include "H265Slice.h"
#include "H265PPS.h"
#include "H265SPS.h"
#include "H265PPS.h"

#include "H265AccessUnit.h"

H265AccessUnit::H265AccessUnit(){
}

H265AccessUnit::~H265AccessUnit(){
    NALUnits.clear();
}

H265Slice* H265AccessUnit::slice() const{
    auto sliceIt = std::find_if(NALUnits.begin(), NALUnits.end(), [](auto& NALUnit){ 
        return NALUnit->isSlice();
    });
    if(sliceIt != NALUnits.end()) return reinterpret_cast<H265Slice*>(sliceIt->get());
    return nullptr;
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
    NALUnits.push_back(std::move(NALUnit));
}

uint32_t H265AccessUnit::count() const{
    return NALUnits.size();
}

uint64_t H265AccessUnit::size() const{
    return std::accumulate(NALUnits.begin(), NALUnits.end(), 0, [](uint64_t acc, const std::unique_ptr<H265NAL>& NALUnit){
        return acc + NALUnit->nal_size;
    });
}

std::optional<uint16_t> H265AccessUnit::frameNumber() const{
    H265Slice* pSlice = slice();
    if(!pSlice) return std::nullopt;
    return pSlice->slice_pic_order_cnt_lsb;
}

void H265AccessUnit::validate(){}

bool H265AccessUnit::isValid() const{
    return errors.empty();
}
