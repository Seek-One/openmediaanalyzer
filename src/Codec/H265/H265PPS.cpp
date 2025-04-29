#include <sstream>
#include <algorithm>

#include "H265SPS.h"

#include "H265PPS.h"


H265PPSRangeExtension::H265PPSRangeExtension(){}

std::vector<std::string> H265PPSRangeExtension::dump_fields(){
	std::vector<std::string> fields;
	return fields;
}

H265PPSMultilayerExtension::H265PPSMultilayerExtension(){}

std::vector<std::string> H265PPSMultilayerExtension::dump_fields(){
	std::vector<std::string> fields;
	return fields;
}


H265PPS3DExtension::H265PPS3DExtension(){}

std::vector<std::string> H265PPS3DExtension::dump_fields(){
	std::vector<std::string> fields;
	return fields;
}

H265PPSSCCExtension::H265PPSSCCExtension(){
	pps_curr_pic_ref_enabled_flag = 0;
}

std::vector<std::string> H265PPSSCCExtension::dump_fields(){
	std::vector<std::string> fields;
	return fields;
}

H265PPS::H265PPS():
	H265PPS(0, UnitType_Unspecified, 0, 0, 0, nullptr)
{}

H265PPS::H265PPS(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, uint8_t* nal_data):
	H265NAL(forbidden_zero_bit, nal_unit_type, nuh_layer_id, nuh_temporal_id_plus1, nal_size, nal_data)
{
	pps_pic_parameter_set_id = 0;
	pps_seq_parameter_set_id = 0;
	dependent_slice_segments_enabled_flag = 0;
	output_flag_present_flag = 0;
	num_extra_slice_header_bits = 0;
	sign_data_hiding_enabled_flag = 0;
	cabac_init_present_flag = 0;
	num_ref_idx_l0_default_active_minus1 = 0;
	num_ref_idx_l1_default_active_minus1 = 0;
	init_qp_minus26 = 0;
	constrained_intra_pred_flag = 0;
	transform_skip_enabled_flag = 0;
	cu_qp_delta_enabled_flag = 0;
	diff_cu_qp_delta_depth = 0;
	pps_cb_qp_offset = 0;
	pps_cr_qp_offset = 0;
	pps_slice_chroma_qp_offsets_present_flag = 0;
	weighted_pred_flag = 0;
	weighted_bipred_flag = 0;
	transquant_bypass_enabled_flag = 0;
	tiles_enabled_flag = 0;
	entropy_coding_sync_enabled_flag = 0;
	num_tile_columns_minus1 = 0;
	num_tile_rows_minus1 = 0;
	uniform_spacing_flag = 1;
	loop_filter_across_tiles_enabled_flag = 1;
	pps_loop_filter_across_slices_enabled_flag = 0;
	deblocking_filter_control_present_flag = 0;
	deblocking_filter_override_enabled_flag = 0;
	pps_deblocking_filter_disabled_flag = 0;
	pps_beta_offset_div2 = 0;
	pps_tc_offset_div2 = 0;
	pps_scaling_list_data_present_flag = 0;
	lists_modification_present_flag = 0;
	log2_parallel_merge_level_minus2 = 0;
	slice_segment_header_extension_present_flag = 0;
	pps_extension_present_flag = 0;
	pps_range_extension_flag = 0;
	pps_multilayer_extension_flag = 0;
	pps_3d_extension_flag = 0;
	pps_scc_extension_flag = 0;
}

H265PPS::~H265PPS(){}

