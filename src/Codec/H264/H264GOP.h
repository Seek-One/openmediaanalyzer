#ifndef TOOLKIT_CODEC_UTILS_H264GOP_H_
#define TOOLKIT_CODEC_UTILS_H264GOP_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <optional>

struct H264AccessUnit;
struct H264Slice;

struct H264GOP {
    H264GOP();
    ~H264GOP();

    uint32_t size() const;
    uint64_t byteSize() const;
    std::vector<H264Slice*> slices() const;
    std::vector<H264AccessUnit*> getAccessUnits() const;
    void validate();
    bool hasMajorErrors() const;
    bool hasMinorErrors() const;

    std::vector<std::unique_ptr<H264AccessUnit>> accessUnits;
    bool hasIDR;
    bool hasSlice;
    std::vector<std::string> minorErrors;
    std::vector<std::string> majorErrors;
};

#endif // TOOLKIT_CODEC_UTILS_H264GOP_H_
