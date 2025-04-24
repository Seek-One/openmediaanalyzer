#ifndef TOOLKIT_CODEC_UTILS_H265ACCESS_UNIT_H_
#define TOOLKIT_CODEC_UTILS_H265ACCESS_UNIT_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <optional>

struct H265NAL;
struct H265Slice;

struct H265AccessUnit {
    H265AccessUnit();
    ~H265AccessUnit();

    H265Slice* slice() const;
    std::vector<H265Slice*> slices() const;
    bool empty() const;
    std::vector<H265NAL*> getNALUnits() const;
    void addNALUnit(std::unique_ptr<H265NAL> NALUnit);
    uint32_t count() const;
    uint64_t size() const;
    std::optional<uint16_t> frameNumber() const;
    void validate();
    bool isValid() const;

    std::vector<std::unique_ptr<H265NAL>> NALUnits;
    std::vector<std::string> errors;
};

#endif // TOOLKIT_CODEC_UTILS_H265ACCESS_UNIT_H_
