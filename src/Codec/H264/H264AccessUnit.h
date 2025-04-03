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
    std::vector<std::string> errors;

    bool empty() const;
    std::vector<H264NAL*> getNALUnits() const;
    void addNALUnit(std::unique_ptr<H264NAL> NALUnit);
    uint32_t count() const;
    uint32_t size() const;
    std::optional<uint16_t> frameNumber() const;
    H264Slice* slice() const;
    H264Slice* primary_coded_slice() const;
    H264NAL* last() const;
    void validate();
};

#endif // TOOLKIT_CODEC_UTILS_H264ACCESS_UNIT_H_
