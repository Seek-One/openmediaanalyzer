#pragma once

#include <QtGlobal>
#include <QObject>
#include <optional>



enum Status {
    OK,
    SKIPPED_FRAME,
    MISSING_IFRAME,
    REFERENCED_IFRAME_MISSING,
    REFERENCED_PPS_OR_SPS_MISSING,

    OUT_OF_ORDER

};

struct H264AccessUnit;

class QAccessUnitModel : public QObject 
{
    Q_OBJECT
public: 
    const H264AccessUnit* m_pAccessUnit;
    std::optional<uint16_t> m_displayedFrameNum;
    Status m_status;

    QAccessUnitModel(const H264AccessUnit* pAccessUnit, uint displayedFrameNum);
    QAccessUnitModel(const H264AccessUnit* pAccessUnit);
    QAccessUnitModel(const H264AccessUnit* pAccessUnit, Status status);

    virtual ~QAccessUnitModel();

private:
    void setDisplayedFrameNumber();

};