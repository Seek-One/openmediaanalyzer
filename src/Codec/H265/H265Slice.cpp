#include <algorithm>

#include "../H26X/H26XUtils.h"
#include "H265PPS.h"
#include "H265SPS.h"
#include "H265VPS.h"

#include "H265Slice.h"

RefPicListsModification::RefPicListsModification()
{
	ref_pic_list_modification_flag_l0 = 0;
	ref_pic_list_modification_flag_l1 = 0;
}

void RefPicListsModification::dump(H26XDumpObject& dumpObject, const H265Slice& slice) const
{
	dumpObject.startUnitFieldList("Reference Picture Lists Modification");
	{
		dumpObject.startValueUnitFieldList("ref_pic_list_modification_flag_l0", ref_pic_list_modification_flag_l0);
		if (ref_pic_list_modification_flag_l0) {
			for (uint32_t i = 0; i <= slice.num_ref_idx_l0_active_minus1; ++i) {
				dumpObject.addIdxUnitField("list_entry_l0", i, list_entry_l0[i]);
			}
		}
		dumpObject.endValueUnitFieldList();

		if (slice.slice_type == H265Slice::SliceType_B) {
			dumpObject.startValueUnitFieldList("ref_pic_list_modification_flag_l1", ref_pic_list_modification_flag_l1);
			if (ref_pic_list_modification_flag_l0) {
				for (uint32_t i = 0; i <= slice.num_ref_idx_l1_active_minus1; ++i)
					dumpObject.addIdxUnitField("list_entry_l1", i, list_entry_l1[i]);
			}
			dumpObject.endValueUnitFieldList();
		}
	}
	dumpObject.endUnitFieldList();
}

void RefPicListsModification::validate(const H265Slice& h265Slice){
	uint32_t list_entry_limit = h265Slice.NumPicTotalCurr-1;
	for(uint32_t i = 0;i < list_entry_l0.size();++i){
		if(list_entry_l0[i] > list_entry_limit) errors.add(H26XError::Minor, H26XUtils::formatString("[Slice RPLM] list_entry_l0[%d] value (%ld) not in valid range (0..{})", i, list_entry_l0[i], list_entry_limit));
	}
	for(uint32_t i = 0;i < list_entry_l1.size();++i){
		if(list_entry_l1[i] > list_entry_limit) errors.add(H26XError::Minor, H26XUtils::formatString("[Slice RPLM] list_entry_l1[%d] value (%ld) not in valid range (0..{})", i, list_entry_l1[i], list_entry_limit));
	}
}

H265PredWeightTable::H265PredWeightTable(){
	luma_log2_weight_denom = 0;
	delta_chroma_log2_weight_denom = 0;
	for(int i = 0;i < 15;++i){
		luma_weight_l0_flag[i] = 0;
		chroma_weight_l0_flag[i] = 0;
		delta_luma_weight_l0[i] = 0;
		luma_offset_l0[i] = 0;
		luma_weight_l1_flag[i] = 0;
		chroma_weight_l1_flag[i] = 0;
		delta_luma_weight_l1[i] = 0;
		luma_offset_l1[i] = 0;
		for(int j = 0;j < 2;++j){
			delta_chroma_weight_l0[i][j] = 0;
			delta_chroma_offset_l0[i][j] = 0;
			delta_chroma_weight_l1[i][j] = 0;
			delta_chroma_offset_l1[i][j] = 0;
		}
	}
}

