#include <iostream>
#include <cmath>

#include "../H26X/H26XMath.h"
#include "H265BitstreamReader.h"

H265BitstreamReader::H265BitstreamReader(const uint8_t* pNALData, uint32_t iNALLength)
	: H26XBitstreamReader(pNALData, iNALLength)
{

}

void H265BitstreamReader::readNALHeader(H265NAL& h265NAL)
{
	h265NAL.forbidden_zero_bit = readBits(1);
	h265NAL.nal_unit_type = (H265NAL::UnitType)readBits(6);
	h265NAL.nuh_layer_id = readBits(6);
	h265NAL.nuh_temporal_id_plus1 = readBits(3);
}

void H265BitstreamReader::readVPS(H265VPS& h265VPS)
{
	h265VPS.vps_video_parameter_set_id = readBits(4);
	h265VPS.vps_base_layer_internal_flag = readBits(1);
	h265VPS.vps_base_layer_available_flag = readBits(1);
	h265VPS.vps_max_layers_minus1 = readBits(6);
	h265VPS.vps_max_sub_layers_minus1 = readBits(3);
	h265VPS.vps_temporal_id_nesting_flag = readBits(1);
	// vps_reserved_0xffff_16bits
	readBits(16);
	h265VPS.profile_tier_level = readProfileTierLevel(1, h265VPS.vps_max_sub_layers_minus1);
	h265VPS.vps_sub_layer_ordering_info_present_flag = readBits(1);
	h265VPS.vps_max_dec_pic_buffering_minus1.resize(h265VPS.vps_max_sub_layers_minus1 + 1);
	h265VPS.vps_max_num_reorder_pics.resize(h265VPS.vps_max_sub_layers_minus1 + 1);
	h265VPS.vps_max_latency_increase_plus1.resize(h265VPS.vps_max_sub_layers_minus1 + 1);
	for (int i = (h265VPS.vps_sub_layer_ordering_info_present_flag ? 0 : h265VPS.vps_max_sub_layers_minus1); i <= h265VPS.vps_max_sub_layers_minus1; ++i) {
		h265VPS.vps_max_dec_pic_buffering_minus1[i] = readGolombUE();
		h265VPS.vps_max_num_reorder_pics[i] = readGolombUE();
		h265VPS.vps_max_latency_increase_plus1[i] = readGolombUE();
	}
	h265VPS.vps_max_layer_id = readBits(6);
	h265VPS.vps_num_layer_sets_minus1 = readGolombUE();
	h265VPS.layer_id_included_flag.resize(h265VPS.vps_num_layer_sets_minus1 + 1);
	for (uint32_t i = 1; i <= h265VPS.vps_num_layer_sets_minus1; ++i) {
		h265VPS.layer_id_included_flag[i].resize(h265VPS.vps_max_layer_id + 1);
		for (uint32_t j = 0; j <= h265VPS.vps_max_layer_id; ++j) {
			h265VPS.layer_id_included_flag[i][j] = readBits(1);
		}
	}
	h265VPS.vps_timing_info_present_flag = readBits(1);
	if (h265VPS.vps_timing_info_present_flag) {
		h265VPS.vps_num_units_in_tick = readBits(32);
		h265VPS.vps_time_scale = readBits(32);
		h265VPS.vps_poc_proportional_to_timing_flag = readBits(1);
		if (h265VPS.vps_poc_proportional_to_timing_flag) {
			h265VPS.vps_num_ticks_poc_diff_one_minus1 = readGolombUE();
		}
		h265VPS.vps_num_hrd_parameters = readGolombUE();
		h265VPS.hrd_layer_set_idx.resize(h265VPS.vps_num_hrd_parameters);
		h265VPS.cprms_present_flag.resize(h265VPS.vps_num_hrd_parameters);
		for (uint32_t i = 0; i < h265VPS.vps_num_hrd_parameters; ++i) {
			h265VPS.hrd_layer_set_idx[i] = readGolombUE();
			if (i > 0) {
				h265VPS.cprms_present_flag[i] = readBits(1);
			} else {
				h265VPS.cprms_present_flag[i] = 0;
			}
			// TODO E.2.2
			// hrd_parameters( cprms_present_flag[ i ], vps_max_sub_layers_minus1 )
		}
	}

	// TODO
	// vps_extension_flag
	// if( vps_extension_flag )
	// 	while( more_rbsp_data( ) ) u(1)
	// 		vps_extension_data_flag
	// rbsp_trailing_bits( )
}

