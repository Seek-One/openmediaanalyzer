#include <sstream>
#include <algorithm>

#include "H265SPS.h"

H265VuiParameters::H265VuiParameters(){}

std::vector<std::string> H265VuiParameters::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "aspect_ratio_info_present_flag:" << (int)aspect_ratio_info_present_flag).str());
	if(aspect_ratio_info_present_flag){
		fields.push_back((std::ostringstream() << "  aspect_ratio_idc:" << (int)aspect_ratio_idc).str());
		if(aspect_ratio_idc == EXTENDED_SAR){
			fields.push_back((std::ostringstream() << "    sar_width:" << (int)sar_width).str());
			fields.push_back((std::ostringstream() << "    sar_height:" << (int)sar_height).str());
		}
	}
	fields.push_back((std::ostringstream() << "overscan_info_present_flag:" << (int)overscan_info_present_flag).str());
	if(overscan_info_present_flag) fields.push_back((std::ostringstream() << "  overscan_appropriate_flag:" << (int)overscan_appropriate_flag).str());
	fields.push_back((std::ostringstream() << "video_signal_type_present_flag:" << (int)video_signal_type_present_flag).str());
	if(video_signal_type_present_flag){
		fields.push_back((std::ostringstream() << "  video_format:" << (int)video_format).str());
		fields.push_back((std::ostringstream() << "  video_full_range_flag:" << (int)video_full_range_flag).str());
		fields.push_back((std::ostringstream() << "  colour_description_present_flag:" << (int)colour_description_present_flag).str());
		if(colour_description_present_flag){
			fields.push_back((std::ostringstream() << "    colour_primaries:" << (int)colour_primaries).str());
			fields.push_back((std::ostringstream() << "    transfer_characteristics:" << (int)transfer_characteristics).str());
			fields.push_back((std::ostringstream() << "    matrix_coeffs:" << (int)matrix_coeffs).str());
		}
	}
	fields.push_back((std::ostringstream() << "chroma_loc_info_present_flag:" << (int)chroma_loc_info_present_flag).str());
	if(chroma_loc_info_present_flag){
		fields.push_back((std::ostringstream() << "  chroma_sample_loc_type_top_field:" << (int)chroma_sample_loc_type_top_field).str());
		fields.push_back((std::ostringstream() << "  chroma_sample_loc_type_bottom_field:" << (int)chroma_sample_loc_type_bottom_field).str());
	}
	fields.push_back((std::ostringstream() << "neutral_chroma_indication_flag:" << (int)neutral_chroma_indication_flag).str());
	fields.push_back((std::ostringstream() << "field_seq_flag:" << (int)field_seq_flag).str());
	fields.push_back((std::ostringstream() << "frame_field_info_present_flag:" << (int)frame_field_info_present_flag).str());
	fields.push_back((std::ostringstream() << "default_display_window_flag:" << (int)default_display_window_flag).str());
	if(default_display_window_flag){
		fields.push_back((std::ostringstream() << "  def_disp_win_left_offset:" << def_disp_win_left_offset).str());
		fields.push_back((std::ostringstream() << "  def_disp_win_right_offset:" << def_disp_win_right_offset).str());
		fields.push_back((std::ostringstream() << "  def_disp_win_top_offset:" << def_disp_win_top_offset).str());
		fields.push_back((std::ostringstream() << "  def_disp_win_bottom_offset:" << def_disp_win_bottom_offset).str());
	}
	fields.push_back((std::ostringstream() << "vui_timing_info_present_flag:" << (int)vui_timing_info_present_flag).str());
	if(vui_timing_info_present_flag){
		fields.push_back((std::ostringstream() << "  vui_num_units_in_tick:" << vui_num_units_in_tick).str());
		fields.push_back((std::ostringstream() << "  vui_time_scale:" << vui_time_scale).str());
		fields.push_back((std::ostringstream() << "  vui_poc_proportional_to_timing_flag:" << (int)vui_poc_proportional_to_timing_flag).str());
		if(vui_poc_proportional_to_timing_flag) fields.push_back((std::ostringstream() << "    vui_num_ticks_poc_diff_one_minus1:" << vui_num_ticks_poc_diff_one_minus1).str());
	}
	fields.push_back((std::ostringstream() << "vui_hrd_parameters_present_flag:" << (int)vui_hrd_parameters_present_flag).str());
	if(vui_hrd_parameters_present_flag){
		std::vector<std::string> hrdParametersFields = hrd_parameters.dump_fields(1);
		std::transform(hrdParametersFields.begin(), hrdParametersFields.end(), std::back_inserter(fields), [](const std::string& subField){
			return "  " + subField;
		});
	}
	return fields;
}

