#ifndef TOOLKIT_CODEC_UTILS_H264ACCESS_UNIT_H_
#define TOOLKIT_CODEC_UTILS_H264ACCESS_UNIT_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <optional>

struct H264NAL;
struct H264Slice;

struct H264AccessUnit {
    H264AccessUnit();
    ~H264AccessUnit();

    std::vector<std::unique_ptr<H264NAL>> NALUnits;
    bool decodable;
    std::vector<std::string> minorErrors;
    std::vector<std::string> majorErrors;
    bool hasFrameGaps;

    bool empty() const;
    std::vector<H264NAL*> getNALUnits() const;
    void addNALUnit(std::unique_ptr<H264NAL> NALUnit);
    uint32_t size() const;
    uint64_t byteSize() const;
    std::optional<uint16_t> frameNumber() const;
    H264Slice* slice() const;
    std::vector<H264Slice*> slices() const;
    H264Slice* primary_coded_slice() const;
    H264NAL* last() const;
    void validate();
    bool isValid() const;
    bool hasMajorErrors() const;
    bool hasMinorErrors() const;
    bool hasNonReferencePicture() const;
    bool hasReferencePicture() const;
};

#endif // TOOLKIT_CODEC_UTILS_H264ACCESS_UNIT_H_