void H265BitstreamReader::readSPS(H265SPS& h265SPS)
{
	h265SPS.sps_video_parameter_set_id = readBits(4);
	h265SPS.sps_max_sub_layers_minus1 = readBits(3);
	h265SPS.sps_temporal_id_nesting_flag = readBits(1);
	h265SPS.profile_tier_level = readProfileTierLevel(1, h265SPS.sps_max_sub_layers_minus1);
	h265SPS.sps_seq_parameter_set_id = readGolombUE();
	h265SPS.chroma_format_idc = readGolombUE();
	if (h265SPS.chroma_format_idc == 3) {
		h265SPS.separate_colour_plane_flag = readBits(1);
	}

	if (h265SPS.chroma_format_idc == 1 && h265SPS.separate_colour_plane_flag == 0) {
		h265SPS.SubWidthC = 2;
		h265SPS.SubHeightC = 2;
	} else if (h265SPS.chroma_format_idc == 2 && h265SPS.separate_colour_plane_flag == 0) {
		h265SPS.SubWidthC = 2;
		h265SPS.SubHeightC = 1;
	} else {
		h265SPS.SubWidthC = 1;
		h265SPS.SubHeightC = 1;
	}

	h265SPS.pic_width_in_luma_samples = readGolombUE();
	h265SPS.pic_height_in_luma_samples = readGolombUE();
	h265SPS.conformance_window_flag = readBits(1);
	if (h265SPS.conformance_window_flag) {
		h265SPS.conf_win_left_offset = readGolombUE();
		h265SPS.conf_win_right_offset = readGolombUE();
		h265SPS.conf_win_top_offset = readGolombUE();
		h265SPS.conf_win_bottom_offset = readGolombUE();
	}
	h265SPS.bit_depth_luma_minus8 = readGolombUE();
	h265SPS.bit_depth_chroma_minus8 = readGolombUE();
	h265SPS.log2_max_pic_order_cnt_lsb_minus4 = readGolombUE();
	h265SPS.sps_sub_layer_ordering_info_present_flag = readBits(1);
	h265SPS.sps_max_dec_pic_buffering_minus1.resize(h265SPS.sps_max_sub_layers_minus1 + 1);
	h265SPS.sps_max_num_reorder_pics.resize(h265SPS.sps_max_sub_layers_minus1 + 1);
	h265SPS.sps_max_latency_increase_plus1.resize(h265SPS.sps_max_sub_layers_minus1 + 1);
	for (uint8_t i = (h265SPS.sps_sub_layer_ordering_info_present_flag ? 0 : h265SPS.sps_max_sub_layers_minus1); i <= h265SPS.sps_max_sub_layers_minus1; ++i) {
		h265SPS.sps_max_dec_pic_buffering_minus1[i] = readGolombUE();
		h265SPS.sps_max_num_reorder_pics[i] = readGolombUE();
		h265SPS.sps_max_latency_increase_plus1[i] = readGolombUE();
	}
	h265SPS.log2_min_luma_coding_block_size_minus3 = readGolombUE();
	h265SPS.log2_diff_max_min_luma_coding_block_size = readGolombUE();
	h265SPS.log2_min_luma_transform_block_size_minus2 = readGolombUE();
	h265SPS.log2_diff_max_min_luma_transform_block_size = readGolombUE();
	h265SPS.max_transform_hierarchy_depth_inter = readGolombUE();
	h265SPS.max_transform_hierarchy_depth_intra = readGolombUE();
	h265SPS.scaling_list_enabled_flag = readBits(1);
	if (h265SPS.scaling_list_enabled_flag) {
		h265SPS.sps_scaling_list_data_present_flag = readBits(1);
		if (h265SPS.sps_scaling_list_data_present_flag) {
			h265SPS.scaling_list_data = readScalingList();
		}
	}
	h265SPS.amp_enabled_flag = readBits(1);
	h265SPS.sample_adaptive_offset_enabled_flag = readBits(1);
	h265SPS.pcm_enabled_flag = readBits(1);
	if (h265SPS.pcm_enabled_flag) {
		h265SPS.pcm_sample_bit_depth_luma_minus1 = readBits(4);
		h265SPS.pcm_sample_bit_depth_chroma_minus1 = readBits(4);
		h265SPS.log2_min_pcm_luma_coding_block_size_minus3 = readGolombUE();
		h265SPS.log2_diff_max_min_pcm_luma_coding_block_size = readGolombUE();
		h265SPS.pcm_loop_filter_disabled_flag = readBits(1);
	}
	h265SPS.num_short_term_ref_pic_sets = readGolombUE();
	h265SPS.short_term_ref_pic_set.resize(h265SPS.num_short_term_ref_pic_sets + 1);
	for (uint32_t i = 0; i < h265SPS.num_short_term_ref_pic_sets; ++i) {
		h265SPS.short_term_ref_pic_set[i] = readShortTermRefPicSet(i, h265SPS);
	}
	h265SPS.long_term_ref_pics_present_flag = readBits(1);
	if (h265SPS.long_term_ref_pics_present_flag) {
		h265SPS.num_long_term_ref_pics_sps = readGolombUE();
		h265SPS.lt_ref_pic_poc_lsb_sps.resize(h265SPS.num_long_term_ref_pics_sps);
		h265SPS.used_by_curr_pic_lt_sps_flag.resize(h265SPS.num_long_term_ref_pics_sps);
		for (uint32_t i = 0; i < h265SPS.num_long_term_ref_pics_sps; ++i) {
			h265SPS.lt_ref_pic_poc_lsb_sps[i] = readBits(h265SPS.log2_max_pic_order_cnt_lsb_minus4 + 4);
			h265SPS.used_by_curr_pic_lt_sps_flag[i] = readBits(1);
		}
	}
	h265SPS.sps_temporal_mvp_enabled_flag = readBits(1);
	h265SPS.strong_intra_smoothing_enabled_flag = readBits(1);

	// TODO: handle vui and SPS extension

	// Compute the slice segment address length (cf eq 7.10 -> 7.22)
	h265SPS.MinCbLog2SizeY = h265SPS.log2_min_luma_coding_block_size_minus3 + 3;
	h265SPS.CtbLog2SizeY = h265SPS.MinCbLog2SizeY + h265SPS.log2_diff_max_min_luma_coding_block_size;
	h265SPS.MinCbSizeY = 1 << h265SPS.MinCbLog2SizeY;
	h265SPS.CtbSizeY = 1 << h265SPS.CtbLog2SizeY;
	h265SPS.PicWidthInMinCbsY = h265SPS.pic_width_in_luma_samples / h265SPS.MinCbSizeY;
	h265SPS.PicWidthInCtbsY = H26XMath::ceil(h265SPS.pic_width_in_luma_samples, h265SPS.CtbSizeY);
	h265SPS.PicHeightInMinCbsY = h265SPS.pic_height_in_luma_samples / h265SPS.MinCbSizeY;
	h265SPS.PicHeightInCtbsY = H26XMath::ceil(h265SPS.pic_height_in_luma_samples, h265SPS.CtbSizeY);
	h265SPS.PicSizeInMinCbsY = h265SPS.PicWidthInMinCbsY * h265SPS.PicHeightInMinCbsY;
	h265SPS.PicSizeInCtbsY = h265SPS.PicWidthInCtbsY * h265SPS.PicHeightInCtbsY;
	h265SPS.PicSizeInSamplesY = h265SPS.pic_width_in_luma_samples * h265SPS.pic_height_in_luma_samples;

}

