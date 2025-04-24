#pragma once

#include <QtGlobal>
#include <QObject>
#include <optional>
#include <QUuid>



enum Status {
    OK,
    SKIPPED_FRAME,
    MISSING_IFRAME,
    REFERENCED_IFRAME_MISSING,
    REFERENCED_PPS_OR_SPS_MISSING,
    OUT_OF_ORDER

};

struct H264AccessUnit;
struct H265AccessUnit;

class QAccessUnitModel : public QObject 
{
    Q_OBJECT
public: 

    QAccessUnitModel(const H264AccessUnit* pAccessUnit, uint displayedFrameNum, QUuid id);
    QAccessUnitModel(const H264AccessUnit* pAccessUnit, QUuid id);
    QAccessUnitModel(const H264AccessUnit* pAccessUnit, Status status, QUuid id);
    QAccessUnitModel(const H265AccessUnit* pAccessUnit, uint displayedFrameNum, QUuid id);
    QAccessUnitModel(const H265AccessUnit* pAccessUnit, QUuid id);
    QAccessUnitModel(const H265AccessUnit* pAccessUnit, Status status, QUuid id);

    virtual ~QAccessUnitModel();

    uint8_t* serialize() const;
    void setDisplayedFrameNumber();
    bool isH264() const;
    bool isH265() const;
    const std::variant<const H264AccessUnit*, const H265AccessUnit*> m_pAccessUnit;
    std::optional<uint16_t> m_displayedFrameNum;
    Status m_status;
    QUuid m_id;



};