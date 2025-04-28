#ifndef TOOLKIT_CODEC_UTILS_H264AUD_H_
#define TOOLKIT_CODEC_UTILS_H264AUD_H_

#include <cstdint>
#include <string>
#include <vector>

#include "H264NAL.h"

struct H264AUD : public H264NAL {
	H264AUD();
    H264AUD(uint8_t forbidden_zero_bit, uint8_t nal_ref_idc, uint32_t nal_size, uint8_t* nal_data);

    static const inline std::vector<std::vector<uint8_t>> slice_type_values = {
        {2, 7},
        {0, 2, 5, 7},
        {0, 1, 2, 5, 6, 7},
        {4, 9},
        {3, 4, 8, 9},
        {2, 4, 7, 9},
        {0, 2, 3, 4, 5, 7, 8, 9},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
    };

    uint8_t primary_pic_type;
    
	std::vector<std::string> dump_fields() override;
    void validate() override;
};

#endif // TOOLKIT_CODEC_UTILS_H264AUD_H_