void H265PredWeightTable::dump(H26XDumpObject& dumpObject, const H265Slice& h265Slice) const
{
	dumpObject.startUnitFieldList("Prediction Weight Table");
	dumpObject.addUnitField("luma_log2_weight_denom", luma_log2_weight_denom);
	H265SPS* h265SPS = h265Slice.getSPS();
	if(h265SPS->ChromaArrayType != 0){
		dumpObject.addUnitField("delta_chroma_log2_weight_denom", delta_chroma_log2_weight_denom);
	}
	for(uint32_t i = 0;i <= h265Slice.num_ref_idx_l0_active_minus1;++i)
	{
		dumpObject.startIdxValueUnitFieldList("luma_weight_l0_flag", luma_weight_l0_flag[i], i);
		if(luma_weight_l0_flag[i]){
			dumpObject.addIdxUnitField("delta_luma_weight_l0", i, delta_luma_weight_l0[i]);
			dumpObject.addIdxUnitField("luma_offset_l0", i, luma_offset_l0[i]);
		}
		dumpObject.endIdxValueUnitFieldList();
	}
	if(h265SPS->ChromaArrayType != 0){
		for(uint32_t i = 0;i <= h265Slice.num_ref_idx_l0_active_minus1;++i){
			dumpObject.startIdxValueUnitFieldList("chroma_weight_l0_flag", chroma_weight_l0_flag[i], i);
			if(chroma_weight_l0_flag[i]){
				for(int j = 0;j < 2;++j){
					dumpObject.addDblIdxUnitField("delta_luma_weight_l0", i, j, delta_chroma_weight_l0[i][j]);
					dumpObject.addDblIdxUnitField("delta_chroma_offset_l0", i, j, delta_chroma_offset_l0[i][j]);
				}
			}
			dumpObject.endIdxValueUnitFieldList();
		}
	}
	if(h265Slice.slice_type == H265Slice::SliceType_B){
		for(uint32_t i = 0;i <= h265Slice.num_ref_idx_l1_active_minus1;++i){
			dumpObject.startIdxValueUnitFieldList("luma_weight_l1_flag", luma_weight_l1_flag[i], i);
			if(luma_weight_l1_flag[i]){
				dumpObject.addIdxUnitField("delta_luma_weight_l0", i, delta_luma_weight_l1[i]);
				dumpObject.addIdxUnitField("luma_offset_l0", i, luma_offset_l1[i]);
			}
			dumpObject.endIdxValueUnitFieldList();
		}
		if(h265SPS->ChromaArrayType != 0){
			for(uint32_t i = 0;i <= h265Slice.num_ref_idx_l1_active_minus1;++i){
				dumpObject.startIdxValueUnitFieldList("chroma_weight_l1_flag", chroma_weight_l1_flag[i], i);
				if(chroma_weight_l1_flag[i]){
					for(int j = 0;j < 2;++j){
						dumpObject.addDblIdxUnitField("delta_luma_weight_l0", i, j, delta_chroma_weight_l1[i][j]);
						dumpObject.addDblIdxUnitField("delta_chroma_offset_l0", i, j, delta_chroma_offset_l1[i][j]);
					}
				}
				dumpObject.endIdxValueUnitFieldList();
			}
		}
	}
	dumpObject.endUnitFieldList();
}

