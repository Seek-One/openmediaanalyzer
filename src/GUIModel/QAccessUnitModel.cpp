#include <QDebug>

#include "../Codec/H264/H264AccessUnit.h"

#include "QAccessUnitModel.h"

QAccessUnitModel::QAccessUnitModel(const H264AccessUnit* pAccessUnit, uint displayedFrameNum) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(displayedFrameNum), m_status(Status::OK){
}

QAccessUnitModel::QAccessUnitModel(const H264AccessUnit* pAccessUnit) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(std::nullopt), m_status(Status::OK){
    setDisplayedFrameNumber();
}

QAccessUnitModel::QAccessUnitModel(const H264AccessUnit* pAccessUnit, Status status) 
    : m_pAccessUnit(pAccessUnit), m_displayedFrameNum(std::nullopt), m_status(status){
    setDisplayedFrameNumber();
}

QAccessUnitModel::~QAccessUnitModel(){
}

void QAccessUnitModel::setDisplayedFrameNumber(){
    if(m_pAccessUnit == nullptr) return;
    m_displayedFrameNum = m_pAccessUnit->frameNumber();
}