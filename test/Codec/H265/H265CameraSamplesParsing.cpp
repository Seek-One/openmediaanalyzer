#include "Codec/H265/H265AccessUnit.h"

#include "H265CameraSamplesParsing.h"

const static char g_start_code[4] = {0x00, 0x00, 0x00, 0x01};

H265CameraSamplesParsing::H265CameraSamplesParsing(const char* szDirTestFile)
{
	m_szDirTestFile = szDirTestFile;
}

void H265CameraSamplesParsing::loadStream(const QString& szDirName, H265Stream& stream){
	QDir dirFrame = QDir(QString("%0/stream-samples/%1").arg(m_szDirTestFile, szDirName));

	QStringList listFrame = dirFrame.entryList(QDir::Files, QDir::Name);
	QByteArray bitstream;
	for (int i = 0; i < listFrame.size(); ++i){
		const QString& szFileFrame = listFrame[i];

		QByteArray data = loadFrame(dirFrame, szFileFrame);
		QVERIFY(data.size() > 0);
		bitstream.append(data);
	}

	QVERIFY(stream.parsePacket((uint8_t*)bitstream.data(), bitstream.size()));
}

void H265CameraSamplesParsing::test_h265DahuaBitstream()
{
	H265Stream stream;
	loadStream("h265-dahua", stream);	
	std::vector<H265AccessUnit*> pAccessUnits = stream.getAccessUnits();

	QVERIFY(pAccessUnits.size() == 100);
	QVERIFY(stream.getGOPs().size() == 2);

	H265VPS* pVPS = H265VPS::VPSMap[0];
	QVERIFY(pVPS != nullptr);	
	QVERIFY(pVPS->forbidden_zero_bit == 0);
	QVERIFY(pVPS->nal_unit_type == 32);
	QVERIFY(pVPS->nuh_layer_id == 0);
	QVERIFY(pVPS->nuh_temporal_id_plus1 == 1);
	QVERIFY(pVPS->vps_video_parameter_set_id == 0);
	QVERIFY(pVPS->vps_base_layer_internal_flag == 1);
	QVERIFY(pVPS->vps_base_layer_available_flag == 1);
	QVERIFY(pVPS->vps_max_layers_minus1 == 0);
	QVERIFY(pVPS->vps_max_sub_layers_minus1 == 0);
	QVERIFY(pVPS->vps_temporal_id_nesting_flag == 1);
	QVERIFY(pVPS->profile_tier_level.general_profile_space == 0);
	QVERIFY(pVPS->profile_tier_level.general_tier_flag == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_idc == 1);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[0] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[1] == 1);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[2] == 1);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[3] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[4] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[5] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[6] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[7] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[8] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[9] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[10] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[11] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[12] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[13] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[14] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[15] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[16] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[17] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[18] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[19] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[20] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[21] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[22] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[23] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[24] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[25] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[26] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[27] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[28] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[29] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[30] == 0);
	QVERIFY(pVPS->profile_tier_level.general_profile_compatibility_flag[31] == 0);
	QVERIFY(pVPS->profile_tier_level.general_progressive_source_flag == 1);
	QVERIFY(pVPS->profile_tier_level.general_interlaced_source_flag == 0);
	QVERIFY(pVPS->profile_tier_level.general_non_packed_constraint_flag == 0);
	QVERIFY(pVPS->profile_tier_level.general_frame_only_constraint_flag == 0);
	QVERIFY(pVPS->profile_tier_level.general_one_picture_only_constraint_flag == 0);
	QVERIFY(pVPS->profile_tier_level.general_inbld_flag == 0);
	QVERIFY(pVPS->profile_tier_level.general_level_idc == 120);
	QVERIFY(pVPS->vps_sub_layer_ordering_info_present_flag == 1);
	QVERIFY(pVPS->vps_max_dec_pic_buffering_minus1[0] == 1);
	QVERIFY(pVPS->vps_max_num_reorder_pics[0] == 0);
	QVERIFY(pVPS->vps_max_latency_increase_plus1[0] == 0);
	QVERIFY(pVPS->vps_max_layer_id == 0);
	QVERIFY(pVPS->vps_num_layer_sets_minus1 == 0);
	QVERIFY(pVPS->vps_timing_info_present_flag == 0);
	QVERIFY(pVPS->vps_extension_flag == 0);

	H265SPS* pSPS = H265SPS::SPSMap[0];
	QVERIFY(pSPS != nullptr);
	QVERIFY(pSPS->forbidden_zero_bit == 0);
	QVERIFY(pSPS->nal_unit_type == 33);
	QVERIFY(pSPS->nuh_layer_id == 0);
	QVERIFY(pSPS->nuh_temporal_id_plus1 == 1);
	QVERIFY(pSPS->sps_video_parameter_set_id == 0);
	QVERIFY(pSPS->sps_max_sub_layers_minus1 == 0);
	QVERIFY(pSPS->sps_temporal_id_nesting_flag == 1);
	QVERIFY(pSPS->profile_tier_level.general_profile_space == 0);
	QVERIFY(pSPS->profile_tier_level.general_tier_flag == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_idc == 1);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[0] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[1] == 1);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[2] == 1);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[3] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[4] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[5] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[6] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[7] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[8] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[9] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[10] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[11] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[12] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[13] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[14] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[15] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[16] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[17] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[18] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[19] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[20] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[21] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[22] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[23] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[24] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[25] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[26] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[27] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[28] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[29] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[30] == 0);
	QVERIFY(pSPS->profile_tier_level.general_profile_compatibility_flag[31] == 0);
	QVERIFY(pSPS->profile_tier_level.general_progressive_source_flag == 1);
	QVERIFY(pSPS->profile_tier_level.general_interlaced_source_flag == 0);
	QVERIFY(pSPS->profile_tier_level.general_non_packed_constraint_flag == 0);
	QVERIFY(pSPS->profile_tier_level.general_frame_only_constraint_flag == 0);
	QVERIFY(pSPS->profile_tier_level.general_one_picture_only_constraint_flag == 0);
	QVERIFY(pSPS->profile_tier_level.general_inbld_flag == 0);
	QVERIFY(pSPS->profile_tier_level.general_level_idc == 120);
	QVERIFY(pSPS->sps_seq_parameter_set_id == 0);
	QVERIFY(pSPS->chroma_format_idc == 1);
	QVERIFY(pSPS->pic_width_in_luma_samples == 1920);
	QVERIFY(pSPS->pic_height_in_luma_samples == 1080);
	QVERIFY(pSPS->conformance_window_flag == 1);
	QVERIFY(pSPS->conf_win_left_offset == 0);
	QVERIFY(pSPS->conf_win_right_offset == 0);
	QVERIFY(pSPS->conf_win_top_offset == 0);
	QVERIFY(pSPS->conf_win_bottom_offset == 0);
	QVERIFY(pSPS->bit_depth_luma_minus8 == 0);
	QVERIFY(pSPS->bit_depth_chroma_minus8 == 0);
	QVERIFY(pSPS->log2_max_pic_order_cnt_lsb_minus4 == 12);
	QVERIFY(pSPS->sps_sub_layer_ordering_info_present_flag == 1);
	QVERIFY(pSPS->sps_max_dec_pic_buffering_minus1[0] == 1);
	QVERIFY(pSPS->sps_max_num_reorder_pics[0] == 0);
	QVERIFY(pSPS->sps_max_latency_increase_plus1[0] == 0);
	QVERIFY(pSPS->log2_min_luma_coding_block_size_minus3 == 0);
	QVERIFY(pSPS->log2_diff_max_min_luma_coding_block_size == 2);
	QVERIFY(pSPS->log2_min_luma_transform_block_size_minus2 == 0);
	QVERIFY(pSPS->log2_diff_max_min_luma_transform_block_size == 2);
	QVERIFY(pSPS->max_transform_hierarchy_depth_inter == 1);
	QVERIFY(pSPS->max_transform_hierarchy_depth_intra == 0);
	QVERIFY(pSPS->scaling_list_enabled_flag == 0);
	QVERIFY(pSPS->amp_enabled_flag == 0);
	QVERIFY(pSPS->sample_adaptive_offset_enabled_flag == 1);
	QVERIFY(pSPS->pcm_enabled_flag == 1);
	QVERIFY(pSPS->pcm_sample_bit_depth_luma_minus1 == 7);
	QVERIFY(pSPS->pcm_sample_bit_depth_chroma_minus1 == 7);
	QVERIFY(pSPS->log2_min_pcm_luma_coding_block_size_minus3 == 2);
	QVERIFY(pSPS->log2_diff_max_min_pcm_luma_coding_block_size == 0);
	QVERIFY(pSPS->pcm_loop_filter_disabled_flag == 0);
	QVERIFY(pSPS->num_short_term_ref_pic_sets == 1);
	QVERIFY(pSPS->short_term_ref_pic_set[0].num_negative_pics == 1);
	QVERIFY(pSPS->short_term_ref_pic_set[0].num_positive_pics == 0);
	QVERIFY(pSPS->short_term_ref_pic_set[0].delta_poc_s0_minus1[0] == 0);
	QVERIFY(pSPS->short_term_ref_pic_set[0].used_by_curr_pic_s0_flag[0] == 1);
	QVERIFY(pSPS->long_term_ref_pics_present_flag == 0);
	QVERIFY(pSPS->sps_temporal_mvp_enabled_flag == 1);
	QVERIFY(pSPS->strong_intra_smoothing_enabled_flag == 0);
	QVERIFY(pSPS->vui_parameters_present_flag == 1);
	QVERIFY(pSPS->vui_parameters.aspect_ratio_info_present_flag == 1);
	QVERIFY(pSPS->vui_parameters.aspect_ratio_idc == 1);
	QVERIFY(pSPS->vui_parameters.overscan_info_present_flag == 0);
	QVERIFY(pSPS->vui_parameters.video_signal_type_present_flag == 1);
	QVERIFY(pSPS->vui_parameters.video_format == 5);
	QVERIFY(pSPS->vui_parameters.video_full_range_flag == 1);
	QVERIFY(pSPS->vui_parameters.colour_description_present_flag == 1);
	QVERIFY(pSPS->vui_parameters.colour_primaries == 1);
	QVERIFY(pSPS->vui_parameters.transfer_characteristics == 1);
	QVERIFY(pSPS->vui_parameters.matrix_coeffs == 1);
	QVERIFY(pSPS->vui_parameters.chroma_loc_info_present_flag == 0);
	QVERIFY(pSPS->vui_parameters.neutral_chroma_indication_flag == 0);
	QVERIFY(pSPS->vui_parameters.field_seq_flag == 0);
	QVERIFY(pSPS->vui_parameters.frame_field_info_present_flag == 0);
	QVERIFY(pSPS->vui_parameters.default_display_window_flag == 0);
	QVERIFY(pSPS->vui_parameters.vui_timing_info_present_flag == 1);
	QVERIFY(pSPS->vui_parameters.vui_num_units_in_tick == 1000);
	QVERIFY(pSPS->vui_parameters.vui_time_scale == 25000);
	QVERIFY(pSPS->vui_parameters.vui_poc_proportional_to_timing_flag == 0);
	QVERIFY(pSPS->vui_parameters.vui_hrd_parameters_present_flag == 1);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.nal_hrd_parameters_present_flag == 1);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.vcl_hrd_parameters_present_flag == 1);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.sub_pic_hrd_params_present_flag == 0);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.bit_rate_scale == 4);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.cpb_size_scale == 3);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.initial_cpb_removal_delay_length_minus1 == 23);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.au_cpb_removal_delay_length_minus1 == 23);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.dpb_output_delay_length_minus1 == 5);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.fixed_pic_rate_general_flag[0] == 0);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.fixed_pic_rate_within_cvs_flag[0] == 0);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.low_delay_hrd_flag[0] == 0);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.cpb_cnt_minus1[0] == 0);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.nal_sub_layer_hrd_parameters[0].bit_rate_value_minus1[0] == 1999);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.nal_sub_layer_hrd_parameters[0].cpb_size_value_minus1[0] == 17999);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.nal_sub_layer_hrd_parameters[0].cbr_flag[0] == 0);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.vcl_sub_layer_hrd_parameters[0].bit_rate_value_minus1[0] == 1999);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.vcl_sub_layer_hrd_parameters[0].cpb_size_value_minus1[0] == 17999);
	QVERIFY(pSPS->vui_parameters.hrd_parameters.vcl_sub_layer_hrd_parameters[0].cbr_flag[0] == 0);
	QVERIFY(pSPS->vui_parameters.bitstream_restriction_flag == 0);
	QVERIFY(pSPS->sps_extension_flag == 0);

	H265PPS* pPPS = H265PPS::PPSMap[0];
	QVERIFY(pPPS != nullptr);
	QVERIFY(pPPS->forbidden_zero_bit == 0);
	QVERIFY(pPPS->nal_unit_type == 34);
	QVERIFY(pPPS->nuh_layer_id == 0);
	QVERIFY(pPPS->nuh_temporal_id_plus1 == 1);
	QVERIFY(pPPS->pps_pic_parameter_set_id == 0);
	QVERIFY(pPPS->pps_seq_parameter_set_id == 0);
	QVERIFY(pPPS->dependent_slice_segments_enabled_flag == 0);
	QVERIFY(pPPS->output_flag_present_flag == 0);
	QVERIFY(pPPS->num_extra_slice_header_bits == 0);
	QVERIFY(pPPS->sign_data_hiding_enabled_flag == 1);
	QVERIFY(pPPS->cabac_init_present_flag == 0);
	QVERIFY(pPPS->num_ref_idx_l0_default_active_minus1 == 0);
	QVERIFY(pPPS->num_ref_idx_l1_default_active_minus1 == 0);
	QVERIFY(pPPS->init_qp_minus26 == 0);
	QVERIFY(pPPS->constrained_intra_pred_flag == 0);
	QVERIFY(pPPS->transform_skip_enabled_flag == 0);
	QVERIFY(pPPS->cu_qp_delta_enabled_flag == 1);
	QVERIFY(pPPS->diff_cu_qp_delta_depth == 1);
	QVERIFY(pPPS->pps_cb_qp_offset == 0);
	QVERIFY(pPPS->pps_cr_qp_offset == 0);
	QVERIFY(pPPS->pps_slice_chroma_qp_offsets_present_flag == 0);
	QVERIFY(pPPS->weighted_pred_flag == 0);
	QVERIFY(pPPS->weighted_bipred_flag == 0);
	QVERIFY(pPPS->transquant_bypass_enabled_flag == 0);
	QVERIFY(pPPS->tiles_enabled_flag == 1);
	QVERIFY(pPPS->entropy_coding_sync_enabled_flag == 0);
	QVERIFY(pPPS->num_tile_columns_minus1 == 2);
	QVERIFY(pPPS->num_tile_rows_minus1 == 0);
	QVERIFY(pPPS->uniform_spacing_flag == 0);
	QVERIFY(pPPS->column_width_minus1[0] == 19);
	QVERIFY(pPPS->column_width_minus1[1] == 19);
	QVERIFY(pPPS->loop_filter_across_tiles_enabled_flag == 1);
	QVERIFY(pPPS->pps_loop_filter_across_slices_enabled_flag == 1);
	QVERIFY(pPPS->deblocking_filter_control_present_flag == 0);
	QVERIFY(pPPS->pps_scaling_list_data_present_flag == 0);
	QVERIFY(pPPS->lists_modification_present_flag == 0);
	QVERIFY(pPPS->log2_parallel_merge_level_minus2 == 0);
	QVERIFY(pPPS->slice_segment_header_extension_present_flag == 0);
	QVERIFY(pPPS->pps_extension_present_flag == 0);

	H265Slice* pIDRSlice = pAccessUnits.front()->slice();
	QVERIFY(pIDRSlice != nullptr);
	QVERIFY(pIDRSlice->forbidden_zero_bit == 0);
	QVERIFY(pIDRSlice->nal_unit_type == 19);
	QVERIFY(pIDRSlice->nuh_layer_id == 0);
	QVERIFY(pIDRSlice->nuh_temporal_id_plus1 == 1);
	QVERIFY(pIDRSlice->first_slice_segment_in_pic_flag == 1);
	QVERIFY(pIDRSlice->no_output_of_prior_pics_flag == 0);
	QVERIFY(pIDRSlice->slice_pic_parameter_set_id == 0);
	QVERIFY(pIDRSlice->slice_type == 2);
	QVERIFY(pIDRSlice->slice_sao_luma_flag == 1);
	QVERIFY(pIDRSlice->slice_sao_chroma_flag == 1);
	QVERIFY(pIDRSlice->slice_qp_delta == 7);
	QVERIFY(pIDRSlice->slice_loop_filter_across_slices_enabled_flag == 1);
	QVERIFY(pIDRSlice->num_entry_point_offsets == 0);

	H265Slice* pTrailingPicSlice = pAccessUnits[1]->slice();
	QVERIFY(pTrailingPicSlice != nullptr);
	QVERIFY(pTrailingPicSlice->forbidden_zero_bit == 0);
	QVERIFY(pTrailingPicSlice->nal_unit_type == 1);
	QVERIFY(pTrailingPicSlice->nuh_layer_id == 0);
	QVERIFY(pTrailingPicSlice->nuh_temporal_id_plus1 == 1);
	QVERIFY(pTrailingPicSlice->first_slice_segment_in_pic_flag == 1);
	QVERIFY(pTrailingPicSlice->slice_pic_parameter_set_id == 0);
	QVERIFY(pTrailingPicSlice->slice_type == 1);
	QVERIFY(pTrailingPicSlice->slice_pic_order_cnt_lsb == 1);
	QVERIFY(pTrailingPicSlice->short_term_ref_pic_set_sps_flag == 1);
	QVERIFY(pTrailingPicSlice->slice_temporal_mvp_enabled_flag == 1);
	QVERIFY(pTrailingPicSlice->slice_sao_luma_flag == 1);
	QVERIFY(pTrailingPicSlice->slice_sao_chroma_flag == 1);
	QVERIFY(pTrailingPicSlice->num_ref_idx_active_override_flag == 0);
	QVERIFY(pTrailingPicSlice->five_minus_max_num_merge_cand == 0);
	QVERIFY(pTrailingPicSlice->slice_qp_delta == 7);
	QVERIFY(pTrailingPicSlice->slice_loop_filter_across_slices_enabled_flag == 1);
	QVERIFY(pTrailingPicSlice->num_entry_point_offsets == 0);
}

QByteArray H265CameraSamplesParsing::loadFrame(const QDir& dirFrame, const QString& szFilename)
{
	QByteArray data;
	QFile fileFrame (dirFrame.filePath(szFilename));

	if (fileFrame.open(QFile::ReadOnly)) {
		data = fileFrame.readAll();
		data.prepend(g_start_code, 4);
		fileFrame.close();
	}

	return data;
}