void H265PredWeightTable::validate(const H265Slice& h265Slice)
{
	H265SPS* h265SPS = h265Slice.getSPS();
	int16_t delta_chroma_offset_lower_bound = INT16_MIN;
	int16_t delta_chroma_offset_upper_bound = INT16_MAX;
	if(h265SPS){
		delta_chroma_offset_lower_bound = -4*h265SPS->sps_range_extension.WpOffsetHalfRangeC;
		delta_chroma_offset_upper_bound = 4*h265SPS->sps_range_extension.WpOffsetHalfRangeC-1;
	}
	if(luma_log2_weight_denom > 7){
		errors.add(H26XError::Minor, H26XUtils::formatString("[Slice PWT] luma_log2_weight_denom value (%ld) not in valid range (0..7)", luma_log2_weight_denom));
	}
	if(delta_chroma_log2_weight_denom > 7-luma_log2_weight_denom){
		errors.add(H26XError::Minor, H26XUtils::formatString("[Slice PWT] delta_chroma_log2_weight_denom value (%ld) not in valid range (0..{})", delta_chroma_log2_weight_denom, 7-luma_log2_weight_denom));
	}
	for(int i = 0;i < 15;++i){
		if(luma_weight_l0_flag[i]){
			if(delta_luma_weight_l0[i] < -128 || delta_luma_weight_l0[i] > 127){
				errors.add(H26XError::Minor, H26XUtils::formatString("[Slice PWT] delta_luma_weight_l0[%d] value (%ld) not in valid range (-128..127)", i, delta_luma_weight_l0[i]));
			}
		}
		if(chroma_weight_l0_flag[i]){
			for(int j = 0;j < 2;++j){
				if(delta_chroma_weight_l0[i][j] < -128 || delta_chroma_weight_l0[i][j] > 127){
					errors.add(H26XError::Minor, H26XUtils::formatString("delta_chroma_weight[%d][%d] value (%ld) not in valid range (-128..127)", i, j, delta_chroma_weight_l0[i][j]));
				}
				if(delta_chroma_offset_l0[i][j] < delta_chroma_offset_lower_bound || delta_chroma_offset_l0[i][j] > delta_chroma_offset_upper_bound){
					errors.add(H26XError::Minor, H26XUtils::formatString("[Slice PWT] delta_chroma_offset_l0[%d][%d] value (%ld) not in valid range ({}..{})", i, j, delta_chroma_offset_l0[i][j], delta_chroma_offset_lower_bound, delta_chroma_offset_upper_bound));
				}
			}
		}
		if(luma_weight_l1_flag[i]){
			if(delta_luma_weight_l1[i] < -128 || delta_luma_weight_l1[i] > 127) errors.add(H26XError::Minor, H26XUtils::formatString("[Slice PWT] delta_luma_weight_l1[%d] value (%ld) not in valid range (-128..127)", i, delta_luma_weight_l1[i]));
		}
		if(chroma_weight_l1_flag[i]){
			for(int j = 0;j < 2;++j){
				if(delta_chroma_weight_l1[i][j] < -128 || delta_chroma_weight_l1[i][j] > 127) errors.add(H26XError::Minor, H26XUtils::formatString("delta_chroma_weight[%d][%d] value (%ld) not in valid range (-128..127)", i, j, delta_chroma_weight_l1[i][j]));
				if(delta_chroma_offset_l1[i][j] < delta_chroma_offset_lower_bound || delta_chroma_offset_l1[i][j] > delta_chroma_offset_upper_bound){
					errors.add(H26XError::Minor, H26XUtils::formatString("[Slice PWT] delta_chroma_offset_l1[%d][%d] value (%ld) not in valid range ({}..{})", i, j, delta_chroma_offset_l1[i][j], delta_chroma_offset_lower_bound, delta_chroma_offset_upper_bound));
				}
			}
		}
	}
}

H265Slice::H265Slice():
	H265Slice(0, H265NALUnitType::Unspecified, 0, 0, 0, nullptr)
{}

H265Slice::H265Slice(uint8_t forbidden_zero_bit, H265NALUnitType::Type nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, const uint8_t* nal_data):
	H265NAL(forbidden_zero_bit, nal_unit_type, nuh_layer_id, nuh_temporal_id_plus1, nal_size, nal_data)
{
	first_slice_segment_in_pic_flag = 0;
	no_output_of_prior_pics_flag = 0;
	slice_pic_parameter_set_id = 0;
	dependent_slice_segment_flag = 0;
	slice_segment_address = 0;
	slice_type = SliceType_None;
	pic_output_flag = 1;
	colour_plane_id = 0;
	slice_pic_order_cnt_lsb = 0;
	short_term_ref_pic_set_sps_flag = 0;
	short_term_ref_pic_set_idx = 0;
	num_long_term_sps = 0;
	num_long_term_pics = 0;
	NumLongTerm = 0;
	slice_temporal_mvp_enabled_flag = 0;
	slice_sao_luma_flag = 0;
	slice_sao_chroma_flag = 0;
	num_ref_idx_active_override_flag = 0;
	num_ref_idx_l0_active_minus1 = 0;
	num_ref_idx_l1_active_minus1 = 0;
	mvd_l1_zero_flag = 0;
	cabac_init_flag = 0;
	collocated_from_l0_flag = 0;
	collocated_ref_idx = 0;
	five_minus_max_num_merge_cand = 0;
	slice_qp_delta = 0;
	slice_cb_qp_offset = 0;
	slice_cr_qp_offset = 0;
	slice_act_y_qp_offset = 0;
	slice_act_cb_qp_offset = 0;
	slice_act_cr_qp_offset = 0;
	deblocking_filter_override_flag = 0;
	slice_deblocking_filter_disabled_flag = 0;
	slice_beta_offset_div2 = 0;
	slice_tc_offset_div2 = 0;
	slice_loop_filter_across_slices_enabled_flag = 0;
	num_entry_point_offsets = 0;
	offset_len_minus1 = 0;
	slice_segment_header_extension_length = 0;
	IdrPicFlag = 0;
	NoRaslOutputFlag = 0;
	CurrRpsIdx = 0;
	NumPicTotalCurr = 0;
	NumShortTermPictureSliceHeaderBits = 0;
	NumLongTermPictureSliceHeaderBits = 0;
	NumRpsCurrTempList0 = 0;
	NumRpsCurrTempList1 = 0;
	SliceQpY = 0;
}

