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

    uint32_t count();
    std::vector<H264Slice*> slices();
    std::vector<H264AccessUnit*> getAccessUnits();

    std::vector<std::unique_ptr<H264AccessUnit>> accessUnits;
    bool hasIDR;
    bool hasSlice;
    std::vector<std::string> errors;
};

#endif // TOOLKIT_CODEC_UTILS_H264GOP_H_