std::vector<std::string> H265PPS::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "pps_pic_parameter_set_id:" << (int)pps_pic_parameter_set_id).str());
	fields.push_back((std::ostringstream() << "pps_seq_parameter_set_id:" << (int)pps_seq_parameter_set_id).str());
	fields.push_back((std::ostringstream() << "dependent_slice_segments_enabled_flag:" << (int)dependent_slice_segments_enabled_flag).str());
	fields.push_back((std::ostringstream() << "output_flag_present_flagd:" << (int)output_flag_present_flag).str());
	fields.push_back((std::ostringstream() << "num_extra_slice_header_bitser_set_id:" << (int)num_extra_slice_header_bits).str());
	fields.push_back((std::ostringstream() << "sign_data_hiding_enabled_flag:" << (int)sign_data_hiding_enabled_flag).str());
	fields.push_back((std::ostringstream() << "cabac_init_present_flag:" << (int)cabac_init_present_flag).str());
	fields.push_back((std::ostringstream() << "num_ref_idx_l0_default_active_minus1:" << num_ref_idx_l0_default_active_minus1).str());
	fields.push_back((std::ostringstream() << "num_ref_idx_l1_default_active_minus1:" << num_ref_idx_l1_default_active_minus1).str());
	fields.push_back((std::ostringstream() << "init_qp_minus26:" << init_qp_minus26).str());
	fields.push_back((std::ostringstream() << "constrained_intra_pred_flag:" << (int)constrained_intra_pred_flag).str());
	fields.push_back((std::ostringstream() << "transform_skip_enabled_flag:" << (int)transform_skip_enabled_flag).str());
	fields.push_back((std::ostringstream() << "cu_qp_delta_enabled_flag:" << (int)cu_qp_delta_enabled_flag).str());
	if(cu_qp_delta_enabled_flag){
		fields.push_back((std::ostringstream() << "  diff_cu_qp_delta_depth:" << diff_cu_qp_delta_depth).str());
	}
	fields.push_back((std::ostringstream() << "pps_cb_qp_offset:" << pps_cb_qp_offset).str());
	fields.push_back((std::ostringstream() << "pps_cr_qp_offset:" << pps_cr_qp_offset).str());
	fields.push_back((std::ostringstream() << "pps_slice_chroma_qp_offsets_present_flag:" << (int)pps_slice_chroma_qp_offsets_present_flag).str());
	fields.push_back((std::ostringstream() << "weighted_pred_flag:" << (int)weighted_pred_flag).str());
	fields.push_back((std::ostringstream() << "weighted_bipred_flag:" << (int)weighted_bipred_flag).str());
	fields.push_back((std::ostringstream() << "transquant_bypass_enabled_flag:" << (int)transquant_bypass_enabled_flag).str());
	fields.push_back((std::ostringstream() << "tiles_enabled_flag:" << (int)tiles_enabled_flag).str());
	fields.push_back((std::ostringstream() << "entropy_coding_sync_enabled_flag:" << (int)entropy_coding_sync_enabled_flag).str());
	if(tiles_enabled_flag){
		fields.push_back((std::ostringstream() << "  num_tile_columns_minus1:" << num_tile_columns_minus1).str());
		fields.push_back((std::ostringstream() << "  num_tile_rows_minus1:" << num_tile_rows_minus1).str());
		fields.push_back((std::ostringstream() << "  uniform_spacing_flag:" << (int)uniform_spacing_flag).str());
		if(!uniform_spacing_flag){
			for(int i = 0;i < num_tile_columns_minus1;++i) fields.push_back((std::ostringstream() << "    column_width_minus1[" << i << "]:" << column_width_minus1[i]).str());
			for(int i = 0;i < num_tile_rows_minus1;++i) fields.push_back((std::ostringstream() << "    row_height_minus1[" << i << "]:" << row_height_minus1[i]).str());
		}
		
		fields.push_back((std::ostringstream() << "  loop_filter_across_tiles_enabled_flag:" << (int)loop_filter_across_tiles_enabled_flag).str());
	}
	fields.push_back((std::ostringstream() << "pps_loop_filter_across_slices_enabled_flag:" << (int)pps_loop_filter_across_slices_enabled_flag).str());
	fields.push_back((std::ostringstream() << "deblocking_filter_control_present_flag:" << (int)deblocking_filter_control_present_flag).str());
	if(deblocking_filter_control_present_flag){
		fields.push_back((std::ostringstream() << "  deblocking_filter_override_enabled_flag:" << (int)deblocking_filter_override_enabled_flag).str());
		fields.push_back((std::ostringstream() << "  pps_deblocking_filter_disabled_flag:" << (int)pps_deblocking_filter_disabled_flag).str());
		if(!pps_deblocking_filter_disabled_flag){
			fields.push_back((std::ostringstream() << "    pps_beta_offset_div2:" << pps_beta_offset_div2).str());
			fields.push_back((std::ostringstream() << "    pps_tc_offset_div2:" << pps_tc_offset_div2).str());
		}
	}
	fields.push_back((std::ostringstream() << "pps_scaling_list_data_present_flag:" << (int)pps_scaling_list_data_present_flag).str());
	if(pps_scaling_list_data_present_flag){
		std::vector<std::string> scalingListFields = scaling_list_data.dump_fields();
		std::transform(scalingListFields.begin(), scalingListFields.end(), std::back_inserter(fields), [](const std::string& subField){
			return "  " + subField;
		});
	}
	fields.push_back((std::ostringstream() << "lists_modification_present_flag:" << (int)lists_modification_present_flag).str());
	fields.push_back((std::ostringstream() << "log2_parallel_merge_level_minus2:" << log2_parallel_merge_level_minus2).str());
	fields.push_back((std::ostringstream() << "slice_segment_header_extension_present_flag:" << (int)slice_segment_header_extension_present_flag).str());
	fields.push_back((std::ostringstream() << "pps_extension_present_flag:" << (int)pps_extension_present_flag).str());
	if(pps_extension_present_flag){
		fields.push_back((std::ostringstream() << "  pps_range_extension_flag:" << (int)pps_range_extension_flag).str());
		fields.push_back((std::ostringstream() << "  pps_multilayer_extension_flag:" << (int)pps_multilayer_extension_flag).str());
		fields.push_back((std::ostringstream() << "  pps_3d_extension_flag:" << (int)pps_3d_extension_flag).str());
		fields.push_back((std::ostringstream() << "  pps_scc_extension_flag:" << (int)pps_scc_extension_flag).str());
	}
	// TODO: extension data
	return fields;
}

