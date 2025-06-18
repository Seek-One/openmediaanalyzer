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

UnitFieldList H265ShortTermRefPicSet::dump_fields(uint32_t stRpsIdx, uint32_t num_short_term_ref_pic_sets){
	UnitFieldList fields = UnitFieldList("Short-term Reference Picture Set");
	ValueUnitFieldList inter_ref_pic_set_prediction_flagField = ValueUnitFieldList("inter_ref_pic_set_prediction_flag", inter_ref_pic_set_prediction_flag);
	if(inter_ref_pic_set_prediction_flag){
		if(stRpsIdx == num_short_term_ref_pic_sets) inter_ref_pic_set_prediction_flagField.addItem(UnitField("delta_idx_minus1", delta_idx_minus1));
		inter_ref_pic_set_prediction_flagField.addItem(UnitField("delta_rps_sign", delta_rps_sign));
		inter_ref_pic_set_prediction_flagField.addItem(UnitField("abs_delta_rps_minus1", abs_delta_rps_minus1));
		for(int j = 0;j <= NumDeltaPocs;++j){
			IdxValueUnitFieldList used_by_curr_pic_flagField = IdxValueUnitFieldList("used_by_curr_pic_flag", used_by_curr_pic_flag[j], j);
			if(!used_by_curr_pic_flag[j]) used_by_curr_pic_flagField.addItem(IdxUnitField("use_delta_flag", use_delta_flag[j], j));
			inter_ref_pic_set_prediction_flagField.addItem(std::move(used_by_curr_pic_flagField));
		}
		if(stRpsIdx != 0) fields.addItem(std::move(inter_ref_pic_set_prediction_flagField));
	} else {
		ValueUnitFieldList num_negative_picsField = ValueUnitFieldList("num_negative_pics", num_negative_pics);
		ValueUnitFieldList num_positive_picsField = ValueUnitFieldList("num_positive_pics", num_positive_pics);
		for(int i = 0;i < num_negative_pics;++i){
			num_negative_picsField.addItem(IdxUnitField("delta_poc_s0_minus1", delta_poc_s0_minus1[i], i));
			num_negative_picsField.addItem(IdxUnitField("used_by_curr_pic_s0_flag", used_by_curr_pic_s0_flag[i], i));
		}
		fields.addItem(std::move(num_negative_picsField));
		for(int i = 0;i < num_positive_pics;++i){
			num_positive_picsField.addItem(IdxUnitField("delta_poc_s1_minus1", delta_poc_s1_minus1[i], i));
			num_positive_picsField.addItem(IdxUnitField("used_by_curr_pic_s1_flag", used_by_curr_pic_s1_flag[i], i));
		}
		fields.addItem(std::move(num_positive_picsField));
	}
	return fields;
}
