#include <sstream>
#include <algorithm>

#include "H265SPS.h"

#include "H265PPS.h"


H265PPSRangeExtension::H265PPSRangeExtension(){
	log2_max_transform_skip_block_size_minus2 = 0;
	cross_component_prediction_enabled_flag = 0;
	chroma_qp_offset_list_enabled_flag = 0;
	diff_cu_chroma_qp_offset_depth = 0;
	chroma_qp_offset_list_len_minus1 = 0;
	log2_sao_offset_scale_luma = 0;
	log2_sao_offset_scale_chroma = 0;
}

std::vector<std::string> H265PPSRangeExtension::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "log2_max_transform_skip_block_size_minus2:" << (int)log2_max_transform_skip_block_size_minus2).str());
	fields.push_back((std::ostringstream() << "cross_component_prediction_enabled_flag:" << (int)cross_component_prediction_enabled_flag).str());
	fields.push_back((std::ostringstream() << "chroma_qp_offset_list_enabled_flag:" << (int)chroma_qp_offset_list_enabled_flag).str());
	if(chroma_qp_offset_list_enabled_flag){
		fields.push_back((std::ostringstream() << "  diff_cu_chroma_qp_offset_depth:" << (int)diff_cu_chroma_qp_offset_depth).str());
		fields.push_back((std::ostringstream() << "  chroma_qp_offset_list_len_minus1:" << (int)chroma_qp_offset_list_len_minus1).str());
		for(int i = 0;i < chroma_qp_offset_list_len_minus1;++i){
			fields.push_back((std::ostringstream() << "    cb_qp_offset_list[" << i << "]:" << (int)cb_qp_offset_list[i]).str());
			fields.push_back((std::ostringstream() << "    cr_qp_offset_list[" << i << "]:" << (int)cr_qp_offset_list[i]).str());
		}
	}
	fields.push_back((std::ostringstream() << "log2_sao_offset_scale_luma:" << (int)log2_sao_offset_scale_luma).str());
	fields.push_back((std::ostringstream() << "log2_sao_offset_scale_chroma:" << (int)log2_sao_offset_scale_chroma).str());
	return fields;
}

H265PPSColourMappingOctants::H265PPSColourMappingOctants(){
	split_octant_flag = 0;
	for(int j = 0;j < 4;++j){
		coded_res_flag[j] = 0;
		for(int c = 0;c < 3;++c){
			res_coeff_q[j][c] = 0;
			res_coeff_r[j][c] = 0;
			res_coeff_s[j][c] = 0;
		}
	}
}

H265PPSColourMappingTable::H265PPSColourMappingTable(){
	num_cm_ref_layers_minus1 = 0;
	for(int i = 0;i < 64;++i) cm_ref_layer_id[i] = 0;
	cm_octant_depth = 0;
	cm_y_part_num_log2 = 0;
	luma_bit_depth_cm_input_minus8 = 0;
	chroma_bit_depth_cm_input_minus8 = 0;
	luma_bit_depth_cm_output_minus8 = 0;
	chroma_bit_depth_cm_output_minus8 = 0;
	cm_res_quant_bits = 0;
	cm_delta_flc_bits_minus1 = 0;
	cm_adapt_threshhold_u_delta = 0;
	cm_adapt_threshhold_v_delta = 0;
}

H265PPSMultilayerExtension::H265PPSMultilayerExtension(){
	poc_reset_info_present_flag = 0;
	pps_infer_scaling_list_flag = 0;
	pps_scaling_list_ref_layer_id = 0;
	num_ref_loc_offsets = 0;
	for(int i = 0;i < 64;++i){
		scaled_ref_layer_left_offset[i] = 0;
		scaled_ref_layer_top_offset[i] = 0;
		scaled_ref_layer_right_offset[i] = 0;
		scaled_ref_layer_bottom_offset[i] = 0;
		ref_region_left_offset[i] = 0;
		ref_region_top_offset[i] = 0;
		ref_region_right_offset[i] = 0;
		ref_region_bottom_offset[i] = 0;
		phase_hor_luma[i] = 0;
		phase_ver_luma[i] = 0;
		phase_hor_chroma_plus8[i] = 8;
		phase_ver_chroma_plus8[i] = 8;
	}
	colour_mapping_enabled_flag = 0;
}