void H265PPS::validate(){
	H265NAL::validate();
	if(pps_pic_parameter_set_id > 63) errors.push_back((std::ostringstream() << "[H265 PPS] pps_pic_parameter_set_id value (" << (int)pps_pic_parameter_set_id << ") not in valid range (0..63)").str());
	if(pps_seq_parameter_set_id > 15) errors.push_back((std::ostringstream() << "[H265 PPS] pps_seq_parameter_set_id value(" << (int)pps_seq_parameter_set_id << ") not in valid range (0..63)").str());
	auto referencedSPS = H265SPS::SPSMap.find(pps_seq_parameter_set_id);
	H265SPS* pSps = nullptr;
	if(referencedSPS == H265SPS::SPSMap.end()) errors.push_back((std::ostringstream() << "[H265 PPS] reference to unknown SPS (" << (int)pps_seq_parameter_set_id << ")").str());
	else pSps = referencedSPS->second;
	if(num_extra_slice_header_bits > 2) errors.push_back((std::ostringstream() << "[H265 PPS] num_extra_slice_header_bits value (" << (int)num_extra_slice_header_bits << ") not in valid range (0..2)").str());
	if(num_ref_idx_l0_default_active_minus1 > 14) errors.push_back((std::ostringstream() << "[H265 PPS] num_ref_idx_l0_default_active_minus1 value (" << (int)num_ref_idx_l0_default_active_minus1 << ") not in valid range (0..14)").str());
	if(num_ref_idx_l1_default_active_minus1 > 14) errors.push_back((std::ostringstream() << "[H265 PPS] num_ref_idx_l1_default_active_minus1 value (" << (int)num_ref_idx_l1_default_active_minus1 << ") not in valid range (0..14)").str());
	if(init_qp_minus26 < -26 - pSps->QpBdOffsetY || init_qp_minus26 > 25) errors.push_back((std::ostringstream() << "[H265 PPS] init_qp_minus26 value (" << (int)init_qp_minus26 << ") not in valid range (" << -26 - pSps->QpBdOffsetY << "..25)").str());
	if(diff_cu_qp_delta_depth > pSps->log2_diff_max_min_luma_coding_block_size) errors.push_back((std::ostringstream() << "[H265 PPS] diff_cu_qp_delta_depth value (" << diff_cu_qp_delta_depth << ") not in valid range (0.." << pSps->log2_diff_max_min_luma_coding_block_size << ")").str());
	if(pps_cb_qp_offset < -12 || pps_cb_qp_offset > 12) errors.push_back((std::ostringstream() << "[H265 PPS] pps_cb_qp_offset value (" << pps_cb_qp_offset << ") not in valid range (-12..12)").str());
	if(pps_cr_qp_offset < -12 || pps_cr_qp_offset > 12) errors.push_back((std::ostringstream() << "[H265 PPS] pps_cr_qp_offset value (" << pps_cr_qp_offset << ") not in valid range (-12..12)").str());
	if((tiles_enabled_flag && num_tile_columns_minus1 == 0) || num_tile_columns_minus1 > pSps->PicWidthInCtbsY-1) errors.push_back((std::ostringstream() << "[H265 PPS] num_tile_columns_minus1 value (" << num_tile_columns_minus1 << ") not in valid range (" << (tiles_enabled_flag ? 1 : 0) << ".." << pSps->PicWidthInCtbsY-1 << ")").str());
	if((tiles_enabled_flag && num_tile_rows_minus1 == 0) || num_tile_rows_minus1 > pSps->PicHeightInCtbsY-1) errors.push_back((std::ostringstream() << "[H265 PPS] num_tile_rows_minus1 value (" << num_tile_rows_minus1 << ") not in valid range (" << (tiles_enabled_flag ? 1 : 0) << ".." << pSps->PicHeightInCtbsY-1 << ")").str());
	if(pps_beta_offset_div2 < -6 || pps_beta_offset_div2 > 6) errors.push_back((std::ostringstream() << "[H265 PPS] pps_beta_offset_div2 value (" << pps_beta_offset_div2 << ") not in valid range (-6..6)").str());
	if(pps_tc_offset_div2 < -6 || pps_tc_offset_div2 > 6) errors.push_back((std::ostringstream() << "[H265 PPS] pps_tc_offset_div2 value (" << pps_tc_offset_div2 << ") not in valid range (-6..6)").str());
	if(log2_parallel_merge_level_minus2 > pSps->CtbLog2SizeY-2) errors.push_back((std::ostringstream() << "[H265 PPS] log2_parallel_merge_level_minus2 value (" << log2_parallel_merge_level_minus2 << ") not in valid range (0.." << pSps->CtbLog2SizeY-2 << ")").str());
}
