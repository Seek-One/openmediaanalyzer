#ifndef TOOLKIT_CODEC_UTILS_H264ACCESS_UNIT_H_
#define TOOLKIT_CODEC_UTILS_H264ACCESS_UNIT_H_

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <optional>

#include "../H26X/H26XErrors.h"

struct H264NALUnit;
struct H264Slice;

enum ReferencePictureMarking {
    RPM_Unused,
    RPM_ShortTermReference,
    RPM_LongTermReference
};

struct H264AccessUnit {
    H264AccessUnit();
    ~H264AccessUnit();

    std::vector<std::unique_ptr<H264NALUnit>> NALUnits;
    bool decodable;
	H26XErrors errors;
    bool hasFrameGaps;
    ReferencePictureMarking rpm;

    // picture order count (8.2.1)
    uint16_t TopFieldOrderCnt;
    uint16_t BottomFieldOrderCnt;
    uint16_t PicOrderCntMsb;
    uint16_t PicOrderCntLsb;
    uint16_t FrameNumOffset;
    uint16_t PicOrderCnt;

    // picture numbers (8.2.4.1)
    uint16_t FrameNum;
    uint16_t FrameNumWrap;
    uint16_t PicNum;
    uint16_t LongTermFrameIdx;
    uint16_t LongTermPicNum;

    bool empty() const;
    std::vector<H264NALUnit*> getNALUnits() const;
    void addNALUnit(std::unique_ptr<H264NALUnit> NALUnit);
    uint32_t size() const;
    uint64_t byteSize() const;
    std::optional<uint16_t> frameNumber() const;
    H264Slice* slice() const;
    std::vector<H264Slice*> slices() const;
    H264Slice* primary_coded_slice() const;
    H264NALUnit* last() const;
    void validate();
    bool isValid() const;
    bool hasMajorErrors() const;
    bool hasMinorErrors() const;
    bool hasNonReferencePicture() const;
    bool hasReferencePicture() const;
};

#endif // TOOLKIT_CODEC_UTILS_H264ACCESS_UNIT_H_
