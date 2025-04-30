#include <QDebug>

#include "../Codec/H264/H264AccessUnit.h"
#include "../Codec/H264/H264Slice.h"
#include "../Codec/H264/H264NAL.h"
#include "../Codec/H265/H265AccessUnit.h"
#include "../Codec/H265/H265Slice.h"
#include "../Codec/H265/H265NAL.h"

#include "QAccessUnitModel.h"

QAccessUnitModel::QAccessUnitModel(const H264AccessUnit* pAccessUnit, uint displayedFrameNum, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(displayedFrameNum), m_status(Status::OK), m_id(id), m_sliceType(SliceType_Unspecified){
    setSliceType();
}

QAccessUnitModel::QAccessUnitModel(const H264AccessUnit* pAccessUnit, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(std::nullopt), m_status(Status::OK), m_id(id), m_sliceType(SliceType_Unspecified){
    setDisplayedFrameNumber();
    setSliceType();
}

QAccessUnitModel::QAccessUnitModel(const H264AccessUnit* pAccessUnit, Status status, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(std::nullopt), m_status(status), m_id(id), m_sliceType(SliceType_Unspecified){
    setDisplayedFrameNumber();
    setSliceType();
}

QAccessUnitModel::QAccessUnitModel(const H265AccessUnit* pAccessUnit, uint displayedFrameNum, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(displayedFrameNum), m_status(Status::OK), m_id(id), m_sliceType(SliceType_Unspecified){
    setSliceType();
}

QAccessUnitModel::QAccessUnitModel(const H265AccessUnit* pAccessUnit, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(std::nullopt), m_status(Status::OK), m_id(id), m_sliceType(SliceType_Unspecified){
    setDisplayedFrameNumber();
    setSliceType();
}

QAccessUnitModel::QAccessUnitModel(const H265AccessUnit* pAccessUnit, Status status, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(std::nullopt), m_status(status), m_id(id), m_sliceType(SliceType_Unspecified){
    setDisplayedFrameNumber();
    setSliceType();
}

QAccessUnitModel::~QAccessUnitModel(){
}

uint8_t* QAccessUnitModel::serialize() const{
    uint8_t* serialized = nullptr;
    uint32_t index = 0;
    if(isH264()){
        const H264AccessUnit* pAccessUnit = std::get<const H264AccessUnit*>(m_pAccessUnit);
        serialized = new uint8_t[pAccessUnit->size()];
        for(H264NAL* pNALUnit : pAccessUnit->getNALUnits()){
            memcpy(serialized+index, pNALUnit->nal_data, pNALUnit->nal_size);
            index += pNALUnit->nal_size;
        }
    } else if(isH265()){
        const H265AccessUnit* pAccessUnit = std::get<const H265AccessUnit*>(m_pAccessUnit);
        serialized = new uint8_t[pAccessUnit->size()];
        for(H265NAL* pNALUnit : pAccessUnit->getNALUnits()){
            memcpy(serialized+index, pNALUnit->nal_data, pNALUnit->nal_size);
            index += pNALUnit->nal_size;
        }
    }
    return serialized;
}

void QAccessUnitModel::setDisplayedFrameNumber(){
    if(isH264()) m_displayedFrameNum = std::get<const H264AccessUnit*>(m_pAccessUnit)->frameNumber();
    else if(isH265()) m_displayedFrameNum = std::get<const H265AccessUnit*>(m_pAccessUnit)->frameNumber();
}

void QAccessUnitModel::setSliceType(){
    if(isH264()){
        if(std::get<const H264AccessUnit*>(m_pAccessUnit)->slice()){
            switch(std::get<const H264AccessUnit*>(m_pAccessUnit)->slice()->slice_type % 5){
                case H264Slice::SliceType_B:
                m_sliceType = SliceType_B;
                break;
                case H264Slice::SliceType_P:        
                m_sliceType = SliceType_P;
                break;
                case H264Slice::SliceType_I:
                m_sliceType = SliceType_I;
                break;
                default: break;
            }
        }
    } else if(isH265()){
        if(std::get<const H265AccessUnit*>(m_pAccessUnit)->slice()){
            switch(std::get<const H265AccessUnit*>(m_pAccessUnit)->slice()->slice_type % 5){
                case H265Slice::SliceType_B:
                    m_sliceType = SliceType_B;
                    break;
                case H265Slice::SliceType_P:        
                    m_sliceType = SliceType_P;
                    break;
                case H265Slice::SliceType_I:
                    m_sliceType = SliceType_I;
                    break;
                default: break;
            }
        }
    }
}

bool QAccessUnitModel::isH264() const {
    return std::holds_alternative<const H264AccessUnit*>(m_pAccessUnit);
}

bool QAccessUnitModel::isH265() const {
    return std::holds_alternative<const H265AccessUnit*>(m_pAccessUnit);
}

uint64_t QAccessUnitModel::size() const {
    if(isH264()) return std::get<const H264AccessUnit*>(m_pAccessUnit)->size();
    else if(isH265()) return std::get<const H265AccessUnit*>(m_pAccessUnit)->size();
    return 0;
}