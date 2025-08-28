#include <algorithm>

#include "../H26X/H26XUtils.h"
#include "H265VPS.h"

#include "H265SPS.h"

H265VuiParameters::H265VuiParameters()
{
	aspect_ratio_info_present_flag = 0;
	aspect_ratio_idc = 0;
	sar_width = 0;
	sar_height = 0;
	overscan_info_present_flag = 0;
	overscan_appropriate_flag = 0;
	video_signal_type_present_flag = 0;
	video_format = 5;
	video_full_range_flag = 0;
	colour_description_present_flag = 0;
	colour_primaries = 2;
	transfer_characteristics = 2;
	matrix_coeffs = 2;
	chroma_loc_info_present_flag = 0;
	chroma_sample_loc_type_top_field = 0;
	chroma_sample_loc_type_bottom_field = 0;
	neutral_chroma_indication_flag = 0;
	field_seq_flag = 0;
	frame_field_info_present_flag = 0;
	default_display_window_flag = 0;
	def_disp_win_left_offset = 0;
	def_disp_win_right_offset = 0;
	def_disp_win_top_offset = 0;
	def_disp_win_bottom_offset = 0;
	vui_timing_info_present_flag = 0;
	vui_num_units_in_tick = 0;
	vui_time_scale = 0;
	vui_poc_proportional_to_timing_flag = 0;
	vui_num_ticks_poc_diff_one_minus1 = 0;
	vui_hrd_parameters_present_flag = 0;
	bitstream_restriction_flag = 0;
	tiles_fixed_structure_flag = 0;
	motion_vectors_over_pic_boundaries_flag = 1;
	restricted_ref_pic_lists_flag = 0;
	min_spatial_segmentation_idc = 0;
	max_bytes_per_pic_denom = 2;
	max_bits_per_min_cu_denom = 1;
	log2_max_mv_length_horizontal = 15;
	log2_max_mv_length_vertical = 15;
}

