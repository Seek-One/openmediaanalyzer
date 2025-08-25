#include <algorithm>

#include "H265VPS.h"
#include "../../StringHelpers/StringFormatter.h"
#include "../../StringHelpers/UnitFieldList.h"

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

UnitFieldList H265VuiParameters::dump_fields(){
	UnitFieldList fields = UnitFieldList("Video Usability Information");
	ValueUnitFieldList aspect_ratio_info_present_flagField = ValueUnitFieldList("aspect_ratio_info_present_flag", aspect_ratio_info_present_flag);
	if(aspect_ratio_info_present_flag){
		ValueUnitFieldList aspect_ratio_idcField = ValueUnitFieldList("aspect_ratio_idc", aspect_ratio_idc);
		if(aspect_ratio_idc == EXTENDED_SAR){
			aspect_ratio_idcField.addItem(UnitField("sar_width", sar_width));
			aspect_ratio_idcField.addItem(UnitField("sar_height", sar_height));
		}
		aspect_ratio_info_present_flagField.addItem(std::move(aspect_ratio_idcField));
	}
	fields.addItem(std::move(aspect_ratio_info_present_flagField));
	ValueUnitFieldList overscan_info_present_flagField = ValueUnitFieldList("overscan_info_present_flag", overscan_info_present_flag);
	if(overscan_info_present_flag) overscan_info_present_flagField.addItem(UnitField("overscan_appropriate_flag", overscan_appropriate_flag));
	fields.addItem(std::move(overscan_info_present_flagField));
	ValueUnitFieldList video_signal_type_present_flagField = ValueUnitFieldList("video_signal_type_present_flag", video_signal_type_present_flag);
	if(video_signal_type_present_flag){
		video_signal_type_present_flagField.addItem(UnitField("video_format", video_format));
		video_signal_type_present_flagField.addItem(UnitField("video_full_range_flag", video_full_range_flag));
		ValueUnitFieldList colour_description_present_flagField = ValueUnitFieldList("colour_description_present_flag", colour_description_present_flag);
		if(colour_description_present_flag){
			colour_description_present_flagField.addItem(UnitField("colour_primaries", colour_primaries));
			colour_description_present_flagField.addItem(UnitField("transfer_characteristics", transfer_characteristics));
			colour_description_present_flagField.addItem(UnitField("matrix_coeffs", matrix_coeffs));
		}
		video_signal_type_present_flagField.addItem(std::move(colour_description_present_flagField));
	}
	fields.addItem(std::move(video_signal_type_present_flagField));
	ValueUnitFieldList chroma_loc_info_present_flagField = ValueUnitFieldList("chroma_loc_info_present_flag", chroma_loc_info_present_flag);
	if(chroma_loc_info_present_flag){
		chroma_loc_info_present_flagField.addItem(UnitField("chroma_sample_loc_type_top_field", chroma_sample_loc_type_top_field));
		chroma_loc_info_present_flagField.addItem(UnitField("chroma_sample_loc_type_bottom_field", chroma_sample_loc_type_bottom_field));
	}
	fields.addItem(std::move(chroma_loc_info_present_flagField));
	fields.addItem(UnitField("neutral_chroma_indication_flag", neutral_chroma_indication_flag));
	fields.addItem(UnitField("field_seq_flag", field_seq_flag));
	fields.addItem(UnitField("frame_field_info_present_flag", frame_field_info_present_flag));
	ValueUnitFieldList default_display_window_flagField = ValueUnitFieldList("default_display_window_flag",default_display_window_flag);
	if(default_display_window_flag){
		default_display_window_flagField.addItem(UnitField("def_disp_win_left_offset", def_disp_win_left_offset));
		default_display_window_flagField.addItem(UnitField("def_disp_win_right_offset", def_disp_win_right_offset));
		default_display_window_flagField.addItem(UnitField("def_disp_win_top_offset", def_disp_win_top_offset));
		default_display_window_flagField.addItem(UnitField("def_disp_win_bottom_offset", def_disp_win_bottom_offset));
	}
	fields.addItem(std::move(default_display_window_flagField));
	ValueUnitFieldList vui_timing_info_present_flagField = ValueUnitFieldList("vui_timing_info_present_flag", vui_timing_info_present_flag);
	if(vui_timing_info_present_flag){
		vui_timing_info_present_flagField.addItem(UnitField("vui_num_units_in_tick", vui_num_units_in_tick));
		vui_timing_info_present_flagField.addItem(UnitField("vui_time_scale", vui_time_scale));
		ValueUnitFieldList vui_poc_proportional_to_timing_flagField = ValueUnitFieldList("vui_poc_proportional_to_timing_flag",vui_poc_proportional_to_timing_flag);
		vui_timing_info_present_flagField.addItem(std::move(vui_poc_proportional_to_timing_flagField));
		if(vui_poc_proportional_to_timing_flag) vui_poc_proportional_to_timing_flagField.addItem(UnitField("vui_num_ticks_poc_diff_one_minus1", vui_num_ticks_poc_diff_one_minus1));
	}
	fields.addItem(std::move(vui_timing_info_present_flagField));
	ValueUnitFieldList vui_hrd_parameters_present_flagField = ValueUnitFieldList("vui_hrd_parameters_present_flag", vui_hrd_parameters_present_flag);
	if(vui_hrd_parameters_present_flag){
		vui_hrd_parameters_present_flagField.addItem(hrd_parameters.dump_fields(1));
	}
	fields.addItem(std::move(vui_hrd_parameters_present_flagField));
	return fields;
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

UnitFieldList H265SPSRangeExtension::dump_fields(){
	UnitFieldList fields = UnitFieldList("SPS Range Extension");
	fields.addItem(UnitField("transform_skip_rotation_enabled_flag", transform_skip_rotation_enabled_flag));
	fields.addItem(UnitField("transform_skip_context_enabled_flag", transform_skip_context_enabled_flag));
	fields.addItem(UnitField("implicit_rdpcm_enabled_flag", implicit_rdpcm_enabled_flag));
	fields.addItem(UnitField("explicit_rdpcm_enabled_flag", explicit_rdpcm_enabled_flag));
	fields.addItem(UnitField("extended_precision_processing_flag", extended_precision_processing_flag));
	fields.addItem(UnitField("intra_smoothing_disabled_flag", intra_smoothing_disabled_flag));
	fields.addItem(UnitField("high_precision_offsets_enabled_flag", high_precision_offsets_enabled_flag));
	fields.addItem(UnitField("persistent_rice_adaptation_enabled_flag", persistent_rice_adaptation_enabled_flag));
	fields.addItem(UnitField("cabac_bypass_alignment_enabled_flag", cabac_bypass_alignment_enabled_flag));
	return fields;
}

H265SPSMultilayerExtension::H265SPSMultilayerExtension()
{
	inter_view_mv_vert_constraint_flag = 0;
}

UnitFieldList H265SPSMultilayerExtension::dump_fields(){
	UnitFieldList fields = UnitFieldList("SPS Multilayer Extension");
	fields.addItem(UnitField("inter_view_mv_vert_constraint_flag", inter_view_mv_vert_constraint_flag));
	return fields;
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

UnitFieldList H265SPS3DExtension::dump_fields(){
	UnitFieldList fields = UnitFieldList("SPS 3D Extension");
	for(int d = 0;d <= 1;++d){
		fields.addItem(IdxUnitField("iv_di_mc_enabled_flag", iv_di_mc_enabled_flag[d], d));
		fields.addItem(IdxUnitField("iv_mv_scal_enabled_flag", iv_mv_scal_enabled_flag[d], d));
		if(d == 0){
			fields.addItem(IdxUnitField("log2_ivmc_sub_pb_size_minus3", log2_ivmc_sub_pb_size_minus3[d], d));
			fields.addItem(IdxUnitField("iv_res_pred_enabled_flag", iv_res_pred_enabled_flag[d], d));
			fields.addItem(IdxUnitField("depth_ref_enabled_flag", depth_ref_enabled_flag[d], d));
			fields.addItem(IdxUnitField("vsp_mc_enabled_flag", vsp_mc_enabled_flag[d], d));
			fields.addItem(IdxUnitField("dbbp_enabled_flag", dbbp_enabled_flag[d], d));
		} else {
			fields.addItem(IdxUnitField("tex_mc_enabled_flag", tex_mc_enabled_flag[d], d));
			fields.addItem(IdxUnitField("log2_texmc_sub_pb_size_minus3", log2_texmc_sub_pb_size_minus3[d], d));
			fields.addItem(IdxUnitField("intra_contour_enabled_flag", intra_contour_enabled_flag[d], d));
			fields.addItem(IdxUnitField("intra_dc_only_wedge_enabled_flag", intra_dc_only_wedge_enabled_flag[d], d));
			fields.addItem(IdxUnitField("cqt_cu_part_pred_enabled_flag", cqt_cu_part_pred_enabled_flag[d], d));
			fields.addItem(IdxUnitField("inter_dc_only_enabled_flag", inter_dc_only_enabled_flag[d], d));
			fields.addItem(IdxUnitField("skip_intra_enabled_flag", skip_intra_enabled_flag[d], d));
		}
	}
	return fields;
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

UnitFieldList H265SPSSCCExtension::dump_fields(uint32_t chroma_format_idc){
	UnitFieldList fields = UnitFieldList("SPS SCC Extension");
	fields.addItem(UnitField("sps_curr_pic_ref_enabled_flag", sps_curr_pic_ref_enabled_flag));
	ValueUnitFieldList palette_mode_enabled_flagField = ValueUnitFieldList("palette_mode_enabled_flag", palette_mode_enabled_flag);
	if(palette_mode_enabled_flag){
		palette_mode_enabled_flagField.addItem(UnitField("palette_max_size", palette_max_size));
		palette_mode_enabled_flagField.addItem(UnitField("delta_palette_max_predictor_size", delta_palette_max_predictor_size));
		ValueUnitFieldList sps_palette_predictor_initializers_present_flagField = ValueUnitFieldList("sps_palette_predictor_initializers_present_flag", sps_palette_predictor_initializers_present_flag);
		palette_mode_enabled_flagField.addItem(std::move(sps_palette_predictor_initializers_present_flagField));
		if(sps_palette_predictor_initializers_present_flag){
			uint8_t numComps = (chroma_format_idc == 0) ? 1 : 3;
			for(int comp = 0;comp < numComps;++comp){
				for(uint16_t i = 0;i <= sps_num_palette_predictor_initializers_minus1;++i){
					sps_palette_predictor_initializers_present_flagField.addItem(DblIdxUnitField("sps_palette_predictor_initializer", sps_palette_predictor_initializer[comp][i], comp, i));
				}
			}
		}
	}
	fields.addItem(std::move(palette_mode_enabled_flagField));
	fields.addItem(UnitField("motion_vector_resolution_control_idc", motion_vector_resolution_control_idc));
	fields.addItem(UnitField("intra_boundary_filtering_disabled_flag", intra_boundary_filtering_disabled_flag));
	return fields;
}

H265SPS::H265SPS():
	H265SPS(0, UnitType_Unspecified, 0, 0, 0, nullptr)
{}

H265SPS::H265SPS(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, const uint8_t* nal_data):
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

UnitFieldList H265SPS::dump_fields()
{
	UnitFieldList fields = UnitFieldList("Sequence Parameter Set", H265NAL::dump_fields());
	if(!completelyParsed) return fields;
	fields.addItem(UnitField("sps_video_parameter_set_id", sps_video_parameter_set_id));
	fields.addItem(UnitField("sps_max_sub_layers_minus1", sps_max_sub_layers_minus1));
	fields.addItem(UnitField("sps_temporal_id_nesting_flag", sps_temporal_id_nesting_flag));
	fields.addItem(profile_tier_level.dump_fields());

	fields.addItem(UnitField("sps_seq_parameter_set_id", sps_seq_parameter_set_id));
	ValueUnitFieldList chroma_format_idcField = ValueUnitFieldList("chroma_format_idc", chroma_format_idc);
	if(chroma_format_idc == 3) chroma_format_idcField.addItem(UnitField("separate_colour_plane_flag", separate_colour_plane_flag));
	fields.addItem(std::move(chroma_format_idcField));
	fields.addItem(UnitField("pic_width_in_luma_samples", pic_width_in_luma_samples));
	fields.addItem(UnitField("pic_height_in_luma_samples", pic_height_in_luma_samples));
	ValueUnitFieldList conformance_window_flagField = ValueUnitFieldList("conformance_window_flag", conformance_window_flag);
	if(conformance_window_flag){
		conformance_window_flagField.addItem(UnitField("conf_win_left_offset", conf_win_left_offset));
		conformance_window_flagField.addItem(UnitField("conf_win_right_offset", conf_win_right_offset));
		conformance_window_flagField.addItem(UnitField("conf_win_top_offset", conf_win_top_offset));
		conformance_window_flagField.addItem(UnitField("conf_win_bottom_offset", conf_win_bottom_offset));
	}
	fields.addItem(std::move(conformance_window_flagField));
	fields.addItem(UnitField("bit_depth_luma_minus8", bit_depth_luma_minus8));
	fields.addItem(UnitField("bit_depth_chroma_minus8", bit_depth_chroma_minus8));
	fields.addItem(UnitField("log2_max_pic_order_cnt_lsb_minus4", log2_max_pic_order_cnt_lsb_minus4));
	fields.addItem(UnitField("sps_sub_layer_ordering_info_present_flag", sps_sub_layer_ordering_info_present_flag));
	for(uint8_t i = sps_sub_layer_ordering_info_present_flag ? 0 : sps_max_sub_layers_minus1;i <= sps_max_sub_layers_minus1;++i){
		fields.addItem(IdxUnitField("sps_max_dec_pic_buffering_minus1", sps_max_dec_pic_buffering_minus1[i], i));
		fields.addItem(IdxUnitField("sps_max_num_reorder_pics", sps_max_num_reorder_pics[i], i));
		fields.addItem(IdxUnitField("sps_max_latency_increase_plus1", sps_max_latency_increase_plus1[i], i));
	}
	fields.addItem(UnitField("log2_min_luma_coding_block_size_minus3", log2_min_luma_coding_block_size_minus3));
	fields.addItem(UnitField("log2_diff_max_min_luma_coding_block_size", log2_diff_max_min_luma_coding_block_size));
	fields.addItem(UnitField("log2_min_luma_transform_block_size_minus2", log2_min_luma_transform_block_size_minus2));
	fields.addItem(UnitField("log2_diff_max_min_luma_transform_block_size", log2_diff_max_min_luma_transform_block_size));
	fields.addItem(UnitField("max_transform_hierarchy_depth_inter", max_transform_hierarchy_depth_inter));
	fields.addItem(UnitField("max_transform_hierarchy_depth_intra", max_transform_hierarchy_depth_intra));
	ValueUnitFieldList scaling_list_enabled_flagField = ValueUnitFieldList("scaling_list_enabled_flag", scaling_list_enabled_flag);
	if(scaling_list_enabled_flag){
		ValueUnitFieldList sps_scaling_list_data_present_flagField("sps_scaling_list_data_present_flag", sps_scaling_list_data_present_flag);
		if(sps_scaling_list_data_present_flag){
			sps_scaling_list_data_present_flagField.addItem(scaling_list_data.dump_fields());
		}
		scaling_list_enabled_flagField.addItem(std::move(sps_scaling_list_data_present_flagField));
	}
	fields.addItem(std::move(scaling_list_enabled_flagField));
	fields.addItem(UnitField("amp_enabled_flag", amp_enabled_flag));
	fields.addItem(UnitField("sample_adaptive_offset_enabled_flag", sample_adaptive_offset_enabled_flag));
	ValueUnitFieldList pcm_enabled_flagField = ValueUnitFieldList("pcm_enabled_flag", pcm_enabled_flag);
	if(pcm_enabled_flag){
		pcm_enabled_flagField.addItem(UnitField("pcm_sample_bit_depth_luma_minus1", pcm_sample_bit_depth_luma_minus1));
		pcm_enabled_flagField.addItem(UnitField("pcm_sample_bit_depth_chroma_minus1", pcm_sample_bit_depth_chroma_minus1));
		pcm_enabled_flagField.addItem(UnitField("log2_min_pcm_luma_coding_block_size_minus3", log2_min_pcm_luma_coding_block_size_minus3));
		pcm_enabled_flagField.addItem(UnitField("log2_diff_max_min_luma_coding_block_size", log2_diff_max_min_luma_coding_block_size));
		pcm_enabled_flagField.addItem(UnitField("pcm_loop_filter_disabled_flag", pcm_loop_filter_disabled_flag));
	}
	fields.addItem(std::move(pcm_enabled_flagField));
	ValueUnitFieldList num_short_term_ref_pic_setsField = ValueUnitFieldList("num_short_term_ref_pic_sets", num_short_term_ref_pic_sets);
	for(uint32_t i = 0;i < num_short_term_ref_pic_sets;++i){
		num_short_term_ref_pic_setsField.addItem(short_term_ref_pic_set[i].dump_fields(i, num_short_term_ref_pic_sets));
	}
	fields.addItem(std::move(num_short_term_ref_pic_setsField));
	ValueUnitFieldList long_term_ref_pics_present_flagField = ValueUnitFieldList("long_term_ref_pics_present_flag", long_term_ref_pics_present_flag);
	if(long_term_ref_pics_present_flag){
		ValueUnitFieldList num_long_term_ref_pics_spsField = ValueUnitFieldList("num_long_term_ref_pics_sps",num_long_term_ref_pics_sps);
		long_term_ref_pics_present_flagField.addItem(std::move(num_long_term_ref_pics_spsField));
		for(uint32_t i = 0;i < num_long_term_ref_pics_sps;++i){
			num_long_term_ref_pics_spsField.addItem(IdxUnitField("lt_ref_pic_poc_lsb_sps", lt_ref_pic_poc_lsb_sps[i], i));
			num_long_term_ref_pics_spsField.addItem(IdxUnitField("used_by_curr_pic_lt_sps_flag", used_by_curr_pic_lt_sps_flag[i], i));
		}
	}
	fields.addItem(std::move(long_term_ref_pics_present_flagField));
	fields.addItem(UnitField("sps_temporal_mvp_enabled_flag", sps_temporal_mvp_enabled_flag));
	fields.addItem(UnitField("strong_intra_smoothing_enabled_flag", strong_intra_smoothing_enabled_flag));
	ValueUnitFieldList vui_parameters_present_flagField = ValueUnitFieldList("vui_parameters_present_flag", vui_parameters_present_flag);
	if(vui_parameters_present_flag) vui_parameters_present_flagField.addItem(vui_parameters.dump_fields());
	fields.addItem(std::move(vui_parameters_present_flagField));
	
	ValueUnitFieldList sps_extension_flagField = ValueUnitFieldList("sps_extension_flag", sps_extension_flag);
	
	ValueUnitFieldList sps_range_extension_flagField = ValueUnitFieldList("sps_range_extension_flag", sps_range_extension_flag);
	ValueUnitFieldList sps_multilayer_extension_flagField = ValueUnitFieldList("sps_multilayer_extension_flag", sps_multilayer_extension_flag);
	ValueUnitFieldList sps_3d_extension_flagField = ValueUnitFieldList("sps_3d_extension_flag", sps_3d_extension_flag);
	ValueUnitFieldList sps_scc_extension_flagField = ValueUnitFieldList("sps_scc_extension_flag", sps_scc_extension_flag);
	
	fields.addItem(std::move(sps_extension_flagField));
	if(sps_range_extension_flag)sps_range_extension_flagField.addItem(sps_range_extension.dump_fields());
	if(sps_multilayer_extension_flag) sps_multilayer_extension_flagField.addItem(sps_multilayer_extension.dump_fields());
	if(sps_3d_extension_flag) sps_3d_extension_flagField.addItem(sps_3d_extension.dump_fields());
	if(sps_scc_extension_flag) sps_scc_extension_flagField.addItem(sps_scc_extension.dump_fields(chroma_format_idc));

	if(sps_extension_flag){
		sps_extension_flagField.addItem(std::move(sps_range_extension_flagField));
		sps_extension_flagField.addItem(std::move(sps_multilayer_extension_flagField));
		sps_extension_flagField.addItem(std::move(sps_3d_extension_flagField));
		sps_extension_flagField.addItem(std::move(sps_scc_extension_flagField));
	}
	return fields;
}

void H265SPS::validate(){
	H265NAL::validate();
	if(!completelyParsed) return;
	if(sps_max_sub_layers_minus1 > 6) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_sub_layers_minus1 value (%ld) not in valid range (0..6)", sps_max_sub_layers_minus1));
	auto referencedVPS = H265VPS::VPSMap.find(sps_video_parameter_set_id);
	H265VPS* pVps = nullptr;
	if(referencedVPS == H265VPS::VPSMap.end()) majorErrors.push_back(StringFormatter::formatString("[SPS] reference to unknown VPS (%ld)", sps_video_parameter_set_id));
	else pVps = referencedVPS->second;
	if(pVps && sps_max_sub_layers_minus1 > pVps->vps_max_sub_layers_minus1) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_sub_layers_minus1 value (%ld) not less or equal to vps_max_sub_layers_minus1 value (%ld)", sps_max_sub_layers_minus1, pVps->vps_max_sub_layers_minus1));
	if(pVps && pVps->vps_temporal_id_nesting_flag && !sps_temporal_id_nesting_flag) minorErrors.push_back("[SPS] sps_temporal_id_nesting_flag not set (as enforced by vps_temporal_id_nesting_flag)");
	if(sps_max_sub_layers_minus1 == 0 && !sps_temporal_id_nesting_flag)minorErrors.push_back("[SPS] sps_temporal_id_nesting_flag not set (as enforced by sps_max_sub_layers_minus1)");
	if(sps_seq_parameter_set_id > 15) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_seq_parameter_set_id value (%ld) not in valid range (0..15)", sps_seq_parameter_set_id));
	if(chroma_format_idc > 3) minorErrors.push_back(StringFormatter::formatString("[SPS] chroma_format_idc value (%ld) not in valid range (0..3)", chroma_format_idc));
	if(pic_width_in_luma_samples == 0) minorErrors.push_back("[SPS] pic_width_in_luma_samples value equal to 0");
	if(pic_width_in_luma_samples%MinCbSizeY != 0) minorErrors.push_back("[SPS] pic_width_in_luma_samples value not an integer multiple of MinCbSizeY");
	if(pic_height_in_luma_samples == 0) minorErrors.push_back("[SPS] pic_height_in_luma_samples value equal to 0");
	if(pic_height_in_luma_samples%MinCbSizeY != 0) minorErrors.push_back("[SPS] pic_height_in_luma_samples value not an integer multiple of MinCbSizeY");
	if(conformance_window_flag){
		uint32_t conformanceWindowOffsetWidth = SubWidthC*(conf_win_left_offset+conf_win_right_offset);
		if(conformanceWindowOffsetWidth >= pic_width_in_luma_samples) minorErrors.push_back(StringFormatter::formatString("[SPS] conformanceWindowOffsetWidth value (%ld) greater or equal to pic_width_in_luma_samples", conformanceWindowOffsetWidth));
		uint32_t conformanceWindowOffsetHeight = SubHeightC*(conf_win_top_offset+conf_win_bottom_offset);
		if(conformanceWindowOffsetHeight >= pic_height_in_luma_samples) minorErrors.push_back(StringFormatter::formatString("[SPS] conformanceWindowOffsetHeight value (%ld) greater or equal to pic_height_in_luma_samples", conformanceWindowOffsetHeight));
	}
	if(bit_depth_luma_minus8 > 8) minorErrors.push_back(StringFormatter::formatString("[SPS] bit_depth_luma_minus8 value (%ld) not in valid range (0..8)", bit_depth_luma_minus8));
	if(bit_depth_chroma_minus8 > 8) minorErrors.push_back(StringFormatter::formatString("[SPS] bit_depth_chroma_minus8 value (%ld) not in valid range (0..8)", bit_depth_chroma_minus8));
	if(log2_max_pic_order_cnt_lsb_minus4 > 12) minorErrors.push_back(StringFormatter::formatString("[SPS] log2_max_pic_order_cnt_lsb_minus4 value (%ld) not in valid range (0..12)", log2_max_pic_order_cnt_lsb_minus4));
	for (uint8_t i = (sps_sub_layer_ordering_info_present_flag ? 0 : sps_max_sub_layers_minus1); i <= sps_max_sub_layers_minus1; ++i) {
		if(sps_max_dec_pic_buffering_minus1[i] > MaxDpbSize-1) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_dec_pic_buffering_minus1[%d] value (%ld) not in valid range (0..{})", i, sps_max_dec_pic_buffering_minus1[i], MaxDpbSize-1));
		if(sps_max_num_reorder_pics[i] > sps_max_dec_pic_buffering_minus1[i]) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_num_reorder_pics[%d] value (%ld) not in valid range (0..{})", i, sps_max_num_reorder_pics[i], sps_max_dec_pic_buffering_minus1[i]));
		if(sps_max_latency_increase_plus1[i] == UINT32_MAX) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_latency_increase_plus1[%d] value (%ld) not in valid range (0..4294967294)", i, sps_max_latency_increase_plus1[i]));
		if(i > 0){
			if(sps_max_dec_pic_buffering_minus1[i] < sps_max_dec_pic_buffering_minus1[i-1]) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_dec_pic_buffering_minus1[%d] value (%ld) lesser than previous sps_max_dec_pic_buffering_minus1 value", i, sps_max_dec_pic_buffering_minus1[i]));
			if(sps_max_num_reorder_pics[i] < sps_max_num_reorder_pics[i-1]) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_num_reorder_pics[%d] value (%ld) lesser than previous sps_max_num_reorder_pics value", i, sps_max_num_reorder_pics[i]));
		}
		if(pVps){
			if(sps_max_dec_pic_buffering_minus1[i] > pVps->vps_max_dec_pic_buffering_minus1[i]) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_dec_pic_buffering_minus1[%d] value (%ld) greater than vps_max_dec_pic_buffering_minus1[%d] value (%ld)", i, sps_max_dec_pic_buffering_minus1[i], i, pVps->vps_max_dec_pic_buffering_minus1[i]));
			if(sps_max_num_reorder_pics[i] > pVps->vps_max_num_reorder_pics[i]) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_num_reorder_pics[%d] value (%ld) greater than vps_max_num_reorder_pics[%d] value (%ld)", i, sps_max_num_reorder_pics[i], i, pVps->vps_max_num_reorder_pics[i]));
			if(pVps->vps_max_latency_increase_plus1[i] != 0){
				if(sps_max_latency_increase_plus1[i] == 0) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_latency_increase_plus1[%d] value should not be 0 (as enforced by vps_max_latency_increase_plus1[%d])", i, i));
				if(sps_max_latency_increase_plus1[i] > pVps->vps_max_latency_increase_plus1[i]) minorErrors.push_back(StringFormatter::formatString("[SPS] sps_max_latency_increase_plus1[%d] value (%ld) greater than vps_max_latency_increase_plus1[%d] value (%ld)", i, sps_max_latency_increase_plus1[i], i, pVps->vps_max_latency_increase_plus1[i]));
			}
		}
	}
	if(max_transform_hierarchy_depth_inter > CtbLog2SizeY - MinTbLog2SizeY) minorErrors.push_back(StringFormatter::formatString("[SPS] max_transform_hierarchy_depth_inter value (%ld) not in valid range (0..{})", max_transform_hierarchy_depth_inter, CtbLog2SizeY - MinTbLog2SizeY));
	if(max_transform_hierarchy_depth_intra > CtbLog2SizeY - MinTbLog2SizeY) minorErrors.push_back(StringFormatter::formatString("[SPS] max_transform_hierarchy_depth_intra value (%ld) not in valid range (0..{})", max_transform_hierarchy_depth_intra, CtbLog2SizeY - MinTbLog2SizeY));
	if(PcmBitDepthY > BitDepthY) minorErrors.push_back(StringFormatter::formatString("[SPS] PcmBitDepthY value (%ld) not in valid range (0..{})", PcmBitDepthY, BitDepthY));
	if(PcmBitDepthC > BitDepthC) minorErrors.push_back(StringFormatter::formatString("[SPS] PcmBitDepthC value (%ld) not in valid range (0..{})", PcmBitDepthC, BitDepthC));
	if(pcm_enabled_flag){
		if(Log2MinIpcmCbSizeY < std::min(MinCbLog2SizeY, 5u) || Log2MinIpcmCbSizeY > std::min(CtbLog2SizeY, 5u)) minorErrors.push_back(StringFormatter::formatString("[SPS] Log2MinIpcmCbSizeY value (%ld) not in valid range ({}..{})", Log2MinIpcmCbSizeY, std::min(MinCbLog2SizeY, 5u), std::min(CtbLog2SizeY, 5u)));
		if(Log2MaxIpcmCbSizeY > std::min(CtbLog2SizeY, 5u)) minorErrors.push_back(StringFormatter::formatString("[SPS] Log2MaxIpcmCbSizeY value (%ld) not in valid range (0..{})", Log2MaxIpcmCbSizeY, std::min(CtbLog2SizeY, 5u)));
	}
	if(num_short_term_ref_pic_sets > 64) minorErrors.push_back(StringFormatter::formatString("[SPS] num_short_term_ref_pic_sets value (%ld) not in valid range (0..64)", num_short_term_ref_pic_sets));
	if(num_long_term_ref_pics_sps > 32) minorErrors.push_back(StringFormatter::formatString("[SPS] num_long_term_ref_pics_sps value (%ld) not in valid range (0..32)", num_long_term_ref_pics_sps));
}

uint16_t H265SPS::computeMaxFrameNumber() const{
	if(log2_max_pic_order_cnt_lsb_minus4 >= 12) return UINT16_MAX;
	return 1 << (log2_max_pic_order_cnt_lsb_minus4+4);
}
