#ifndef TOOLKIT_CODEC_UTILS_H265ACCESS_UNIT_H_
#define TOOLKIT_CODEC_UTILS_H265ACCESS_UNIT_H_

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "../H26X/H26XErrors.h"

struct H265NAL;
struct H265Slice;

struct H265AccessUnit {
    enum ReferenceStatus {
        ReferenceStatus_Unused,
        ReferenceStatus_ShortTerm,
        ReferenceStatus_LongTerm
    };

    H265AccessUnit();
    ~H265AccessUnit();
    H265Slice* slice() const;
    std::vector<H265Slice*> slices() const;
    bool empty() const;
    std::vector<H265NAL*> getNALUnits() const;
    void addNALUnit(std::unique_ptr<H265NAL> NALUnit);
    uint32_t size() const;
    uint64_t byteSize() const;
    std::optional<uint16_t> frameNumber() const;
    bool isIRAP() const;
    bool isIDR() const;
    bool isRASL() const;
    bool isRADL() const;
    bool isSLNR() const;
    void validate();
    bool isValid() const;
    bool hasMajorErrors() const;
    bool hasMinorErrors() const;

    std::vector<std::unique_ptr<H265NAL>> NALUnits;
    int32_t PicOrderCntVal;
    uint32_t PicOrderCntMsb;
	H26XErrors errors;
    ReferenceStatus status;
    bool decodable;
};

#endif // TOOLKIT_CODEC_UTILS_H265ACCESS_UNIT_H_
