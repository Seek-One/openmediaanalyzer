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

    void setAccessUnitDecodability();
    bool empty() const;
    uint32_t size() const;
    uint64_t byteSize() const;
    std::vector<H265AccessUnit*> getAccessUnits() const;
    void validate();
    bool hasMajorErrors() const;
    bool hasMinorErrors() const;

    bool hasIDR;
    bool hasSlice;
    std::vector<std::unique_ptr<H265AccessUnit>> accessUnits;
    std::vector<std::string> minorErrors;
    std::vector<std::string> majorErrors;
};

#endif // TOOLKIT_CODEC_UTILS_H265GOP_H_
