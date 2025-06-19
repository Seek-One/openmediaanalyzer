#include <QDebug>

#include "../Codec/H264/H264AccessUnit.h"
#include "../Codec/H264/H264Slice.h"
#include "../Codec/H264/H264NAL.h"
#include "../Codec/H265/H265AccessUnit.h"
#include "../Codec/H265/H265Slice.h"
#include "../Codec/H265/H265NAL.h"

#include "QAccessUnitModel.h"

QAccessUnitModel::QAccessUnitModel(const H264AccessUnit* pAccessUnit, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(std::nullopt), m_status(Status_OK), m_sliceType(SliceType_Unspecified), m_id(id), decoded(false){
    setDisplayedFrameNumber();
    setSliceType();
    setStatus();
}

QAccessUnitModel::QAccessUnitModel(const H265AccessUnit* pAccessUnit, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(std::nullopt), m_status(Status_OK), m_sliceType(SliceType_Unspecified), m_id(id), decoded(false){
    setDisplayedFrameNumber();
    setSliceType();
    setStatus();
}

QAccessUnitModel::~QAccessUnitModel(){
}

uint8_t* QAccessUnitModel::serialize() const{
    uint8_t* serialized = nullptr;
    uint32_t index = 0;
    if(isH264()){
        const H264AccessUnit* pAccessUnit = std::get<const H264AccessUnit*>(m_pAccessUnit);
        serialized = new uint8_t[pAccessUnit->byteSize()];
        for(H264NAL* pNALUnit : pAccessUnit->getNALUnits()){
            memcpy(serialized+index, pNALUnit->nal_data, pNALUnit->nal_size);
            index += pNALUnit->nal_size;
        }
    } else if(isH265()){
        const H265AccessUnit* pAccessUnit = std::get<const H265AccessUnit*>(m_pAccessUnit);
        serialized = new uint8_t[pAccessUnit->byteSize()];
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

void QAccessUnitModel::setStatus(){
    if(m_status >= Status_REFERENCED_IFRAME_MISSING && m_status <= Status_OUT_OF_ORDER) return;
    if(isH264()){
        if(std::get<const H264AccessUnit*>(m_pAccessUnit)->hasMajorErrors()) m_status = Status_INVALID_STRUCTURE;
        else if(std::get<const H264AccessUnit*>(m_pAccessUnit)->hasMinorErrors()) m_status = Status_NON_CONFORMING;
        else m_status = Status_OK;
    } else if(isH265()){
        if(std::get<const H265AccessUnit*>(m_pAccessUnit)->hasMajorErrors()) m_status = Status_INVALID_STRUCTURE;
        else if(std::get<const H265AccessUnit*>(m_pAccessUnit)->hasMinorErrors()) m_status = Status_NON_CONFORMING;
        else m_status = Status_OK;
    } else m_status = Status_INVALID_STRUCTURE;
}

bool QAccessUnitModel::isH264() const {
    return std::holds_alternative<const H264AccessUnit*>(m_pAccessUnit);
}

bool QAccessUnitModel::isH265() const {
    return std::holds_alternative<const H265AccessUnit*>(m_pAccessUnit);
}

uint64_t QAccessUnitModel::size() const {
    if(isH264()) return std::get<const H264AccessUnit*>(m_pAccessUnit)->byteSize();
    else if(isH265()) return std::get<const H265AccessUnit*>(m_pAccessUnit)->byteSize();
    return 0;
}