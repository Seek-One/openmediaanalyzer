#include <iostream>
#include <fmt/core.h>
#include <algorithm>

#include "H265PPS.h"
#include "H265SPS.h"
#include "H265VPS.h"

#include "H265Slice.h"

RefPicListsModification::RefPicListsModification()
{
	ref_pic_list_modification_flag_l0 = 0;
	ref_pic_list_modification_flag_l1 = 0;
}

std::vector<std::string> RefPicListsModification::dump_fields(const H265Slice& slice) const{
	std::vector<std::string> fields;
	fields.push_back(fmt::format("ref_pic_list_modification_flag_l0:{}", ref_pic_list_modification_flag_l0));
	if(ref_pic_list_modification_flag_l0){
		for(int i = 0;i <= slice.num_ref_idx_l0_active_minus1;++i) fields.push_back(fmt::format("  list_entry_l0[{}]:{}", i, list_entry_l0[i]));
	}
	if(slice.slice_type == H265Slice::SliceType_B){
		fields.push_back(fmt::format("ref_pic_list_modification_flag_l1:{}", ref_pic_list_modification_flag_l1));
		if(ref_pic_list_modification_flag_l0){
			for(int i = 0;i <= slice.num_ref_idx_l1_active_minus1;++i) fields.push_back(fmt::format("  list_entry_l1[{}]:{}", i, list_entry_l1[i]));
		}
	}
	return fields;
}

