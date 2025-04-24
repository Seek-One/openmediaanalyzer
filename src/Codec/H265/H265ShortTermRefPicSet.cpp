#include <string>
#include <sstream>

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
	if(stRpsIdx != 0) fields.push_back((std::ostringstream() << "inter_ref_pic_set_prediction_flag:" << (int)inter_ref_pic_set_prediction_flag).str());
	if(inter_ref_pic_set_prediction_flag){
		if(stRpsIdx == num_short_term_ref_pic_sets) fields.push_back((std::ostringstream() << "  delta_idx_minus1:" << delta_idx_minus1).str());
		fields.push_back((std::ostringstream() << "delta_rps_sign:" << (int)delta_rps_sign).str());
		fields.push_back((std::ostringstream() << "abs_delta_rps_minus1:" << abs_delta_rps_minus1).str());
		for(int j = 0;j <= NumDeltaPocs;++j){
			fields.push_back((std::ostringstream() << "  used_by_curr_pic_flag[" << j << "]:" << used_by_curr_pic_flag[j]).str());
			if(!used_by_curr_pic_flag[j]) fields.push_back((std::ostringstream() << "  use_delta_flag[" << j << "]:" << use_delta_flag[j]).str());
		}
	} else {
		fields.push_back((std::ostringstream() << "num_negative_pics:" << num_negative_pics).str());
		fields.push_back((std::ostringstream() << "num_positive_pics:" << num_positive_pics).str());
		for(int i = 0;i < num_negative_pics;++i){
			fields.push_back((std::ostringstream() << "delta_poc_s0_minus1[" << i << "]:" << delta_poc_s0_minus1[i]).str());
			fields.push_back((std::ostringstream() << "used_by_curr_pic_s0_flag[" << i << "]:" << (int)used_by_curr_pic_s0_flag[i]).str());
		}
		for(int i = 0;i < num_positive_pics;++i){
			fields.push_back((std::ostringstream() << "delta_poc_s1_minus1[" << i << "]:" << delta_poc_s1_minus1[i]).str());
			fields.push_back((std::ostringstream() << "used_by_curr_pic_s1_flag[" << i << "]:" << (int)used_by_curr_pic_s1_flag[i]).str());
		}
	}
	return fields;
}
