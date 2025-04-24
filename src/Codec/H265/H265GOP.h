#ifndef TOOLKIT_CODEC_UTILS_H265GOP_H_
#define TOOLKIT_CODEC_UTILS_H265GOP_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <optional>

struct H265AccessUnit;
struct H265Slice;

struct H265GOP {
    H265GOP();
    ~H265GOP();

    bool empty() const;
    uint32_t count() const;
    uint64_t size() const;
    std::vector<H265AccessUnit*> getAccessUnits() const;

    bool hasIDR;
    std::vector<std::unique_ptr<H265AccessUnit>> accessUnits;
    std::vector<std::string> errors;
};

#endif // TOOLKIT_CODEC_UTILS_H265GOP_H_