void H265BitstreamReader::readPPS(H265PPS& h265PPS)
{
	h265PPS.pps_pic_parameter_set_id = readGolombUE();
	h265PPS.pps_seq_parameter_set_id = readGolombUE();
	h265PPS.dependent_slice_segments_enabled_flag = readBits(1);
	h265PPS.output_flag_present_flag = readBits(1);
	h265PPS.num_extra_slice_header_bits = readBits(3);
	h265PPS.sign_data_hiding_enabled_flag = readBits(1);
	h265PPS.cabac_init_present_flag = readBits(1);
	h265PPS.num_ref_idx_l0_default_active_minus1 = readGolombUE();
	h265PPS.num_ref_idx_l1_default_active_minus1 = readGolombUE();
	h265PPS.init_qp_minus26 = readGolombSE();
	h265PPS.constrained_intra_pred_flag = readBits(1);
	h265PPS.transform_skip_enabled_flag = readBits(1);
	h265PPS.cu_qp_delta_enabled_flag = readBits(1);
	if (h265PPS.cu_qp_delta_enabled_flag) {
		h265PPS.diff_cu_qp_delta_depth = readGolombUE();
	}
	h265PPS.pps_cb_qp_offset = readGolombSE();
	h265PPS.pps_cr_qp_offset = readGolombSE();
	h265PPS.pps_slice_chroma_qp_offsets_present_flag = readBits(1);
	h265PPS.weighted_pred_flag = readBits(1);
	h265PPS.weighted_bipred_flag = readBits(1);
	h265PPS.transquant_bypass_enabled_flag = readBits(1);
	h265PPS.tiles_enabled_flag = readBits(1);
	h265PPS.entropy_coding_sync_enabled_flag = readBits(1);
	if (h265PPS.tiles_enabled_flag) {
		h265PPS.num_tile_columns_minus1 = readGolombUE();
		h265PPS.num_tile_rows_minus1 = readGolombUE();
		h265PPS.uniform_spacing_flag = readBits(1);
		if (!h265PPS.uniform_spacing_flag) {
			h265PPS.column_width_minus1.resize(h265PPS.num_tile_columns_minus1);
			for (uint32_t i = 0; i < h265PPS.num_tile_columns_minus1; ++i) {
				h265PPS.column_width_minus1[i] = readGolombUE();
			}
			h265PPS.row_height_minus1.resize(h265PPS.num_tile_rows_minus1);
			for (uint32_t i = 0; i < h265PPS.num_tile_rows_minus1; ++i) {
				h265PPS.row_height_minus1[i] = readGolombUE();;
			}
		}
		h265PPS.loop_filter_across_tiles_enabled_flag = readBits(1);
	}
	h265PPS.pps_loop_filter_across_slices_enabled_flag = readBits(1);
	h265PPS.deblocking_filter_control_present_flag = readBits(1);
	if (h265PPS.deblocking_filter_control_present_flag) {
		h265PPS.deblocking_filter_override_enabled_flag = readBits(1);
		h265PPS.pps_deblocking_filter_disabled_flag = readBits(1);
		if (!h265PPS.pps_deblocking_filter_disabled_flag) {
			h265PPS.pps_beta_offset_div2 = readGolombSE();
			h265PPS.pps_tc_offset_div2 = readGolombSE();
		}
	}

	h265PPS.pps_scaling_list_data_present_flag = readBits(1);
	if (h265PPS.pps_scaling_list_data_present_flag) {
		h265PPS.scaling_list_data = readScalingList();
	}
	h265PPS.lists_modification_present_flag = readBits(1);
	h265PPS.log2_parallel_merge_level_minus2 = readGolombUE();
	h265PPS.slice_segment_header_extension_present_flag = readBits(1);

	h265PPS.pps_extension_present_flag = readBits(1);
	if(h265PPS.pps_extension_present_flag){
		// TODO: handle pps extensions
		std::cerr << "[H265] pps extenstion not handled\n";
	}
}