void RefPicListsModification::validate(const H265Slice& h265Slice){
	uint32_t list_entry_limit = h265Slice.NumPicTotalCurr-1;
	for(int i = 0;i < list_entry_l0.size();++i){
		if(list_entry_l0[i] > list_entry_limit) minorErrors.push_back(fmt::format("[Slice RPLM] list_entry_l0[{}] value ({}) not in valid range (0..{})", i, list_entry_l0[i], list_entry_limit));
	}
	for(int i = 0;i < list_entry_l1.size();++i){
		if(list_entry_l1[i] > list_entry_limit) minorErrors.push_back(fmt::format("[Slice RPLM] list_entry_l1[{}] value ({}) not in valid range (0..{})", i, list_entry_l1[i], list_entry_limit));
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

std::vector<std::string> H265PredWeightTable::dump_fields(const H265Slice& h265Slice){
	std::vector<std::string> fields;
	fields.push_back(fmt::format("luma_log2_weight_denom:{}", luma_log2_weight_denom));
	H265SPS* h265SPS = h265Slice.getSPS();
	if(h265SPS->ChromaArrayType != 0) fields.push_back(fmt::format("delta_chroma_log2_weight_denom:{}", delta_chroma_log2_weight_denom));
	for(int i = 0;i <= h265Slice.num_ref_idx_l0_active_minus1;++i){
		fields.push_back(fmt::format("  luma_weight_l0_flag[{}]:{}", i, luma_weight_l0_flag[i]));
	}
	if(h265SPS->ChromaArrayType != 0){
		for(int i = 0;i <= h265Slice.num_ref_idx_l0_active_minus1;++i){
			fields.push_back(fmt::format("  chroma_weight_l0_flag[{}]:{}", i, chroma_weight_l0_flag[i]));
		}
	}
	for(int i = 0;i <= h265Slice.num_ref_idx_l0_active_minus1;++i){
		if(luma_weight_l0_flag[i]){
			fields.push_back(fmt::format("    delta_luma_weight_l0[{}]:{}", i, delta_luma_weight_l0[i]));
			fields.push_back(fmt::format("    luma_offset_l0[{}]:{}", i, luma_offset_l0[i]));
		}
		if(chroma_weight_l0_flag[i]){
			for(int j = 0;j < 2;++j){
				fields.push_back(fmt::format("    delta_luma_weight_l0[{}][{}]:{}", i, j, delta_chroma_weight_l0[i][j]));
				fields.push_back(fmt::format("    delta_chroma_offset_l0[{}][{}]:{}", i, j, delta_chroma_offset_l0[i][j]));
			}
		}
	}
	if(h265Slice.slice_type == H265Slice::SliceType_B){
		for(int i = 0;i <= h265Slice.num_ref_idx_l1_active_minus1;++i){
			fields.push_back(fmt::format("  luma_weight_l0_flag[{}]:{}", i, luma_weight_l1_flag[i]));
		}
		if(h265SPS->ChromaArrayType != 0){
			for(int i = 0;i <= h265Slice.num_ref_idx_l1_active_minus1;++i){
				fields.push_back(fmt::format("  chroma_weight_l0_flag[{}]:{}", i, chroma_weight_l1_flag[i]));
			}
		}
		for(int i = 0;i <= h265Slice.num_ref_idx_l1_active_minus1;++i){
			if(luma_weight_l1_flag[i]){
				fields.push_back(fmt::format("    delta_luma_weight_l0[{}]:{}", i, delta_luma_weight_l1[i]));
				fields.push_back(fmt::format("    luma_offset_l0[{}]:{}", i, luma_offset_l1[i]));
			}
			if(chroma_weight_l1_flag[i]){
				for(int j = 0;j < 2;++j){
					fields.push_back(fmt::format("    delta_luma_weight_l0[{}][{}]:{}", i, j, delta_chroma_weight_l1[i][j]));
					fields.push_back(fmt::format("    delta_chroma_offset_l0[{}][{}]:{}", i, j, delta_chroma_offset_l1[i][j]));
				}
			}
		}
	}
	return fields;
}

void H265PredWeightTable::validate(const H265Slice& h265Slice){
	H265SPS* h265SPS = h265Slice.getSPS();
	int16_t delta_chroma_offset_lower_bound = INT16_MIN;
	int16_t delta_chroma_offset_upper_bound = INT16_MAX;
	if(h265SPS){
		delta_chroma_offset_lower_bound = -4*h265SPS->sps_range_extension.WpOffsetHalfRangeC;
		delta_chroma_offset_upper_bound = 4*h265SPS->sps_range_extension.WpOffsetHalfRangeC-1;
	}
	if(luma_log2_weight_denom > 7) minorErrors.push_back(fmt::format("[Slice PWT] luma_log2_weight_denom value ({}) not in valid range (0..7)", luma_log2_weight_denom));
	if(delta_chroma_log2_weight_denom > 7-luma_log2_weight_denom) minorErrors.push_back(fmt::format("[Slice PWT] delta_chroma_log2_weight_denom value ({}) not in valid range (0..{})", delta_chroma_log2_weight_denom, 7-luma_log2_weight_denom));
	for(int i = 0;i < 15;++i){
		if(luma_weight_l0_flag[i]){
			if(delta_luma_weight_l0[i] < -128 || delta_luma_weight_l0[i] > 127) minorErrors.push_back(fmt::format("[Slice PWT] delta_luma_weight_l0[{}] value ({}) not in valid range (-128..127)", i, delta_luma_weight_l0[i]));
		}
		if(chroma_weight_l0_flag[i]){
			for(int j = 0;j < 2;++j){
				if(delta_chroma_weight_l0[i][j] < -128 || delta_chroma_weight_l0[i][j] > 127) minorErrors.push_back(fmt::format("delta_chroma_weight[{}][{}] value ({}) not in valid range (-128..127)", i, j, delta_chroma_weight_l0[i][j]));
				if(delta_chroma_offset_l0[i][j] < delta_chroma_offset_lower_bound || delta_chroma_offset_l0[i][j] > delta_chroma_offset_upper_bound){
					minorErrors.push_back(fmt::format("[Slice PWT] delta_chroma_offset_l0[{}][{}] value ({}) not in valid range ({}..{})", i, j, delta_chroma_offset_l0[i][j], delta_chroma_offset_lower_bound, delta_chroma_offset_upper_bound));
				}
			}
		}
		if(luma_weight_l1_flag[i]){
			if(delta_luma_weight_l1[i] < -128 || delta_luma_weight_l1[i] > 127) minorErrors.push_back(fmt::format("[Slice PWT] delta_luma_weight_l1[{}] value ({}) not in valid range (-128..127)", i, delta_luma_weight_l1[i]));
		}
		if(chroma_weight_l1_flag[i]){
			for(int j = 0;j < 2;++j){
				if(delta_chroma_weight_l1[i][j] < -128 || delta_chroma_weight_l1[i][j] > 127) minorErrors.push_back(fmt::format("delta_chroma_weight[{}][{}] value ({}) not in valid range (-128..127)", i, j, delta_chroma_weight_l1[i][j]));
				if(delta_chroma_offset_l1[i][j] < delta_chroma_offset_lower_bound || delta_chroma_offset_l1[i][j] > delta_chroma_offset_upper_bound){
					minorErrors.push_back(fmt::format("[Slice PWT] delta_chroma_offset_l1[{}][{}] value ({}) not in valid range ({}..{})", i, j, delta_chroma_offset_l1[i][j], delta_chroma_offset_lower_bound, delta_chroma_offset_upper_bound));
				}
			}
		}
	}
}

H265Slice::H265Slice():
	H265Slice(0, UnitType_Unspecified, 0, 0, 0, nullptr)
{}

H265Slice::H265Slice(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, uint8_t* nal_data):
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
}

std::vector<std::string> H265Slice::dump_fields(){
	std::vector<std::string> fields = H265NAL::dump_fields();
	fields.push_back(fmt::format("first_slice_segment_in_pic_flag:{}", first_slice_segment_in_pic_flag));
	if(nal_unit_type >= UnitType_BLA_W_LP && nal_unit_type <= UnitType_IRAP_VCL23) fields.push_back(fmt::format("  no_output_of_prior_pics_flag:{}", no_output_of_prior_pics_flag));
	fields.push_back(fmt::format("slice_pic_parameter_set_id:{}", slice_pic_parameter_set_id));
	H265PPS* h265PPS = getPPS();
	H265SPS* h265SPS = getSPS();
	if(!h265PPS || !h265SPS) return fields;
	if(!first_slice_segment_in_pic_flag){
		if(h265PPS->dependent_slice_segments_enabled_flag){
			fields.push_back(fmt::format("    dependent_slice_segment_flag:{}", dependent_slice_segment_flag));
		}
		fields.push_back(fmt::format("  slice_segment_address:{}", slice_segment_address));
	}
	if(!dependent_slice_segment_flag){
		fields.push_back(fmt::format("  slice_type:{}", slice_type));
		if(h265PPS->output_flag_present_flag) fields.push_back(fmt::format("    pic_output_flag:{}", pic_output_flag));
		if(h265SPS->separate_colour_plane_flag == 1) fields.push_back(fmt::format("    colour_plane_id:{}", colour_plane_id));
		fields.push_back(fmt::format("    slice_pic_order_cnt_lsb:{}", slice_pic_order_cnt_lsb));
		if(!IdrPicFlag){
			fields.push_back(fmt::format("    short_term_ref_pic_set_sps_flag:{}", short_term_ref_pic_set_sps_flag));
			if(!short_term_ref_pic_set_sps_flag){
				std::vector<std::string> shortTermRefPicSetFields = h265SPS->short_term_ref_pic_set[h265SPS->num_short_term_ref_pic_sets].dump_fields(h265SPS->num_short_term_ref_pic_sets, h265SPS->num_short_term_ref_pic_sets);
				std::transform(shortTermRefPicSetFields.begin(), shortTermRefPicSetFields.end(), std::back_inserter(fields), [](const std::string& subField){
					return "    " + subField;
				});
			} else if(h265SPS->num_short_term_ref_pic_sets > 1) fields.push_back(fmt::format("      short_term_ref_pic_set_idx:{}", short_term_ref_pic_set_idx));
			if(h265SPS->long_term_ref_pics_present_flag){
				if(h265SPS->num_long_term_ref_pics_sps > 0) fields.push_back(fmt::format("      num_long_term_sps:{}", num_long_term_sps));
				fields.push_back(fmt::format("    num_long_term_pics:{}", num_long_term_pics));
				for(int i = 0;i < num_long_term_sps + num_long_term_pics;++i){
					if(i < num_long_term_sps){
						if(num_long_term_sps) fields.push_back(fmt::format("      lt_idx_sps[{}]:{}", i, lt_idx_sps[i]));
					} else {
						fields.push_back(fmt::format("      poc_lsb_lt[{}]:{}", i, poc_lsb_lt[i]));
						fields.push_back(fmt::format("      used_by_curr_pic_lt_flag[{}]:{}", i, used_by_curr_pic_lt_flag[i]));
					}
					fields.push_back(fmt::format("    delta_poc_msb_present_flag[{}]:{}", i, delta_poc_msb_present_flag[i]));
					if(delta_poc_msb_present_flag[i]) fields.push_back(fmt::format("      delta_poc_msb_cycle_lt[{}]:{}", i, delta_poc_msb_cycle_lt[i]));
				}
			}
			if(h265SPS->sps_temporal_mvp_enabled_flag) fields.push_back(fmt::format("  slice_temporal_mvp_enabled_flag:{}", slice_temporal_mvp_enabled_flag));
		}
		if(h265SPS->sample_adaptive_offset_enabled_flag){
			fields.push_back(fmt::format("  slice_sao_luma_flag:{}", slice_sao_luma_flag));
			if(h265SPS->ChromaArrayType !=0) fields.push_back(fmt::format("    slice_sao_chroma_flag:{}", slice_sao_chroma_flag));
		}
		if(slice_type == SliceType_P || slice_type == SliceType_B){
			fields.push_back(fmt::format("  num_ref_idx_active_override_flag:{}", num_ref_idx_active_override_flag));
			if(num_ref_idx_active_override_flag){
				fields.push_back(fmt::format("    num_ref_idx_l0_active_minus1:{}", num_ref_idx_l0_active_minus1));
				if(slice_type == SliceType_B) fields.push_back(fmt::format("      num_ref_idx_l1_active_minus1:{}", num_ref_idx_l1_active_minus1));
			}
			if(h265PPS->lists_modification_present_flag && NumPicTotalCurr > 1){
				std::vector<std::string> refPicListsModificationFields =  ref_pic_lists_modification.dump_fields(*this);
				std::transform(refPicListsModificationFields.begin(), refPicListsModificationFields.end(), std::back_inserter(fields), [](const std::string& subField){
					return "    " + subField;
				});
			}
			if(slice_type == SliceType_B) fields.push_back(fmt::format("    mvd_l1_zero_flag:{}", mvd_l1_zero_flag));
			if(cabac_init_flag) fields.push_back(fmt::format("    cabac_init_flag:{}", cabac_init_flag));
			if(slice_temporal_mvp_enabled_flag){
				if(slice_type == SliceType_B) fields.push_back(fmt::format("      collocated_from_l0_flag:{}", collocated_from_l0_flag));
				if((collocated_from_l0_flag && num_ref_idx_l0_active_minus1 > 0) || (!collocated_from_l0_flag && num_ref_idx_l1_active_minus1 > 0)){
					fields.push_back(fmt::format("      collocated_ref_idx:{}", collocated_ref_idx));
				}
			}
			if((h265PPS->weighted_pred_flag && slice_type == SliceType_P) ||
				(h265PPS->weighted_bipred_flag && slice_type == SliceType_B)){
					std::vector<std::string> pred_weight_table_fields = pred_weight_table.dump_fields(*this);
					std::transform(pred_weight_table_fields.begin(), pred_weight_table_fields.end(), std::back_inserter(fields), [](const std::string& subField){
						return "  " + subField;
					});
			}
			fields.push_back(fmt::format("  five_minus_max_num_merge_cand:{}", five_minus_max_num_merge_cand));
			if(h265SPS->sps_scc_extension.motion_vector_resolution_control_idc == 2){
				fields.push_back(fmt::format("  use_integer_mv_flag:{}", use_integer_mv_flag));
			}
		}
		fields.push_back(fmt::format("  slice_qp_delta:{}", slice_qp_delta));
		if(h265PPS->pps_slice_chroma_qp_offsets_present_flag){
			fields.push_back(fmt::format("    slice_cb_qp_offset:{}", slice_cb_qp_offset));
			fields.push_back(fmt::format("    slice_cr_qp_offset:{}", slice_cr_qp_offset));
		}
		if(h265PPS->pps_scc_extension.pps_slice_act_qp_offsets_present_flag){
			fields.push_back(fmt::format("    slice_act_cb_qp_offset:{}", slice_act_cb_qp_offset));
			fields.push_back(fmt::format("    slice_act_cr_qp_offset:{}", slice_act_cr_qp_offset));
			fields.push_back(fmt::format("    slice_act_y_qp_offset:{}", slice_act_y_qp_offset));
		}
		if(h265PPS->pps_range_extension.chroma_qp_offset_list_enabled_flag){
			fields.push_back(fmt::format("    cu_chroma_qp_offset_enabled_flag:{}", cu_chroma_qp_offset_enabled_flag));
		}
		if(h265PPS->deblocking_filter_override_enabled_flag){
			fields.push_back(fmt::format("    deblocking_filter_override_flag:{}", deblocking_filter_override_flag));
		}
		if(deblocking_filter_override_flag){
			fields.push_back(fmt::format("    slice_deblocking_filter_disabled_flag:{}", slice_deblocking_filter_disabled_flag));
			if(!slice_deblocking_filter_disabled_flag){
				fields.push_back(fmt::format("      slice_beta_offset_div2:{}", slice_beta_offset_div2));
				fields.push_back(fmt::format("      slice_tc_offset_div2:{}", slice_tc_offset_div2));
				
			}
		}
		if(h265PPS->pps_loop_filter_across_slices_enabled_flag && (slice_sao_luma_flag || slice_sao_chroma_flag || !slice_deblocking_filter_disabled_flag)){
			fields.push_back(fmt::format("    slice_loop_filter_across_slices_enabled_flag:{}", slice_loop_filter_across_slices_enabled_flag));
		}
		if(h265PPS->tiles_enabled_flag || h265PPS->entropy_coding_sync_enabled_flag){
			fields.push_back(fmt::format("    num_entry_point_offsets:{}", num_entry_point_offsets));
			if(num_entry_point_offsets > 0){
				fields.push_back(fmt::format("      offset_len_minus1:{}", offset_len_minus1));
				for(int i = 0;i < num_entry_point_offsets;++i){
					fields.push_back(fmt::format("        entry_point_offset_minus1[{}]:{}", i, entry_point_offset_minus1[i]));
				}
			}
		}
	}
	return fields;
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

H265VPS* H265Slice::getVPS() const{
	H265SPS* pSps = getSPS();
	if(!pSps) return nullptr;
	auto referencedVPS = H265VPS::VPSMap.find(pSps->sps_video_parameter_set_id);
	if(referencedVPS == H265VPS::VPSMap.end()) return nullptr;
	return referencedVPS->second;
}

void H265Slice::validate(){
	H265NAL::validate();
	if(slice_pic_parameter_set_id > 63) minorErrors.push_back(fmt::format("[Slice] slice_pic_parameter_set_id value ({}) not in valid range (0..63)", slice_pic_parameter_set_id));
	H265PPS* pPps = getPPS();
	if(!pPps){
		majorErrors.push_back(fmt::format("[Slice] reference to unknown PPS ({})", slice_pic_parameter_set_id));
		return;
	}
	H265SPS* pSps = getSPS();
	if(!pSps){
		majorErrors.push_back(fmt::format("[Slice] reference to unknown SPS ({})", pPps->pps_seq_parameter_set_id));
		return;
	}
	H265VPS* pVps = getVPS();
	if(!pVps) majorErrors.push_back(fmt::format("[Slice] reference to unknown VPS ({})", pSps->sps_video_parameter_set_id));
	if(pPps->TemporalId > TemporalId) minorErrors.push_back("[Slice] referenced PPS has a greater TemporalId value");
	if(pPps->nuh_layer_id > nuh_layer_id) minorErrors.push_back("[Slice] referenced PPS has a greater nuh_layer_id value");
	if(pSps->nuh_layer_id > nuh_layer_id) minorErrors.push_back("[Slice] referenced SPS has a greater nuh_layer_id value");
	if(slice_segment_address > pSps->PicSizeInCtbsY-1) minorErrors.push_back(fmt::format("[Slice] slice_segment_address value ({}) not in valid range (0..{})", slice_segment_address, pSps->PicSizeInCtbsY-1));
	if(!pPps->dependent_slice_segments_enabled_flag){
		if(slice_type > 2) minorErrors.push_back(fmt::format("[Slice] slice_type value ({}) not in valid range (0..2)", slice_type));
		if(nal_unit_type >= UnitType_BLA_W_LP && nal_unit_type <= UnitType_IRAP_VCL23 &&
			nuh_layer_id == 0 && !pPps->pps_scc_extension.pps_curr_pic_ref_enabled_flag &&
			slice_type != 2) minorErrors.push_back("[Slice] slice_type value of an IRAP picture not equal to 2");
		if(pSps->sps_max_dec_pic_buffering_minus1[TemporalId] == 0 && nuh_layer_id == 0 &&
			!pPps->pps_scc_extension.pps_curr_pic_ref_enabled_flag && slice_type != 2) {
			minorErrors.push_back("[Slice] slice_type value not equal to 2");
		}
		if(colour_plane_id > 2) minorErrors.push_back(fmt::format("[Slice] colour_plane_id value ({}) not in valid range (0..2)", colour_plane_id));
	}
	if(slice_pic_order_cnt_lsb > pSps->MaxPicOrderCntLsb-1) minorErrors.push_back(fmt::format("[Slice] slice_pic_order_cnt_lsb value ({}) not in valid range (0..{})", slice_pic_order_cnt_lsb, pSps->MaxPicOrderCntLsb-1));
	if(short_term_ref_pic_set_idx > pSps->num_short_term_ref_pic_sets-1) minorErrors.push_back(fmt::format("[Slice] short_term_ref_pic_set_idx value ({}) not in valid range (0..{})", short_term_ref_pic_set_idx, pSps->num_short_term_ref_pic_sets-1));
	if(num_long_term_sps > pSps->num_long_term_ref_pics_sps) minorErrors.push_back(fmt::format("[Slice] num_long_term_sps value ({}) not in valid range (0..{})", num_long_term_sps, pSps->num_long_term_ref_pics_sps));
	uint32_t num_long_term_pics_limit = pSps->sps_max_dec_pic_buffering_minus1[TemporalId] - 
										pSps->short_term_ref_pic_set[CurrRpsIdx].NumNegativePics -
										pSps->short_term_ref_pic_set[CurrRpsIdx].NumPositivePics -
										num_long_term_sps - pPps->TwoVersionsOfCurrDecPicFlag;
	if(nuh_layer_id == 0 && num_long_term_pics > num_long_term_pics_limit){
		minorErrors.push_back(fmt::format("[Slice] num_long_term_pics value ({}) not in valid range (0..{})", num_long_term_pics, num_long_term_pics_limit));
	}
	for(int i = 0;i < lt_idx_sps.size();++i){
		if(lt_idx_sps[i] > pSps->num_long_term_ref_pics_sps-1) minorErrors.push_back(fmt::format("[Slice] lt_idx_sps[{}] value ({}) not in valid range (0..{})", i, lt_idx_sps[i], pSps->num_long_term_ref_pics_sps-1));
	}
	uint32_t delta_poc_msb_cycle_lt_limit = 1 << (32 - pSps->log2_max_pic_order_cnt_lsb_minus4-4);
	for(int i = 0;i < delta_poc_msb_cycle_lt.size();++i){
		if(delta_poc_msb_cycle_lt[i] > delta_poc_msb_cycle_lt_limit) minorErrors.push_back(fmt::format("[Slice] delta_poc_msb_cycle_lt[{}] value ({}) not in valid range (0..{})", i, delta_poc_msb_cycle_lt[i], delta_poc_msb_cycle_lt_limit));
	}
	if(slice_type == SliceType_P || slice_type == SliceType_B){
		if(num_ref_idx_active_override_flag){
			if(num_ref_idx_l0_active_minus1 > 14) minorErrors.push_back(fmt::format("[Slice] num_ref_idx_l0_active_minus1 value ({}) not in valid range (0..14)", num_ref_idx_l0_active_minus1));
			if(num_ref_idx_l1_active_minus1 > 14) minorErrors.push_back(fmt::format("[Slice] num_ref_idx_l1_active_minus1 value ({}) not in valid range (0..14)", num_ref_idx_l1_active_minus1));
		}
		if(pPps->lists_modification_present_flag && NumPicTotalCurr > 1){
			ref_pic_lists_modification.validate(*this);
			minorErrors.insert(minorErrors.end(), ref_pic_lists_modification.minorErrors.begin(), ref_pic_lists_modification.minorErrors.end());
			majorErrors.insert(majorErrors.end(), ref_pic_lists_modification.majorErrors.begin(), ref_pic_lists_modification.majorErrors.end());
			ref_pic_lists_modification.minorErrors.clear();
			ref_pic_lists_modification.majorErrors.clear();
		}
		if(collocated_ref_idx > num_ref_idx_l1_active_minus1){
			minorErrors.push_back(fmt::format("[Slice] collocated_ref_idx value ({}) not in valid range (0..{})", collocated_ref_idx, num_ref_idx_l1_active_minus1));
		}
		pred_weight_table.validate(*this);
		minorErrors.insert(minorErrors.end(), pred_weight_table.minorErrors.begin(), pred_weight_table.minorErrors.end());
		majorErrors.insert(majorErrors.end(), pred_weight_table.majorErrors.begin(), pred_weight_table.majorErrors.end());
		pred_weight_table.minorErrors.clear();
		pred_weight_table.majorErrors.clear();
		if(five_minus_max_num_merge_cand > 4) minorErrors.push_back(fmt::format("[Slice] five_minus_max_num_merge_cand value ({}) not in valid range (0..4)", five_minus_max_num_merge_cand));
	}
	if(SliceQpY < -pSps->QpBdOffsetY || SliceQpY > 51) minorErrors.push_back(fmt::format("[Slice] SliceQpY value ({}) not in valid range ({}..51)", SliceQpY, -pSps->QpBdOffsetY));
	if(slice_cb_qp_offset < -12 || slice_cb_qp_offset > 12) minorErrors.push_back(fmt::format("[Slice] slice_cb_qp_offset value ({}) not in valid range (-12..12)", slice_cb_qp_offset));
	if(slice_cr_qp_offset < -12 || slice_cr_qp_offset > 12) minorErrors.push_back(fmt::format("[Slice] slice_cr_qp_offset value ({}) not in valid range (-12..12)", slice_cr_qp_offset));
	if(slice_beta_offset_div2 < -6 || slice_beta_offset_div2 > 6) minorErrors.push_back(fmt::format("[Slice] slice_beta_offset_div2 value ({}) not in valid range (-6..6)", slice_beta_offset_div2));
	if(slice_tc_offset_div2 < -6 || slice_tc_offset_div2 > 6) minorErrors.push_back(fmt::format("[Slice] slice_tc_offset_div2 value ({}) not in valid range (-6..6)", slice_tc_offset_div2));
	uint32_t num_entry_point_offsets_limit = num_entry_point_offsets;
	if(!pPps->tiles_enabled_flag && pPps->entropy_coding_sync_enabled_flag){
		num_entry_point_offsets_limit = pSps->PicHeightInCtbsY-1;
	} else if (pPps->tiles_enabled_flag && !pPps->entropy_coding_sync_enabled_flag){
		num_entry_point_offsets_limit = (pPps->num_tile_columns_minus1+1)*(pPps->num_tile_rows_minus1+1)-1;
	} else if (pPps->tiles_enabled_flag && pPps->entropy_coding_sync_enabled_flag){
		num_entry_point_offsets_limit = (pPps->num_tile_columns_minus1+1)*pSps->PicHeightInCtbsY-1;
	}
	if(num_entry_point_offsets > num_entry_point_offsets_limit) minorErrors.push_back(fmt::format("[Slice] num_entry_point_offsets value ({}) not in valid range (0..{})", num_entry_point_offsets, num_entry_point_offsets_limit)); 
	if(offset_len_minus1 > 31) minorErrors.push_back(fmt::format("[Slice] offset_len_minus1 value ({}) not in valid range (0..31)", offset_len_minus1)); 
}