std::string getSpecificSliceType(H265NALUnitType::Type unitType){
	switch(unitType) {
	case H265NALUnitType::TRAIL_N:
		return "Trailing picture (N)";
	case H265NALUnitType::TRAIL_R:
		return "Trailing picture (R)";
	case H265NALUnitType::TSA_N:
		return "Temporal sub-layer access picture (N)";
	case H265NALUnitType::TSA_R:
		return "Temporal sub-layer access picture (R)";
	case H265NALUnitType::STSA_N:
		 return "Step-wise temporal sub-layer access picture (N)";	
	case H265NALUnitType::STSA_R:
		return "Step-wise temporal sub-layer access picture (R)";
	case H265NALUnitType::RADL_N:
		return "Random access decodable leading picture (N)";
	case H265NALUnitType::RADL_R:
		return "Random access decodable leading picture (R)";
	case H265NALUnitType::RASL_N:
		return "Random access skipped leading picture (N)";
	case H265NALUnitType::RASL_R:
		return "Random access skipped leading picture (R)";
	case H265NALUnitType::BLA_W_LP:
		return "Broken link access picture (with LP)";
	case H265NALUnitType::BLA_W_RADL:
		return "Broken link access picture (with RADL pictures)";
	case H265NALUnitType::BLA_N_LP:
		return "Broken link access picture (no LP)";
	default:
		return "Unindentified Slice";
	}
	return "Unindentified Slice";
}

