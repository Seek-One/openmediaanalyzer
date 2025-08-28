#include <algorithm>
#include <numeric>

#include "../H26X/H26XUtils.h"
#include "H265NALUnit.h"
#include "H265Slice.h"
#include "H265PPS.h"
#include "H265SPS.h"

#include "H265AccessUnit.h"

H265AccessUnit::H265AccessUnit():
    decodable(false)
{
}

H265AccessUnit::~H265AccessUnit(){
    NALUnits.clear();
}

H265Slice* H265AccessUnit::slice() const{
    auto sliceIt = std::find_if(NALUnits.begin(), NALUnits.end(), [](const std::unique_ptr<H265NALUnit>& NALUnit){
        return NALUnit->getNALHeader()->isSlice();
    });
    if(sliceIt != NALUnits.end()) return reinterpret_cast<H265Slice*>(sliceIt->get());
    return nullptr;
}

std::vector<H265Slice*> H265AccessUnit::slices() const{
    std::vector<H265Slice*> pSlices;
    for(const std::unique_ptr<H265NALUnit>& NALUnit : NALUnits){
        if(NALUnit->getNALHeader()->isSlice()){
			pSlices.push_back(reinterpret_cast<H265Slice*>(NALUnit.get()));
		}
    }
    return pSlices;
}

bool H265AccessUnit::empty() const{
    return NALUnits.empty();
}

std::vector<H265NALUnit*> H265AccessUnit::getNALUnits() const{
    std::vector<H265NALUnit*> pNALUnits;
    std::transform(NALUnits.begin(), NALUnits.end(), std::back_inserter(pNALUnits), [](const std::unique_ptr<H265NALUnit>& pNALUnit){
        return pNALUnit.get();
    });
    return pNALUnits;
}

void H265AccessUnit::addNALUnit(std::unique_ptr<H265NALUnit> NALUnit){
    NALUnit->validate();
    NALUnits.push_back(std::move(NALUnit));
}

uint32_t H265AccessUnit::size() const{
    return NALUnits.size();
}

uint64_t H265AccessUnit::byteSize() const{
    return std::accumulate(NALUnits.begin(), NALUnits.end(), 0, [](uint64_t acc, const std::unique_ptr<H265NALUnit>& NALUnit){
        return acc + NALUnit->nal_size;
    });
}

std::optional<uint16_t> H265AccessUnit::frameNumber() const{
    H265Slice* pSlice = slice();
    if(!pSlice) return std::nullopt;
    return pSlice->slice_pic_order_cnt_lsb;
}

bool H265AccessUnit::isIRAP() const
{
    H265Slice* pSlice = slice();
    if(!pSlice){
		return false;
	}
    return pSlice->getNALHeader()->isIRAP();
}

bool H265AccessUnit::isIDR() const {
    H265Slice* pSlice = slice();
    if(!pSlice) return false;
    return pSlice->getNALHeader()->isIDR();
}

bool H265AccessUnit::isRASL() const {
    H265Slice* pSlice = slice();
    if(!pSlice){
		return false;
	}
    return pSlice->getNalUnitType() == H265NALUnitType::RASL_N || pSlice->getNalUnitType() == H265NALUnitType::RASL_R;
}

bool H265AccessUnit::isRADL() const {
    H265Slice* pSlice = slice();
    if(!pSlice) return false;
    return pSlice->getNalUnitType() == H265NALUnitType::RADL_N || pSlice->getNalUnitType() == H265NALUnitType::RADL_R;
}

bool H265AccessUnit::isSLNR() const {
    H265Slice* pSlice = slice();
    if(!pSlice){
		return false;
	}
    switch(pSlice->getNalUnitType()){
	case H265NALUnitType::TRAIL_N:
	case H265NALUnitType::TSA_N:
	case H265NALUnitType::STSA_N:
	case H265NALUnitType::RADL_N:
	case H265NALUnitType::RASL_N:
		return true;
	default:
		return false;
    }
    return false;
}

void H265AccessUnit::validate(){
    errors.clear();
    std::vector<H265Slice*> pSlices = slices();
    if(pSlices.empty()){
		return;
	}
    // header-related checks
    if(pSlices.size() > 1){
        if(std::any_of(pSlices.begin(), pSlices.end(), [pSlices](H265Slice* pSlice){
            return pSlice->getNalUnitType() != pSlices.front()->getNalUnitType();
        })){
			errors.add(H26XError::Major, "Differing nal_unit_type values detected");
		}
        if(std::any_of(pSlices.begin(), pSlices.end(), [pSlices](H265Slice* pSlice){
            return pSlice->getNalUnitType() != pSlices.front()->getNalUnitType();
        })){
			errors.add(H26XError::Major, "Differing nuh_layer_id values detected");
		}
    }
    for(auto& pNALUnit : NALUnits){
        if(pNALUnit->getNALHeader()->isSlice()){
			continue;
		}

		auto FrontTemporalId = pSlices.front()->getNALHeader()->TemporalId;
		auto TemporalId = pNALUnit->getNALHeader()->TemporalId;

        switch(pNALUnit->getNalUnitType()){
		case H265NALUnitType::VPS:
		case H265NALUnitType::SPS:
			if(FrontTemporalId != 0){
				errors.add(H26XError::Minor, "TemporalId of access unit not equal to 0 as constrained by SPS/VPS unit");
			}
			break;
		case H265NALUnitType::EOS_NUT:
		case H265NALUnitType::EOB_NUT:
			break;
		case H265NALUnitType::AUD:
		case H265NALUnitType::FD_NUT:
			if(TemporalId != FrontTemporalId){
				errors.add(H26XError::Minor, "TemporalId of AUD/FD unit not equal to TemporalId of access unit");
			}
			break;
		default:
			if(TemporalId < FrontTemporalId){
				errors.add(H26XError::Minor, "TemporalId of non-VCL unit lesser than TemporalId of access unit");
			}
			break;
        }
    }
    // order-related checks
    uint32_t lastVCLIndex = NALUnits.size();
    for(uint32_t i = 0;i < NALUnits.size();++i){
		if(NALUnits[i]->getNALHeader()->isSlice()){
			lastVCLIndex = i;
		}
	}
    for(uint32_t i = lastVCLIndex+1;i < NALUnits.size();++i){
		auto nal_unit_type = NALUnits[i]->getNalUnitType();
        switch(nal_unit_type){
		case H265NALUnitType::VPS:
		case H265NALUnitType::SPS:
		case H265NALUnitType::PPS:
		case H265NALUnitType::SEI_PREFIX:
			errors.add(H26XError::Minor, H26XUtils::formatString("NAL unit of type {} found after last VCL unit", nal_unit_type));
		default:
			continue;
        }
    }
    // VCL units order-related checks
    if(!pSlices.front()->first_slice_segment_in_pic_flag){
		errors.add(H26XError::Minor, "first_slice_segment_in_pic_flag not set for first VCL unit");
	}
}

bool H265AccessUnit::isValid() const{
    return !hasMajorErrors() && !hasMinorErrors();
}

bool H265AccessUnit::hasMajorErrors() const{
    return !errors.hasMajorErrors() || std::any_of(NALUnits.begin(), NALUnits.end(), [](const std::unique_ptr<H265NALUnit>& NALUnit){
        return !NALUnit->errors.hasMajorErrors();
    });
}

bool H265AccessUnit::hasMinorErrors() const{
    return !errors.hasMinorErrors() || std::any_of(NALUnits.begin(), NALUnits.end(), [](const std::unique_ptr<H265NALUnit>& NALUnit){
        return !NALUnit->errors.hasMinorErrors();
    });
}