H265SPSRangeExtension::H265SPSRangeExtension(){}

std::vector<std::string> H265SPSRangeExtension::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "transform_skip_rotation_enabled_flag:" << (int)transform_skip_rotation_enabled_flag).str());
	fields.push_back((std::ostringstream() << "transform_skip_context_enabled_flag:" << (int)transform_skip_context_enabled_flag).str());
	fields.push_back((std::ostringstream() << "implicit_rdpcm_enabled_flag:" << (int)implicit_rdpcm_enabled_flag).str());
	fields.push_back((std::ostringstream() << "explicit_rdpcm_enabled_flag:" << (int)explicit_rdpcm_enabled_flag).str());
	fields.push_back((std::ostringstream() << "extended_precision_processing_flag:" << (int)extended_precision_processing_flag).str());
	fields.push_back((std::ostringstream() << "intra_smoothing_disabled_flag:" << (int)intra_smoothing_disabled_flag).str());
	fields.push_back((std::ostringstream() << "high_precision_offsets_enabled_flag:" << (int)high_precision_offsets_enabled_flag).str());
	fields.push_back((std::ostringstream() << "persistent_rice_adaptation_enabled_flag:" << (int)persistent_rice_adaptation_enabled_flag).str());
	fields.push_back((std::ostringstream() << "cabac_bypass_alignment_enabled_flag:" << (int)cabac_bypass_alignment_enabled_flag).str());
	return fields;
}

H265SPSMultilayerExtension::H265SPSMultilayerExtension(){}

std::vector<std::string> H265SPSMultilayerExtension::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "inter_view_mv_vert_constraint_flag:" << (int)inter_view_mv_vert_constraint_flag).str());
	return fields;
}

H265SPS3DExtension::H265SPS3DExtension(){}

std::vector<std::string> H265SPS3DExtension::dump_fields(){
	std::vector<std::string> fields;
	for(int d = 0;d <= 1;++d){
		fields.push_back((std::ostringstream() << "iv_di_mc_enabled_flag[" << d << "]:" << (int)iv_di_mc_enabled_flag[d]).str());
		fields.push_back((std::ostringstream() << "iv_mv_scal_enabled_flag[" << d << "]:" << (int)iv_mv_scal_enabled_flag[d]).str());
		if(d == 0){
			fields.push_back((std::ostringstream() << "  log2_ivmc_sub_pb_size_minus3[" << d << "]:" << log2_ivmc_sub_pb_size_minus3[d]).str());
			fields.push_back((std::ostringstream() << "  iv_res_pred_enabled_flag[" << d << "]:" << (int)iv_res_pred_enabled_flag[d]).str());
			fields.push_back((std::ostringstream() << "  depth_ref_enabled_flag[" << d << "]:" << (int)depth_ref_enabled_flag[d]).str());
			fields.push_back((std::ostringstream() << "  vsp_mc_enabled_flag[" << d << "]:" << (int)vsp_mc_enabled_flag[d]).str());
			fields.push_back((std::ostringstream() << "  dbbp_enabled_flag[" << d << "]:" << (int)dbbp_enabled_flag[d]).str());
		} else {
			fields.push_back((std::ostringstream() << "  tex_mc_enabled_flag[" << d << "]:" << (int)tex_mc_enabled_flag[d]).str());
			fields.push_back((std::ostringstream() << "  log2_texmc_sub_pb_size_minus3[" << d << "]:" << log2_texmc_sub_pb_size_minus3[d]).str());
			fields.push_back((std::ostringstream() << "  intra_contour_enabled_flag[" << d << "]:" << (int)intra_contour_enabled_flag[d]).str());
			fields.push_back((std::ostringstream() << "  intra_dc_only_wedge_enabled[" << d << "]:" << (int)intra_dc_only_wedge_enabled[d]).str());
			fields.push_back((std::ostringstream() << "  cqt_cu_part_pred_enabled_flag[" << d << "]:" << (int)cqt_cu_part_pred_enabled_flag[d]).str());
			fields.push_back((std::ostringstream() << "  inter_dc_only_enabled_flag[" << d << "]:" << (int)inter_dc_only_enabled_flag[d]).str());
			fields.push_back((std::ostringstream() << "  skip_intra_enabled_flag[" << d << "]:" << (int)skip_intra_enabled_flag[d]).str());
		}
	}
	return fields;
}

