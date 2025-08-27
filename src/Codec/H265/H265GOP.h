#ifndef TOOLKIT_CODEC_UTILS_H265GOP_H_
#define TOOLKIT_CODEC_UTILS_H265GOP_H_

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "../H26X/H26XErrors.h"

struct H265AccessUnit;

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
	H26XErrors errors;
};

#endif // TOOLKIT_CODEC_UTILS_H265GOP_H_