void H265BitstreamReader::readSlice(H265Slice& h265Slice, const H265NAL& h265NAL, H265SPS& h265SPS, const H265PPS& h265PPS)
{
	uint8_t ChromaArrayType = (h265SPS.separate_colour_plane_flag == 0 ? h265SPS.chroma_format_idc : 0);

	h265Slice.first_slice_segment_in_pic_flag = readBits(1);
	if (h265NAL.nal_unit_type >= H265NAL::UnitType_BLA_W_LP && h265NAL.nal_unit_type <= H265NAL::UnitType_IRAP_VCL23) {
		h265Slice.no_output_of_prior_pics_flag = readBits(1);
	}
	h265Slice.slice_pic_parameter_set_id = readGolombUE();
	if (!h265Slice.first_slice_segment_in_pic_flag) {
		if (h265PPS.dependent_slice_segments_enabled_flag) {
			h265Slice.dependent_slice_segment_flag = readBits(1);
		}

		// if (h265SPS.chroma_format_idc != 1) {
		// 	TkCore::Logger::debug("[H265::bitstream] Other format than YUV 4:2:0 aren't supported");
		// }
		// uint32_t PicWidthInSamplesC = h265SPS.pic_width_in_luma_samples / 2;
		// uint32_t PicHeightInSamplesC = h265SPS.pic_height_in_luma_samples / 2;

		h265Slice.slice_segment_address = readBits((uint8_t)ceil(log2(h265SPS.PicSizeInCtbsY)));
	}

	if (!h265Slice.dependent_slice_segment_flag) {
		// Skip reserved flags
		for (int i = 0; i < h265PPS.num_extra_slice_header_bits; ++i) {
			readBits(1);
		}
		h265Slice.slice_type = (H265Slice::SliceType)readGolombUE();
		if (h265PPS.output_flag_present_flag) {
			h265Slice.pic_output_flag = readBits(1);
		}
		if (h265SPS.separate_colour_plane_flag == 1) {
			h265Slice.colour_plane_id = readBits(2);
		}

		if (h265NAL.nal_unit_type != H265NAL::UnitType_IDR_W_RADL && h265NAL.nal_unit_type != H265NAL::UnitType_IDR_N_LP) {
			h265Slice.slice_pic_order_cnt_lsb = readBits(h265SPS.log2_max_pic_order_cnt_lsb_minus4 + 4);
			h265Slice.short_term_ref_pic_set_sps_flag = readBits(1);

			uint64_t iStartShortTermBitsCount = m_iRemainingBits;
			if (!h265Slice.short_term_ref_pic_set_sps_flag) {
				h265SPS.short_term_ref_pic_set[h265SPS.num_short_term_ref_pic_sets] = readShortTermRefPicSet(h265SPS.num_short_term_ref_pic_sets, h265SPS);
			} else if (h265SPS.num_short_term_ref_pic_sets > 1)  {
				h265Slice.short_term_ref_pic_set_idx = readBits((uint8_t)ceil(log2(h265SPS.num_short_term_ref_pic_sets)));
			}
			h265Slice.NumShortTermPictureSliceHeaderBits = iStartShortTermBitsCount - m_iRemainingBits;

			uint64_t iStartLongTermBitsCount = m_iRemainingBits;
			if (h265SPS.long_term_ref_pics_present_flag) {
				if (h265SPS.num_long_term_ref_pics_sps > 0) {
					h265Slice.num_long_term_sps = readGolombUE();
				}
				h265Slice.num_long_term_pics = readGolombUE();

				h265Slice.NumLongTerm = h265Slice.num_long_term_sps + h265Slice.num_long_term_pics;
				h265Slice.lt_idx_sps.resize(h265Slice.NumLongTerm);
				h265Slice.poc_lsb_lt.resize(h265Slice.NumLongTerm);
				h265Slice.used_by_curr_pic_lt_flag.resize(h265Slice.NumLongTerm);
				h265Slice.delta_poc_msb_present_flag.resize(h265Slice.NumLongTerm);
				h265Slice.delta_poc_msb_cycle_lt.resize(h265Slice.NumLongTerm);
				h265Slice.DeltaPocMsbCycleLt.resize(h265Slice.NumLongTerm);
				for (unsigned i = 0; i < h265Slice.NumLongTerm; ++i) {
					if (i < h265Slice.num_long_term_sps) {
						if (h265SPS.num_long_term_ref_pics_sps > 1) {
							h265Slice.lt_idx_sps[i] = readBits((uint8_t)ceil(log2(h265SPS.num_long_term_ref_pics_sps)));
						}
					} else {
						h265Slice.poc_lsb_lt[i] = readBits(h265SPS.log2_max_pic_order_cnt_lsb_minus4 + 4);
						h265Slice.used_by_curr_pic_lt_flag[i] = readBits(1);
					}
					h265Slice.delta_poc_msb_present_flag[i] = readBits(1);
					if (h265Slice.delta_poc_msb_present_flag[i]) {
						h265Slice.delta_poc_msb_cycle_lt[i] = readGolombUE();
					}
				}
			}
			h265Slice.NumLongTermPictureSliceHeaderBits = iStartLongTermBitsCount - m_iRemainingBits;

			if (h265SPS.sps_temporal_mvp_enabled_flag) {
				h265Slice.slice_temporal_mvp_enabled_flag = readBits(1);
			}
		}
		if (h265SPS.sample_adaptive_offset_enabled_flag) {
			h265Slice.slice_sao_luma_flag = readBits(1);
			if (ChromaArrayType != 0) {
				h265Slice.slice_sao_chroma_flag = readBits(1);
			}
		}
		if (h265Slice.slice_type == H265Slice::SliceType_P || h265Slice.slice_type == H265Slice::SliceType_B) {
			h265Slice.num_ref_idx_active_override_flag = readBits(1);
			if (h265Slice.num_ref_idx_active_override_flag) {
				h265Slice.num_ref_idx_l0_active_minus1 = readGolombUE();
				if (h265Slice.slice_type == H265Slice::SliceType_B) {
					h265Slice.num_ref_idx_l1_active_minus1 = readGolombUE();
				}
			}else{
				h265Slice.num_ref_idx_l0_active_minus1 = h265PPS.num_ref_idx_l0_default_active_minus1;
				h265Slice.num_ref_idx_l1_active_minus1 = h265PPS.num_ref_idx_l1_default_active_minus1;
			}
			if (h265PPS.lists_modification_present_flag) {
				computeNumPicTotalCurr(h265Slice, h265SPS);
				if (h265Slice.NumPicTotalCurr > 1) {
					readRefPicListsModification(h265Slice);
				}
			}
			if (h265Slice.slice_type == H265Slice::SliceType_B) {
				h265Slice.mvd_l1_zero_flag = readBits(1);
			}
			if (h265PPS.cabac_init_present_flag) {
				h265Slice.cabac_init_flag = readBits(1);
			}
			if (h265Slice.slice_temporal_mvp_enabled_flag) {
				if (h265Slice.slice_type == H265Slice::SliceType_B) {
					h265Slice.collocated_from_l0_flag = readBits(1);
				}
				if ((h265Slice.collocated_from_l0_flag && h265Slice.num_ref_idx_l0_active_minus1 > 0) ||
					(!h265Slice.collocated_from_l0_flag && h265Slice.num_ref_idx_l1_active_minus1 > 0))
				{
					h265Slice.collocated_ref_idx = readGolombUE();
				}
			}
			if ((h265PPS.weighted_pred_flag && h265Slice.slice_type == H265Slice::SliceType_P) ||
				(h265PPS.weighted_bipred_flag && h265Slice.slice_type == H265Slice::SliceType_B))
			{
				std::cerr << "[H265::bitstream] pred_weight_table() not handled\n";
			}
			h265Slice.five_minus_max_num_merge_cand = readGolombUE();
			// No SPS extentions handled
			// if( motion_vector_resolution_control_idc = = 2 )
			// 	use_integer_mv_flag
		}
		h265Slice.slice_qp_delta = readGolombSE();
		if (h265PPS.pps_slice_chroma_qp_offsets_present_flag) {
			h265Slice.slice_cb_qp_offset = readGolombSE();
			h265Slice.slice_cr_qp_offset = readGolombSE();
		}
		if (h265PPS.pps_slice_chroma_qp_offsets_present_flag) {
			h265Slice.slice_act_y_qp_offset = readGolombSE();
			h265Slice.slice_act_cb_qp_offset = readGolombSE();
			h265Slice.slice_act_cr_qp_offset = readGolombSE();
		}
		// // PPS rang not handeled
		// if (h265PPS.chroma_qp_offset_list_enabled_flag) {
		// 	h265Slice.cu_chroma_qp_offset_enabled_flag = readBits(1);
		// }

		if (h265PPS.deblocking_filter_override_enabled_flag) {
			h265Slice.deblocking_filter_override_flag = readBits(1);
		}
		if (h265Slice.deblocking_filter_override_flag) {
			h265Slice.slice_deblocking_filter_disabled_flag = readBits(1);
			if (!h265Slice.slice_deblocking_filter_disabled_flag) {
				h265Slice.slice_beta_offset_div2 = readGolombSE();
				h265Slice.slice_tc_offset_div2 = readGolombSE();
			}
		}
		if (h265PPS.pps_loop_filter_across_slices_enabled_flag &&
			(h265Slice.slice_sao_luma_flag || h265Slice.slice_sao_chroma_flag || !h265Slice.slice_deblocking_filter_disabled_flag))
		{
			h265Slice.slice_loop_filter_across_slices_enabled_flag = readBits(1);
		}
	}
	if (h265PPS.tiles_enabled_flag || h265PPS.entropy_coding_sync_enabled_flag) {
		h265Slice.num_entry_point_offsets = readGolombUE();
		if (h265Slice.num_entry_point_offsets > 0) {
			h265Slice.offset_len_minus1 = readGolombUE();
			h265Slice.entry_point_offset_minus1.resize(h265Slice.num_entry_point_offsets);
			for (unsigned i = 0; i < h265Slice.num_entry_point_offsets; ++i) {
				h265Slice.entry_point_offset_minus1[i] = readBits(h265Slice.offset_len_minus1 + 1);
			}
		}
	}

	// Compute derived variables
	if (h265Slice.short_term_ref_pic_set_sps_flag) {
		h265Slice.CurrRpsIdx = h265Slice.short_term_ref_pic_set_idx;
	} else {
		h265Slice.CurrRpsIdx = h265SPS.num_short_term_ref_pic_sets;
	}

	h265Slice.DeltaPocMsbCycleLt.resize(h265Slice.NumLongTerm);
	for (unsigned i = 0; i < h265Slice.NumLongTerm; ++i) {
		uint32_t delta_poc_msb_cycle_lt = ((int)i < h265Slice.delta_poc_msb_cycle_lt.size()) ? h265Slice.delta_poc_msb_cycle_lt[i] : 0;
		if (i == 0 || i == h265Slice.num_long_term_sps) {
			h265Slice.DeltaPocMsbCycleLt[i] = delta_poc_msb_cycle_lt;
		} else {
			h265Slice.DeltaPocMsbCycleLt[i] = delta_poc_msb_cycle_lt + h265Slice.DeltaPocMsbCycleLt[i - 1];
		}
	}

	computeNumPicTotalCurr(h265Slice, h265SPS);

	// Compute reference picture list
	h265Slice.NumRpsCurrTempList0 = std::max(h265Slice.num_ref_idx_l0_active_minus1 + 1, h265Slice.NumPicTotalCurr);
	h265Slice.NumRpsCurrTempList1 = std::max(h265Slice.num_ref_idx_l1_active_minus1 + 1, h265Slice.NumPicTotalCurr);
}

