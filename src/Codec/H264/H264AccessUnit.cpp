#include <iostream>
#include <algorithm>
#include <numeric>

#include "H264NAL.h"
#include "H264Slice.h"
#include "H264SEI.h"
#include "H264PPS.h"
#include "H264SPS.h"
#include "H264AUD.h"

#include "H264AccessUnit.h"

H264AccessUnit::H264AccessUnit():
    decodable(false), hasFrameGaps(false), rpm(RPM_Unused),
    TopFieldOrderCnt(0), BottomFieldOrderCnt(0), PicOrderCntMsb(0), PicOrderCntLsb(0), PicOrderCnt(0),
    FrameNum(0), FrameNumWrap(0), PicNum(0), LongTermFrameIdx(0), LongTermPicNum(0)
{
}

H264AccessUnit::~H264AccessUnit(){
    NALUnits.clear();
}

bool H264AccessUnit::empty() const{
    return NALUnits.empty();
}

std::vector<H264NAL*> H264AccessUnit::getNALUnits() const{
    std::vector<H264NAL*> pNALUnits;
    std::transform(NALUnits.begin(), NALUnits.end(), std::back_inserter(pNALUnits), [](const std::unique_ptr<H264NAL>& pNALUnit){
        return pNALUnit.get();
    });
    return pNALUnits;
}

void H264AccessUnit::addNALUnit(std::unique_ptr<H264NAL> NALUnit){
    NALUnit->validate();
    NALUnits.push_back(std::move(NALUnit));
}

uint32_t H264AccessUnit::size() const{
    return NALUnits.size();
}

uint64_t H264AccessUnit::byteSize() const{
    return std::accumulate(NALUnits.begin(), NALUnits.end(), 0, [](uint64_t acc, const std::unique_ptr<H264NAL>& unit){
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

std::vector<H264Slice*> H264AccessUnit::slices() const{
    std::vector<H264Slice*> pSlices;
    for(const std::unique_ptr<H264NAL>& NALUnit : NALUnits){
        if(H264Slice::isSlice(NALUnit.get())) pSlices.push_back(reinterpret_cast<H264Slice*>(NALUnit.get()));
    }
    return pSlices;
}

H264Slice* H264AccessUnit::primary_coded_slice() const{
    for(auto& NALUnit : NALUnits){
        if(H264Slice::isSlice(NALUnit.get())){
            H264Slice* pSlice = reinterpret_cast<H264Slice*>(NALUnit.get());
            if(pSlice->redundant_pic_cnt == 0) return pSlice;
        }
    }
    return nullptr;
}

H264NAL* H264AccessUnit::last() const{
    if(empty()) return nullptr;
    return NALUnits.back().get();
}

void H264AccessUnit::validate()
{
    errors.clear();
    H264Slice* pPrimarySlice = primary_coded_slice();
    if(!pPrimarySlice){
		errors.add(H26XError::Major, "No primary coded picture detected");
	}

    int AUDs = 0;
    H264AUD* AUDUnit = nullptr;
    for(auto& NALUnit : NALUnits){
		if(NALUnit->nal_unit_type == H264NAL::UnitType_AUD) {
			if (!AUDUnit) {
				AUDUnit = reinterpret_cast<H264AUD *>(NALUnit.get());
			}
			++AUDs;
		}
    }
    if(AUDs > 0){
        if(NALUnits[0]->nal_unit_type != H264NAL::UnitType_AUD){
			errors.add(H26XError::Minor, "Access unit delimiter not in first position");
		}
        if(AUDs > 1){
			errors.add(H26XError::Minor, "Multiple access unit delimiters detected");
		}
        std::vector<uint8_t> allowedSliceTypes = H264AUD::slice_type_values[AUDUnit->primary_pic_type];
        for(auto& NALUnit : NALUnits){
            if(H264Slice::isSlice(NALUnit.get()) && std::find(allowedSliceTypes.begin(), allowedSliceTypes.end(), reinterpret_cast<H264Slice*>(NALUnit.get())->slice_type-1) == allowedSliceTypes.end())
			{
                errors.add(H26XError::Major, "Slice type not in values allowed by access unit delimiter");
            }
        }
    }

    for(uint32_t i = 0;i < NALUnits.size();++i){
        if(NALUnits[i]->nal_unit_type == H264NAL::UnitType_SEI){
            if(i+1 < NALUnits.size()){
                if(NALUnits[i+1]->nal_unit_type != H264NAL::UnitType_SEI && !H264Slice::isSlice(NALUnits[i+1].get())){
                    errors.add(H26XError::Minor, "SEI units block is not preceding the primary coded picture");
                }
            }
            H264SEI* SEIUnit = reinterpret_cast<H264SEI*>(NALUnits[i].get());
            for(uint32_t j = 0;j < SEIUnit->messages.size();++j){
                if(SEIUnit->messages[j]->payloadType == SEI_BUFFERING_PERIOD && j != 0){
                    errors.add(H26XError::Minor, "SEI buffering period message not leading SEI unit");
                }
            }
        }
    }

    int lastSliceRedundantPicCnt = -1;
    for(uint32_t i = 0;i < NALUnits.size();++i){
        if(H264Slice::isSlice(NALUnits[i].get())){
            H264Slice* pSlice = reinterpret_cast<H264Slice*>(NALUnits[i].get());
            if(pSlice->getPPS() && !pSlice->getPPS()->redundant_pic_cnt_present_flag){
                lastSliceRedundantPicCnt = -1;
                continue;
            }
            if((int)pSlice->redundant_pic_cnt <= lastSliceRedundantPicCnt){
				errors.add(H26XError::Minor, "Pictures are not ordered in ascending order of redundant_pic_cnt");
			}
            lastSliceRedundantPicCnt = (int)pSlice->redundant_pic_cnt;
        }
    }
}

bool H264AccessUnit::isValid() const
{
    return !hasMajorErrors() && !hasMinorErrors();
}

bool H264AccessUnit::hasMajorErrors() const
{
    return !errors.hasMajorErrors() || std::any_of(NALUnits.begin(), NALUnits.end(), [](const std::unique_ptr<H264NAL>& NALUnit){
        return !NALUnit->errors.hasMajorErrors();
    });
}

bool H264AccessUnit::hasMinorErrors() const
{
    return !errors.hasMinorErrors() || std::any_of(NALUnits.begin(), NALUnits.end(), [](const std::unique_ptr<H264NAL>& NALUnit){
        return !NALUnit->errors.hasMinorErrors();
    });
}

bool H264AccessUnit::hasNonReferencePicture() const {
    return std::any_of(NALUnits.begin(), NALUnits.end(), [](const std::unique_ptr<H264NAL>& NALUnit){
        return (NALUnit->nal_unit_type == H264NAL::UnitType_IDRFrame || NALUnit->nal_unit_type == H264NAL::UnitType_NonIDRFrame) && NALUnit->nal_ref_idc == 0;
    });
}

bool H264AccessUnit::hasReferencePicture() const {
    return std::any_of(NALUnits.begin(), NALUnits.end(), [](const std::unique_ptr<H264NAL>& NALUnit){
        return (NALUnit->nal_unit_type == H264NAL::UnitType_IDRFrame || NALUnit->nal_unit_type == H264NAL::UnitType_NonIDRFrame) && NALUnit->nal_ref_idc != 0;
    });
}