std::vector<std::string> H265PPSMultilayerExtension::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "poc_reset_info_present_flag:" << (int)poc_reset_info_present_flag).str());
	fields.push_back((std::ostringstream() << "pps_infer_scaling_list_flag:" << (int)pps_infer_scaling_list_flag).str());
	if(pps_infer_scaling_list_flag) fields.push_back((std::ostringstream() << "pps_scaling_list_ref_layer_id:" << (int)pps_scaling_list_ref_layer_id).str());
	fields.push_back((std::ostringstream() << "num_ref_loc_offsets:" << (int)num_ref_loc_offsets).str());
	for(int i = 0;i < num_ref_loc_offsets;++i){
		fields.push_back((std::ostringstream() << "  ref_loc_offset_layer_id[" << i << "]:" << (int)ref_loc_offset_layer_id[i]).str());
		fields.push_back((std::ostringstream() << "  scaled_ref_layer_offset_present_flag[" << i << "]:" << (int)scaled_ref_layer_offset_present_flag[i]).str());
		if(scaled_ref_layer_offset_present_flag[i]){
			fields.push_back((std::ostringstream() << "    scaled_ref_layer_left_offset[" << ref_loc_offset_layer_id[i] << "]:" << (int)scaled_ref_layer_left_offset[ref_loc_offset_layer_id[i]]).str());
			fields.push_back((std::ostringstream() << "    scaled_ref_layer_top_offset[" << ref_loc_offset_layer_id[i] << "]:" << (int)scaled_ref_layer_top_offset[ref_loc_offset_layer_id[i]]).str());
			fields.push_back((std::ostringstream() << "    scaled_ref_layer_right_offset[" << ref_loc_offset_layer_id[i] << "]:" << (int)scaled_ref_layer_right_offset[ref_loc_offset_layer_id[i]]).str());
			fields.push_back((std::ostringstream() << "    scaled_ref_layer_bottom_offset[" << ref_loc_offset_layer_id[i] << "]:" << (int)scaled_ref_layer_bottom_offset[ref_loc_offset_layer_id[i]]).str());
		}
		fields.push_back((std::ostringstream() << "  ref_region_offset_present_flag[" << i << "]:" << (int)ref_region_offset_present_flag[i]).str());
		if(scaled_ref_layer_offset_present_flag[i]){
			fields.push_back((std::ostringstream() << "    ref_region_left_offset[" << ref_loc_offset_layer_id[i] << "]:" << (int)ref_region_left_offset[ref_loc_offset_layer_id[i]]).str());
			fields.push_back((std::ostringstream() << "    ref_region_top_offset[" << ref_loc_offset_layer_id[i] << "]:" << (int)ref_region_top_offset[ref_loc_offset_layer_id[i]]).str());
			fields.push_back((std::ostringstream() << "    ref_region_right_offset[" << ref_loc_offset_layer_id[i] << "]:" << (int)ref_region_right_offset[ref_loc_offset_layer_id[i]]).str());
			fields.push_back((std::ostringstream() << "    ref_region_bottom_offset[" << ref_loc_offset_layer_id[i] << "]:" << (int)ref_region_bottom_offset[ref_loc_offset_layer_id[i]]).str());
		}
		fields.push_back((std::ostringstream() << "  resample_phase_set_present_flag[" << i << "]:" << (int)resample_phase_set_present_flag[i]).str());
		if(scaled_ref_layer_offset_present_flag[i]){
			fields.push_back((std::ostringstream() << "    phase_hor_luma[" << ref_loc_offset_layer_id[i] << "]:" << (int)phase_hor_luma[ref_loc_offset_layer_id[i]]).str());
			fields.push_back((std::ostringstream() << "    phase_ver_luma[" << ref_loc_offset_layer_id[i] << "]:" << (int)phase_ver_luma[ref_loc_offset_layer_id[i]]).str());
			fields.push_back((std::ostringstream() << "    phase_hor_chroma_plus8[" << ref_loc_offset_layer_id[i] << "]:" << (int)phase_hor_chroma_plus8[ref_loc_offset_layer_id[i]]).str());
			fields.push_back((std::ostringstream() << "    phase_ver_chroma_plus8[" << ref_loc_offset_layer_id[i] << "]:" << (int)phase_ver_chroma_plus8[ref_loc_offset_layer_id[i]]).str());
		}
	}
	fields.push_back((std::ostringstream() << "colour_mapping_enabled_flag:" << (int)colour_mapping_enabled_flag).str());
	return fields;
}