H265SPSSCCExtension::H265SPSSCCExtension(){}

std::vector<std::string> H265SPSSCCExtension::dump_fields(uint32_t chroma_format_idc){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "sps_curr_pic_ref_enabled_flag:" << (int)sps_curr_pic_ref_enabled_flag).str());
	fields.push_back((std::ostringstream() << "palette_mode_enabled_flag:" << (int)palette_mode_enabled_flag).str());
	if(palette_mode_enabled_flag){
		fields.push_back((std::ostringstream() << "  palette_max_size:" << palette_max_size).str());
		fields.push_back((std::ostringstream() << "  delta_palette_max_predictor_size:" << delta_palette_max_predictor_size).str());
		fields.push_back((std::ostringstream() << "  sps_palette_predictor_initializers_present_flag:" << (int)sps_palette_predictor_initializers_present_flag).str());
		if(sps_palette_predictor_initializers_present_flag){
			uint8_t numComps = (chroma_format_idc == 0) ? 1 : 3;
			for(int comp = 0;comp < numComps;++comp){
				for(int i = 0;i <= sps_num_palette_predictor_initializers_minus1;++i){
					fields.push_back((std::ostringstream() << "  sps_palette_predictor_initializer[" << comp << "][" << i << "]:" << (int)sps_palette_predictor_initializer[comp][i]).str());
				}
			}
		}
	}
	fields.push_back((std::ostringstream() << "motion_vector_resolution_control_idc:" << (int)motion_vector_resolution_control_idc).str());
	fields.push_back((std::ostringstream() << "intra_boundary_filtering_disabled_flag:" << (int)intra_boundary_filtering_disabled_flag).str());
	return fields;
}

H265SPS::H265SPS():
	H265SPS(0, UnitType_Unspecified, 0, 0, 0, nullptr)
{}

