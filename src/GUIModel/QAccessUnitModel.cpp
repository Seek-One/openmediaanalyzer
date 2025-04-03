#include <QDebug>

#include "../Codec/H264/H264AccessUnit.h"
#include "../Codec/H264/H264NAL.h"

#include "QAccessUnitModel.h"

QAccessUnitModel::QAccessUnitModel(const H264AccessUnit* pAccessUnit, uint displayedFrameNum, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(displayedFrameNum), m_status(Status::OK), m_id(id), m_frameDecoded(false){
}

QAccessUnitModel::QAccessUnitModel(const H264AccessUnit* pAccessUnit, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(std::nullopt), m_status(Status::OK), m_id(id), m_frameDecoded(false){
    setDisplayedFrameNumber();
}

QAccessUnitModel::QAccessUnitModel(const H264AccessUnit* pAccessUnit, Status status, QUuid id) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(std::nullopt), m_status(status), m_id(id), m_frameDecoded(false){
    setDisplayedFrameNumber();
}

QAccessUnitModel::~QAccessUnitModel(){
}

uint8_t* QAccessUnitModel::serialize() const{
    uint8_t* serialized = new uint8_t[m_pAccessUnit->size()];
    uint32_t index = 0;
    for(H264NAL* pNALUnit : m_pAccessUnit->getNALUnits()){
        memcpy(serialized+index, pNALUnit->nal_data, pNALUnit->nal_size);
        index += pNALUnit->nal_size;
    }
    return serialized;
}

void QAccessUnitModel::setDisplayedFrameNumber(){
    if(m_pAccessUnit == nullptr) return;
    m_displayedFrameNum = m_pAccessUnit->frameNumber();
}