H265PPSDeltaLookupTable::H265PPSDeltaLookupTable(){
	num_val_delta_dlt = 0;
	max_diff = 0;
	min_diff_minus1 = 0;
	delta_dlt_val0 = 0;
}

H265PPS3DExtension::H265PPS3DExtension(){
	dlts_present_flag = 0;
	pps_depth_layers_minus1 = 0;
	pps_bit_depth_for_depth_layers_minus8 = 0;
	for(int i = 0;i < 64;++i){
		dlt_flag[i] = 0;
		dlt_pred_flag[i] = 0;
		dlt_val_flags_present_flag[i] = 0;
	}
}

std::vector<std::string> H265PPS3DExtension::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "dlts_present_flag:" << (int)dlts_present_flag).str());
	if(dlts_present_flag){
		fields.push_back((std::ostringstream() << "pps_depth_layers_minus1:" << (int)pps_depth_layers_minus1).str());
		fields.push_back((std::ostringstream() << "pps_bit_depth_for_depth_layers_minus8:" << (int)pps_bit_depth_for_depth_layers_minus8).str());
		for(int i = 0;i <= pps_bit_depth_for_depth_layers_minus8;++i){
			fields.push_back((std::ostringstream() << "  dlt_flag[" << i << "]:" << (int)dlt_flag[i]).str());
			if(dlt_flag[i]){
				fields.push_back((std::ostringstream() << "    dlt_pred_flag[" << i << "]:" << (int)dlt_pred_flag[i]).str());
				if(!dlt_pred_flag[i]) fields.push_back((std::ostringstream() << "    dlt_val_flags_present_flag[" << i << "]:" << (int)dlt_val_flags_present_flag[i]).str());
				if(dlt_val_flags_present_flag[i]){
					for(int j = 0;j <= dlt_value_flag[i].size();++j) fields.push_back((std::ostringstream() << "      dlt_value_flag[" << i << "][" << j << "]:" << (int)dlt_value_flag[i][j]).str());
				}
			}
		}
	}
	return fields;
}

H265PPSSCCExtension::H265PPSSCCExtension(){
	pps_curr_pic_ref_enabled_flag = 0;
	residual_adaptive_colour_transform_enabled_flag = 0;
	pps_slice_act_qp_offsets_present_flag = 0;
	pps_act_y_qp_offset_plus5 = 0;
	pps_act_cb_qp_offset_plus5 = 0;
	pps_act_cr_qp_offset_plus3 = 0;
	pps_palette_predictor_initializers_present_flag = 0;
	monochrome_palette_flag = 1;
	luma_bit_depth_entry_minus8 = 0;
	chroma_bit_depth_entry_minus8 = 0;
}

