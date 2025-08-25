
#include <algorithm>

#include "H265SPS.h"
#include "../../StringHelpers/StringFormatter.h"
#include "../../StringHelpers/UnitFieldList.h"

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

UnitFieldList H265PPSRangeExtension::dump_fields(){
	UnitFieldList fields = UnitFieldList("PPS Range Extension");
	fields.addItem(UnitField("log2_max_transform_skip_block_size_minus2", log2_max_transform_skip_block_size_minus2));
	fields.addItem(UnitField("cross_component_prediction_enabled_flag", cross_component_prediction_enabled_flag));
	ValueUnitFieldList chroma_qp_offset_list_enabled_flagField = ValueUnitFieldList("chroma_qp_offset_list_enabled_flag", chroma_qp_offset_list_enabled_flag);
	if(chroma_qp_offset_list_enabled_flag){
		chroma_qp_offset_list_enabled_flagField.addItem(UnitField("diff_cu_chroma_qp_offset_depth", diff_cu_chroma_qp_offset_depth));
		chroma_qp_offset_list_enabled_flagField.addItem(UnitField("chroma_qp_offset_list_len_minus1", chroma_qp_offset_list_len_minus1));
		for(uint8_t i = 0;i < chroma_qp_offset_list_len_minus1;++i){
			chroma_qp_offset_list_enabled_flagField.addItem(IdxUnitField("cb_qp_offset_list", cb_qp_offset_list[i], i));
			chroma_qp_offset_list_enabled_flagField.addItem(IdxUnitField("cr_qp_offset_list", cr_qp_offset_list[i], i));
		}
	}
	fields.addItem(std::move(chroma_qp_offset_list_enabled_flagField));
	fields.addItem(UnitField("log2_sao_offset_scale_luma", log2_sao_offset_scale_luma));
	fields.addItem(UnitField("log2_sao_offset_scale_chroma", log2_sao_offset_scale_chroma));
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
	for(uint8_t i = 0;i < 64u;++i) cm_ref_layer_id[i] = 0;
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
	for(uint8_t i = 0;i < 64u;++i){
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

UnitFieldList H265PPSMultilayerExtension::dump_fields(){
	UnitFieldList fields = UnitFieldList("PPS Multilayer Extension");
	fields.addItem(UnitField("poc_reset_info_present_flag", poc_reset_info_present_flag));
	ValueUnitFieldList pps_infer_scaling_list_flagField = ValueUnitFieldList("pps_infer_scaling_list_flag", pps_infer_scaling_list_flag);
	if(pps_infer_scaling_list_flag) pps_infer_scaling_list_flagField.addItem(UnitField("pps_scaling_list_ref_layer_id", pps_scaling_list_ref_layer_id));
	fields.addItem(std::move(pps_infer_scaling_list_flagField));
	fields.addItem(UnitField("num_ref_loc_offsets", num_ref_loc_offsets));
	for(uint8_t i = 0;i < num_ref_loc_offsets;++i){
		fields.addItem(IdxUnitField("ref_loc_offset_layer_id", ref_loc_offset_layer_id[i], i));
		IdxValueUnitFieldList scaled_ref_layer_offset_present_flagField = IdxValueUnitFieldList("scaled_ref_layer_offset_present_flag", scaled_ref_layer_offset_present_flag[i], i);
		if(scaled_ref_layer_offset_present_flag[i]){
			scaled_ref_layer_offset_present_flagField.addItem(IdxUnitField("scaled_ref_layer_left_offset[%d]:{}", ref_loc_offset_layer_id[i], scaled_ref_layer_left_offset[ref_loc_offset_layer_id[i]]));
			scaled_ref_layer_offset_present_flagField.addItem(IdxUnitField("scaled_ref_layer_top_offset[%d]:{}", ref_loc_offset_layer_id[i], scaled_ref_layer_top_offset[ref_loc_offset_layer_id[i]]));
			scaled_ref_layer_offset_present_flagField.addItem(IdxUnitField("scaled_ref_layer_right_offset[%d]:{}", ref_loc_offset_layer_id[i], scaled_ref_layer_right_offset[ref_loc_offset_layer_id[i]]));
			scaled_ref_layer_offset_present_flagField.addItem(IdxUnitField("scaled_ref_layer_bottom_offset[%d]:{}", ref_loc_offset_layer_id[i], scaled_ref_layer_bottom_offset[ref_loc_offset_layer_id[i]]));
		}
		fields.addItem(std::move(scaled_ref_layer_offset_present_flagField));
		IdxValueUnitFieldList ref_region_offset_present_flagField = IdxValueUnitFieldList("ref_region_offset_present_flag", ref_region_offset_present_flag[i], i);
		if(scaled_ref_layer_offset_present_flag[i]){
			ref_region_offset_present_flagField.addItem(IdxUnitField("ref_region_left_offset[%d]:{}", ref_loc_offset_layer_id[i], ref_region_left_offset[ref_loc_offset_layer_id[i]]));
			ref_region_offset_present_flagField.addItem(IdxUnitField("ref_region_top_offset[%d]:{}", ref_loc_offset_layer_id[i], ref_region_top_offset[ref_loc_offset_layer_id[i]]));
			ref_region_offset_present_flagField.addItem(IdxUnitField("ref_region_right_offset[%d]:{}", ref_loc_offset_layer_id[i], ref_region_right_offset[ref_loc_offset_layer_id[i]]));
			ref_region_offset_present_flagField.addItem(IdxUnitField("ref_region_bottom_offset[%d]:{}", ref_loc_offset_layer_id[i], ref_region_bottom_offset[ref_loc_offset_layer_id[i]]));
		}
		fields.addItem(std::move(ref_region_offset_present_flagField));
		IdxValueUnitFieldList resample_phase_set_present_flagField = IdxValueUnitFieldList("resample_phase_set_present_flag", resample_phase_set_present_flag[i], i);
		if(scaled_ref_layer_offset_present_flag[i]){
			resample_phase_set_present_flagField.addItem(IdxUnitField("phase_hor_luma[%d]:{}", ref_loc_offset_layer_id[i], phase_hor_luma[ref_loc_offset_layer_id[i]]));
			resample_phase_set_present_flagField.addItem(IdxUnitField("phase_ver_luma[%d]:{}", ref_loc_offset_layer_id[i], phase_ver_luma[ref_loc_offset_layer_id[i]]));
			resample_phase_set_present_flagField.addItem(IdxUnitField("phase_hor_chroma_plus8[%d]:{}", ref_loc_offset_layer_id[i], phase_hor_chroma_plus8[ref_loc_offset_layer_id[i]]));
			resample_phase_set_present_flagField.addItem(IdxUnitField("phase_ver_chroma_plus8[%d]:{}", ref_loc_offset_layer_id[i], phase_ver_chroma_plus8[ref_loc_offset_layer_id[i]]));
		}
		fields.addItem(std::move(resample_phase_set_present_flagField));
	}
	fields.addItem(UnitField("colour_mapping_enabled_flag", colour_mapping_enabled_flag));
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
	for(uint8_t i = 0;i < 64u;++i){
		dlt_flag[i] = 0;
		dlt_pred_flag[i] = 0;
		dlt_val_flags_present_flag[i] = 0;
	}
}

UnitFieldList H265PPS3DExtension::dump_fields(){
	UnitFieldList fields = UnitFieldList("PPS 3D Extension");
	ValueUnitFieldList dlts_present_flagField = ValueUnitFieldList("dlts_present_flag", dlts_present_flag);
	if(dlts_present_flag){
		dlts_present_flagField.addItem(UnitField("pps_depth_layers_minus1", pps_depth_layers_minus1));
		dlts_present_flagField.addItem(UnitField("pps_bit_depth_for_depth_layers_minus8", pps_bit_depth_for_depth_layers_minus8));
		for(uint8_t i = 0;i <= pps_bit_depth_for_depth_layers_minus8;++i){
			IdxValueUnitFieldList dlt_flagField = IdxValueUnitFieldList("dlt_flag", dlt_flag[i], i);
			dlts_present_flagField.addItem(std::move(dlt_flagField));
			if(dlt_flag[i]){
				IdxValueUnitFieldList dlt_pred_flagField = IdxValueUnitFieldList("dlt_pred_flag", dlt_pred_flag[i], i);
				dlt_flagField.addItem(std::move(dlt_pred_flagField));
				IdxValueUnitFieldList dlt_val_flags_present_flagField = IdxValueUnitFieldList("dlt_val_flags_present_flag", dlt_val_flags_present_flag[i], i);
				if(!dlt_pred_flag[i]){
					dlt_pred_flagField.addItem(std::move(dlt_val_flags_present_flagField));
				} 
				
				if(dlt_val_flags_present_flag[i]){
					for(uint32_t j = 0;j < dlt_value_flag[i].size();++j) dlt_val_flags_present_flagField.addItem(DblIdxUnitField("dlt_value_flag", dlt_value_flag[i][j], i, j));
				}
			}
		}
	}
	fields.addItem(std::move(dlts_present_flagField));
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

UnitFieldList H265PPSSCCExtension::dump_fields(){
	UnitFieldList fields = UnitFieldList("PPS SCC Extension");
	fields.addItem(UnitField("pps_curr_pic_ref_enabled_flag", pps_curr_pic_ref_enabled_flag));
	ValueUnitFieldList residual_adaptive_colour_transform_enabled_flagField = ValueUnitFieldList("residual_adaptive_colour_transform_enabled_flag", residual_adaptive_colour_transform_enabled_flag);
	if(residual_adaptive_colour_transform_enabled_flag){
		residual_adaptive_colour_transform_enabled_flagField.addItem(UnitField("pps_slice_act_qp_offsets_present_flag", pps_slice_act_qp_offsets_present_flag));
		residual_adaptive_colour_transform_enabled_flagField.addItem(UnitField("pps_act_y_qp_offset_plus5", pps_act_y_qp_offset_plus5));
		residual_adaptive_colour_transform_enabled_flagField.addItem(UnitField("pps_act_cb_qp_offset_plus5", pps_act_cb_qp_offset_plus5));
		residual_adaptive_colour_transform_enabled_flagField.addItem(UnitField("pps_act_cr_qp_offset_plus3", pps_act_cr_qp_offset_plus3));
	}
	fields.addItem(std::move(residual_adaptive_colour_transform_enabled_flagField));
	ValueUnitFieldList pps_palette_predictor_initializers_present_flagField = ValueUnitFieldList("pps_palette_predictor_initializers_present_flag", pps_palette_predictor_initializers_present_flag);
	if(pps_palette_predictor_initializers_present_flag){
		pps_palette_predictor_initializers_present_flagField.addItem(UnitField("pps_num_palette_predictor_initializer", pps_num_palette_predictor_initializer));
		if(pps_num_palette_predictor_initializer > 0){
			ValueUnitFieldList monochrome_palette_flagField = ValueUnitFieldList("monochrome_palette_flag", monochrome_palette_flag);
			pps_palette_predictor_initializers_present_flagField.addItem(std::move(monochrome_palette_flagField));
			pps_palette_predictor_initializers_present_flagField.addItem(UnitField("luma_bit_depth_entry_minus8", luma_bit_depth_entry_minus8));
			if(!monochrome_palette_flag) monochrome_palette_flagField.addItem(UnitField("chroma_bit_depth_entry_minus8", chroma_bit_depth_entry_minus8));
			for(uint8_t comp = 0;comp < (monochrome_palette_flag ? 1 : 3);++comp){
				for(uint32_t i = 0;i < pps_num_palette_predictor_initializer;++i){
					pps_palette_predictor_initializers_present_flagField.addItem(StringFormatter::formatString("pps_palette_predictor_initializer", pps_palette_predictor_initializer[comp][i], comp, i));
				}
			}
		}
	}
	fields.addItem(std::move(pps_palette_predictor_initializers_present_flagField));
	return fields;
}

H265PPS::H265PPS():
	H265PPS(0, UnitType_Unspecified, 0, 0, 0, nullptr)
{}

H265PPS::H265PPS(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, const uint8_t* nal_data):
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

UnitFieldList H265PPS::dump_fields(){
	UnitFieldList fields = UnitFieldList("Picture Parameter Set", H265NAL::dump_fields());
	if(!completelyParsed) return fields;
	fields.addItem(UnitField("pps_pic_parameter_set_id", pps_pic_parameter_set_id));
	fields.addItem(UnitField("pps_seq_parameter_set_id", pps_seq_parameter_set_id));
	fields.addItem(UnitField("dependent_slice_segments_enabled_flag", dependent_slice_segments_enabled_flag));
	fields.addItem(UnitField("output_flag_present_flagd", output_flag_present_flag));
	fields.addItem(UnitField("num_extra_slice_header_bitser_set_id", num_extra_slice_header_bits));
	fields.addItem(UnitField("sign_data_hiding_enabled_flag", sign_data_hiding_enabled_flag));
	fields.addItem(UnitField("cabac_init_present_flag", cabac_init_present_flag));
	fields.addItem(UnitField("num_ref_idx_l0_default_active_minus1", num_ref_idx_l0_default_active_minus1));
	fields.addItem(UnitField("num_ref_idx_l1_default_active_minus1", num_ref_idx_l1_default_active_minus1));
	fields.addItem(UnitField("init_qp_minus26", init_qp_minus26));
	fields.addItem(UnitField("constrained_intra_pred_flag", constrained_intra_pred_flag));
	fields.addItem(UnitField("transform_skip_enabled_flag", transform_skip_enabled_flag));
	ValueUnitFieldList cu_qp_delta_enabled_flagField = ValueUnitFieldList("cu_qp_delta_enabled_flag", cu_qp_delta_enabled_flag);
	if(cu_qp_delta_enabled_flag){
		cu_qp_delta_enabled_flagField.addItem(UnitField("diff_cu_qp_delta_depth", diff_cu_qp_delta_depth));
	}
	fields.addItem(std::move(cu_qp_delta_enabled_flagField));
	fields.addItem(UnitField("pps_cb_qp_offset", pps_cb_qp_offset));
	fields.addItem(UnitField("pps_cr_qp_offset", pps_cr_qp_offset));
	fields.addItem(UnitField("pps_slice_chroma_qp_offsets_present_flag", pps_slice_chroma_qp_offsets_present_flag));
	fields.addItem(UnitField("weighted_pred_flag", weighted_pred_flag));
	fields.addItem(UnitField("weighted_bipred_flag", weighted_bipred_flag));
	fields.addItem(UnitField("transquant_bypass_enabled_flag", transquant_bypass_enabled_flag));
	fields.addItem(UnitField("entropy_coding_sync_enabled_flag", entropy_coding_sync_enabled_flag));
	ValueUnitFieldList tiles_enabled_flagField = ValueUnitFieldList("tiles_enabled_flag", tiles_enabled_flag);
	if(tiles_enabled_flag){
		tiles_enabled_flagField.addItem(UnitField("num_tile_columns_minus1", num_tile_columns_minus1));
		tiles_enabled_flagField.addItem(UnitField("num_tile_rows_minus1", num_tile_rows_minus1));
		ValueUnitFieldList uniform_spacing_flagField = ValueUnitFieldList("uniform_spacing_flag", uniform_spacing_flag);
		if(!uniform_spacing_flag){
			for(uint32_t i = 0;i < num_tile_columns_minus1;++i) uniform_spacing_flagField.addItem(IdxUnitField("column_width_minus1", column_width_minus1[i], i));
			for(uint32_t i = 0;i < num_tile_rows_minus1;++i) uniform_spacing_flagField.addItem(IdxUnitField("row_height_minus1", row_height_minus1[i], i));
		}
		tiles_enabled_flagField.addItem(std::move(uniform_spacing_flagField));
		
		tiles_enabled_flagField.addItem(UnitField("loop_filter_across_tiles_enabled_flag", loop_filter_across_tiles_enabled_flag));
	}
	fields.addItem(std::move(tiles_enabled_flagField));
	fields.addItem(UnitField("pps_loop_filter_across_slices_enabled_flag", pps_loop_filter_across_slices_enabled_flag));
	ValueUnitFieldList deblocking_filter_control_present_flagField = ValueUnitFieldList("deblocking_filter_control_present_flag", deblocking_filter_control_present_flag);
	if(deblocking_filter_control_present_flag){
		deblocking_filter_control_present_flagField.addItem(UnitField("deblocking_filter_override_enabled_flag", deblocking_filter_override_enabled_flag));
		ValueUnitFieldList pps_deblocking_filter_disabled_flagField = ValueUnitFieldList("pps_deblocking_filter_disabled_flag", pps_deblocking_filter_disabled_flag);
		if(!pps_deblocking_filter_disabled_flag){
			pps_deblocking_filter_disabled_flagField.addItem(UnitField("pps_beta_offset_div2", pps_beta_offset_div2));
			pps_deblocking_filter_disabled_flagField.addItem(UnitField("pps_tc_offset_div2", pps_tc_offset_div2));
		}
		deblocking_filter_control_present_flagField.addItem(std::move(pps_deblocking_filter_disabled_flagField));
	}
	fields.addItem(std::move(deblocking_filter_control_present_flagField));
	ValueUnitFieldList pps_scaling_list_data_present_flagField = ValueUnitFieldList("pps_scaling_list_data_present_flag", pps_scaling_list_data_present_flag);
	if(pps_scaling_list_data_present_flag)pps_scaling_list_data_present_flagField.addItem(scaling_list_data.dump_fields());
	fields.addItem(std::move(pps_scaling_list_data_present_flagField));
	fields.addItem(UnitField("lists_modification_present_flag", lists_modification_present_flag));
	fields.addItem(UnitField("log2_parallel_merge_level_minus2", log2_parallel_merge_level_minus2));
	fields.addItem(UnitField("slice_segment_header_extension_present_flag", slice_segment_header_extension_present_flag));
	ValueUnitFieldList pps_extension_present_flagField = ValueUnitFieldList("pps_extension_present_flag", pps_extension_present_flag);
	if(pps_extension_present_flag){
		ValueUnitFieldList pps_range_extension_flagField = ValueUnitFieldList("pps_range_extension_flag", pps_range_extension_flag);
		pps_extension_present_flagField.addItem(std::move(pps_range_extension_flagField));
		if(pps_range_extension_flag) pps_range_extension_flagField.addItem(pps_range_extension.dump_fields());
		
		ValueUnitFieldList pps_multilayer_extension_flagField = ValueUnitFieldList("pps_multilayer_extension_flag", pps_multilayer_extension_flag);
		pps_extension_present_flagField.addItem(std::move(pps_multilayer_extension_flagField));
		if(pps_multilayer_extension_flag) pps_multilayer_extension_flagField.addItem(pps_multilayer_extension.dump_fields());
		
		ValueUnitFieldList pps_3d_extension_flagField = ValueUnitFieldList("pps_3d_extension_flag", pps_3d_extension_flag);
		pps_extension_present_flagField.addItem(std::move(pps_3d_extension_flagField));
		if(pps_3d_extension_flag) pps_3d_extension_flagField.addItem(pps_3d_extension.dump_fields());
		
		ValueUnitFieldList pps_scc_extension_flagField = ValueUnitFieldList("pps_scc_extension_flag", pps_scc_extension_flag);
		pps_extension_present_flagField.addItem(std::move(pps_scc_extension_flagField));
		if(pps_scc_extension_flag) pps_scc_extension_flagField.addItem(pps_scc_extension.dump_fields());
	}
	fields.addItem(std::move(pps_extension_present_flagField));
	return fields;
}

void H265PPS::validate(){
	H265NAL::validate();
	if(!completelyParsed) return;
	if(pps_pic_parameter_set_id > 63) minorErrors.push_back(StringFormatter::formatString("[PPS] pps_pic_parameter_set_id value (%ld) not in valid range (0..63)", pps_pic_parameter_set_id));
	if(pps_seq_parameter_set_id > 15) minorErrors.push_back(StringFormatter::formatString("[PPS] pps_seq_parameter_set_id value(%ld) not in valid range (0..63)", pps_seq_parameter_set_id));
	auto referencedSPS = H265SPS::SPSMap.find(pps_seq_parameter_set_id);
	H265SPS* pSps = nullptr;
	if(referencedSPS == H265SPS::SPSMap.end()) {
		majorErrors.push_back(StringFormatter::formatString("[PPS] reference to unknown SPS (%ld)", pps_seq_parameter_set_id));
		return;
	}
	pSps = referencedSPS->second;
	if(num_extra_slice_header_bits > 2) minorErrors.push_back(StringFormatter::formatString("[PPS] num_extra_slice_header_bits value (%ld) not in valid range (0..2)", num_extra_slice_header_bits));
	if(num_ref_idx_l0_default_active_minus1 > 14) minorErrors.push_back(StringFormatter::formatString("[PPS] num_ref_idx_l0_default_active_minus1 value (%ld) not in valid range (0..14)", num_ref_idx_l0_default_active_minus1));
	if(num_ref_idx_l1_default_active_minus1 > 14) minorErrors.push_back(StringFormatter::formatString("[PPS] num_ref_idx_l1_default_active_minus1 value (%ld) not in valid range (0..14)", num_ref_idx_l1_default_active_minus1));
	if(init_qp_minus26 < -26 - pSps->QpBdOffsetY || init_qp_minus26 > 25) minorErrors.push_back(StringFormatter::formatString("[PPS] init_qp_minus26 value (%ld) not in valid range ({}..25)", init_qp_minus26, -26 - pSps->QpBdOffsetY));
	if(diff_cu_qp_delta_depth > pSps->log2_diff_max_min_luma_coding_block_size) minorErrors.push_back(StringFormatter::formatString("[PPS] diff_cu_qp_delta_depth value (%ld) not in valid range (0..{})", diff_cu_qp_delta_depth, pSps->log2_diff_max_min_luma_coding_block_size));
	if(pps_cb_qp_offset < -12 || pps_cb_qp_offset > 12) minorErrors.push_back(StringFormatter::formatString("[PPS] pps_cb_qp_offset value (%ld) not in valid range (-12..12)", pps_cb_qp_offset));
	if(pps_cr_qp_offset < -12 || pps_cr_qp_offset > 12) minorErrors.push_back(StringFormatter::formatString("[PPS] pps_cr_qp_offset value (%ld) not in valid range (-12..12)", pps_cr_qp_offset));
	if(num_tile_columns_minus1 > pSps->PicWidthInCtbsY-1) minorErrors.push_back(StringFormatter::formatString("[PPS] num_tile_columns_minus1 value (%ld) not in valid range (0..{})", num_tile_columns_minus1, pSps->PicWidthInCtbsY-1));
	if(num_tile_rows_minus1 > pSps->PicHeightInCtbsY-1) minorErrors.push_back(StringFormatter::formatString("[PPS] num_tile_rows_minus1 value (%ld) not in valid range (0..{})", num_tile_rows_minus1, pSps->PicHeightInCtbsY-1));
	if(tiles_enabled_flag && num_tile_columns_minus1 == 0 && num_tile_rows_minus1 == 0) minorErrors.push_back("[PPS] num_tile_columns_minus1 and num_tile_rows_minus1 both equal to 0 with set tiles_enabled_flag");
	if(pps_beta_offset_div2 < -6 || pps_beta_offset_div2 > 6) minorErrors.push_back(StringFormatter::formatString("[PPS] pps_beta_offset_div2 value (%ld) not in valid range (-6..6)", pps_beta_offset_div2));
	if(pps_tc_offset_div2 < -6 || pps_tc_offset_div2 > 6) minorErrors.push_back(StringFormatter::formatString("[PPS] pps_tc_offset_div2 value (%ld) not in valid range (-6..6)", pps_tc_offset_div2));
	if((uint32_t)log2_parallel_merge_level_minus2 > pSps->CtbLog2SizeY-2) minorErrors.push_back(StringFormatter::formatString("[PPS] log2_parallel_merge_level_minus2 value (%ld) not in valid range (0..{})", log2_parallel_merge_level_minus2, pSps->CtbLog2SizeY-2));
}
