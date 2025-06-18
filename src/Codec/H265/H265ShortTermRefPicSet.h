#ifndef TOOLKIT_CODEC_UTILS_H265_SHORT_TERM_REF_PIC_SET_H_
#define TOOLKIT_CODEC_UTILS_H265_SHORT_TERM_REF_PIC_SET_H_

#include <vector>
#include <cstdint>

#include "../../StringHelpers/UnitFieldList.h"

struct H265ShortTermRefPicSet {
	H265ShortTermRefPicSet();

	uint8_t inter_ref_pic_set_prediction_flag;
	uint32_t delta_idx_minus1;
	uint8_t delta_rps_sign;
	uint32_t abs_delta_rps_minus1;
	std::vector<uint8_t> used_by_curr_pic_flag;
	std::vector<uint8_t> use_delta_flag;
	uint32_t num_negative_pics;
	uint32_t num_positive_pics;
	std::vector<int32_t> delta_poc_s0_minus1;
	std::vector<uint8_t> used_by_curr_pic_s0_flag;
	std::vector<int32_t> delta_poc_s1_minus1;
	std::vector<uint8_t> used_by_curr_pic_s1_flag;

	// Derived variables
	uint32_t RefRpsIdx;
	uint32_t NumNegativePics;
	std::vector<int32_t> DeltaPocS0;
	std::vector<uint8_t> UsedByCurrPicS0;
	uint32_t NumPositivePics;
	std::vector<int32_t> DeltaPocS1;
	std::vector<uint8_t> UsedByCurrPicS1;
	uint32_t NumDeltaPocs;

	UnitFieldList dump_fields(uint32_t stRpsId, uint32_t num_short_term_ref_pic_sets);
};


#endif // TOOLKIT_CODEC_UTILS_H265_SHORT_TERM_REF_PIC_SET_H_