std::vector<std::string> H265PPSSCCExtension::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "pps_curr_pic_ref_enabled_flag:" << (int)pps_curr_pic_ref_enabled_flag).str());
	fields.push_back((std::ostringstream() << "residual_adaptive_colour_transform_enabled_flag:" << (int)residual_adaptive_colour_transform_enabled_flag).str());
	if(residual_adaptive_colour_transform_enabled_flag){
		fields.push_back((std::ostringstream() << "  pps_slice_act_qp_offsets_present_flag:" << (int)pps_slice_act_qp_offsets_present_flag).str());
		fields.push_back((std::ostringstream() << "  pps_act_y_qp_offset_plus5:" << (int)pps_act_y_qp_offset_plus5).str());
		fields.push_back((std::ostringstream() << "  pps_act_cb_qp_offset_plus5:" << (int)pps_act_cb_qp_offset_plus5).str());
		fields.push_back((std::ostringstream() << "  pps_act_cr_qp_offset_plus3:" << (int)pps_act_cr_qp_offset_plus3).str());
	}
	fields.push_back((std::ostringstream() << "pps_palette_predictor_initializers_present_flag:" << (int)pps_palette_predictor_initializers_present_flag).str());
	if(pps_palette_predictor_initializers_present_flag){
		fields.push_back((std::ostringstream() << "  pps_num_palette_predictor_initializer:" << pps_num_palette_predictor_initializer).str());
		if(pps_num_palette_predictor_initializer > 0){
			fields.push_back((std::ostringstream() << "    monochrome_palette_flag:" << monochrome_palette_flag).str());
			fields.push_back((std::ostringstream() << "    luma_bit_depth_entry_minus8:" << (int)luma_bit_depth_entry_minus8).str());
			if(!monochrome_palette_flag) fields.push_back((std::ostringstream() << "    chroma_bit_depth_entry_minus8:" << (int)chroma_bit_depth_entry_minus8).str());
			for(int comp = 0;comp < monochrome_palette_flag ? 1 : 3;++comp){
				for(int i = 0;i < pps_num_palette_predictor_initializer;++i){
					fields.push_back((std::ostringstream() << "       pps_palette_predictor_initializer[" << comp << "][" << i << "]:" << pps_palette_predictor_initializer[comp][i]).str());
				}
			}
		}
	}
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
	std::vector<std::string> fields = H265NAL::dump_fields();
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
		if(pps_range_extension_flag){
			std::vector<std::string> pps_range_extension_fields = pps_range_extension.dump_fields();
			std::transform(pps_range_extension_fields.begin(), pps_range_extension_fields.end(), std::back_inserter(fields), [](const std::string& subField){
				return "    " + subField;
			});
		}
		fields.push_back((std::ostringstream() << "  pps_multilayer_extension_flag:" << (int)pps_multilayer_extension_flag).str());
		if(pps_multilayer_extension_flag){
			std::vector<std::string> pps_multilayer_extension_fields = pps_multilayer_extension.dump_fields();
			std::transform(pps_multilayer_extension_fields.begin(), pps_multilayer_extension_fields.end(), std::back_inserter(fields), [](const std::string& subField){
				return "    " + subField;
			});
		}
		fields.push_back((std::ostringstream() << "  pps_3d_extension_flag:" << (int)pps_3d_extension_flag).str());
		if(pps_3d_extension_flag){
			std::vector<std::string> pps_3d_extension_fields = pps_3d_extension.dump_fields();
			std::transform(pps_3d_extension_fields.begin(), pps_3d_extension_fields.end(), std::back_inserter(fields), [](const std::string& subField){
				return "    " + subField;
			});
		}
		fields.push_back((std::ostringstream() << "  pps_scc_extension_flag:" << (int)pps_scc_extension_flag).str());
		if(pps_scc_extension_flag){
			std::vector<std::string> pps_scc_extension_fields = pps_scc_extension.dump_fields();
			std::transform(pps_scc_extension_fields.begin(), pps_scc_extension_fields.end(), std::back_inserter(fields), [](const std::string& subField){
				return "    " + subField;
			});
		}
	}
	return fields;
}