H265ProfileTierLevel H265BitstreamReader::readProfileTierLevel(uint8_t iProfilePresentFlag, uint8_t iMaxNumSubLayersMinus1)
{
	H265ProfileTierLevel h265Profile;

	if (iProfilePresentFlag) {
		h265Profile.general_profile_space = readBits(2);
		h265Profile.general_tier_flag = readBits(1);
		h265Profile.general_profile_idc = readBits(5);
		for (int i = 0; i < 32; ++i) {
			h265Profile.general_profile_compatibility_flag[i] = readBits(1);
		}
		h265Profile.general_progressive_source_flag = readBits(1);
		h265Profile.general_interlaced_source_flag = readBits(1);
		h265Profile.general_non_packed_constraint_flag = readBits(1);
		h265Profile.general_frame_only_constraint_flag = readBits(1);

		if( h265Profile.general_profile_idc ==  4 || h265Profile.general_profile_compatibility_flag[ 4] ||
			h265Profile.general_profile_idc ==  5 || h265Profile.general_profile_compatibility_flag[ 5] ||
			h265Profile.general_profile_idc ==  6 || h265Profile.general_profile_compatibility_flag[ 6] ||
			h265Profile.general_profile_idc ==  7 || h265Profile.general_profile_compatibility_flag[ 7] ||
			h265Profile.general_profile_idc ==  8 || h265Profile.general_profile_compatibility_flag[ 8] ||
			h265Profile.general_profile_idc ==  9 || h265Profile.general_profile_compatibility_flag[ 9] ||
			h265Profile.general_profile_idc == 10 || h265Profile.general_profile_compatibility_flag[10] ||
			h265Profile.general_profile_idc == 11 || h265Profile.general_profile_compatibility_flag[11]) {
			h265Profile.general_max_12bit_constraint_flag = readBits(1);
			h265Profile.general_max_10bit_constraint_flag = readBits(1);
			h265Profile.general_max_8bit_constraint_flag = readBits(1);
			h265Profile.general_max_422chroma_constraint_flag = readBits(1);
			h265Profile.general_max_420chroma_constraint_flag = readBits(1);
			h265Profile.general_max_monochrome_constraint_flag = readBits(1);
			h265Profile.general_intra_constraint_flag = readBits(1);
			h265Profile.general_one_picture_only_constraint_flag = readBits(1);
			h265Profile.general_lower_bit_rate_constraint_flag = readBits(1);

			if( h265Profile.general_profile_idc ==  5 || h265Profile.general_profile_compatibility_flag[ 5] ||
				h265Profile.general_profile_idc ==  9 || h265Profile.general_profile_compatibility_flag[ 9] ||
				h265Profile.general_profile_idc == 10 || h265Profile.general_profile_compatibility_flag[10] ||
				h265Profile.general_profile_idc == 11 || h265Profile.general_profile_compatibility_flag[11]) {
				h265Profile.general_max_14bit_constraint_flag = readBits(1);
				// general_reserved_zero_33bits
				readBits(33);
			} else {
				// general_reserved_zero_34bits
				readBits(34);
			}
		} else if (h265Profile.general_profile_idc == 2 || h265Profile.general_profile_compatibility_flag[2]) {
			// general_reserved_zero_7bits
			readBits(7);
			h265Profile.general_one_picture_only_constraint_flag = readBits(1);
			// general_reserved_zero_35bits
			readBits(35);
		} else {
			// general_reserved_zero_43bits
			readBits(43);
		}

		if( h265Profile.general_profile_idc ==  1 || h265Profile.general_profile_compatibility_flag[ 1] ||
			h265Profile.general_profile_idc ==  2 || h265Profile.general_profile_compatibility_flag[ 2] ||
			h265Profile.general_profile_idc ==  3 || h265Profile.general_profile_compatibility_flag[ 3] ||
			h265Profile.general_profile_idc ==  4 || h265Profile.general_profile_compatibility_flag[ 4] ||
			h265Profile.general_profile_idc ==  5 || h265Profile.general_profile_compatibility_flag[ 5] ||
			h265Profile.general_profile_idc ==  9 || h265Profile.general_profile_compatibility_flag[ 9] ||
			h265Profile.general_profile_idc == 11 || h265Profile.general_profile_compatibility_flag[11]) {
			h265Profile.general_inbld_flag = readBits(1);
		} else {
			// general_reserved_zero_bit
			readBits(1);
		}
	}

	h265Profile.general_level_idc = readBits(8);
	h265Profile.sub_layer_profile_present_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_level_present_flag.resize(iMaxNumSubLayersMinus1);
	for (uint8_t i = 0; i < iMaxNumSubLayersMinus1; ++i) {
		h265Profile.sub_layer_profile_present_flag[i] = readBits(1);
		h265Profile.sub_layer_level_present_flag[i] = readBits(1);
	}

	if (iMaxNumSubLayersMinus1 > 0) {
		for (int i = iMaxNumSubLayersMinus1; i < 8; ++i) {
			// reserved_zero_2bits
			readBits(2);
		}
	}

	h265Profile.sub_layer_profile_space.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_tier_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_profile_idc.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_profile_compatibility_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_progressive_source_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_interlaced_source_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_non_packed_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_frame_only_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_max_12bit_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_max_10bit_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_max_8bit_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_max_422chroma_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_max_420chroma_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_max_monochrome_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_intra_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_one_picture_only_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_lower_bit_rate_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_max_14bit_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_one_picture_only_constraint_flag.resize(iMaxNumSubLayersMinus1);
	h265Profile.sub_layer_inbld_flag.resize(iMaxNumSubLayersMinus1);

	for (uint8_t i = 0; i < iMaxNumSubLayersMinus1; ++i) {
		if (h265Profile.sub_layer_profile_present_flag[i]) {
			h265Profile.sub_layer_profile_space[i] = readBits(2);
			h265Profile.sub_layer_tier_flag[i] = readBits(1);
			h265Profile.sub_layer_profile_idc[i] = readBits(5);
			h265Profile.sub_layer_profile_compatibility_flag[i].resize(32);
			for (int j = 0; j < 32; ++j) {
				h265Profile.sub_layer_profile_compatibility_flag[i][j] = readBits(1);
			}
			h265Profile.sub_layer_progressive_source_flag[i] = readBits(1);
			h265Profile.sub_layer_interlaced_source_flag[i] = readBits(1);
			h265Profile.sub_layer_non_packed_constraint_flag[i] = readBits(1);
			h265Profile.sub_layer_frame_only_constraint_flag[i] = readBits(1);

			if( h265Profile.sub_layer_profile_idc[i] ==  4 || h265Profile.sub_layer_profile_compatibility_flag[i][ 4] ||
				h265Profile.sub_layer_profile_idc[i] ==  5 || h265Profile.sub_layer_profile_compatibility_flag[i][ 5] ||
				h265Profile.sub_layer_profile_idc[i] ==  6 || h265Profile.sub_layer_profile_compatibility_flag[i][ 6] ||
				h265Profile.sub_layer_profile_idc[i] ==  7 || h265Profile.sub_layer_profile_compatibility_flag[i][ 7] ||
				h265Profile.sub_layer_profile_idc[i] ==  8 || h265Profile.sub_layer_profile_compatibility_flag[i][ 8] ||
				h265Profile.sub_layer_profile_idc[i] ==  9 || h265Profile.sub_layer_profile_compatibility_flag[i][ 9] ||
				h265Profile.sub_layer_profile_idc[i] == 10 || h265Profile.sub_layer_profile_compatibility_flag[i][10] ||
				h265Profile.sub_layer_profile_idc[i] == 11 || h265Profile.sub_layer_profile_compatibility_flag[i][11]) {
				h265Profile.sub_layer_max_12bit_constraint_flag[i] = readBits(1);
				h265Profile.sub_layer_max_10bit_constraint_flag[i] = readBits(1);
				h265Profile.sub_layer_max_8bit_constraint_flag[i] = readBits(1);
				h265Profile.sub_layer_max_422chroma_constraint_flag[i] = readBits(1);
				h265Profile.sub_layer_max_420chroma_constraint_flag[i] = readBits(1);
				h265Profile.sub_layer_max_monochrome_constraint_flag[i] = readBits(1);
				h265Profile.sub_layer_intra_constraint_flag[i] = readBits(1);
				h265Profile.sub_layer_one_picture_only_constraint_flag[i] = readBits(1);
				h265Profile.sub_layer_lower_bit_rate_constraint_flag[i] = readBits(1);

				if( h265Profile.sub_layer_profile_idc[i] ==  5 || h265Profile.sub_layer_profile_compatibility_flag[i][ 5] ||
					h265Profile.sub_layer_profile_idc[i] ==  9 || h265Profile.sub_layer_profile_compatibility_flag[i][ 9] ||
					h265Profile.sub_layer_profile_idc[i] == 10 || h265Profile.sub_layer_profile_compatibility_flag[i][10] ||
					h265Profile.sub_layer_profile_idc[i] == 11 || h265Profile.sub_layer_profile_compatibility_flag[i][11]) {
					h265Profile.sub_layer_max_14bit_constraint_flag[i] = readBits(1);
					// sub_layer_reserved_zero_33bits
					readBits(33);
				} else {
					// sub_layer_reserved_zero_34bits
					readBits(34);
				}
			} else if (h265Profile.sub_layer_profile_idc[i] == 2 || h265Profile.sub_layer_profile_compatibility_flag[i][2]) {
				// sub_layer_reserved_zero_7bits
				readBits(7);
				h265Profile.sub_layer_one_picture_only_constraint_flag[i] = readBits(1);
				// sub_layer_reserved_zero_35bits
				readBits(35);
			} else {
				// sub_layer_reserved_zero_43bits
				readBits(43);
			}

			if( h265Profile.sub_layer_profile_idc[i] ==  1 || h265Profile.sub_layer_profile_compatibility_flag[i][ 1] ||
				h265Profile.sub_layer_profile_idc[i] ==  2 || h265Profile.sub_layer_profile_compatibility_flag[i][ 2] ||
				h265Profile.sub_layer_profile_idc[i] ==  3 || h265Profile.sub_layer_profile_compatibility_flag[i][ 3] ||
				h265Profile.sub_layer_profile_idc[i] ==  4 || h265Profile.sub_layer_profile_compatibility_flag[i][ 4] ||
				h265Profile.sub_layer_profile_idc[i] ==  5 || h265Profile.sub_layer_profile_compatibility_flag[i][ 5] ||
				h265Profile.sub_layer_profile_idc[i] ==  9 || h265Profile.sub_layer_profile_compatibility_flag[i][ 9] ||
				h265Profile.sub_layer_profile_idc[i] == 11 || h265Profile.sub_layer_profile_compatibility_flag[i][11]) {
				h265Profile.sub_layer_inbld_flag[i] = readBits(1);
			} else {
				// sub_layer_reserved_zero_bits
				readBits(1);
			}
		}
	}

	return h265Profile;
}

