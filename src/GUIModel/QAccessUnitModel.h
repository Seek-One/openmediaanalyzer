#pragma once

#include <QtGlobal>
#include <QObject>
#include <optional>
#include <QUuid>

struct H264AccessUnit;
struct H265AccessUnit;

class QAccessUnitModel : public QObject 
{
    Q_OBJECT
public: 

    enum Status {
        Status_OK,
        Status_INVALID_STRUCTURE,
        Status_REFERENCED_IFRAME_MISSING,
        Status_REFERENCED_PPS_OR_SPS_MISSING,
        Status_OUT_OF_ORDER,
        Status_NON_CONFORMING
    };

    enum SliceType {
        SliceType_I,
        SliceType_P,
        SliceType_B,
        SliceType_Unspecified
    };

    QAccessUnitModel(const H264AccessUnit* pAccessUnit, QUuid id);
    QAccessUnitModel(const H265AccessUnit* pAccessUnit, QUuid id);


    virtual ~QAccessUnitModel();

    uint8_t* serialize() const;
    void setDisplayedFrameNumber();
    void setSliceType();
    void setStatus();
    bool isH264() const;
    bool isH265() const;
    uint64_t size() const;

    const std::variant<const H264AccessUnit*, const H265AccessUnit*> m_pAccessUnit;
    std::optional<uint16_t> m_displayedFrameNum;
    Status m_status;
    SliceType m_sliceType;
    QUuid m_id;
};