void H265PPS::validate(){
	H265NAL::validate();
	if(pps_pic_parameter_set_id > 63) minorErrors.push_back((std::ostringstream() << "[PPS] pps_pic_parameter_set_id value (" << (int)pps_pic_parameter_set_id << ") not in valid range (0..63)").str());
	if(pps_seq_parameter_set_id > 15) minorErrors.push_back((std::ostringstream() << "[PPS] pps_seq_parameter_set_id value(" << (int)pps_seq_parameter_set_id << ") not in valid range (0..63)").str());
	auto referencedSPS = H265SPS::SPSMap.find(pps_seq_parameter_set_id);
	H265SPS* pSps = nullptr;
	if(referencedSPS == H265SPS::SPSMap.end()) {
		majorErrors.push_back((std::ostringstream() << "[PPS] reference to unknown SPS (" << (int)pps_seq_parameter_set_id << ")").str());
		return;
	}
	pSps = referencedSPS->second;
	if(num_extra_slice_header_bits > 2) minorErrors.push_back((std::ostringstream() << "[PPS] num_extra_slice_header_bits value (" << (int)num_extra_slice_header_bits << ") not in valid range (0..2)").str());
	if(num_ref_idx_l0_default_active_minus1 > 14) minorErrors.push_back((std::ostringstream() << "[PPS] num_ref_idx_l0_default_active_minus1 value (" << (int)num_ref_idx_l0_default_active_minus1 << ") not in valid range (0..14)").str());
	if(num_ref_idx_l1_default_active_minus1 > 14) minorErrors.push_back((std::ostringstream() << "[PPS] num_ref_idx_l1_default_active_minus1 value (" << (int)num_ref_idx_l1_default_active_minus1 << ") not in valid range (0..14)").str());
	if(init_qp_minus26 < -26 - pSps->QpBdOffsetY || init_qp_minus26 > 25) minorErrors.push_back((std::ostringstream() << "[PPS] init_qp_minus26 value (" << (int)init_qp_minus26 << ") not in valid range (" << -26 - pSps->QpBdOffsetY << "..25)").str());
	if(diff_cu_qp_delta_depth > pSps->log2_diff_max_min_luma_coding_block_size) minorErrors.push_back((std::ostringstream() << "[PPS] diff_cu_qp_delta_depth value (" << diff_cu_qp_delta_depth << ") not in valid range (0.." << pSps->log2_diff_max_min_luma_coding_block_size << ")").str());
	if(pps_cb_qp_offset < -12 || pps_cb_qp_offset > 12) minorErrors.push_back((std::ostringstream() << "[PPS] pps_cb_qp_offset value (" << pps_cb_qp_offset << ") not in valid range (-12..12)").str());
	if(pps_cr_qp_offset < -12 || pps_cr_qp_offset > 12) minorErrors.push_back((std::ostringstream() << "[PPS] pps_cr_qp_offset value (" << pps_cr_qp_offset << ") not in valid range (-12..12)").str());
	if(num_tile_columns_minus1 > pSps->PicWidthInCtbsY-1) minorErrors.push_back((std::ostringstream() << "[PPS] num_tile_columns_minus1 value (" << num_tile_columns_minus1 << ") not in valid range (0.." << pSps->PicWidthInCtbsY-1 << ")").str());
	if(num_tile_rows_minus1 > pSps->PicHeightInCtbsY-1) minorErrors.push_back((std::ostringstream() << "[PPS] num_tile_rows_minus1 value (" << num_tile_rows_minus1 << ") not in valid range (0.." << pSps->PicHeightInCtbsY-1 << ")").str());
	if(tiles_enabled_flag && num_tile_columns_minus1 == 0 && num_tile_rows_minus1 == 0) minorErrors.push_back("[PPS] num_tile_columns_minus1 and num_tile_rows_minus1 both equal to 0 with set tiles_enabled_flag");
	if(pps_beta_offset_div2 < -6 || pps_beta_offset_div2 > 6) minorErrors.push_back((std::ostringstream() << "[PPS] pps_beta_offset_div2 value (" << pps_beta_offset_div2 << ") not in valid range (-6..6)").str());
	if(pps_tc_offset_div2 < -6 || pps_tc_offset_div2 > 6) minorErrors.push_back((std::ostringstream() << "[PPS] pps_tc_offset_div2 value (" << pps_tc_offset_div2 << ") not in valid range (-6..6)").str());
	if(log2_parallel_merge_level_minus2 > pSps->CtbLog2SizeY-2) minorErrors.push_back((std::ostringstream() << "[PPS] log2_parallel_merge_level_minus2 value (" << log2_parallel_merge_level_minus2 << ") not in valid range (0.." << pSps->CtbLog2SizeY-2 << ")").str());
}