H265ScalingList H265BitstreamReader::readScalingList()
{
	H265ScalingList scalingList;

	scalingList.scaling_list_pred_mode_flag.resize(4);
	scalingList.scaling_list_pred_matrix_id_delta.resize(4);
	scalingList.scaling_list_delta_coef.resize(4);

	for (uint64_t iSizeId = 0; iSizeId < 4; ++iSizeId) {
		scalingList.scaling_list_pred_mode_flag[iSizeId].resize(6);
		scalingList.scaling_list_pred_matrix_id_delta[iSizeId].resize(6);
		scalingList.scaling_list_delta_coef[iSizeId].resize(6);
		if(iSizeId >= 2) {
			scalingList.scaling_list_dc_coef_minus8[iSizeId-2].resize(6);
		}

		for (uint64_t iMatrixId = 0; iMatrixId < 6; iMatrixId += (iSizeId == 3) ? 3 : 1) {
			scalingList.scaling_list_pred_mode_flag[iSizeId][iMatrixId] = readBits(1);
			if (!scalingList.scaling_list_pred_mode_flag[iSizeId][iMatrixId]) {
				scalingList.scaling_list_pred_matrix_id_delta[iSizeId][iMatrixId] = readGolombUE();
			} else {
				int32_t iNextCoeff = 8;
				int32_t iCoeffNum = std::min(64, (1 << (4 + (iSizeId << 1))));
				if (iSizeId > 1) {
					scalingList.scaling_list_dc_coef_minus8[iSizeId-2][iMatrixId] = readGolombSE();
					iNextCoeff = scalingList.scaling_list_dc_coef_minus8[iSizeId-2][iMatrixId] + 8;
				}

				scalingList.scaling_list_delta_coef[iSizeId][iMatrixId].resize((int)iCoeffNum);
				for (int32_t i = 0; i < iCoeffNum; ++i) {
					scalingList.scaling_list_delta_coef[iSizeId][iMatrixId][i] = readGolombSE();
					iNextCoeff = (iNextCoeff + scalingList.scaling_list_delta_coef[iSizeId][iMatrixId][i] + 256 ) % 256;
					scalingList.ScalingList[iSizeId][iMatrixId][i] = iNextCoeff;
				}
			}
		}
	}

	return scalingList;
}