void H265Slice::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList(getSpecificSliceType(nal_unit_type).c_str());
	H26X_BREAKABLE_SCOPE(H26XDumpScope) {
		H265NAL::dump(dumpObject);

		if (!completelyParsed) {
			break;
		}

		H265PPS* h265PPS = getPPS();
		H265SPS* h265SPS = getSPS();

		dumpObject.startValueUnitFieldList("first_slice_segment_in_pic_flag", first_slice_segment_in_pic_flag);
		{
			if(nal_unit_type >= H265NALUnitType::BLA_W_LP && nal_unit_type <= H265NALUnitType::IRAP_VCL23){
				dumpObject.addUnitField("no_output_of_prior_pics_flag", no_output_of_prior_pics_flag);
			}
			dumpObject.addUnitField("slice_pic_parameter_set_id", slice_pic_parameter_set_id);
			if(!h265PPS || !h265SPS){
				break;
			}
			if(!first_slice_segment_in_pic_flag){
				if(h265PPS->dependent_slice_segments_enabled_flag){
					dumpObject.addUnitField("dependent_slice_segment_flag", dependent_slice_segment_flag);
				}
				dumpObject.addUnitField("slice_segment_address", slice_segment_address);
			}
		}
		dumpObject.endValueUnitFieldList();

		if(!dependent_slice_segment_flag){
			dumpObject.addUnitField("slice_type", slice_type);
			if(h265PPS->output_flag_present_flag){
				dumpObject.addUnitField("pic_output_flag", pic_output_flag);
			}
			if(h265SPS->separate_colour_plane_flag == 1){
				dumpObject.addUnitField("colour_plane_id", colour_plane_id);
			}
			dumpObject.addUnitField("slice_pic_order_cnt_lsb", slice_pic_order_cnt_lsb);
			if(!IdrPicFlag){

				dumpObject.startValueUnitFieldList("short_term_ref_pic_set_sps_flag", short_term_ref_pic_set_sps_flag);
				if(!short_term_ref_pic_set_sps_flag){
					h265SPS->short_term_ref_pic_set[h265SPS->num_short_term_ref_pic_sets].dump(dumpObject, h265SPS->num_short_term_ref_pic_sets, h265SPS->num_short_term_ref_pic_sets);
				} else if(h265SPS->num_short_term_ref_pic_sets > 1){
					dumpObject.addUnitField("short_term_ref_pic_set_idx", short_term_ref_pic_set_idx);
				}
				dumpObject.endValueUnitFieldList();

				if(h265SPS->long_term_ref_pics_present_flag){
					if(h265SPS->num_long_term_ref_pics_sps > 0){
						dumpObject.addUnitField("num_long_term_sps", num_long_term_sps);
					}
					dumpObject.addUnitField("num_long_term_pics", num_long_term_pics);
					for(uint32_t i = 0;i < num_long_term_sps + num_long_term_pics;++i){
						if(i < num_long_term_sps){
							if(num_long_term_sps){
								dumpObject.addIdxUnitField("lt_idx_sps", i, lt_idx_sps[i]);
							}
						} else {
							dumpObject.addIdxUnitField("poc_lsb_lt", i, poc_lsb_lt[i]);
							dumpObject.addIdxUnitField("used_by_curr_pic_lt_flag", i, used_by_curr_pic_lt_flag[i]);
						}
						dumpObject.startIdxValueUnitFieldList("delta_poc_msb_present_flag", i, delta_poc_msb_present_flag[i]);
						if(delta_poc_msb_present_flag[i]){
							dumpObject.addIdxUnitField("delta_poc_msb_cycle_lt", i, delta_poc_msb_cycle_lt[i]);
						}
						dumpObject.endIdxValueUnitFieldList();
					}
				}
			}
			if(h265SPS->sample_adaptive_offset_enabled_flag){
				dumpObject.addUnitField("slice_sao_luma_flag", slice_sao_luma_flag);
				if(h265SPS->ChromaArrayType !=0){
					dumpObject.addUnitField("slice_sao_chroma_flag", slice_sao_chroma_flag);
				}
			}
			if(slice_type == SliceType_P || slice_type == SliceType_B){
				dumpObject.startValueUnitFieldList("num_ref_idx_active_override_flag", num_ref_idx_active_override_flag);
				if(num_ref_idx_active_override_flag){
					dumpObject.addUnitField("num_ref_idx_l0_active_minus1", num_ref_idx_l0_active_minus1);
					if(slice_type == SliceType_B){
						dumpObject.addUnitField("num_ref_idx_l1_active_minus1", num_ref_idx_l1_active_minus1);
					}
				}
				dumpObject.endValueUnitFieldList();

				if(h265PPS->lists_modification_present_flag && NumPicTotalCurr > 1){
					ref_pic_lists_modification.dump(dumpObject, *this);
				}
				if(slice_type == SliceType_B){
					dumpObject.addUnitField("mvd_l1_zero_flag", mvd_l1_zero_flag);
				}
				if(h265PPS->cabac_init_present_flag) dumpObject.addUnitField("cabac_init_flag", cabac_init_flag);
				if(slice_temporal_mvp_enabled_flag){
					if(slice_type == SliceType_B){
						dumpObject.addUnitField("collocated_from_l0_flag", collocated_from_l0_flag);
					}
					if((collocated_from_l0_flag && num_ref_idx_l0_active_minus1 > 0) || (!collocated_from_l0_flag && num_ref_idx_l1_active_minus1 > 0)){
						dumpObject.addUnitField("collocated_ref_idx", collocated_ref_idx);
					}
				}
				if((h265PPS->weighted_pred_flag && slice_type == SliceType_P) ||
				   (h265PPS->weighted_bipred_flag && slice_type == SliceType_B)){
					pred_weight_table.dump(dumpObject, *this);
				}
				dumpObject.addUnitField("five_minus_max_num_merge_cand", five_minus_max_num_merge_cand);
				if(h265SPS->sps_scc_extension.motion_vector_resolution_control_idc == 2){
					dumpObject.addUnitField("use_integer_mv_flag", use_integer_mv_flag);
				}
			}
			if(h265SPS->sps_temporal_mvp_enabled_flag){
				dumpObject.startValueUnitFieldList("slice_temporal_mvp_enabled_flag", slice_temporal_mvp_enabled_flag);
				dumpObject.endValueUnitFieldList();
			}

			dumpObject.addUnitField("slice_qp_delta", slice_qp_delta);
			if(h265PPS->pps_slice_chroma_qp_offsets_present_flag){
				dumpObject.addUnitField("slice_cb_qp_offset", slice_cb_qp_offset);
				dumpObject.addUnitField("slice_cr_qp_offset", slice_cr_qp_offset);
			}
			if(h265PPS->pps_scc_extension.pps_slice_act_qp_offsets_present_flag){
				dumpObject.addUnitField("slice_act_cb_qp_offset", slice_act_cb_qp_offset);
				dumpObject.addUnitField("slice_act_cr_qp_offset", slice_act_cr_qp_offset);
				dumpObject.addUnitField("slice_act_y_qp_offset", slice_act_y_qp_offset);
			}
			if(h265PPS->pps_range_extension.chroma_qp_offset_list_enabled_flag){
				dumpObject.addUnitField("cu_chroma_qp_offset_enabled_flag", cu_chroma_qp_offset_enabled_flag);
			}

			if(h265PPS->deblocking_filter_override_enabled_flag) {
				dumpObject.startValueUnitFieldList("deblocking_filter_override_flag", deblocking_filter_override_flag);
				if (deblocking_filter_override_flag) {
					dumpObject.startValueUnitFieldList("slice_deblocking_filter_disabled_flag", slice_deblocking_filter_disabled_flag);
					if (!slice_deblocking_filter_disabled_flag) {
						dumpObject.addUnitField("slice_beta_offset_div2", slice_beta_offset_div2);
						dumpObject.addUnitField("slice_tc_offset_div2", slice_tc_offset_div2);
					}
					dumpObject.endValueUnitFieldList();

					dumpObject.addUnitField("slice_tc_offset_div2", slice_tc_offset_div2);
				}
				dumpObject.endValueUnitFieldList();
			}

			if(h265PPS->pps_loop_filter_across_slices_enabled_flag && (slice_sao_luma_flag || slice_sao_chroma_flag || !slice_deblocking_filter_disabled_flag)){
				dumpObject.addUnitField("slice_loop_filter_across_slices_enabled_flag", slice_loop_filter_across_slices_enabled_flag);
			}
			if(h265PPS->tiles_enabled_flag || h265PPS->entropy_coding_sync_enabled_flag){
				dumpObject.startValueUnitFieldList("num_entry_point_offsets", num_entry_point_offsets);
				if(num_entry_point_offsets > 0){
					dumpObject.addUnitField("offset_len_minus1", offset_len_minus1);
					for(uint32_t i = 0;i < num_entry_point_offsets;++i){
						dumpObject.addIdxUnitField("entry_point_offset_minus1", i, entry_point_offset_minus1[i]);
					}
				}
				dumpObject.endValueUnitFieldList();
			}
		}
	}

	dumpObject.endUnitFieldList();
}