void H265VuiParameters::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Video Usability Information");
	{
		dumpObject.startValueUnitFieldList("aspect_ratio_info_present_flag", aspect_ratio_info_present_flag);
		if(aspect_ratio_info_present_flag){
			dumpObject.startValueUnitFieldList("aspect_ratio_idc", aspect_ratio_idc);
			if(aspect_ratio_idc == EXTENDED_SAR){
				dumpObject.addUnitField("sar_width", sar_width);
				dumpObject.addUnitField("sar_height", sar_height);
			}
			dumpObject.endValueUnitFieldList();
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.startValueUnitFieldList("overscan_info_present_flag", overscan_info_present_flag);
		if(overscan_info_present_flag){
			dumpObject.addUnitField("overscan_appropriate_flag", overscan_appropriate_flag);
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.startValueUnitFieldList("video_signal_type_present_flag", video_signal_type_present_flag);
		if(video_signal_type_present_flag){
			dumpObject.addUnitField("video_format", video_format);
			dumpObject.addUnitField("video_full_range_flag", video_full_range_flag);
			dumpObject.startValueUnitFieldList("colour_description_present_flag", colour_description_present_flag);
			if(colour_description_present_flag){
				dumpObject.addUnitField("colour_primaries", colour_primaries);
				dumpObject.addUnitField("transfer_characteristics", transfer_characteristics);
				dumpObject.addUnitField("matrix_coeffs", matrix_coeffs);
			}
			dumpObject.endValueUnitFieldList();
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.startValueUnitFieldList("chroma_loc_info_present_flag", chroma_loc_info_present_flag);
		if(chroma_loc_info_present_flag){
			dumpObject.addUnitField("chroma_sample_loc_type_top_field", chroma_sample_loc_type_top_field);
			dumpObject.addUnitField("chroma_sample_loc_type_bottom_field", chroma_sample_loc_type_bottom_field);
		}
		dumpObject.endValueUnitFieldList();
		
		dumpObject.addUnitField("neutral_chroma_indication_flag", neutral_chroma_indication_flag);
		dumpObject.addUnitField("field_seq_flag", field_seq_flag);
		dumpObject.addUnitField("frame_field_info_present_flag", frame_field_info_present_flag);

		dumpObject.startValueUnitFieldList("default_display_window_flag",default_display_window_flag);
		if(default_display_window_flag){
			dumpObject.addUnitField("def_disp_win_left_offset", def_disp_win_left_offset);
			dumpObject.addUnitField("def_disp_win_right_offset", def_disp_win_right_offset);
			dumpObject.addUnitField("def_disp_win_top_offset", def_disp_win_top_offset);
			dumpObject.addUnitField("def_disp_win_bottom_offset", def_disp_win_bottom_offset);
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.startValueUnitFieldList("vui_timing_info_present_flag", vui_timing_info_present_flag);
		if(vui_timing_info_present_flag){
			dumpObject.addUnitField("vui_num_units_in_tick", vui_num_units_in_tick);
			dumpObject.addUnitField("vui_time_scale", vui_time_scale);
			dumpObject.startValueUnitFieldList("vui_poc_proportional_to_timing_flag", vui_poc_proportional_to_timing_flag);
			if(vui_poc_proportional_to_timing_flag){
				dumpObject.addUnitField("vui_num_ticks_poc_diff_one_minus1", vui_num_ticks_poc_diff_one_minus1);
			}
			dumpObject.endValueUnitFieldList();
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.startValueUnitFieldList("vui_hrd_parameters_present_flag", vui_hrd_parameters_present_flag);
		if(vui_hrd_parameters_present_flag){
			hrd_parameters.dump(dumpObject, 1);
		}
		dumpObject.endValueUnitFieldList();
	}
	dumpObject.endUnitFieldList();
}

H265SPSRangeExtension::H265SPSRangeExtension()
{
	transform_skip_rotation_enabled_flag = 0;
	transform_skip_context_enabled_flag = 0;
	implicit_rdpcm_enabled_flag = 0;
	explicit_rdpcm_enabled_flag = 0;
	extended_precision_processing_flag = 0;
	intra_smoothing_disabled_flag = 0;
	high_precision_offsets_enabled_flag = 0;
	persistent_rice_adaptation_enabled_flag = 0;
	cabac_bypass_alignment_enabled_flag = 0;
}

void H265SPSRangeExtension::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("SPS Range Extension");
	dumpObject.addUnitField("transform_skip_rotation_enabled_flag", transform_skip_rotation_enabled_flag);
	dumpObject.addUnitField("transform_skip_context_enabled_flag", transform_skip_context_enabled_flag);
	dumpObject.addUnitField("implicit_rdpcm_enabled_flag", implicit_rdpcm_enabled_flag);
	dumpObject.addUnitField("explicit_rdpcm_enabled_flag", explicit_rdpcm_enabled_flag);
	dumpObject.addUnitField("extended_precision_processing_flag", extended_precision_processing_flag);
	dumpObject.addUnitField("intra_smoothing_disabled_flag", intra_smoothing_disabled_flag);
	dumpObject.addUnitField("high_precision_offsets_enabled_flag", high_precision_offsets_enabled_flag);
	dumpObject.addUnitField("persistent_rice_adaptation_enabled_flag", persistent_rice_adaptation_enabled_flag);
	dumpObject.addUnitField("cabac_bypass_alignment_enabled_flag", cabac_bypass_alignment_enabled_flag);
	dumpObject.endUnitFieldList();
}

H265SPSMultilayerExtension::H265SPSMultilayerExtension()
{
	inter_view_mv_vert_constraint_flag = 0;
}

void H265SPSMultilayerExtension::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("SPS Multilayer Extension");
	dumpObject.addUnitField("inter_view_mv_vert_constraint_flag", inter_view_mv_vert_constraint_flag);
	dumpObject.endUnitFieldList();
}

H265SPS3DExtension::H265SPS3DExtension()
{
	for(int d = 0;d <= 1;++d){
		iv_di_mc_enabled_flag[d] = 0;
		iv_mv_scal_enabled_flag[d] = 0;
		log2_ivmc_sub_pb_size_minus3[d] = 0;
		iv_res_pred_enabled_flag[d] = 0;
		vsp_mc_enabled_flag[d] = 0;
		dbbp_enabled_flag[d] = 0;
		depth_ref_enabled_flag[d] = 0;
		tex_mc_enabled_flag[d] = 0;
		log2_texmc_sub_pb_size_minus3[d] = 0;
		intra_contour_enabled_flag[d] = 0;
		intra_dc_only_wedge_enabled_flag[d] = 0;
		cqt_cu_part_pred_enabled_flag[d] = 0;
		inter_dc_only_enabled_flag[d] = 0;
		skip_intra_enabled_flag[d] = 0;
	}
}

void H265SPS3DExtension::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("SPS 3D Extension");
	for(int d = 0;d <= 1;++d){
		dumpObject.addIdxUnitField("iv_di_mc_enabled_flag", d, iv_di_mc_enabled_flag[d]);
		dumpObject.addIdxUnitField("iv_mv_scal_enabled_flag", d, iv_mv_scal_enabled_flag[d]);
		if(d == 0){
			dumpObject.addIdxUnitField("log2_ivmc_sub_pb_size_minus3", d, log2_ivmc_sub_pb_size_minus3[d]);
			dumpObject.addIdxUnitField("iv_res_pred_enabled_flag", d, iv_res_pred_enabled_flag[d]);
			dumpObject.addIdxUnitField("depth_ref_enabled_flag", d, depth_ref_enabled_flag[d]);
			dumpObject.addIdxUnitField("vsp_mc_enabled_flag", d, vsp_mc_enabled_flag[d]);
			dumpObject.addIdxUnitField("dbbp_enabled_flag", d, dbbp_enabled_flag[d]);
		} else {
			dumpObject.addIdxUnitField("tex_mc_enabled_flag", d, tex_mc_enabled_flag[d]);
			dumpObject.addIdxUnitField("log2_texmc_sub_pb_size_minus3", d, log2_texmc_sub_pb_size_minus3[d]);
			dumpObject.addIdxUnitField("intra_contour_enabled_flag", d, intra_contour_enabled_flag[d]);
			dumpObject.addIdxUnitField("intra_dc_only_wedge_enabled_flag", d, intra_dc_only_wedge_enabled_flag[d]);
			dumpObject.addIdxUnitField("cqt_cu_part_pred_enabled_flag", d, cqt_cu_part_pred_enabled_flag[d]);
			dumpObject.addIdxUnitField("inter_dc_only_enabled_flag", d, inter_dc_only_enabled_flag[d]);
			dumpObject.addIdxUnitField("skip_intra_enabled_flag", d, skip_intra_enabled_flag[d]);
		}
	}
	dumpObject.endUnitFieldList();
}

H265SPSSCCExtension::H265SPSSCCExtension()
{
	sps_curr_pic_ref_enabled_flag = 0;
	palette_mode_enabled_flag = 0;
	palette_max_size = 0;
	delta_palette_max_predictor_size = 0;
	sps_palette_predictor_initializers_present_flag = 0;
	sps_num_palette_predictor_initializers_minus1 = 0;
	motion_vector_resolution_control_idc = 0;
	intra_boundary_filtering_disabled_flag = 0;
}

void H265SPSSCCExtension::dump(H26XDumpObject& dumpObject, uint32_t chroma_format_idc) const
{
	dumpObject.startUnitFieldList("SPS SCC Extension");
	dumpObject.addUnitField("sps_curr_pic_ref_enabled_flag", sps_curr_pic_ref_enabled_flag);

	dumpObject.startValueUnitFieldList("palette_mode_enabled_flag", palette_mode_enabled_flag);
	if(palette_mode_enabled_flag){
		dumpObject.addUnitField("palette_max_size", palette_max_size);
		dumpObject.addUnitField("delta_palette_max_predictor_size", delta_palette_max_predictor_size);

		dumpObject.startValueUnitFieldList("sps_palette_predictor_initializers_present_flag", sps_palette_predictor_initializers_present_flag);
		if(sps_palette_predictor_initializers_present_flag){
			uint8_t numComps = (chroma_format_idc == 0) ? 1 : 3;
			for(int comp = 0;comp < numComps;++comp){
				for(uint16_t i = 0;i <= sps_num_palette_predictor_initializers_minus1;++i){
					dumpObject.addDblIdxUnitField("sps_palette_predictor_initializer", comp, i, sps_palette_predictor_initializer[comp][i]);
				}
			}
		}
		dumpObject.endValueUnitFieldList();
	}
	dumpObject.endValueUnitFieldList();

	dumpObject.addUnitField("motion_vector_resolution_control_idc", motion_vector_resolution_control_idc);
	dumpObject.addUnitField("intra_boundary_filtering_disabled_flag", intra_boundary_filtering_disabled_flag);
	dumpObject.endUnitFieldList();
}

H265SPS::H265SPS():
	H265SPS(0, H265NALUnitType::Unspecified, 0, 0, 0, nullptr)
{}

H265SPS::H265SPS(uint8_t forbidden_zero_bit, H265NALUnitType::Type nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, const uint8_t* nal_data):
	H265NAL(forbidden_zero_bit, nal_unit_type, nuh_layer_id, nuh_temporal_id_plus1, nal_size, nal_data)
{
	sps_video_parameter_set_id = 0;
	sps_max_sub_layers_minus1 = 0;
	sps_ext_or_max_sub_layers_minus1 = 0;
	sps_temporal_id_nesting_flag = 0;
	sps_seq_parameter_set_id = 0;
	update_rep_format_flag = 0;
	sps_rep_format_idx = 0;
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
	sps_infer_scaling_list_flag = 0;
	sps_scaling_list_ref_layer_id = 0;
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
	sps_extension_4bits = 0;
	sps_extension_data_flag = 0;

	short_term_ref_pic_set.resize(1);

	SubWidthC = 0;
	SubHeightC = 0;
	MinCbLog2SizeY = 3;
	CtbLog2SizeY = 0;
	MinCbSizeY = 1 << MinCbLog2SizeY;
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

	sps_range_extension.CoeffMinY = -(1 << (sps_range_extension.extended_precision_processing_flag ? std::max(15, BitDepthY+6) : 15));
	sps_range_extension.CoeffMinC = -(1 << (sps_range_extension.extended_precision_processing_flag ? std::max(15, BitDepthC+6) : 15));
	sps_range_extension.CoeffMaxY = (1 << (sps_range_extension.extended_precision_processing_flag ? std::max(15, BitDepthY+6) : 15))-1;
	sps_range_extension.CoeffMaxC = (1 << (sps_range_extension.extended_precision_processing_flag ? std::max(15, BitDepthC+6) : 15))-1;

	sps_range_extension.WpOffsetBdShiftY = sps_range_extension.high_precision_offsets_enabled_flag ? 0 : (BitDepthY-8);
	sps_range_extension.WpOffsetBdShiftC = sps_range_extension.high_precision_offsets_enabled_flag ? 0 : (BitDepthC-8);
	sps_range_extension.WpOffsetHalfRangeY = 1 << (sps_range_extension.high_precision_offsets_enabled_flag ? (BitDepthY-1) : 7);
	sps_range_extension.WpOffsetHalfRangeC = 1 << (sps_range_extension.high_precision_offsets_enabled_flag ? (BitDepthC-1) : 7);
}

H265SPS::~H265SPS(){}

void H265SPS::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Sequence Parameter Set");
	H26X_BREAKABLE_SCOPE(H26XDumpScope) {
		H265NAL::dump(dumpObject);

		if (!completelyParsed) {
			break;
		}
		
		dumpObject.addUnitField("sps_video_parameter_set_id", sps_video_parameter_set_id);
		dumpObject.addUnitField("sps_max_sub_layers_minus1", sps_max_sub_layers_minus1);
		dumpObject.addUnitField("sps_temporal_id_nesting_flag", sps_temporal_id_nesting_flag);
		profile_tier_level.dump(dumpObject);
	
		dumpObject.addUnitField("sps_seq_parameter_set_id", sps_seq_parameter_set_id);

		dumpObject.startValueUnitFieldList("chroma_format_idc", chroma_format_idc);
		if(chroma_format_idc == 3){
			dumpObject.addUnitField("separate_colour_plane_flag", separate_colour_plane_flag);
		}
		dumpObject.endValueUnitFieldList();
		
		dumpObject.addUnitField("pic_width_in_luma_samples", pic_width_in_luma_samples);
		dumpObject.addUnitField("pic_height_in_luma_samples", pic_height_in_luma_samples);

		dumpObject.startValueUnitFieldList("conformance_window_flag", conformance_window_flag);
		if(conformance_window_flag){
			dumpObject.addUnitField("conf_win_left_offset", conf_win_left_offset);
			dumpObject.addUnitField("conf_win_right_offset", conf_win_right_offset);
			dumpObject.addUnitField("conf_win_top_offset", conf_win_top_offset);
			dumpObject.addUnitField("conf_win_bottom_offset", conf_win_bottom_offset);
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.addUnitField("bit_depth_luma_minus8", bit_depth_luma_minus8);
		dumpObject.addUnitField("bit_depth_chroma_minus8", bit_depth_chroma_minus8);
		dumpObject.addUnitField("log2_max_pic_order_cnt_lsb_minus4", log2_max_pic_order_cnt_lsb_minus4);
		dumpObject.addUnitField("sps_sub_layer_ordering_info_present_flag", sps_sub_layer_ordering_info_present_flag);
		for(uint8_t i = sps_sub_layer_ordering_info_present_flag ? 0 : sps_max_sub_layers_minus1;i <= sps_max_sub_layers_minus1;++i){
			dumpObject.addIdxUnitField("sps_max_dec_pic_buffering_minus1", i, sps_max_dec_pic_buffering_minus1[i]);
			dumpObject.addIdxUnitField("sps_max_num_reorder_pics", i, sps_max_num_reorder_pics[i]);
			dumpObject.addIdxUnitField("sps_max_latency_increase_plus1", i, sps_max_latency_increase_plus1[i]);
		}
		dumpObject.addUnitField("log2_min_luma_coding_block_size_minus3", log2_min_luma_coding_block_size_minus3);
		dumpObject.addUnitField("log2_diff_max_min_luma_coding_block_size", log2_diff_max_min_luma_coding_block_size);
		dumpObject.addUnitField("log2_min_luma_transform_block_size_minus2", log2_min_luma_transform_block_size_minus2);
		dumpObject.addUnitField("log2_diff_max_min_luma_transform_block_size", log2_diff_max_min_luma_transform_block_size);
		dumpObject.addUnitField("max_transform_hierarchy_depth_inter", max_transform_hierarchy_depth_inter);
		dumpObject.addUnitField("max_transform_hierarchy_depth_intra", max_transform_hierarchy_depth_intra);

		dumpObject.startValueUnitFieldList("scaling_list_enabled_flag", scaling_list_enabled_flag);
		if(scaling_list_enabled_flag){
			dumpObject.startValueUnitFieldList("sps_scaling_list_data_present_flag", sps_scaling_list_data_present_flag);
			if(sps_scaling_list_data_present_flag){
				scaling_list_data.dump(dumpObject);
			}
			dumpObject.endValueUnitFieldList();
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.addUnitField("amp_enabled_flag", amp_enabled_flag);
		dumpObject.addUnitField("sample_adaptive_offset_enabled_flag", sample_adaptive_offset_enabled_flag);

		dumpObject.startValueUnitFieldList("pcm_enabled_flag", pcm_enabled_flag);
		if(pcm_enabled_flag){
			dumpObject.addUnitField("pcm_sample_bit_depth_luma_minus1", pcm_sample_bit_depth_luma_minus1);
			dumpObject.addUnitField("pcm_sample_bit_depth_chroma_minus1", pcm_sample_bit_depth_chroma_minus1);
			dumpObject.addUnitField("log2_min_pcm_luma_coding_block_size_minus3", log2_min_pcm_luma_coding_block_size_minus3);
			dumpObject.addUnitField("log2_diff_max_min_luma_coding_block_size", log2_diff_max_min_luma_coding_block_size);
			dumpObject.addUnitField("pcm_loop_filter_disabled_flag", pcm_loop_filter_disabled_flag);
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.startValueUnitFieldList("num_short_term_ref_pic_sets", num_short_term_ref_pic_sets);
		for(uint32_t i = 0;i < num_short_term_ref_pic_sets;++i){
			short_term_ref_pic_set[i].dump(dumpObject, i, num_short_term_ref_pic_sets);
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.startValueUnitFieldList("long_term_ref_pics_present_flag", long_term_ref_pics_present_flag);
		if(long_term_ref_pics_present_flag){
			dumpObject.startValueUnitFieldList("num_long_term_ref_pics_sps",num_long_term_ref_pics_sps);
			for(uint32_t i = 0;i < num_long_term_ref_pics_sps;++i){
				dumpObject.addIdxUnitField("lt_ref_pic_poc_lsb_sps", i, lt_ref_pic_poc_lsb_sps[i]);
				dumpObject.addIdxUnitField("used_by_curr_pic_lt_sps_flag", i, used_by_curr_pic_lt_sps_flag[i]);
			}
			dumpObject.endValueUnitFieldList();
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.addUnitField("sps_temporal_mvp_enabled_flag", sps_temporal_mvp_enabled_flag);
		dumpObject.addUnitField("strong_intra_smoothing_enabled_flag", strong_intra_smoothing_enabled_flag);

		dumpObject.startValueUnitFieldList("vui_parameters_present_flag", vui_parameters_present_flag);
		if(vui_parameters_present_flag){
			vui_parameters.dump(dumpObject);
		}
		dumpObject.endValueUnitFieldList();

		dumpObject.startValueUnitFieldList("sps_extension_flag", sps_extension_flag);
		{
			if (sps_extension_flag)
			{
				dumpObject.startValueUnitFieldList("sps_range_extension_flag", sps_range_extension_flag);
				if (sps_range_extension_flag) {
					sps_range_extension.dump(dumpObject);
				}
				dumpObject.endValueUnitFieldList();

				dumpObject.startValueUnitFieldList("sps_multilayer_extension_flag", sps_multilayer_extension_flag);
				if (sps_multilayer_extension_flag) {
					sps_multilayer_extension.dump(dumpObject);
				}
				dumpObject.endValueUnitFieldList();

				dumpObject.startValueUnitFieldList("sps_3d_extension_flag", sps_3d_extension_flag);
				if (sps_3d_extension_flag) {
					sps_3d_extension.dump(dumpObject);
				}
				dumpObject.endValueUnitFieldList();

				dumpObject.startValueUnitFieldList("sps_scc_extension_flag", sps_scc_extension_flag);
				if (sps_scc_extension_flag) {
					sps_scc_extension.dump(dumpObject, chroma_format_idc);
				}
				dumpObject.endValueUnitFieldList();
			}
		}
		dumpObject.endValueUnitFieldList();
	}
	dumpObject.endUnitFieldList();
}

void H265SPS::validate(){
	H265NAL::validate();
	if(!completelyParsed){
		return;
	}
	if(sps_max_sub_layers_minus1 > 6){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_sub_layers_minus1 value (%ld) not in valid range (0..6)", sps_max_sub_layers_minus1));
	}
	auto referencedVPS = H265VPS::VPSMap.find(sps_video_parameter_set_id);
	H265VPS* pVps = nullptr;
	if(referencedVPS == H265VPS::VPSMap.end()){
		errors.add(H26XError::Major, H26XUtils::formatString("[SPS] reference to unknown VPS (%ld)", sps_video_parameter_set_id));
	}else{
		pVps = referencedVPS->second;
	}
	if(pVps && sps_max_sub_layers_minus1 > pVps->vps_max_sub_layers_minus1){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_sub_layers_minus1 value (%ld) not less or equal to vps_max_sub_layers_minus1 value (%ld)", sps_max_sub_layers_minus1, pVps->vps_max_sub_layers_minus1));
	}
	if(pVps && pVps->vps_temporal_id_nesting_flag && !sps_temporal_id_nesting_flag){
		errors.add(H26XError::Minor, "[SPS] sps_temporal_id_nesting_flag not set (as enforced by vps_temporal_id_nesting_flag)");
	}
	if(sps_max_sub_layers_minus1 == 0 && !sps_temporal_id_nesting_flag){
		errors.add(H26XError::Minor, "[SPS] sps_temporal_id_nesting_flag not set (as enforced by sps_max_sub_layers_minus1)");
	}
	if(sps_seq_parameter_set_id > 15){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_seq_parameter_set_id value (%ld) not in valid range (0..15)", sps_seq_parameter_set_id));
	}
	if(chroma_format_idc > 3){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] chroma_format_idc value (%ld) not in valid range (0..3)", chroma_format_idc));
	}
	if(pic_width_in_luma_samples == 0){
		errors.add(H26XError::Minor, "[SPS] pic_width_in_luma_samples value equal to 0");
	}
	if(pic_width_in_luma_samples%MinCbSizeY != 0){
		errors.add(H26XError::Minor, "[SPS] pic_width_in_luma_samples value not an integer multiple of MinCbSizeY");
	}
	if(pic_height_in_luma_samples == 0){
		errors.add(H26XError::Minor, "[SPS] pic_height_in_luma_samples value equal to 0");
	}
	if(pic_height_in_luma_samples%MinCbSizeY != 0){
		errors.add(H26XError::Minor, "[SPS] pic_height_in_luma_samples value not an integer multiple of MinCbSizeY");
	}
	if(conformance_window_flag){
		uint32_t conformanceWindowOffsetWidth = SubWidthC*(conf_win_left_offset+conf_win_right_offset);
		if(conformanceWindowOffsetWidth >= pic_width_in_luma_samples){
			errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] conformanceWindowOffsetWidth value (%ld) greater or equal to pic_width_in_luma_samples", conformanceWindowOffsetWidth));
		}
		uint32_t conformanceWindowOffsetHeight = SubHeightC*(conf_win_top_offset+conf_win_bottom_offset);
		if(conformanceWindowOffsetHeight >= pic_height_in_luma_samples){
			errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] conformanceWindowOffsetHeight value (%ld) greater or equal to pic_height_in_luma_samples", conformanceWindowOffsetHeight));
		}
	}
	if(bit_depth_luma_minus8 > 8){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] bit_depth_luma_minus8 value (%ld) not in valid range (0..8)", bit_depth_luma_minus8));
	}
	if(bit_depth_chroma_minus8 > 8){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] bit_depth_chroma_minus8 value (%ld) not in valid range (0..8)", bit_depth_chroma_minus8));
	}
	if(log2_max_pic_order_cnt_lsb_minus4 > 12){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] log2_max_pic_order_cnt_lsb_minus4 value (%ld) not in valid range (0..12)", log2_max_pic_order_cnt_lsb_minus4));
	}
	for (uint8_t i = (sps_sub_layer_ordering_info_present_flag ? 0 : sps_max_sub_layers_minus1); i <= sps_max_sub_layers_minus1; ++i) {
		if(sps_max_dec_pic_buffering_minus1[i] > MaxDpbSize-1){
			errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_dec_pic_buffering_minus1[%d] value (%ld) not in valid range (0..{})", i, sps_max_dec_pic_buffering_minus1[i], MaxDpbSize-1));
		}
		if(sps_max_num_reorder_pics[i] > sps_max_dec_pic_buffering_minus1[i]){
			errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_num_reorder_pics[%d] value (%ld) not in valid range (0..{})", i, sps_max_num_reorder_pics[i], sps_max_dec_pic_buffering_minus1[i]));
		}
		if(sps_max_latency_increase_plus1[i] == UINT32_MAX){
			errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_latency_increase_plus1[%d] value (%ld) not in valid range (0..4294967294)", i, sps_max_latency_increase_plus1[i]));
		}
		if(i > 0){
			if(sps_max_dec_pic_buffering_minus1[i] < sps_max_dec_pic_buffering_minus1[i-1]){
				errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_dec_pic_buffering_minus1[%d] value (%ld) lesser than previous sps_max_dec_pic_buffering_minus1 value", i, sps_max_dec_pic_buffering_minus1[i]));
			}
			if(sps_max_num_reorder_pics[i] < sps_max_num_reorder_pics[i-1]){
				errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_num_reorder_pics[%d] value (%ld) lesser than previous sps_max_num_reorder_pics value", i, sps_max_num_reorder_pics[i]));
			}
		}
		if(pVps){
			if(sps_max_dec_pic_buffering_minus1[i] > pVps->vps_max_dec_pic_buffering_minus1[i]){
				errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_dec_pic_buffering_minus1[%d] value (%ld) greater than vps_max_dec_pic_buffering_minus1[%d] value (%ld)", i, sps_max_dec_pic_buffering_minus1[i], i, pVps->vps_max_dec_pic_buffering_minus1[i]));
			}
			if(sps_max_num_reorder_pics[i] > pVps->vps_max_num_reorder_pics[i]){
				errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_num_reorder_pics[%d] value (%ld) greater than vps_max_num_reorder_pics[%d] value (%ld)", i, sps_max_num_reorder_pics[i], i, pVps->vps_max_num_reorder_pics[i]));
			}
			if(pVps->vps_max_latency_increase_plus1[i] != 0){
				if(sps_max_latency_increase_plus1[i] == 0){
					errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_latency_increase_plus1[%d] value should not be 0 (as enforced by vps_max_latency_increase_plus1[%d])", i, i));
				}
				if(sps_max_latency_increase_plus1[i] > pVps->vps_max_latency_increase_plus1[i]){
					errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] sps_max_latency_increase_plus1[%d] value (%ld) greater than vps_max_latency_increase_plus1[%d] value (%ld)", i, sps_max_latency_increase_plus1[i], i, pVps->vps_max_latency_increase_plus1[i]));
				}
			}
		}
	}
	if(max_transform_hierarchy_depth_inter > CtbLog2SizeY - MinTbLog2SizeY){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] max_transform_hierarchy_depth_inter value (%ld) not in valid range (0..{})", max_transform_hierarchy_depth_inter, CtbLog2SizeY - MinTbLog2SizeY));
	}
	if(max_transform_hierarchy_depth_intra > CtbLog2SizeY - MinTbLog2SizeY){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] max_transform_hierarchy_depth_intra value (%ld) not in valid range (0..{})", max_transform_hierarchy_depth_intra, CtbLog2SizeY - MinTbLog2SizeY));
	}
	if(PcmBitDepthY > BitDepthY){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] PcmBitDepthY value (%ld) not in valid range (0..{})", PcmBitDepthY, BitDepthY));
	}
	if(PcmBitDepthC > BitDepthC){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] PcmBitDepthC value (%ld) not in valid range (0..{})", PcmBitDepthC, BitDepthC));
	}
	if(pcm_enabled_flag){
		if(Log2MinIpcmCbSizeY < std::min(MinCbLog2SizeY, 5u) || Log2MinIpcmCbSizeY > std::min(CtbLog2SizeY, 5u)){
			errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] Log2MinIpcmCbSizeY value (%ld) not in valid range ({}..{})", Log2MinIpcmCbSizeY, std::min(MinCbLog2SizeY, 5u), std::min(CtbLog2SizeY, 5u)));
		}
		if(Log2MaxIpcmCbSizeY > std::min(CtbLog2SizeY, 5u)){
			errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] Log2MaxIpcmCbSizeY value (%ld) not in valid range (0..{})", Log2MaxIpcmCbSizeY, std::min(CtbLog2SizeY, 5u)));
		}
	}
	if(num_short_term_ref_pic_sets > 64){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] num_short_term_ref_pic_sets value (%ld) not in valid range (0..64)", num_short_term_ref_pic_sets));
	}
	if(num_long_term_ref_pics_sps > 32){
		errors.add(H26XError::Minor, H26XUtils::formatString("[SPS] num_long_term_ref_pics_sps value (%ld) not in valid range (0..32)", num_long_term_ref_pics_sps));
	}
}

uint16_t H265SPS::computeMaxFrameNumber() const
{
	if(log2_max_pic_order_cnt_lsb_minus4 >= 12) return UINT16_MAX;
	return 1 << (log2_max_pic_order_cnt_lsb_minus4+4);
}
