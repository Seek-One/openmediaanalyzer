#include <string>

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

void H265ShortTermRefPicSet::dump(H26XDumpObject& dumpObject, uint32_t stRpsIdx, uint32_t num_short_term_ref_pic_sets) const
{
	dumpObject.startUnitFieldList("Short-term Reference Picture Set");
	{
		if(inter_ref_pic_set_prediction_flag){
			if(stRpsIdx != 0){
				dumpObject.startValueUnitFieldList("inter_ref_pic_set_prediction_flag", inter_ref_pic_set_prediction_flag);
				
				if(stRpsIdx == num_short_term_ref_pic_sets){
					dumpObject.addUnitField("delta_idx_minus1", delta_idx_minus1);
				}
				dumpObject.addUnitField("delta_rps_sign", delta_rps_sign);
				dumpObject.addUnitField("abs_delta_rps_minus1", abs_delta_rps_minus1);
				for(uint32_t j = 0;j <= NumDeltaPocs; j++)
				{
					dumpObject.startIdxValueUnitFieldList("used_by_curr_pic_flag", j, used_by_curr_pic_flag[j]);
					if(!used_by_curr_pic_flag[j]){
						dumpObject.addIdxUnitField("use_delta_flag", j, use_delta_flag[j]);
					}
					dumpObject.endIdxValueUnitFieldList();
				}
				dumpObject.endValueUnitFieldList();
			}
		} else {
			dumpObject.startValueUnitFieldList("num_negative_pics", num_negative_pics);
			for(uint32_t i = 0;i < num_negative_pics;++i){
				dumpObject.addIdxUnitField("delta_poc_s0_minus1", i, delta_poc_s0_minus1[i]);
				dumpObject.addIdxUnitField("used_by_curr_pic_s0_flag", i, used_by_curr_pic_s0_flag[i]);
			}
			dumpObject.endValueUnitFieldList();

			dumpObject.startValueUnitFieldList("num_positive_pics", num_positive_pics);
			for(uint32_t i = 0;i < num_positive_pics;++i){
				dumpObject.addIdxUnitField("delta_poc_s1_minus1", i, delta_poc_s1_minus1[i]);
				dumpObject.addIdxUnitField("used_by_curr_pic_s1_flag", i, used_by_curr_pic_s1_flag[i]);
			}
			dumpObject.endValueUnitFieldList();
		}
	}
	dumpObject.endUnitFieldList();
}