H265SPS::H265SPS(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, uint8_t* nal_data):
	H265NAL(forbidden_zero_bit, nal_unit_type, nuh_layer_id, nuh_temporal_id_plus1, nal_size, nal_data)
{
	sps_video_parameter_set_id = 0;
	sps_max_sub_layers_minus1 = 0;
	sps_temporal_id_nesting_flag = 0;
	sps_seq_parameter_set_id = 0;
	chroma_format_idc = 0;
	separate_colour_plane_flag = 0;
	pic_width_in_luma_samples = 0;
	pic_height_in_luma_samples = 0;
	conformance_window_flag = 0;
	conf_win_left_offset = 0;
	conf_win_right_offset = 0;
	conf_win_top_offset = 0;
	conf_win_bottom_offset = 0;
	bit_depth_luma_minus8 = 0;
	bit_depth_chroma_minus8 = 0;
	log2_max_pic_order_cnt_lsb_minus4 = 0;
	sps_sub_layer_ordering_info_present_flag = 0;
	log2_min_luma_coding_block_size_minus3 = 0;
	log2_diff_max_min_luma_coding_block_size = 0;
	log2_min_luma_transform_block_size_minus2 = 0;
	log2_diff_max_min_luma_transform_block_size = 0;
	max_transform_hierarchy_depth_inter = 0;
	max_transform_hierarchy_depth_intra = 0;
	scaling_list_enabled_flag = 0;
	sps_scaling_list_data_present_flag = 0;
	amp_enabled_flag = 0;
	sample_adaptive_offset_enabled_flag = 0;
	pcm_enabled_flag = 0;
	pcm_sample_bit_depth_luma_minus1 = 0;
	pcm_sample_bit_depth_chroma_minus1 = 0;
	log2_min_pcm_luma_coding_block_size_minus3 = 0;
	log2_diff_max_min_pcm_luma_coding_block_size = 0;
	pcm_loop_filter_disabled_flag = 0;
	num_short_term_ref_pic_sets = 0;
	long_term_ref_pics_present_flag = 0;
	num_long_term_ref_pics_sps = 0;
	sps_temporal_mvp_enabled_flag = 0;
	strong_intra_smoothing_enabled_flag = 0;
	sps_extension_flag = 0;
	sps_range_extension_flag = 0;
	sps_multilayer_extension_flag = 0;
	sps_3d_extension_flag = 0;
	sps_scc_extension_flag = 0;

	short_term_ref_pic_set.resize(1);

	SubWidthC = 0;
	SubHeightC = 0;
	MinCbLog2SizeY = 0;
	CtbLog2SizeY = 0;
	MinCbSizeY = 0;
	CtbSizeY = 0;
	PicWidthInMinCbsY = 0;
	PicWidthInCtbsY = 0;
	PicHeightInMinCbsY = 0;
	PicHeightInCtbsY = 0;
	PicSizeInMinCbsY = 0;
	PicSizeInCtbsY = 0;
	PicSizeInSamplesY = 0;
	BitDepthY = 0;
	QpBdOffsetY = 0;
	BitDepthC = 0;
	QpBdOffsetC = 0;
}

H265SPS::~H265SPS(){}

