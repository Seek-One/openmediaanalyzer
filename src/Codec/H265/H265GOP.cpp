#include <iostream>
#include <algorithm>
#include <numeric>

#include "H265AccessUnit.h"
#include "H265Slice.h"

#include "H265GOP.h"

H265GOP::H265GOP(){
}

H265GOP::~H265GOP(){
    accessUnits.clear();
}

bool H265GOP::empty() const {
    return accessUnits.empty();
}

uint32_t H265GOP::count() const {
    return accessUnits.size();
}

uint64_t H265GOP::size() const {
    return std::accumulate(accessUnits.begin(), accessUnits.end(), 0, [](uint64_t acc, const std::unique_ptr<H265AccessUnit>& accessUnit){
        return acc + accessUnit->size();
    });
}

std::vector<H265AccessUnit*> H265GOP::getAccessUnits() const {
    std::vector<H265AccessUnit*> pAccessUnits;
    std::transform(accessUnits.begin(), accessUnits.end(), std::back_inserter(pAccessUnits), [](const std::unique_ptr<H265AccessUnit>& pAccessUnit){
        return pAccessUnit.get();
    });
    return pAccessUnits;
}