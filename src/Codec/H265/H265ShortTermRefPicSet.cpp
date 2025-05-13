#include <string>
#include <fmt/core.h>

#include "H265ShortTermRefPicSet.h"

H265ShortTermRefPicSet::H265ShortTermRefPicSet()
{
	inter_ref_pic_set_prediction_flag = 0;
	delta_idx_minus1 = 0;
	delta_rps_sign = 0;
	abs_delta_rps_minus1 = 0;
	num_negative_pics = 0;
	num_positive_pics = 0;
	RefRpsIdx = 0;
	NumNegativePics = 0;
	NumPositivePics = 0;
	NumDeltaPocs = 0;
}

std::vector<std::string> H265ShortTermRefPicSet::dump_fields(uint32_t stRpsIdx, uint32_t num_short_term_ref_pic_sets){
	std::vector<std::string> fields;
	if(stRpsIdx != 0) fields.push_back(fmt::format("inter_ref_pic_set_prediction_flag:{}", inter_ref_pic_set_prediction_flag));
	if(inter_ref_pic_set_prediction_flag){
		if(stRpsIdx == num_short_term_ref_pic_sets) fields.push_back(fmt::format("  delta_idx_minus1:{}", delta_idx_minus1));
		fields.push_back(fmt::format("delta_rps_sign:{}", delta_rps_sign));
		fields.push_back(fmt::format("abs_delta_rps_minus1:{}", abs_delta_rps_minus1));
		for(int j = 0;j <= NumDeltaPocs;++j){
			fields.push_back(fmt::format("  used_by_curr_pic_flag[{}]:{}", j, used_by_curr_pic_flag[j]));
			if(!used_by_curr_pic_flag[j]) fields.push_back(fmt::format("  use_delta_flag[{}]:{}", j, use_delta_flag[j]));
		}
	} else {
		fields.push_back(fmt::format("num_negative_pics:{}", num_negative_pics));
		fields.push_back(fmt::format("num_positive_pics:{}", num_positive_pics));
		for(int i = 0;i < num_negative_pics;++i){
			fields.push_back(fmt::format("delta_poc_s0_minus1[{}]:{}", i, delta_poc_s0_minus1[i]));
			fields.push_back(fmt::format("used_by_curr_pic_s0_flag[{}]:{}", i, used_by_curr_pic_s0_flag[i]));
		}
		for(int i = 0;i < num_positive_pics;++i){
			fields.push_back(fmt::format("delta_poc_s1_minus1[{}]:{}", i, delta_poc_s1_minus1[i]));
			fields.push_back(fmt::format("used_by_curr_pic_s1_flag[{}]:{}", i, used_by_curr_pic_s1_flag[i]));
		}
	}
	return fields;
}