std::vector<std::string> H265SPS::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "sps_video_parameter_set_id:" << (int)sps_video_parameter_set_id).str());
	fields.push_back((std::ostringstream() << "sps_max_sub_layers_minus1:" << (int)sps_max_sub_layers_minus1).str());
	fields.push_back((std::ostringstream() << "sps_temporal_id_nesting_flag:" << (int)sps_temporal_id_nesting_flag).str());
	std::vector<std::string> profileTierLevelFields = profile_tier_level.dump_fields();
	std::transform(profileTierLevelFields.begin(), profileTierLevelFields.end(), std::back_inserter(fields), [](const std::string& subField){
		return subField;
	});
	fields.push_back((std::ostringstream() << "sps_seq_parameter_set_id:" << (int)sps_seq_parameter_set_id).str());
	fields.push_back((std::ostringstream() << "chroma_format_idc:" << chroma_format_idc).str());
	if(chroma_format_idc == 3) fields.push_back((std::ostringstream() << "  separate_colour_plane_flag:" << (int)separate_colour_plane_flag).str());
	fields.push_back((std::ostringstream() << "pic_width_in_luma_samples:" << pic_width_in_luma_samples).str());
	fields.push_back((std::ostringstream() << "pic_height_in_luma_samples:" << pic_height_in_luma_samples).str());
	fields.push_back((std::ostringstream() << "conformance_window_flag:" << (int)conformance_window_flag).str());
	if(conformance_window_flag){
		fields.push_back((std::ostringstream() << "  conf_win_left_offset:" << conf_win_left_offset).str());
		fields.push_back((std::ostringstream() << "  conf_win_right_offset:" << conf_win_right_offset).str());
		fields.push_back((std::ostringstream() << "  conf_win_top_offset:" << conf_win_top_offset).str());
		fields.push_back((std::ostringstream() << "  conf_win_bottom_offset:" << conf_win_bottom_offset).str());
	}
	fields.push_back((std::ostringstream() << "bit_depth_luma_minus8:" << (int)bit_depth_luma_minus8).str());
	fields.push_back((std::ostringstream() << "bit_depth_chroma_minus8:" << (int)bit_depth_chroma_minus8).str());
	fields.push_back((std::ostringstream() << "log2_max_pic_order_cnt_lsb_minus4:" << log2_max_pic_order_cnt_lsb_minus4).str());
	fields.push_back((std::ostringstream() << "sps_sub_layer_ordering_info_present_flag:" << (int)sps_sub_layer_ordering_info_present_flag).str());
	for(int i = sps_sub_layer_ordering_info_present_flag ? 0 : sps_max_sub_layers_minus1;i <= sps_max_sub_layers_minus1;++i){
		fields.push_back((std::ostringstream() << "  sps_max_dec_pic_buffering_minus1[" << i << "]:" << sps_max_dec_pic_buffering_minus1[i]).str());
		fields.push_back((std::ostringstream() << "  sps_max_num_reorder_pics[" << i << "]:" << sps_max_num_reorder_pics[i]).str());
		fields.push_back((std::ostringstream() << "  sps_max_latency_increase_plus1[" << i << "]:" << sps_max_latency_increase_plus1[i]).str());
	}
	fields.push_back((std::ostringstream() << "log2_min_luma_coding_block_size_minus3:" << log2_min_luma_coding_block_size_minus3).str());
	fields.push_back((std::ostringstream() << "log2_diff_max_min_luma_coding_block_size:" << log2_diff_max_min_luma_coding_block_size).str());
	fields.push_back((std::ostringstream() << "log2_min_luma_transform_block_size_minus2:" << log2_min_luma_transform_block_size_minus2).str());
	fields.push_back((std::ostringstream() << "log2_diff_max_min_luma_transform_block_size:" << log2_diff_max_min_luma_transform_block_size).str());
	fields.push_back((std::ostringstream() << "max_transform_hierarchy_depth_inter:" << max_transform_hierarchy_depth_inter).str());
	fields.push_back((std::ostringstream() << "max_transform_hierarchy_depth_intra:" << max_transform_hierarchy_depth_intra).str());
	fields.push_back((std::ostringstream() << "scaling_list_enabled_flag:" << (int)scaling_list_enabled_flag).str());
	if(scaling_list_enabled_flag){
		fields.push_back((std::ostringstream() << "  sps_scaling_list_data_present_flag:" << (int)sps_scaling_list_data_present_flag).str());
		if(sps_scaling_list_data_present_flag){
			std::vector<std::string> scalingListDataFields = scaling_list_data.dump_fields();
			std::transform(scalingListDataFields.begin(), scalingListDataFields.end(), std::back_inserter(fields), [](const std::string& subField){
				return "    " + subField;
			});
		}
	}
	fields.push_back((std::ostringstream() << "amp_enabled_flag:" << (int)amp_enabled_flag).str());
	fields.push_back((std::ostringstream() << "sample_adaptive_offset_enabled_flag:" << (int)sample_adaptive_offset_enabled_flag).str());
	fields.push_back((std::ostringstream() << "pcm_enabled_flag:" << (int)pcm_enabled_flag).str());
	if(pcm_enabled_flag){
		fields.push_back((std::ostringstream() << "  pcm_sample_bit_depth_luma_minus1:" << (int)pcm_sample_bit_depth_luma_minus1).str());
		fields.push_back((std::ostringstream() << "  pcm_sample_bit_depth_chroma_minus1:" << (int)pcm_sample_bit_depth_chroma_minus1).str());
		fields.push_back((std::ostringstream() << "  log2_min_pcm_luma_coding_block_size_minus3:" << log2_min_pcm_luma_coding_block_size_minus3).str());
		fields.push_back((std::ostringstream() << "  log2_diff_max_min_luma_coding_block_size:" << log2_diff_max_min_luma_coding_block_size).str());
		fields.push_back((std::ostringstream() << "  pcm_loop_filter_disabled_flag:" << (int)pcm_loop_filter_disabled_flag).str());
	}
	fields.push_back((std::ostringstream() << "num_short_term_ref_pic_sets:" << num_short_term_ref_pic_sets).str());
	for(int i = 0;i < num_short_term_ref_pic_sets;++i){
		std::vector<std::string> shortTermRefPicSetFields = short_term_ref_pic_set[i].dump_fields(i, num_short_term_ref_pic_sets);
		std::transform(shortTermRefPicSetFields.begin(), shortTermRefPicSetFields.end(), std::back_inserter(fields), [](const std::string& subField){
			return "  " + subField;
		});
	}
	fields.push_back((std::ostringstream() << "long_term_ref_pics_present_flag:" << (int)long_term_ref_pics_present_flag).str());
	if(long_term_ref_pics_present_flag){
		fields.push_back((std::ostringstream() << "  num_long_term_ref_pics_sps:" << (int)num_long_term_ref_pics_sps).str());
		for(int i = 0;i < num_long_term_ref_pics_sps;++i){
			fields.push_back((std::ostringstream() << "    lt_ref_pic_poc_lsb_sps[" << i << "]:" << lt_ref_pic_poc_lsb_sps[i]).str());
			fields.push_back((std::ostringstream() << "    used_by_curr_pic_lt_sps_flag[" << i << "]:" << (int)used_by_curr_pic_lt_sps_flag[i]).str());
		}
	}
	fields.push_back((std::ostringstream() << "sps_temporal_mvp_enabled_flag:" << (int)sps_temporal_mvp_enabled_flag).str());
	fields.push_back((std::ostringstream() << "strong_intra_smoothing_enabled_flag:" << (int)strong_intra_smoothing_enabled_flag).str());
	fields.push_back((std::ostringstream() << "vui_parameters_present_flag:" << (int)vui_parameters_present_flag).str());
	if(vui_parameters_present_flag){
		std::vector<std::string> vuiParametersFields = vui_parameters.dump_fields();
		std::transform(vuiParametersFields.begin(), vuiParametersFields.end(), std::back_inserter(fields), [](const std::string& subField){
			return "  " + subField;
		});
	}
	fields.push_back((std::ostringstream() << "sps_extension_flag:" << (int)sps_extension_flag).str());
	if(sps_extension_flag){
		fields.push_back((std::ostringstream() << "  sps_extension_flag:" << (int)sps_range_extension_flag).str());
		fields.push_back((std::ostringstream() << "  sps_multilayer_extension_flag:" << (int)sps_multilayer_extension_flag).str());
		fields.push_back((std::ostringstream() << "  sps_3d_extension_flag:" << (int)sps_3d_extension_flag).str());
		fields.push_back((std::ostringstream() << "  sps_scc_extension_flag:" << (int)sps_scc_extension_flag).str());
	}
	if(sps_range_extension_flag){
		std::vector<std::string> rangeExtensionFields = sps_range_extension.dump_fields();
		std::transform(rangeExtensionFields.begin(), rangeExtensionFields.end(), std::back_inserter(fields), [](const std::string& subField){
			return "  " + subField;
		});
	}
	if(sps_multilayer_extension_flag){
		std::vector<std::string> multilayerExtensionFields = sps_multilayer_extension.dump_fields();
		std::transform(multilayerExtensionFields.begin(), multilayerExtensionFields.end(), std::back_inserter(fields), [](const std::string& subField){
			return "  " + subField;
		});
	}
	if(sps_3d_extension_flag){
		std::vector<std::string> ThreeDExtensionFields = sps_3d_extension.dump_fields();
		std::transform(ThreeDExtensionFields.begin(), ThreeDExtensionFields.end(), std::back_inserter(fields), [](const std::string& subField){
			return "  " + subField;
		});
	}
	if(sps_scc_extension_flag){
		std::vector<std::string> sccExtensionFields = sps_scc_extension.dump_fields(chroma_format_idc);
		std::transform(sccExtensionFields.begin(), sccExtensionFields.end(), std::back_inserter(fields), [](const std::string& subField){
			return "  " + subField;
		});
	}
	return fields;
}
