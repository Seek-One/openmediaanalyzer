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