H265PPS* H265Slice::getPPS() const{
	auto referencedPPS = H265PPS::PPSMap.find(slice_pic_parameter_set_id);
	if(referencedPPS == H265PPS::PPSMap.end()) return nullptr;
	return referencedPPS->second;
}

H265SPS* H265Slice::getSPS() const{
	H265PPS* pPps = getPPS();
	if(!pPps) return nullptr;
	auto referencedSPS = H265SPS::SPSMap.find(pPps->pps_seq_parameter_set_id);
	if(referencedSPS == H265SPS::SPSMap.end()) return nullptr;
	return referencedSPS->second;
}

H265VPS* H265Slice::getVPS() const
{
	H265SPS* pSps = getSPS();
	if(!pSps){
		return nullptr;
	}
	auto referencedVPS = H265VPS::VPSMap.find(pSps->sps_video_parameter_set_id);
	if(referencedVPS == H265VPS::VPSMap.end()){
		return nullptr;
	}
	return referencedVPS->second;
}

void H265Slice::validate(){
	H265NAL::validate();
	if(!completelyParsed) return;
	if(slice_pic_parameter_set_id > 63){
		errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] slice_pic_parameter_set_id value (%ld) not in valid range (0..63)", slice_pic_parameter_set_id));
	}
	H265PPS* pPps = getPPS();
	if(!pPps){
		errors.add(H26XError::Major, H26XUtils::formatString("[Slice] reference to unknown PPS (%ld)", slice_pic_parameter_set_id));
		return;
	}
	H265SPS* pSps = getSPS();
	if(!pSps){
		errors.add(H26XError::Major, H26XUtils::formatString("[Slice] reference to unknown SPS (%ld)", pPps->pps_seq_parameter_set_id));
		return;
	}
	H265VPS* pVps = getVPS();
	if(!pVps) {
		errors.add(H26XError::Major, H26XUtils::formatString("[Slice] reference to unknown VPS (%ld)", pSps->sps_video_parameter_set_id));
		return;
	}
	if(!pPps->completelyParsed){
		errors.add(H26XError::Major, "[Slice] referenced PPS is incomplete");
		return;
	}
	if(!pSps->completelyParsed){
		errors.add(H26XError::Major, "[Slice] referenced SPS is incomplete");
		return;
	}
	if(!pVps->completelyParsed){
		errors.add(H26XError::Major, "[Slice] referenced VPS is incomplete");
		return;
	}
	if(pPps->TemporalId > TemporalId){
		errors.add(H26XError::Minor, "[Slice] referenced PPS has a greater TemporalId value");
	}
	if(pPps->nuh_layer_id > nuh_layer_id){
		errors.add(H26XError::Minor, "[Slice] referenced PPS has a greater nuh_layer_id value");
	}
	if(pSps->nuh_layer_id > nuh_layer_id){
		errors.add(H26XError::Minor, "[Slice] referenced SPS has a greater nuh_layer_id value");
	}
	if(slice_segment_address > pSps->PicSizeInCtbsY-1){
		errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] slice_segment_address value (%ld) not in valid range (0..{})", slice_segment_address, pSps->PicSizeInCtbsY-1));
	}
	if(!pPps->dependent_slice_segments_enabled_flag){
		if(slice_type > 2){
			errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] slice_type value (%ld) not in valid range (0..2)", slice_type));
		}
		if(nal_unit_type >= H265NALUnitType::BLA_W_LP && nal_unit_type <= H265NALUnitType::IRAP_VCL23 &&
			nuh_layer_id == 0 && !pPps->pps_scc_extension.pps_curr_pic_ref_enabled_flag &&
			slice_type != 2)
		{
			errors.add(H26XError::Minor, "[Slice] slice_type value of an IRAP picture not equal to 2");
		}
		if(pSps->sps_max_dec_pic_buffering_minus1[TemporalId] == 0 && nuh_layer_id == 0 &&
			!pPps->pps_scc_extension.pps_curr_pic_ref_enabled_flag && slice_type != 2)
		{
			errors.add(H26XError::Minor, "[Slice] slice_type value not equal to 2");
		}
		if(colour_plane_id > 2){
			errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] colour_plane_id value (%ld) not in valid range (0..2)", colour_plane_id));
		}
	}
	if(slice_pic_order_cnt_lsb > pSps->MaxPicOrderCntLsb-1){
		errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] slice_pic_order_cnt_lsb value (%ld) not in valid range (0..{})", slice_pic_order_cnt_lsb, pSps->MaxPicOrderCntLsb-1));
	}
	if(short_term_ref_pic_set_idx > pSps->num_short_term_ref_pic_sets-1){
		errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] short_term_ref_pic_set_idx value (%ld) not in valid range (0..{})", short_term_ref_pic_set_idx, pSps->num_short_term_ref_pic_sets-1));
	}
	if(num_long_term_sps > pSps->num_long_term_ref_pics_sps){
		errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] num_long_term_sps value (%ld) not in valid range (0..{})", num_long_term_sps, pSps->num_long_term_ref_pics_sps));
	}
	uint32_t num_long_term_pics_limit = pSps->sps_max_dec_pic_buffering_minus1[TemporalId] - 
										pSps->short_term_ref_pic_set[CurrRpsIdx].NumNegativePics -
										pSps->short_term_ref_pic_set[CurrRpsIdx].NumPositivePics -
										num_long_term_sps - pPps->TwoVersionsOfCurrDecPicFlag;
	if(nuh_layer_id == 0 && num_long_term_pics > num_long_term_pics_limit){
		errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] num_long_term_pics value (%ld) not in valid range (0..{})", num_long_term_pics, num_long_term_pics_limit));
	}
	for(uint32_t i = 0;i < lt_idx_sps.size();++i){
		if(lt_idx_sps[i] > pSps->num_long_term_ref_pics_sps-1){
			errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] lt_idx_sps[%d] value (%ld) not in valid range (0..{})", i, lt_idx_sps[i], pSps->num_long_term_ref_pics_sps-1));
		}
	}
	uint32_t delta_poc_msb_cycle_lt_limit = 1 << (32 - pSps->log2_max_pic_order_cnt_lsb_minus4-4);
	for(uint32_t i = 0;i < delta_poc_msb_cycle_lt.size();++i){
		if(delta_poc_msb_cycle_lt[i] > delta_poc_msb_cycle_lt_limit){
			errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] delta_poc_msb_cycle_lt[%d] value (%ld) not in valid range (0..{})", i, delta_poc_msb_cycle_lt[i], delta_poc_msb_cycle_lt_limit));
		}
	}
	if(slice_type == SliceType_P || slice_type == SliceType_B){
		if(num_ref_idx_active_override_flag){
			if(num_ref_idx_l0_active_minus1 > 14){
				errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] num_ref_idx_l0_active_minus1 value (%ld) not in valid range (0..14)", num_ref_idx_l0_active_minus1));
			}
			if(num_ref_idx_l1_active_minus1 > 14){
				errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] num_ref_idx_l1_active_minus1 value (%ld) not in valid range (0..14)", num_ref_idx_l1_active_minus1));
			}
		}
		if(pPps->lists_modification_present_flag && NumPicTotalCurr > 1){
			ref_pic_lists_modification.validate(*this);
			errors.add(ref_pic_lists_modification.errors);
			ref_pic_lists_modification.errors.clear();
		}
		if(collocated_ref_idx > num_ref_idx_l1_active_minus1){
			errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] collocated_ref_idx value (%ld) not in valid range (0..{})", collocated_ref_idx, num_ref_idx_l1_active_minus1));
		}
		pred_weight_table.validate(*this);
		errors.add(pred_weight_table.errors);
		pred_weight_table.errors.clear();
		if(five_minus_max_num_merge_cand > 4){
			errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] five_minus_max_num_merge_cand value (%ld) not in valid range (0..4)", five_minus_max_num_merge_cand));
		}
	}
	if(SliceQpY < -pSps->QpBdOffsetY || SliceQpY > 51) errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] SliceQpY value (%ld) not in valid range ({}..51)", SliceQpY, -pSps->QpBdOffsetY));
	if(slice_cb_qp_offset < -12 || slice_cb_qp_offset > 12) errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] slice_cb_qp_offset value (%ld) not in valid range (-12..12)", slice_cb_qp_offset));
	if(slice_cr_qp_offset < -12 || slice_cr_qp_offset > 12) errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] slice_cr_qp_offset value (%ld) not in valid range (-12..12)", slice_cr_qp_offset));
	if(slice_beta_offset_div2 < -6 || slice_beta_offset_div2 > 6) errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] slice_beta_offset_div2 value (%ld) not in valid range (-6..6)", slice_beta_offset_div2));
	if(slice_tc_offset_div2 < -6 || slice_tc_offset_div2 > 6) errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] slice_tc_offset_div2 value (%ld) not in valid range (-6..6)", slice_tc_offset_div2));
	uint32_t num_entry_point_offsets_limit = num_entry_point_offsets;
	if(!pPps->tiles_enabled_flag && pPps->entropy_coding_sync_enabled_flag){
		num_entry_point_offsets_limit = pSps->PicHeightInCtbsY-1;
	} else if (pPps->tiles_enabled_flag && !pPps->entropy_coding_sync_enabled_flag){
		num_entry_point_offsets_limit = (pPps->num_tile_columns_minus1+1)*(pPps->num_tile_rows_minus1+1)-1;
	} else if (pPps->tiles_enabled_flag && pPps->entropy_coding_sync_enabled_flag){
		num_entry_point_offsets_limit = (pPps->num_tile_columns_minus1+1)*pSps->PicHeightInCtbsY-1;
	}
	if(num_entry_point_offsets > num_entry_point_offsets_limit) errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] num_entry_point_offsets value (%ld) not in valid range (0..{})", num_entry_point_offsets, num_entry_point_offsets_limit));
	if(offset_len_minus1 > 31) errors.add(H26XError::Minor, H26XUtils::formatString("[Slice] offset_len_minus1 value (%ld) not in valid range (0..31)", offset_len_minus1));
}