H265ShortTermRefPicSet H265BitstreamReader::readShortTermRefPicSet(uint32_t iShortTermSetIndex, const H265SPS& h265SPS)
{
	H265ShortTermRefPicSet shortTermRefPicSet;

	if (iShortTermSetIndex != 0) {
		shortTermRefPicSet.inter_ref_pic_set_prediction_flag = readBits(1);
	}

	if (shortTermRefPicSet.inter_ref_pic_set_prediction_flag) {
		if (iShortTermSetIndex == h265SPS.num_short_term_ref_pic_sets) {
			shortTermRefPicSet.delta_idx_minus1 = readGolombUE();
		}

		shortTermRefPicSet.delta_rps_sign = readBits(1);
		shortTermRefPicSet.abs_delta_rps_minus1 = readGolombUE();

		shortTermRefPicSet.RefRpsIdx = iShortTermSetIndex - (shortTermRefPicSet.delta_idx_minus1 + 1); // 7-59
		uint32_t RefRpsIdx = shortTermRefPicSet.RefRpsIdx;
		uint32_t NumDeltaPocs = 0;

		if (h265SPS.short_term_ref_pic_set[RefRpsIdx].inter_ref_pic_set_prediction_flag) {
			for (int i = 0; i < h265SPS.short_term_ref_pic_set[RefRpsIdx].used_by_curr_pic_flag.size(); ++i) {
				if(h265SPS.short_term_ref_pic_set[RefRpsIdx].used_by_curr_pic_flag[i] || h265SPS.short_term_ref_pic_set[RefRpsIdx].use_delta_flag[i]) {
					NumDeltaPocs++;
				}
			}
		} else {
			NumDeltaPocs = h265SPS.short_term_ref_pic_set[RefRpsIdx].num_negative_pics + h265SPS.short_term_ref_pic_set[RefRpsIdx].num_positive_pics;
		}

		shortTermRefPicSet.used_by_curr_pic_flag.resize(NumDeltaPocs + 1);
		shortTermRefPicSet.use_delta_flag.resize(NumDeltaPocs + 1);
		for (unsigned i = 0; i < NumDeltaPocs + 1; ++i) {
			shortTermRefPicSet.use_delta_flag[i] = 0;
		}

		for (uint64_t i = 0; i <= NumDeltaPocs; ++i) {
			shortTermRefPicSet.used_by_curr_pic_flag[i] = readBits(1);
			if(!shortTermRefPicSet.used_by_curr_pic_flag[i]) {
				shortTermRefPicSet.use_delta_flag[i] = readBits(1);
			}
		}

		// 7-61 calculate NumNegativePics, DeltaPocS0 and UsedByCurrPicS0
		uint32_t deltaRps = (1 - 2 * shortTermRefPicSet.delta_rps_sign) * (shortTermRefPicSet.abs_delta_rps_minus1 + 1); // 7-60
		const H265ShortTermRefPicSet& RefRps = h265SPS.short_term_ref_pic_set[RefRpsIdx];

		uint32_t i = 0;
		for (uint32_t j = (RefRps.NumPositivePics - 1); j >= 0; j--) {
			uint32_t dPoc = RefRps.DeltaPocS1[j] + deltaRps;
			if (dPoc < 0 && shortTermRefPicSet.use_delta_flag[RefRps.NumNegativePics + j]) {
				shortTermRefPicSet.DeltaPocS0[i] = dPoc;
				shortTermRefPicSet.UsedByCurrPicS0[i++] =
					shortTermRefPicSet.used_by_curr_pic_flag[RefRps.NumNegativePics + j];
			}
		}

		if (deltaRps < 0 && shortTermRefPicSet.use_delta_flag[RefRps.NumDeltaPocs]) {
			shortTermRefPicSet.DeltaPocS0[i] = deltaRps;
			shortTermRefPicSet.UsedByCurrPicS0[i++] = shortTermRefPicSet.used_by_curr_pic_flag[RefRps.NumDeltaPocs];
		}

		for (uint32_t j = 0; j < RefRps.NumNegativePics; j++) {
			uint32_t dPoc = RefRps.DeltaPocS0[j] + deltaRps;
			if (dPoc < 0 && shortTermRefPicSet.use_delta_flag[j]) {
				shortTermRefPicSet.DeltaPocS0[i] = dPoc;
				shortTermRefPicSet.UsedByCurrPicS0[i++] = shortTermRefPicSet.used_by_curr_pic_flag[j];
			}
		}
		shortTermRefPicSet.NumNegativePics = i;

		// 7-62 calculate NumPositivePics, DeltaPocS1 and UsedByCurrPicS1
		i = 0;
		for (uint32_t j = (RefRps.NumNegativePics - 1); j >= 0; j--) {
			uint32_t dPoc = RefRps.DeltaPocS0[j] + deltaRps;
			if (dPoc > 0 && shortTermRefPicSet.use_delta_flag[j]) {
				shortTermRefPicSet.DeltaPocS1[i] = dPoc;
				shortTermRefPicSet.UsedByCurrPicS1[i++] = shortTermRefPicSet.used_by_curr_pic_flag[j];
			}
		}
		if (deltaRps > 0 && shortTermRefPicSet.use_delta_flag[RefRps.NumDeltaPocs]) {
			shortTermRefPicSet.DeltaPocS1[i] = deltaRps;
			shortTermRefPicSet.UsedByCurrPicS1[i++] = shortTermRefPicSet.used_by_curr_pic_flag[RefRps.NumDeltaPocs];
		}
		for (uint32_t j = 0; j < RefRps.NumPositivePics; j++) {
			uint32_t dPoc = RefRps.DeltaPocS1[j] + deltaRps;
			if (dPoc > 0 && shortTermRefPicSet.use_delta_flag[RefRps.NumNegativePics + j]) {
				shortTermRefPicSet.DeltaPocS1[i] = dPoc;
				shortTermRefPicSet.UsedByCurrPicS1[i++] =
					shortTermRefPicSet.used_by_curr_pic_flag[RefRps.NumNegativePics + j];
			}
		}
		shortTermRefPicSet.NumPositivePics = i;
	} else {
		shortTermRefPicSet.num_negative_pics = readGolombUE();
		shortTermRefPicSet.num_positive_pics = readGolombUE();

		shortTermRefPicSet.delta_poc_s0_minus1.resize(shortTermRefPicSet.num_negative_pics);
		shortTermRefPicSet.used_by_curr_pic_s0_flag.resize(shortTermRefPicSet.num_negative_pics);
		shortTermRefPicSet.DeltaPocS0.resize(shortTermRefPicSet.num_negative_pics);
		shortTermRefPicSet.UsedByCurrPicS0.resize(shortTermRefPicSet.num_negative_pics);
		for (uint32_t i = 0; i < shortTermRefPicSet.num_negative_pics; ++i) {
			shortTermRefPicSet.delta_poc_s0_minus1[i] = readGolombUE();
			shortTermRefPicSet.used_by_curr_pic_s0_flag[i] = readBits(1);

			if (i ==0) {
				shortTermRefPicSet.DeltaPocS0[i] = -(shortTermRefPicSet.delta_poc_s0_minus1[i] + 1);
			} else {
				shortTermRefPicSet.DeltaPocS0[i] = shortTermRefPicSet.DeltaPocS0[i - 1] -(shortTermRefPicSet.delta_poc_s0_minus1[i] + 1);
			}
		}

		shortTermRefPicSet.delta_poc_s1_minus1.resize(shortTermRefPicSet.num_positive_pics);
		shortTermRefPicSet.used_by_curr_pic_s1_flag.resize(shortTermRefPicSet.num_positive_pics);
		shortTermRefPicSet.DeltaPocS1.resize(shortTermRefPicSet.num_positive_pics);
		shortTermRefPicSet.UsedByCurrPicS1.resize(shortTermRefPicSet.num_positive_pics);
		for (uint32_t i = 0; i < shortTermRefPicSet.num_positive_pics; ++i) {
			shortTermRefPicSet.delta_poc_s1_minus1[i] = readGolombUE();
			shortTermRefPicSet.used_by_curr_pic_s1_flag[i] = readBits(1);

			if (i ==0) {
				shortTermRefPicSet.DeltaPocS1[i] = -(shortTermRefPicSet.delta_poc_s1_minus1[i] + 1);
			} else {
				shortTermRefPicSet.DeltaPocS1[i] = shortTermRefPicSet.DeltaPocS1[i - 1] -(shortTermRefPicSet.delta_poc_s1_minus1[i] + 1);
			}
		}

		shortTermRefPicSet.NumNegativePics = shortTermRefPicSet.num_negative_pics; // 7-63
		shortTermRefPicSet.NumPositivePics = shortTermRefPicSet.num_positive_pics; // 7-64

		for (uint32_t i = 0; i < shortTermRefPicSet.num_negative_pics; ++i) {
			shortTermRefPicSet.UsedByCurrPicS0[i] = shortTermRefPicSet.used_by_curr_pic_s0_flag[i]; // 7-65

			if (i == 0) {
				shortTermRefPicSet.DeltaPocS0[i] = -(shortTermRefPicSet.delta_poc_s0_minus1[i] + 1); // 7-67
			} else {
				shortTermRefPicSet.DeltaPocS0[i] =
					shortTermRefPicSet.DeltaPocS0[i - 1] - (shortTermRefPicSet.delta_poc_s0_minus1[i] + 1); // 7-69
			}
		}

		for (uint32_t i = 0; i < shortTermRefPicSet.num_positive_pics; ++i) {
			shortTermRefPicSet.UsedByCurrPicS1[i] = shortTermRefPicSet.used_by_curr_pic_s1_flag[i]; // 7-66

			if (i == 0) {
				shortTermRefPicSet.DeltaPocS1[i] = -(shortTermRefPicSet.delta_poc_s1_minus1[i] + 1); // 7-68
			} else {
				shortTermRefPicSet.DeltaPocS1[i] =
					shortTermRefPicSet.DeltaPocS1[i - 1] - (shortTermRefPicSet.delta_poc_s1_minus1[i] + 1); // 7-70
			}
		}
	}

	shortTermRefPicSet.NumDeltaPocs = shortTermRefPicSet.NumPositivePics + shortTermRefPicSet.NumNegativePics; // 7-71

	return shortTermRefPicSet;
}

void H265BitstreamReader::computeNumPicTotalCurr(H265Slice& h265Slice, const H265SPS& h265SPS)
{
	h265Slice.PocLsbLt.resize(h265Slice.NumLongTerm);
	h265Slice.UsedByCurrPicLt.resize(h265Slice.NumLongTerm);

	for (uint32_t i = 0; i < h265Slice.NumLongTerm; ++i) {
		if (i < h265Slice.num_long_term_sps) {
			h265Slice.PocLsbLt[i] = h265SPS.lt_ref_pic_poc_lsb_sps[ h265Slice.lt_idx_sps[i] ];
			h265Slice.UsedByCurrPicLt[i] = h265SPS.used_by_curr_pic_lt_sps_flag[ h265Slice.lt_idx_sps[i] ];
		} else {
			h265Slice.PocLsbLt[i] = h265Slice.poc_lsb_lt[i];
			h265Slice.UsedByCurrPicLt[i] = h265Slice.used_by_curr_pic_lt_flag[i];
		}
	}

	h265Slice.NumPicTotalCurr = 0;

	uint8_t CurrRpsIdx = h265Slice.CurrRpsIdx;
	const H265ShortTermRefPicSet& stRsp = h265SPS.short_term_ref_pic_set[CurrRpsIdx];

	for (uint32_t i = 0; i < stRsp.NumNegativePics; i++ ) {
		if (stRsp.UsedByCurrPicS0[i]) {
			h265Slice.NumPicTotalCurr++;
		}
	}

	for (uint32_t i = 0; i < stRsp.NumPositivePics; i++) {
		if (stRsp.UsedByCurrPicS1[i]) {
			h265Slice.NumPicTotalCurr++;
		}
	}

	for (uint32_t i = 0; i < h265Slice.NumLongTerm; i++) {
		if (h265Slice.UsedByCurrPicLt[i]) {
			h265Slice.NumPicTotalCurr++;
		}
	}

	// PPS extention not handled
	// if (h265Slice.pps_curr_pic_ref_enabled_flag) {
	// 	h265Slice.NumPicTotalCurr++;
	// }
}

void H265BitstreamReader::readRefPicListsModification(H265Slice& h265Slice)
{
	RefPicListsModification& ref_pic_lists_modification = h265Slice.ref_pic_lists_modification;

	ref_pic_lists_modification.ref_pic_list_modification_flag_l0 = readBits(1);
	if(ref_pic_lists_modification.ref_pic_list_modification_flag_l0){
		for(uint32_t i = 0; i <= h265Slice.num_ref_idx_l0_active_minus1; i++ ){
			uint32_t iVal = readBits( (uint8_t)ceil(log2( h265Slice.NumPicTotalCurr)));
			ref_pic_lists_modification.list_entry_l0.push_back(iVal);
		}
	}

	if(h265Slice.slice_type == H265Slice::SliceType_B) {
		ref_pic_lists_modification.ref_pic_list_modification_flag_l1 = readBits(1);
		if (ref_pic_lists_modification.ref_pic_list_modification_flag_l1) {
			for (uint32_t i = 0; i <= h265Slice.num_ref_idx_l1_active_minus1; i++) {
				uint32_t iVal = readBits((uint8_t) ceil(log2(h265Slice.NumPicTotalCurr)));
				ref_pic_lists_modification.list_entry_l1.push_back(iVal);
			}
		}
	}
}