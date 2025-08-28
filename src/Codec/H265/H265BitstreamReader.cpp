#include <iostream>
#include <cmath>

#include "../H26X/H26XMath.h"
#include "H265AccessUnit.h"

#include "H265BitstreamReader.h"

H265BitstreamReader::H265BitstreamReader(const uint8_t* pNALData, uint32_t iNALLength)
	: H26XBitstreamReader(pNALData, iNALLength)
{

}

void H265BitstreamReader::readNALHeader(H265NAL& h265NAL)
{
	h265NAL.forbidden_zero_bit = readBits(1);
	h265NAL.nal_unit_type = (H265NALUnitType::Type)readBits(6);
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
			h265VPS.hrd_parameters[i] = readHrdParameters(h265VPS.cprms_present_flag[i], h265VPS.vps_max_sub_layers_minus1);
		}
	}

	h265VPS.vps_extension_flag = readBits(1);
	// if( vps_extension_flag )
	// 	while( more_rbsp_data( ) ) u(1)
	// 		vps_extension_data_flag
	// rbsp_trailing_bits( )
}

void H265BitstreamReader::readSPS(H265SPS& h265SPS)
{
	// F.7.3.2.2.1 General sequence parameter set RBSP syntax
	h265SPS.sps_video_parameter_set_id = readBits(4);
	if(h265SPS.nuh_layer_id == 0) {
		h265SPS.sps_max_sub_layers_minus1 = readBits(3);
	}else{
		h265SPS.sps_ext_or_max_sub_layers_minus1 = readBits(3);
	}
	bool bMultiLayerExtSpsFlag = (h265SPS.nuh_layer_id != 0 && h265SPS.sps_ext_or_max_sub_layers_minus1 == 7);
	if(!bMultiLayerExtSpsFlag) {
		h265SPS.sps_temporal_id_nesting_flag = readBits(1);
		h265SPS.profile_tier_level = readProfileTierLevel(1, h265SPS.sps_max_sub_layers_minus1);
	}
	h265SPS.sps_seq_parameter_set_id = readGolombUE();
	if(bMultiLayerExtSpsFlag) {
		h265SPS.update_rep_format_flag = readBits(1);
		if(h265SPS.update_rep_format_flag){
			h265SPS.sps_rep_format_idx = readBits(8);
		}
	}else {
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
		h265SPS.ChromaArrayType = (h265SPS.separate_colour_plane_flag == 0 ? h265SPS.chroma_format_idc : 0);

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
	}
	h265SPS.log2_max_pic_order_cnt_lsb_minus4 = readGolombUE();
	if(!bMultiLayerExtSpsFlag) {
		h265SPS.sps_sub_layer_ordering_info_present_flag = readBits(1);
		h265SPS.sps_max_dec_pic_buffering_minus1.resize(h265SPS.sps_max_sub_layers_minus1 + 1);
		h265SPS.sps_max_num_reorder_pics.resize(h265SPS.sps_max_sub_layers_minus1 + 1);
		h265SPS.sps_max_latency_increase_plus1.resize(h265SPS.sps_max_sub_layers_minus1 + 1);
		for (uint8_t i = (h265SPS.sps_sub_layer_ordering_info_present_flag ? 0 : h265SPS.sps_max_sub_layers_minus1); i <= h265SPS.sps_max_sub_layers_minus1; ++i) {
			h265SPS.sps_max_dec_pic_buffering_minus1[i] = readGolombUE();
			h265SPS.sps_max_num_reorder_pics[i] = readGolombUE();
			h265SPS.sps_max_latency_increase_plus1[i] = readGolombUE();
		}
	}
	h265SPS.log2_min_luma_coding_block_size_minus3 = readGolombUE();
	h265SPS.log2_diff_max_min_luma_coding_block_size = readGolombUE();
	h265SPS.log2_min_luma_transform_block_size_minus2 = readGolombUE();
	h265SPS.log2_diff_max_min_luma_transform_block_size = readGolombUE();
	h265SPS.max_transform_hierarchy_depth_inter = readGolombUE();
	h265SPS.max_transform_hierarchy_depth_intra = readGolombUE();
	h265SPS.scaling_list_enabled_flag = readBits(1);
	if (h265SPS.scaling_list_enabled_flag) {
		if(bMultiLayerExtSpsFlag){
			h265SPS.sps_infer_scaling_list_flag = readBits(1);
		}
		if(h265SPS.sps_infer_scaling_list_flag){
			h265SPS.sps_scaling_list_ref_layer_id = readBits(6);
		}
		h265SPS.sps_scaling_list_data_present_flag = readBits(1);
		if (h265SPS.sps_scaling_list_data_present_flag) {
			h265SPS.scaling_list_data = readScalingListData();
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
	
	h265SPS.vui_parameters_present_flag = readBits(1);
	if(h265SPS.vui_parameters_present_flag){
		h265SPS.vui_parameters = readVuiParameters(h265SPS.sps_max_sub_layers_minus1);
	}
	
	h265SPS.sps_extension_flag = readBits(1);
	if(h265SPS.sps_extension_flag){
		h265SPS.sps_range_extension_flag = readBits(1);
		h265SPS.sps_multilayer_extension_flag = readBits(1);
		h265SPS.sps_3d_extension_flag = readBits(1);
		h265SPS.sps_scc_extension_flag = readBits(1);
		h265SPS.sps_extension_4bits = readBits(5);
	}
	
	// Compute the slice segment address length (cf eq 7.10 -> 7.22)
	h265SPS.MinCbLog2SizeY = h265SPS.log2_min_luma_coding_block_size_minus3 + 3;
	h265SPS.CtbLog2SizeY = h265SPS.MinCbLog2SizeY + h265SPS.log2_diff_max_min_luma_coding_block_size;
	h265SPS.MinCbSizeY = 1 << h265SPS.MinCbLog2SizeY;
	h265SPS.CtbSizeY = 1 << h265SPS.CtbLog2SizeY;
	h265SPS.MinTbLog2SizeY = h265SPS.log2_min_luma_transform_block_size_minus2 + 2;
	h265SPS.PicWidthInMinCbsY = h265SPS.pic_width_in_luma_samples / h265SPS.MinCbSizeY;
	h265SPS.PicWidthInCtbsY = H26XMath::ceil(h265SPS.pic_width_in_luma_samples, h265SPS.CtbSizeY);
	h265SPS.PicHeightInMinCbsY = h265SPS.pic_height_in_luma_samples / h265SPS.MinCbSizeY;
	h265SPS.PicHeightInCtbsY = H26XMath::ceil(h265SPS.pic_height_in_luma_samples, h265SPS.CtbSizeY);
	h265SPS.PicSizeInMinCbsY = h265SPS.PicWidthInMinCbsY * h265SPS.PicHeightInMinCbsY;
	h265SPS.PicSizeInCtbsY = h265SPS.PicWidthInCtbsY * h265SPS.PicHeightInCtbsY;
	h265SPS.PicSizeInSamplesY = h265SPS.pic_width_in_luma_samples * h265SPS.pic_height_in_luma_samples;
	h265SPS.BitDepthY = 8 + h265SPS.bit_depth_luma_minus8;
	h265SPS.QpBdOffsetY = 6 * h265SPS.bit_depth_luma_minus8;
	h265SPS.BitDepthC = 8 + h265SPS.bit_depth_chroma_minus8;
	h265SPS.PcmBitDepthY = h265SPS.pcm_sample_bit_depth_luma_minus1 + 1;
	h265SPS.PcmBitDepthC = h265SPS.pcm_sample_bit_depth_chroma_minus1 + 1;
	h265SPS.QpBdOffsetC = 6 * h265SPS.bit_depth_chroma_minus8;
	h265SPS.Log2MinIpcmCbSizeY = h265SPS.log2_min_pcm_luma_coding_block_size_minus3+3;
	h265SPS.Log2MaxIpcmCbSizeY = h265SPS.log2_diff_max_min_luma_coding_block_size + h265SPS.Log2MinIpcmCbSizeY;
	h265SPS.MaxPicOrderCntLsb = 1 << (h265SPS.log2_max_pic_order_cnt_lsb_minus4+4);
	
	if(h265SPS.sps_range_extension_flag){
		h265SPS.sps_range_extension = readSPSRangeExtension();
	}
	if(h265SPS.sps_multilayer_extension_flag){
		h265SPS.sps_multilayer_extension = readSPSMultilayerExtension();
	}
	if(h265SPS.sps_3d_extension_flag){
		h265SPS.sps_3d_extension = readSPS3DExtension(h265SPS);
	}
	if(h265SPS.sps_scc_extension_flag){
		h265SPS.sps_scc_extension = readSPSSCCExtension(h265SPS);
	}
	if(h265SPS.sps_extension_4bits){
		while(hasMoreRBSPData()){
			h265SPS.sps_extension_data_flag = readBits(1);
		}
	}
	//readRBSPTrailingBits();

	uint32_t spsMaxLumaPs = H265SPS::MaxLumaPs[H265SPS::level_limit_index[h265SPS.profile_tier_level.general_level_idc]];
	uint8_t maxDpbPicBuf = !h265SPS.sps_scc_extension_flag ? 6 : 7;
	if(h265SPS.PicSizeInSamplesY <= (spsMaxLumaPs >> 2)){
		h265SPS.MaxDpbSize = std::min(4*maxDpbPicBuf, 16);
	}else if (h265SPS.PicSizeInSamplesY <= (spsMaxLumaPs >> 1)){
		h265SPS.MaxDpbSize = std::min(2*maxDpbPicBuf, 16);
	}else if (h265SPS.PicSizeInSamplesY <= ((3*spsMaxLumaPs) >> 2)){
		h265SPS.MaxDpbSize = std::min(4*maxDpbPicBuf/3, 16);
	}else{
		h265SPS.MaxDpbSize = maxDpbPicBuf;
	}
	h265SPS.sps_range_extension.CoeffMinY = -(1 << (h265SPS.sps_range_extension.extended_precision_processing_flag ? std::max(15, h265SPS.BitDepthY+6) : 15));
	h265SPS.sps_range_extension.CoeffMinC = -(1 << (h265SPS.sps_range_extension.extended_precision_processing_flag ? std::max(15, h265SPS.BitDepthC+6) : 15));
	h265SPS.sps_range_extension.CoeffMaxY = (1 << (h265SPS.sps_range_extension.extended_precision_processing_flag ? std::max(15, h265SPS.BitDepthY+6) : 15))-1;
	h265SPS.sps_range_extension.CoeffMaxC = (1 << (h265SPS.sps_range_extension.extended_precision_processing_flag ? std::max(15, h265SPS.BitDepthC+6) : 15))-1;

	h265SPS.sps_range_extension.WpOffsetBdShiftY = h265SPS.sps_range_extension.high_precision_offsets_enabled_flag ? 0 : (h265SPS.BitDepthY-8);
	h265SPS.sps_range_extension.WpOffsetBdShiftC = h265SPS.sps_range_extension.high_precision_offsets_enabled_flag ? 0 : (h265SPS.BitDepthC-8);
	h265SPS.sps_range_extension.WpOffsetHalfRangeY = 1 << (h265SPS.sps_range_extension.high_precision_offsets_enabled_flag ? (h265SPS.BitDepthY-1) : 7);
	h265SPS.sps_range_extension.WpOffsetHalfRangeC = 1 << (h265SPS.sps_range_extension.high_precision_offsets_enabled_flag ? (h265SPS.BitDepthC-1) : 7);
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
				h265PPS.row_height_minus1[i] = readGolombUE();
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
		h265PPS.scaling_list_data = readScalingListData();
	}
	h265PPS.lists_modification_present_flag = readBits(1);
	h265PPS.log2_parallel_merge_level_minus2 = readGolombUE();
	h265PPS.slice_segment_header_extension_present_flag = readBits(1);

	h265PPS.pps_extension_present_flag = readBits(1);
	if(h265PPS.pps_extension_present_flag){
		h265PPS.pps_range_extension_flag = readBits(1);
		h265PPS.pps_multilayer_extension_flag = readBits(1);
		h265PPS.pps_3d_extension_flag = readBits(1);
		h265PPS.pps_scc_extension_flag = readBits(1);
		skipBits(4);
	}
	if(h265PPS.pps_range_extension_flag) h265PPS.pps_range_extension = readH265PPSRangeExtension(h265PPS);
	if(h265PPS.pps_multilayer_extension_flag) h265PPS.pps_multilayer_extension = readH265PPSMultilayerExtension();
	if(h265PPS.pps_3d_extension_flag) h265PPS.pps_3d_extension = readH265PPS3DExtension();
	if(h265PPS.pps_scc_extension_flag) h265PPS.pps_scc_extension = readH265PPSSCCExtension();
	auto referencedSPS = H265SPS::SPSMap.find(h265PPS.pps_seq_parameter_set_id);
	H265SPS* pSps = referencedSPS == H265SPS::SPSMap.end() ? nullptr : referencedSPS->second;
	if(!pSps) return;
	h265PPS.TwoVersionsOfCurrDecPicFlag = h265PPS.pps_scc_extension.pps_curr_pic_ref_enabled_flag && 
											(pSps->sample_adaptive_offset_enabled_flag || !h265PPS.pps_deblocking_filter_disabled_flag ||
											h265PPS.deblocking_filter_override_enabled_flag);
}

void H265BitstreamReader::readSlice(H265Slice& h265Slice, std::vector<H265AccessUnit*> pAccessUnits, H265AccessUnit* pNextAccessUnit)
{
	h265Slice.IdrPicFlag = h265Slice.isIDR();
	h265Slice.IRAPPicture = (h265Slice.nal_unit_type >= H265NALUnitType::BLA_W_LP) && (h265Slice.nal_unit_type <= H265NALUnitType::IRAP_VCL23);
	h265Slice.NoRaslOutputFlag = h265Slice.NoRaslOutputFlag || 
		h265Slice.nal_unit_type == H265NALUnitType::IDR_W_RADL ||
		h265Slice.nal_unit_type == H265NALUnitType::IDR_N_LP ||
		h265Slice.nal_unit_type == H265NALUnitType::BLA_W_LP ||
		h265Slice.nal_unit_type == H265NALUnitType::BLA_W_RADL ||
		h265Slice.nal_unit_type == H265NALUnitType::BLA_N_LP;
	
	h265Slice.first_slice_segment_in_pic_flag = readBits(1);
	if (h265Slice.nal_unit_type >= H265NALUnitType::BLA_W_LP && h265Slice.nal_unit_type <= H265NALUnitType::IRAP_VCL23) {
		h265Slice.no_output_of_prior_pics_flag = readBits(1);
	}
	h265Slice.slice_pic_parameter_set_id = readGolombUE();
	H265PPS* h265PPS = h265Slice.getPPS();
	if(!h265PPS) {
		return;
	}
	if (!h265Slice.first_slice_segment_in_pic_flag) {
		if (h265PPS->dependent_slice_segments_enabled_flag) {
			h265Slice.dependent_slice_segment_flag = readBits(1);
		}

		// if (h265SPS.chroma_format_idc != 1) {
			// 	TkCore::Logger::debug("[H265::bitstream] Other format than YUV 4:2:0 aren't supported");
			// }
			// uint32_t PicWidthInSamplesC = h265SPS.pic_width_in_luma_samples / 2;
			// uint32_t PicHeightInSamplesC = h265SPS.pic_height_in_luma_samples / 2;
			H265SPS* h265SPS = h265Slice.getSPS();
			if(!h265SPS) {
				return;
		}
		h265Slice.slice_segment_address = readBits((uint8_t)ceil(log2(h265SPS->PicSizeInCtbsY)));
	}

	if (!h265Slice.dependent_slice_segment_flag) {
		// Skip reserved flags
		skipBits(h265PPS->num_extra_slice_header_bits);
		h265Slice.slice_type = (H265Slice::SliceType)readGolombUE();
		if (h265PPS->output_flag_present_flag) {
			h265Slice.pic_output_flag = readBits(1);
		}
		H265SPS* h265SPS = h265Slice.getSPS();
		if(!h265SPS) return;
		if (h265SPS->separate_colour_plane_flag == 1) {
			h265Slice.colour_plane_id = readBits(2);
		}

		if (!h265Slice.IdrPicFlag) {
			h265Slice.slice_pic_order_cnt_lsb = readBits(h265SPS->log2_max_pic_order_cnt_lsb_minus4 + 4);
			h265Slice.short_term_ref_pic_set_sps_flag = readBits(1);

			uint64_t iStartShortTermBitsCount = m_iRemainingBits;
			if (!h265Slice.short_term_ref_pic_set_sps_flag) {
				h265Slice.CurrRpsIdx = h265SPS->num_short_term_ref_pic_sets;
				h265SPS->short_term_ref_pic_set[h265SPS->num_short_term_ref_pic_sets] = readShortTermRefPicSet(h265SPS->num_short_term_ref_pic_sets, *h265SPS);
			} else if (h265SPS->num_short_term_ref_pic_sets > 1)  {
				h265Slice.short_term_ref_pic_set_idx = readBits((uint8_t)ceil(log2(h265SPS->num_short_term_ref_pic_sets)));
			}
			if (h265Slice.short_term_ref_pic_set_sps_flag) {
				h265Slice.CurrRpsIdx = h265Slice.short_term_ref_pic_set_idx;
			}
			h265Slice.NumShortTermPictureSliceHeaderBits = iStartShortTermBitsCount - m_iRemainingBits;

			uint64_t iStartLongTermBitsCount = m_iRemainingBits;
			if (h265SPS->long_term_ref_pics_present_flag) {
				if (h265SPS->num_long_term_ref_pics_sps > 0) {
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
						if (h265SPS->num_long_term_ref_pics_sps > 1) {
							h265Slice.lt_idx_sps[i] = readBits((uint8_t)ceil(log2(h265SPS->num_long_term_ref_pics_sps)));
						}
					} else {
						h265Slice.poc_lsb_lt[i] = readBits(h265SPS->log2_max_pic_order_cnt_lsb_minus4 + 4);
						h265Slice.used_by_curr_pic_lt_flag[i] = readBits(1);
					}
					h265Slice.delta_poc_msb_present_flag[i] = readBits(1);
					if (h265Slice.delta_poc_msb_present_flag[i]) {
						h265Slice.delta_poc_msb_cycle_lt[i] = readGolombUE();
					}
				}
			}
			h265Slice.NumLongTermPictureSliceHeaderBits = iStartLongTermBitsCount - m_iRemainingBits;

			if (h265SPS->sps_temporal_mvp_enabled_flag) {
				h265Slice.slice_temporal_mvp_enabled_flag = readBits(1);
			}
		}
		computePOC(h265Slice, pAccessUnits);
		if (h265SPS->sample_adaptive_offset_enabled_flag) {
			h265Slice.slice_sao_luma_flag = readBits(1);
			if (h265SPS->ChromaArrayType != 0) {
				h265Slice.slice_sao_chroma_flag = readBits(1);
			}
		}
		h265Slice.num_ref_idx_l0_active_minus1 = h265PPS->num_ref_idx_l0_default_active_minus1;
		h265Slice.num_ref_idx_l1_active_minus1 = h265PPS->num_ref_idx_l1_default_active_minus1;
		if (h265Slice.slice_type == H265Slice::SliceType_P || h265Slice.slice_type == H265Slice::SliceType_B) {
			h265Slice.num_ref_idx_active_override_flag = readBits(1);
			if (h265Slice.num_ref_idx_active_override_flag) {
				h265Slice.num_ref_idx_l0_active_minus1 = readGolombUE();
				if (h265Slice.slice_type == H265Slice::SliceType_B) {
					h265Slice.num_ref_idx_l1_active_minus1 = readGolombUE();
				}
			}
			if (h265PPS->lists_modification_present_flag) {
				computeNumPicTotalCurr(h265Slice, *h265SPS);
				if (h265Slice.NumPicTotalCurr > 1) {
					readRefPicListsModification(h265Slice);
				}
			}
			computeRPL(h265Slice, pAccessUnits, pNextAccessUnit);
			if (h265Slice.slice_type == H265Slice::SliceType_B) {
				h265Slice.mvd_l1_zero_flag = readBits(1);
			}
			if (h265PPS->cabac_init_present_flag) {
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
			if ((h265PPS->weighted_pred_flag && h265Slice.slice_type == H265Slice::SliceType_P) ||
				(h265PPS->weighted_bipred_flag && h265Slice.slice_type == H265Slice::SliceType_B))
			{
				h265Slice.pred_weight_table = readSlicePredWeightTable(h265Slice);
			}
			h265Slice.five_minus_max_num_merge_cand = readGolombUE();
			if(h265SPS->sps_scc_extension.motion_vector_resolution_control_idc == 2 )
				h265Slice.use_integer_mv_flag = readBits(1);
		}
		h265Slice.slice_qp_delta = readGolombSE();
		if (h265PPS->pps_slice_chroma_qp_offsets_present_flag) {
			h265Slice.slice_cb_qp_offset = readGolombSE();
			h265Slice.slice_cr_qp_offset = readGolombSE();
		}
		h265Slice.SliceQpY = 26 + h265PPS->init_qp_minus26 + h265Slice.slice_qp_delta;
		if (h265PPS->pps_scc_extension.pps_slice_act_qp_offsets_present_flag) {
			h265Slice.slice_act_y_qp_offset = readGolombSE();
			h265Slice.slice_act_cb_qp_offset = readGolombSE();
			h265Slice.slice_act_cr_qp_offset = readGolombSE();
		}

		if (h265PPS->pps_range_extension.chroma_qp_offset_list_enabled_flag) {
			h265Slice.cu_chroma_qp_offset_enabled_flag = readBits(1);
		}

		if (h265PPS->deblocking_filter_override_enabled_flag) {
			h265Slice.deblocking_filter_override_flag = readBits(1);
		}
		if (h265Slice.deblocking_filter_override_flag) {
			h265Slice.slice_deblocking_filter_disabled_flag = readBits(1);
			if (!h265Slice.slice_deblocking_filter_disabled_flag) {
				h265Slice.slice_beta_offset_div2 = readGolombSE();
				h265Slice.slice_tc_offset_div2 = readGolombSE();
			}
		}
		if (h265PPS->pps_loop_filter_across_slices_enabled_flag &&
			(h265Slice.slice_sao_luma_flag || h265Slice.slice_sao_chroma_flag || !h265Slice.slice_deblocking_filter_disabled_flag))
		{
			h265Slice.slice_loop_filter_across_slices_enabled_flag = readBits(1);
		}
	}
	if (h265PPS->tiles_enabled_flag || h265PPS->entropy_coding_sync_enabled_flag) {
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
	h265Slice.DeltaPocMsbCycleLt.resize(h265Slice.NumLongTerm);
	for (uint32_t i = 0; i < h265Slice.NumLongTerm; ++i) {
		uint32_t delta_poc_msb_cycle_lt = (i < h265Slice.delta_poc_msb_cycle_lt.size()) ? h265Slice.delta_poc_msb_cycle_lt[i] : 0;
		if (i == 0 || i == h265Slice.num_long_term_sps) {
			h265Slice.DeltaPocMsbCycleLt[i] = delta_poc_msb_cycle_lt;
		} else {
			h265Slice.DeltaPocMsbCycleLt[i] = delta_poc_msb_cycle_lt + h265Slice.DeltaPocMsbCycleLt[i - 1];
		}
	}

	// computeNumPicTotalCurr(h265Slice, h265SPS);

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
		for (uint8_t j = 0; j < 32u; j++) {
			h265Profile.general_profile_compatibility_flag[j] = readBits(1);
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
	h265Profile.sub_layer_level_idc.resize(iMaxNumSubLayersMinus1);
	for (uint8_t i = 0; i < iMaxNumSubLayersMinus1; i++) {
		h265Profile.sub_layer_profile_present_flag[i] = readBits(1);
		h265Profile.sub_layer_level_present_flag[i] = readBits(1);
	}

	if (iMaxNumSubLayersMinus1 > 0) {
		for (uint8_t i = iMaxNumSubLayersMinus1; i < 8; i++) {
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

	for (uint8_t i = 0; i < iMaxNumSubLayersMinus1; i++) {
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

			if(h265Profile.sub_layer_level_present_flag[i]){
				h265Profile.sub_layer_level_idc[i] = readBits(8);
			}
		}
	}

	return h265Profile;
}

H265ScalingList H265BitstreamReader::readScalingListData()
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
				for (int32_t i = 0; i < iCoeffNum; i++) {
					int scaling_list_delta_coef = readGolombSE();
					scalingList.scaling_list_delta_coef[iSizeId][iMatrixId][i] = scaling_list_delta_coef;
					iNextCoeff = (iNextCoeff + scaling_list_delta_coef + 256 ) % 256;
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
			for (uint32_t i = 0; i < h265SPS.short_term_ref_pic_set[RefRpsIdx].used_by_curr_pic_flag.size(); ++i) {
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
		int32_t deltaRps = (1 - 2 * shortTermRefPicSet.delta_rps_sign) * (shortTermRefPicSet.abs_delta_rps_minus1 + 1); // 7-60
		const H265ShortTermRefPicSet& RefRps = h265SPS.short_term_ref_pic_set[RefRpsIdx];

		auto iTabSizeDeltaPoc = RefRps.NumPositivePics + RefRps.NumNegativePics + RefRps.NumDeltaPocs + 1;
		shortTermRefPicSet.DeltaPocS0.resize(iTabSizeDeltaPoc);
		shortTermRefPicSet.UsedByCurrPicS0.resize(iTabSizeDeltaPoc);
		shortTermRefPicSet.DeltaPocS1.resize(iTabSizeDeltaPoc);
		shortTermRefPicSet.UsedByCurrPicS1.resize(iTabSizeDeltaPoc);

		uint32_t i = 0;
		for (int32_t j = ((int)RefRps.NumPositivePics - 1); j >= 0; j--) {
			int32_t dPoc = RefRps.DeltaPocS1[j] + deltaRps;
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

		for (int32_t j = 0; j < (int)RefRps.NumNegativePics; j++) {
			int32_t dPoc = RefRps.DeltaPocS0[j] + deltaRps;
			if (dPoc < 0 && shortTermRefPicSet.use_delta_flag[j]) {
				shortTermRefPicSet.DeltaPocS0[i] = dPoc;
				shortTermRefPicSet.UsedByCurrPicS0[i++] = shortTermRefPicSet.used_by_curr_pic_flag[j];
			}
		}
		shortTermRefPicSet.NumNegativePics = i;

		// 7-62 calculate NumPositivePics, DeltaPocS1 and UsedByCurrPicS1
		i = 0;
		for (int32_t j = ((int)RefRps.NumNegativePics - 1); j >= 0; j--) {
			int32_t dPoc = RefRps.DeltaPocS0[j] + deltaRps;
			if (dPoc > 0 && shortTermRefPicSet.use_delta_flag[j]) {
				shortTermRefPicSet.DeltaPocS1[i] = dPoc;
				shortTermRefPicSet.UsedByCurrPicS1[i++] = shortTermRefPicSet.used_by_curr_pic_flag[j];
			}
		}
		if (deltaRps > 0 && shortTermRefPicSet.use_delta_flag[RefRps.NumDeltaPocs]) {
			shortTermRefPicSet.DeltaPocS1[i] = deltaRps;
			shortTermRefPicSet.UsedByCurrPicS1[i++] = shortTermRefPicSet.used_by_curr_pic_flag[RefRps.NumDeltaPocs];
		}
		for (int32_t j = 0; j < (int)RefRps.NumPositivePics; j++) {
			int32_t dPoc = RefRps.DeltaPocS1[j] + deltaRps;
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

H265HrdParameters H265BitstreamReader::readHrdParameters(uint8_t commonInfPresentFlag, uint8_t maxNumSubLayersMinus1){
	H265HrdParameters h265HrdParameters;
	if(commonInfPresentFlag){
		h265HrdParameters.nal_hrd_parameters_present_flag = readBits(1);
		h265HrdParameters.vcl_hrd_parameters_present_flag = readBits(1);
		if(h265HrdParameters.nal_hrd_parameters_present_flag || h265HrdParameters.vcl_hrd_parameters_present_flag){
			h265HrdParameters.sub_pic_hrd_params_present_flag = readBits(1);
			if(h265HrdParameters.sub_pic_hrd_params_present_flag){
				h265HrdParameters.tick_divisor_minus2 = readBits(8);
				h265HrdParameters.du_cpb_removal_delay_increment_length_minus1 = readBits(5);
				h265HrdParameters.sub_pic_cpb_params_in_pic_timing_sei_flag = readBits(1);
				h265HrdParameters.dpb_output_delay_du_length_minus1 = readBits(5);
			}
			h265HrdParameters.bit_rate_scale = readBits(4);
			h265HrdParameters.cpb_size_scale = readBits(4);
			if(h265HrdParameters.sub_pic_hrd_params_present_flag){
				h265HrdParameters.cpb_size_du_scale = readBits(4);
			}
			h265HrdParameters.initial_cpb_removal_delay_length_minus1 = readBits(5);
			h265HrdParameters.au_cpb_removal_delay_length_minus1 = readBits(5);
			h265HrdParameters.dpb_output_delay_length_minus1 = readBits(5);
		}
	}
	h265HrdParameters.fixed_pic_rate_general_flag.resize(maxNumSubLayersMinus1+1);
	h265HrdParameters.fixed_pic_rate_within_cvs_flag.resize(maxNumSubLayersMinus1+1);
	h265HrdParameters.elemental_duration_in_tc_minus1.resize(maxNumSubLayersMinus1+1);
	h265HrdParameters.low_delay_hrd_flag.resize(maxNumSubLayersMinus1+1);
	h265HrdParameters.cpb_cnt_minus1.resize(maxNumSubLayersMinus1+1);
	h265HrdParameters.nal_sub_layer_hrd_parameters.resize(maxNumSubLayersMinus1+1);
	h265HrdParameters.vcl_sub_layer_hrd_parameters.resize(maxNumSubLayersMinus1+1);
	for(uint8_t i = 0;i <= maxNumSubLayersMinus1;++i){
		h265HrdParameters.fixed_pic_rate_general_flag[i] = readBits(1);
		if(!h265HrdParameters.fixed_pic_rate_general_flag[i]){
			h265HrdParameters.fixed_pic_rate_within_cvs_flag[i] = readBits(1);
		}else{
			h265HrdParameters.fixed_pic_rate_within_cvs_flag[i] = 1;
		}
		if(h265HrdParameters.fixed_pic_rate_within_cvs_flag[i]) {
			h265HrdParameters.elemental_duration_in_tc_minus1[i] = readGolombUE();
		}else{
			h265HrdParameters.low_delay_hrd_flag[i] = readBits(1);
		}
		if(!h265HrdParameters.low_delay_hrd_flag[i]){
			h265HrdParameters.cpb_cnt_minus1[i] = readGolombUE();
		}else{
			h265HrdParameters.cpb_cnt_minus1[i] = 0;
		}
		if(h265HrdParameters.nal_hrd_parameters_present_flag){
			h265HrdParameters.nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1.resize(h265HrdParameters.cpb_cnt_minus1[i]+1);
			h265HrdParameters.nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1.resize(h265HrdParameters.cpb_cnt_minus1[i]+1);
			if(h265HrdParameters.sub_pic_hrd_params_present_flag){
				h265HrdParameters.nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1.resize(h265HrdParameters.cpb_cnt_minus1[i]+1);
				h265HrdParameters.nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1.resize(h265HrdParameters.cpb_cnt_minus1[i]+1);
			}
			h265HrdParameters.nal_sub_layer_hrd_parameters[i].cbr_flag.resize(h265HrdParameters.cpb_cnt_minus1[i]+1);
			for(int j = 0;j <= h265HrdParameters.cpb_cnt_minus1[i];j++){
				h265HrdParameters.nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] = readGolombUE();
				h265HrdParameters.nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] = readGolombUE();
				if(h265HrdParameters.sub_pic_hrd_params_present_flag){
					h265HrdParameters.nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] = readGolombUE();
					h265HrdParameters.nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] = readGolombUE();
				}
				h265HrdParameters.nal_sub_layer_hrd_parameters[i].cbr_flag[j] = readBits(1);
			}
		}
		if(h265HrdParameters.vcl_hrd_parameters_present_flag){
			h265HrdParameters.vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1.resize(h265HrdParameters.cpb_cnt_minus1[i]+1);
			h265HrdParameters.vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1.resize(h265HrdParameters.cpb_cnt_minus1[i]+1);
			if(h265HrdParameters.sub_pic_hrd_params_present_flag){
				h265HrdParameters.vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1.resize(h265HrdParameters.cpb_cnt_minus1[i]+1);
				h265HrdParameters.vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1.resize(h265HrdParameters.cpb_cnt_minus1[i]+1);
			}
			h265HrdParameters.vcl_sub_layer_hrd_parameters[i].cbr_flag.resize(h265HrdParameters.cpb_cnt_minus1[i]+1);
			for(int j = 0;j <= h265HrdParameters.cpb_cnt_minus1[i];++j){
				h265HrdParameters.vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] = readGolombUE();
				h265HrdParameters.vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] = readGolombUE();

				if(h265HrdParameters.sub_pic_hrd_params_present_flag){
					h265HrdParameters.vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] = readGolombUE();
					h265HrdParameters.vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] = readGolombUE();
				}
				h265HrdParameters.vcl_sub_layer_hrd_parameters[i].cbr_flag[j] = readBits(1);
			}
		}
	}
	return h265HrdParameters;
}

H265VuiParameters H265BitstreamReader::readVuiParameters(uint8_t sps_max_sub_layers_minus1){
	H265VuiParameters h265VuiParameters;
	h265VuiParameters.aspect_ratio_info_present_flag = readBits(1);
	if(h265VuiParameters.aspect_ratio_info_present_flag){
		h265VuiParameters.aspect_ratio_idc = readBits(8);
		if(h265VuiParameters.aspect_ratio_idc == EXTENDED_SAR){
			h265VuiParameters.sar_width = readBits(16);
			h265VuiParameters.sar_height = readBits(16);
		}
	}
	h265VuiParameters.overscan_info_present_flag = readBits(1);
	if(h265VuiParameters.overscan_info_present_flag){
		h265VuiParameters.overscan_appropriate_flag = readBits(1);
	}
	h265VuiParameters.video_signal_type_present_flag = readBits(1);
	if(h265VuiParameters.video_signal_type_present_flag){
		h265VuiParameters.video_format = readBits(3);
		h265VuiParameters.video_full_range_flag = readBits(1);
		h265VuiParameters.colour_description_present_flag = readBits(1);
		if(h265VuiParameters.colour_description_present_flag){
			h265VuiParameters.colour_primaries = readBits(8);
			h265VuiParameters.transfer_characteristics = readBits(8);
			h265VuiParameters.matrix_coeffs = readBits(8);
		}
	}
	h265VuiParameters.chroma_loc_info_present_flag = readBits(1);
	if(h265VuiParameters.chroma_loc_info_present_flag){
		h265VuiParameters.chroma_sample_loc_type_top_field = readGolombUE();
		h265VuiParameters.chroma_sample_loc_type_bottom_field = readGolombUE();
	}
	h265VuiParameters.neutral_chroma_indication_flag = readBits(1);
	h265VuiParameters.field_seq_flag = readBits(1);
	h265VuiParameters.frame_field_info_present_flag = readBits(1);
	h265VuiParameters.default_display_window_flag = readBits(1);
	if(h265VuiParameters.default_display_window_flag){
		h265VuiParameters.def_disp_win_left_offset = readGolombUE();
		h265VuiParameters.def_disp_win_right_offset = readGolombUE();
		h265VuiParameters.def_disp_win_top_offset = readGolombUE();
		h265VuiParameters.def_disp_win_bottom_offset = readGolombUE();
	}
	h265VuiParameters.vui_timing_info_present_flag = readBits(1);
	if(h265VuiParameters.vui_timing_info_present_flag){
		h265VuiParameters.vui_num_units_in_tick = readBits(32);
		h265VuiParameters.vui_time_scale = readBits(32);
		h265VuiParameters.vui_poc_proportional_to_timing_flag = readBits(1);
		if(h265VuiParameters.vui_poc_proportional_to_timing_flag){
			h265VuiParameters.vui_num_ticks_poc_diff_one_minus1 = readGolombUE();
		}
		h265VuiParameters.vui_hrd_parameters_present_flag = readBits(1);
		if(h265VuiParameters.vui_hrd_parameters_present_flag){
			h265VuiParameters.hrd_parameters = readHrdParameters(1, sps_max_sub_layers_minus1);
		}
	}
	h265VuiParameters.bitstream_restriction_flag = readBits(1);
	if(h265VuiParameters.bitstream_restriction_flag){
		h265VuiParameters.tiles_fixed_structure_flag = readBits(1);
		h265VuiParameters.motion_vectors_over_pic_boundaries_flag = readBits(1);
		h265VuiParameters.restricted_ref_pic_lists_flag = readBits(1);
		h265VuiParameters.min_spatial_segmentation_idc = readGolombUE();
		h265VuiParameters.max_bytes_per_pic_denom = readGolombUE();
		h265VuiParameters.max_bits_per_min_cu_denom = readGolombUE();
		h265VuiParameters.log2_max_mv_length_horizontal = readGolombUE();
		h265VuiParameters.log2_max_mv_length_vertical = readGolombUE();
	}
	return h265VuiParameters;
}

H265SPSRangeExtension H265BitstreamReader::readSPSRangeExtension(){
	H265SPSRangeExtension h265RSPSangeExtension;
	h265RSPSangeExtension.transform_skip_rotation_enabled_flag = readBits(1);
	h265RSPSangeExtension.transform_skip_context_enabled_flag = readBits(1);
	h265RSPSangeExtension.implicit_rdpcm_enabled_flag = readBits(1);
	h265RSPSangeExtension.explicit_rdpcm_enabled_flag = readBits(1);
	h265RSPSangeExtension.extended_precision_processing_flag = readBits(1);
	h265RSPSangeExtension.intra_smoothing_disabled_flag = readBits(1);
	h265RSPSangeExtension.high_precision_offsets_enabled_flag = readBits(1);
	h265RSPSangeExtension.persistent_rice_adaptation_enabled_flag = readBits(1);
	h265RSPSangeExtension.cabac_bypass_alignment_enabled_flag = readBits(1);
	return h265RSPSangeExtension;
}

H265SPSMultilayerExtension H265BitstreamReader::readSPSMultilayerExtension(){
	H265SPSMultilayerExtension h265SPSMultilayerExtension;
	h265SPSMultilayerExtension.inter_view_mv_vert_constraint_flag = readBits(1);
	return h265SPSMultilayerExtension;
}

H265SPS3DExtension H265BitstreamReader::readSPS3DExtension(const H265SPS& h265SPS){
	H265SPS3DExtension h265SPS3DExtension;
	for(int d = 0;d <= 1;++d){
		h265SPS3DExtension.iv_di_mc_enabled_flag[d] = readBits(1);
		h265SPS3DExtension.iv_mv_scal_enabled_flag[d] = readBits(1);
		if(d == 0){
			h265SPS3DExtension.log2_ivmc_sub_pb_size_minus3[d] = readGolombUE();
			h265SPS3DExtension.iv_res_pred_enabled_flag[d] = readBits(1);
			h265SPS3DExtension.depth_ref_enabled_flag[d] = readBits(1);
			h265SPS3DExtension.vsp_mc_enabled_flag[d] = readBits(1);
			h265SPS3DExtension.dbbp_enabled_flag[d] = readBits(1);
		} else {
			h265SPS3DExtension.tex_mc_enabled_flag[d] = readBits(1);
			h265SPS3DExtension.log2_texmc_sub_pb_size_minus3[d] = readBits(1);
			h265SPS3DExtension.intra_contour_enabled_flag[d] = readBits(1);
			h265SPS3DExtension.intra_dc_only_wedge_enabled_flag[d] = readBits(1);
			h265SPS3DExtension.cqt_cu_part_pred_enabled_flag[d] = readBits(1);
			h265SPS3DExtension.inter_dc_only_enabled_flag[d] = readBits(1);
			h265SPS3DExtension.skip_intra_enabled_flag[d] = readBits(1);

			h265SPS3DExtension.log2_ivmc_sub_pb_size_minus3[d] = h265SPS.CtbLog2SizeY-3;
		}
	}
	return h265SPS3DExtension;
}

H265SPSSCCExtension H265BitstreamReader::readSPSSCCExtension(const H265SPS& h265SPS){
	H265SPSSCCExtension h265SPSSCCExtension;
	h265SPSSCCExtension.sps_curr_pic_ref_enabled_flag = readBits(1);
	h265SPSSCCExtension.palette_mode_enabled_flag = readBits(1);
	if(h265SPSSCCExtension.palette_mode_enabled_flag){
		h265SPSSCCExtension.palette_max_size = readGolombUE();
		h265SPSSCCExtension.delta_palette_max_predictor_size = readGolombUE();
		h265SPSSCCExtension.sps_palette_predictor_initializers_present_flag = readBits(1);
		if(h265SPSSCCExtension.sps_palette_predictor_initializers_present_flag){
			uint8_t numComps = (h265SPS.chroma_format_idc == 0) ? 1 : 3;
			for(int comp = 0;comp < numComps;++comp){
				for(uint16_t i = 0;i <= h265SPSSCCExtension.sps_num_palette_predictor_initializers_minus1;++i){
					h265SPSSCCExtension.sps_palette_predictor_initializer[comp][i] = readBits(comp == 0 ? h265SPS.BitDepthY : h265SPS.BitDepthC);
				}
			}
		}
	}
	h265SPSSCCExtension.motion_vector_resolution_control_idc = readBits(2);
	h265SPSSCCExtension.intra_boundary_filtering_disabled_flag = readBits(1);
	return h265SPSSCCExtension;
}

H265PPSRangeExtension H265BitstreamReader::readH265PPSRangeExtension(const H265PPS& h265PPS){
	H265PPSRangeExtension h265PPSRangeExtension;
	if(h265PPS.transform_skip_enabled_flag) h265PPSRangeExtension.log2_max_transform_skip_block_size_minus2 = readGolombUE();
	h265PPSRangeExtension.cross_component_prediction_enabled_flag = readBits(1);
	h265PPSRangeExtension.chroma_qp_offset_list_enabled_flag = readBits(1);
	if(h265PPSRangeExtension.chroma_qp_offset_list_enabled_flag){
		h265PPSRangeExtension.diff_cu_chroma_qp_offset_depth = readGolombUE();
		h265PPSRangeExtension.chroma_qp_offset_list_len_minus1 = readGolombUE();
		for(uint8_t i = 0;i <= h265PPSRangeExtension.chroma_qp_offset_list_len_minus1;++i){
			h265PPSRangeExtension.cb_qp_offset_list.push_back(readGolombSE());
			h265PPSRangeExtension.cr_qp_offset_list.push_back(readGolombSE());
		}
	}
	h265PPSRangeExtension.log2_sao_offset_scale_luma = readGolombUE();
	h265PPSRangeExtension.log2_sao_offset_scale_chroma = readGolombUE();
	return h265PPSRangeExtension;
}

H265PPSColourMappingOctants H265BitstreamReader::readH265PPSColorMappingOctants(uint8_t inpDepth, uint8_t idxY, uint8_t idxCb, uint8_t idxCr, uint8_t inpLength, const H265PPSColourMappingTable& h265PPSColorMappingTable){
	H265PPSColourMappingOctants h265PPSColourMappingOctants;
	if(inpDepth < h265PPSColorMappingTable.cm_octant_depth) h265PPSColourMappingOctants.split_octant_flag = readBits(1);
	if(h265PPSColourMappingOctants.split_octant_flag){
		uint8_t half_inpLength = inpLength/2;
		h265PPSColourMappingOctants.sub_color_mapping_octants.resize(2);
		for(int k = 0;k < 2;++k){
			h265PPSColourMappingOctants.sub_color_mapping_octants[k].resize(2);
			for(int m = 0;m < 2;++m){
				h265PPSColourMappingOctants.sub_color_mapping_octants[k][m].resize(2);
				for(int n = 0;n < 2;++n){
					h265PPSColourMappingOctants.sub_color_mapping_octants[k][m][n] = readH265PPSColorMappingOctants(inpDepth+1, idxY+h265PPSColorMappingTable.PartNumY*k*half_inpLength, 
																					idxCb + m*half_inpLength, idxCr + n*half_inpLength, half_inpLength, h265PPSColorMappingTable);
				}
			}
		}
	} else {
		for(uint8_t i = 0;i < h265PPSColorMappingTable.PartNumY;++i){
			for(int j = 0;j < 4;++j){
				h265PPSColourMappingOctants.coded_res_flag[j] = readBits(1);
				if(h265PPSColourMappingOctants.coded_res_flag[i]){
					for(int c = 0;c < 3;++c){
						h265PPSColourMappingOctants.res_coeff_q[j][c] = readGolombUE();
						h265PPSColourMappingOctants.res_coeff_r[j][c] = readBits(h265PPSColorMappingTable.CMResLSBits);
						if(h265PPSColourMappingOctants.res_coeff_q[j][c] || h265PPSColourMappingOctants.res_coeff_r[j][c]){
							h265PPSColourMappingOctants.res_coeff_s[j][c] = readBits(1);
						}
					}
				}
			}
		}
	}
	return h265PPSColourMappingOctants;
}

H265PPSColourMappingTable H265BitstreamReader::readH265PPSColorMappingTable(){
	H265PPSColourMappingTable h265PPSColourMappingTable;
	h265PPSColourMappingTable.num_cm_ref_layers_minus1 = readGolombUE();
	for(uint8_t i = 0;i <= h265PPSColourMappingTable.num_cm_ref_layers_minus1;++i) h265PPSColourMappingTable.cm_ref_layer_id[i] = readBits(6);
	h265PPSColourMappingTable.cm_octant_depth = readBits(2);
	h265PPSColourMappingTable.cm_y_part_num_log2 = readBits(2);
	h265PPSColourMappingTable.luma_bit_depth_cm_input_minus8 = readGolombUE();
	h265PPSColourMappingTable.chroma_bit_depth_cm_input_minus8 = readGolombUE();
	h265PPSColourMappingTable.luma_bit_depth_cm_output_minus8 = readGolombUE();
	h265PPSColourMappingTable.chroma_bit_depth_cm_output_minus8 = readGolombUE();
	h265PPSColourMappingTable.cm_res_quant_bits = readBits(2);
	h265PPSColourMappingTable.cm_delta_flc_bits_minus1 = readBits(2);
	if(h265PPSColourMappingTable.cm_octant_depth == 1){
		h265PPSColourMappingTable.cm_adapt_threshhold_u_delta = readGolombSE();
		h265PPSColourMappingTable.cm_adapt_threshhold_v_delta = readGolombSE();
	}

	h265PPSColourMappingTable.OctantNumC = 1 << h265PPSColourMappingTable.cm_octant_depth;
	h265PPSColourMappingTable.OctantNumY = 1 << (h265PPSColourMappingTable.cm_octant_depth + h265PPSColourMappingTable.cm_y_part_num_log2);
	h265PPSColourMappingTable.PartNumY = 1 << h265PPSColourMappingTable.cm_y_part_num_log2;
	h265PPSColourMappingTable.BitDepthCmInputY = 8 + h265PPSColourMappingTable.luma_bit_depth_cm_input_minus8;
	h265PPSColourMappingTable.BitDepthCmInputC = 8 + h265PPSColourMappingTable.chroma_bit_depth_cm_input_minus8;
	h265PPSColourMappingTable.BitDepthCmOutputY = 8 + h265PPSColourMappingTable.luma_bit_depth_cm_output_minus8;
	h265PPSColourMappingTable.BitDepthCmOutputC = 8 + h265PPSColourMappingTable.chroma_bit_depth_cm_output_minus8;
	h265PPSColourMappingTable.CMResLSBits = std::max(0, (10+h265PPSColourMappingTable.BitDepthCmInputY-h265PPSColourMappingTable.BitDepthCmOutputY-
											h265PPSColourMappingTable.cm_res_quant_bits-(h265PPSColourMappingTable.cm_delta_flc_bits_minus1+1)));
	h265PPSColourMappingTable.CMThreshU = h265PPSColourMappingTable.cm_adapt_threshhold_u_delta + (1 << (h265PPSColourMappingTable.BitDepthCmInputC-1));
	h265PPSColourMappingTable.CMThreshV = h265PPSColourMappingTable.cm_adapt_threshhold_v_delta + (1 << (h265PPSColourMappingTable.BitDepthCmInputC-1));

	h265PPSColourMappingTable.colour_mapping_octants = readH265PPSColorMappingOctants(0, 0, 0, 0, 1 << h265PPSColourMappingTable.cm_octant_depth, h265PPSColourMappingTable);
	return h265PPSColourMappingTable;
}

H265PPSMultilayerExtension H265BitstreamReader::readH265PPSMultilayerExtension(){
	H265PPSMultilayerExtension h265PPSMultilayerExtension;
	h265PPSMultilayerExtension.poc_reset_info_present_flag = readBits(1);
	h265PPSMultilayerExtension.pps_infer_scaling_list_flag = readBits(1);
	if(h265PPSMultilayerExtension.pps_infer_scaling_list_flag) h265PPSMultilayerExtension.pps_scaling_list_ref_layer_id = readBits(6);
	h265PPSMultilayerExtension.num_ref_loc_offsets = readGolombUE();
	for(uint8_t i = 0;i < h265PPSMultilayerExtension.num_ref_loc_offsets;++i){
		h265PPSMultilayerExtension.ref_loc_offset_layer_id.push_back(readBits(6));
		h265PPSMultilayerExtension.scaled_ref_layer_offset_present_flag.push_back(readBits(1));
		if(h265PPSMultilayerExtension.scaled_ref_layer_offset_present_flag[i]){
			h265PPSMultilayerExtension.scaled_ref_layer_left_offset[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombSE();
			h265PPSMultilayerExtension.scaled_ref_layer_top_offset[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombSE();
			h265PPSMultilayerExtension.scaled_ref_layer_right_offset[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombSE();
			h265PPSMultilayerExtension.scaled_ref_layer_bottom_offset[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombSE();
		}
		h265PPSMultilayerExtension.ref_region_offset_present_flag.push_back(readBits(1));
		if(h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]){
			h265PPSMultilayerExtension.ref_region_left_offset[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombSE();
			h265PPSMultilayerExtension.ref_region_top_offset[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombSE();
			h265PPSMultilayerExtension.ref_region_right_offset[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombSE();
			h265PPSMultilayerExtension.ref_region_bottom_offset[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombSE();
		}
		h265PPSMultilayerExtension.resample_phase_set_present_flag.push_back(readBits(1));
		if(h265PPSMultilayerExtension.resample_phase_set_present_flag[i]){
			h265PPSMultilayerExtension.phase_hor_luma[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombUE();
			h265PPSMultilayerExtension.phase_ver_luma[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombUE();
			h265PPSMultilayerExtension.phase_hor_chroma_plus8[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombUE();
			h265PPSMultilayerExtension.phase_ver_chroma_plus8[h265PPSMultilayerExtension.ref_loc_offset_layer_id[i]] = readGolombUE();
		}
	}
	h265PPSMultilayerExtension.colour_mapping_enabled_flag = readBits(1);
	if(h265PPSMultilayerExtension.colour_mapping_enabled_flag) h265PPSMultilayerExtension.colour_mapping_table = readH265PPSColorMappingTable();
	return h265PPSMultilayerExtension;
}

H265PPSDeltaLookupTable H265BitstreamReader::readH265PPSDeltaLookupTable(const H265PPS3DExtension& h265PPS3DExtension){
	H265PPSDeltaLookupTable h265PPSDeltaLookupTable;
	h265PPSDeltaLookupTable.num_val_delta_dlt = readBits(h265PPS3DExtension.pps_bit_depth_for_depth_layers_minus8+8);
	if(h265PPSDeltaLookupTable.num_val_delta_dlt > 0){
		if(h265PPSDeltaLookupTable.num_val_delta_dlt > 1){
			h265PPSDeltaLookupTable.max_diff = readBits(h265PPS3DExtension.pps_bit_depth_for_depth_layers_minus8+8);
		}
		if(h265PPSDeltaLookupTable.num_val_delta_dlt > 2 && h265PPSDeltaLookupTable.max_diff > 0){
			h265PPSDeltaLookupTable.min_diff_minus1 = readBits(ceil(log2(h265PPSDeltaLookupTable.max_diff+1)));
		} else h265PPSDeltaLookupTable.min_diff_minus1 = h265PPSDeltaLookupTable.max_diff-1;
		h265PPSDeltaLookupTable.delta_dlt_val0 = readBits(h265PPS3DExtension.pps_bit_depth_for_depth_layers_minus8+8);
		if(h265PPSDeltaLookupTable.max_diff > (h265PPSDeltaLookupTable.min_diff_minus1+1)){
			uint8_t delta_val_diff_size = ceil(log2(h265PPSDeltaLookupTable.max_diff-h265PPSDeltaLookupTable.min_diff_minus1+2));
			for(int k = 1;k < h265PPSDeltaLookupTable.num_val_delta_dlt;++k){
				h265PPSDeltaLookupTable.delta_val_diff_minus_min.push_back(readBits(delta_val_diff_size));
			}
		} else {
			for(int k = 1;k < h265PPSDeltaLookupTable.num_val_delta_dlt;++k){
				h265PPSDeltaLookupTable.delta_val_diff_minus_min.push_back(0);
			}
		}
	}
	return h265PPSDeltaLookupTable;
}

H265PPS3DExtension H265BitstreamReader::readH265PPS3DExtension(){
	H265PPS3DExtension h265PPS3DExtension;
	h265PPS3DExtension.dlts_present_flag = readBits(1);
	if(h265PPS3DExtension.dlts_present_flag){
		h265PPS3DExtension.pps_depth_layers_minus1 = readBits(6);
		h265PPS3DExtension.pps_bit_depth_for_depth_layers_minus8 = readBits(4);
		uint32_t depthMaxValue =  (1 << (h265PPS3DExtension.pps_bit_depth_for_depth_layers_minus8+8))-1;
		for(uint8_t i = 0;i <= h265PPS3DExtension.pps_depth_layers_minus1;++i){
			h265PPS3DExtension.dlt_flag[i] = readBits(1);
			if(h265PPS3DExtension.dlt_flag[i]){
				h265PPS3DExtension.dlt_pred_flag[i] = readBits(1);
				if(!h265PPS3DExtension.dlt_pred_flag[i]) h265PPS3DExtension.dlt_val_flags_present_flag[i] = readBits(1);
				if(h265PPS3DExtension.dlt_val_flags_present_flag[i]){
					for(uint32_t j = 0;j <= depthMaxValue;++j) h265PPS3DExtension.dlt_value_flag[i].push_back(readBits(1));
				} else h265PPS3DExtension.delta_dlt[i] = readH265PPSDeltaLookupTable(h265PPS3DExtension);
			}
		}
	}
	return h265PPS3DExtension;
}

H265PPSSCCExtension H265BitstreamReader::readH265PPSSCCExtension(){
	H265PPSSCCExtension h265PPSSCCExtension;
	h265PPSSCCExtension.pps_curr_pic_ref_enabled_flag = readBits(1);
	h265PPSSCCExtension.residual_adaptive_colour_transform_enabled_flag = readBits(1);
	if(h265PPSSCCExtension.residual_adaptive_colour_transform_enabled_flag){
		h265PPSSCCExtension.pps_slice_act_qp_offsets_present_flag = readBits(1);
		h265PPSSCCExtension.pps_act_y_qp_offset_plus5 = readGolombSE();
		h265PPSSCCExtension.pps_act_cb_qp_offset_plus5 = readGolombSE();
		h265PPSSCCExtension.pps_act_cr_qp_offset_plus3 = readGolombSE();
	}
	h265PPSSCCExtension.pps_palette_predictor_initializers_present_flag = readBits(1);
	if(h265PPSSCCExtension.pps_palette_predictor_initializers_present_flag){
		h265PPSSCCExtension.pps_num_palette_predictor_initializer = readGolombUE();
		if(h265PPSSCCExtension.pps_num_palette_predictor_initializer > 0){
			h265PPSSCCExtension.monochrome_palette_flag = readBits(1);
			h265PPSSCCExtension.luma_bit_depth_entry_minus8 = readGolombUE();
			if(!h265PPSSCCExtension.monochrome_palette_flag) h265PPSSCCExtension.chroma_bit_depth_entry_minus8 = readGolombUE();
			uint8_t numComps = h265PPSSCCExtension.monochrome_palette_flag ? 1 : 3;
			for(uint8_t comp = 0;comp < numComps;++comp){
				uint8_t readBitsLength = (comp == 0) ? h265PPSSCCExtension.luma_bit_depth_entry_minus8+8 : h265PPSSCCExtension.chroma_bit_depth_entry_minus8+8;
				for(uint32_t i = 0;i < h265PPSSCCExtension.pps_num_palette_predictor_initializer;++i){
					h265PPSSCCExtension.pps_palette_predictor_initializer[comp].push_back(readBits(readBitsLength));
				}
			}
		}
	}
	return h265PPSSCCExtension;
}

void H265BitstreamReader::computePOC(H265Slice& h265Slice, std::vector<H265AccessUnit*> pAccessUnits){
	if(!h265Slice.first_slice_segment_in_pic_flag) {
		h265Slice.PicOrderCntVal = pAccessUnits.back()->PicOrderCntVal;
		return;
	}
	H265SPS* pCurrentSPS = h265Slice.getSPS();
	if(h265Slice.isIRAP() && h265Slice.NoRaslOutputFlag){
		h265Slice.PicOrderCntMsb = 0;
	} else {
		H265AccessUnit* prevTid0Pic = nullptr;
		for(auto accessUnitIt = pAccessUnits.rbegin() + (pAccessUnits.back()->slice() ? 0 : 1);accessUnitIt != pAccessUnits.rend() && !prevTid0Pic;++accessUnitIt){
			H265AccessUnit* pAccessUnit = *accessUnitIt;
			H265Slice* pSlice = pAccessUnit->slice();
			if(pSlice && pSlice->TemporalId == 0 && !pAccessUnit->isRASL() && !pAccessUnit->isRADL() && !pAccessUnit->isSLNR()) prevTid0Pic = pAccessUnit;
		}
		if(!prevTid0Pic){
			std::cerr << "[Stream] Couldn't compute POC for non-initial IRAP picture : missing prevTid0Pic\n";
			return;
		}
		uint32_t prevPicOrderCntLsb = prevTid0Pic->slice()->slice_pic_order_cnt_lsb;
		int32_t prevPicOrderCntMsb = prevTid0Pic->PicOrderCntMsb;
		if((h265Slice.slice_pic_order_cnt_lsb < prevPicOrderCntLsb) &&
		   ((prevPicOrderCntLsb-h265Slice.slice_pic_order_cnt_lsb) >= (pCurrentSPS->MaxPicOrderCntLsb/2))){
			h265Slice.PicOrderCntMsb = prevPicOrderCntMsb + pCurrentSPS->MaxPicOrderCntLsb;
		} else if((h265Slice.slice_pic_order_cnt_lsb > prevPicOrderCntLsb) &&
		          ((h265Slice.slice_pic_order_cnt_lsb - prevPicOrderCntLsb) > (pCurrentSPS->MaxPicOrderCntLsb/2))){
			h265Slice.PicOrderCntMsb = prevPicOrderCntMsb - pCurrentSPS->MaxPicOrderCntLsb;
		} else h265Slice.PicOrderCntMsb = prevPicOrderCntMsb;
	}
	h265Slice.PicOrderCntVal = h265Slice.PicOrderCntMsb + h265Slice.slice_pic_order_cnt_lsb;
}

void H265BitstreamReader::computeRPL(H265Slice& h265Slice, std::vector<H265AccessUnit*> pAccessUnits, H265AccessUnit* pNextAccessUnit){
	H265AccessUnit* pSliceAccessUnit = pAccessUnits.back()->slice() ? pNextAccessUnit : pAccessUnits.back();
	H265SPS* pCurrentSPS = h265Slice.getSPS();
	if(h265Slice.isIRAP() && h265Slice.NoRaslOutputFlag){
		for(auto accessUnitIt = pAccessUnits.rbegin() + (pAccessUnits.back()->slice() ? 0 : 1);accessUnitIt != pAccessUnits.rend();++accessUnitIt){
			if((*accessUnitIt)->slice()->nuh_layer_id == h265Slice.nuh_layer_id) (*accessUnitIt)->status = H265AccessUnit::ReferenceStatus_Unused;
		}
	}

	if(!h265Slice.isIDR()){
		if(!pCurrentSPS) {
			return;
		}
		H265ShortTermRefPicSet currRPS = pCurrentSPS->short_term_ref_pic_set[h265Slice.CurrRpsIdx];
		for(uint32_t i = 0;i < currRPS.NumNegativePics;++i){
			if(currRPS.UsedByCurrPicS0[i]) h265Slice.PocStCurrBefore.push_back(h265Slice.PicOrderCntVal + currRPS.DeltaPocS0[i]);
			else h265Slice.PocStFoll.push_back(h265Slice.PicOrderCntVal + currRPS.DeltaPocS0[i]);
		}
		for(uint32_t i = 0;i < currRPS.NumPositivePics;++i){
			if(currRPS.UsedByCurrPicS1[i]) h265Slice.PocStCurrAfter.push_back(h265Slice.PicOrderCntVal + currRPS.DeltaPocS1[i]);
			else h265Slice.PocStFoll.push_back(h265Slice.PicOrderCntVal + currRPS.DeltaPocS1[i]);
		}
		for(uint32_t i = 0;i < h265Slice.num_long_term_sps + h265Slice.num_long_term_pics;++i){
			uint32_t pocLt = h265Slice.PocLsbLt[i];
			if(h265Slice.delta_poc_msb_present_flag[i]){
				pocLt += h265Slice.PicOrderCntVal - h265Slice.DeltaPocMsbCycleLt[i]*pCurrentSPS->MaxPicOrderCntLsb - 
				(h265Slice.PicOrderCntVal & (pCurrentSPS->MaxPicOrderCntLsb-1));
			}
			if(h265Slice.UsedByCurrPicLt[i]){
				h265Slice.PocLtCurr.push_back(pocLt);
				h265Slice.CurrDeltaPocMsbPresentFlag.push_back(h265Slice.delta_poc_msb_present_flag[i]);
			} else {
				h265Slice.PocLtFoll.push_back(pocLt);
				h265Slice.FollDeltaPocMsbPresentFlag.push_back(h265Slice.delta_poc_msb_present_flag[i]);
			}
		}
	}
	std::vector<H265AccessUnit*> RefPicSetStCurrBefore;
    std::vector<H265AccessUnit*> RefPicSetStCurrAfter;
    std::vector<H265AccessUnit*> RefPicSetStFoll;
    std::vector<H265AccessUnit*> RefPicSetLtCurr;
    std::vector<H265AccessUnit*> RefPicSetLtFoll;
	// TODO: double check if all pictures in the DPB are eligible to be used as reference pictures
	RefPicSetLtCurr.resize(h265Slice.PocLtCurr.size());
	for(uint32_t i = 0;i < h265Slice.PocLtCurr.size();++i){
		RefPicSetLtCurr[i] = nullptr;
		if(!h265Slice.CurrDeltaPocMsbPresentFlag[i]){
			for(auto accessUnitIt = pAccessUnits.rbegin();accessUnitIt != pAccessUnits.rend() && !RefPicSetLtCurr[i];++accessUnitIt){
				if((int)((*accessUnitIt)->PicOrderCntVal & (pCurrentSPS->MaxPicOrderCntLsb-1)) == h265Slice.PocLtCurr[i] &&
					(*accessUnitIt)->slice()->nuh_layer_id == h265Slice.nuh_layer_id){
					RefPicSetLtCurr[i] = *accessUnitIt;
					RefPicSetLtCurr[i]->status = H265AccessUnit::ReferenceStatus_LongTerm;
				}
			}
		} else {
			for(auto accessUnitIt = pAccessUnits.rbegin();accessUnitIt != pAccessUnits.rend() && !RefPicSetLtCurr[i];++accessUnitIt){
				if((*accessUnitIt)->PicOrderCntVal == h265Slice.PocLtCurr[i] &&
					(*accessUnitIt)->slice()->nuh_layer_id == h265Slice.nuh_layer_id){
					RefPicSetLtCurr[i] = *accessUnitIt;
					RefPicSetLtCurr[i]->status = H265AccessUnit::ReferenceStatus_LongTerm;
				}
			}
		}
	}
	RefPicSetLtFoll.resize(h265Slice.PocLtFoll.size());
	for(uint32_t i = 0;i < h265Slice.PocLtFoll.size();++i){
		RefPicSetLtFoll[i] = nullptr;
		if(!h265Slice.FollDeltaPocMsbPresentFlag[i]){
			for(auto accessUnitIt = pAccessUnits.rbegin();accessUnitIt != pAccessUnits.rend() && !RefPicSetLtFoll[i];++accessUnitIt){
				if((int)((*accessUnitIt)->PicOrderCntVal & (pCurrentSPS->MaxPicOrderCntLsb-1)) == h265Slice.PocLtFoll[i] &&
					(*accessUnitIt)->slice()->nuh_layer_id == h265Slice.nuh_layer_id){
					RefPicSetLtFoll[i] = *accessUnitIt;
					RefPicSetLtFoll[i]->status = H265AccessUnit::ReferenceStatus_LongTerm;
				}
			}
		} else {
			for(auto accessUnitIt = pAccessUnits.rbegin();accessUnitIt != pAccessUnits.rend() && !RefPicSetLtFoll[i];++accessUnitIt){
				if((*accessUnitIt)->PicOrderCntVal == h265Slice.PocLtFoll[i] &&
					(*accessUnitIt)->slice()->nuh_layer_id == h265Slice.nuh_layer_id){
					RefPicSetLtFoll[i] = *accessUnitIt;
					RefPicSetLtFoll[i]->status = H265AccessUnit::ReferenceStatus_LongTerm;
				}
			}
		}
	}

	RefPicSetStCurrBefore.resize(h265Slice.PocStCurrBefore.size());
	for(uint32_t i = 0;i < h265Slice.PocStCurrBefore.size();++i){
		RefPicSetStCurrBefore[i] = nullptr;
		for(auto accessUnitIt = pAccessUnits.rbegin();accessUnitIt != pAccessUnits.rend() && !RefPicSetStCurrBefore[i];++accessUnitIt){
			if((*accessUnitIt)->PicOrderCntVal == h265Slice.PocStCurrBefore[i] && (*accessUnitIt)->slice()->nuh_layer_id == h265Slice.nuh_layer_id){
				RefPicSetStCurrBefore[i] = *accessUnitIt;
				RefPicSetStCurrBefore[i]->status = H265AccessUnit::ReferenceStatus_ShortTerm;
			}
		}
	}
	RefPicSetStCurrAfter.resize(h265Slice.PocStCurrAfter.size());
	for(uint32_t i = 0;i < h265Slice.PocStCurrAfter.size();++i){
		RefPicSetStCurrAfter[i] = nullptr;
		for(auto accessUnitIt = pAccessUnits.rbegin();accessUnitIt != pAccessUnits.rend() && !RefPicSetStCurrAfter[i];++accessUnitIt){
			if((*accessUnitIt)->PicOrderCntVal == h265Slice.PocStCurrAfter[i] && (*accessUnitIt)->slice()->nuh_layer_id == h265Slice.nuh_layer_id){
				RefPicSetStCurrAfter[i] = *accessUnitIt;
				RefPicSetStCurrAfter[i]->status = H265AccessUnit::ReferenceStatus_ShortTerm;
			}
		}
	}
	RefPicSetStFoll.resize(h265Slice.PocStFoll.size());
	for(uint32_t i = 0;i < h265Slice.PocStFoll.size();++i){
		RefPicSetStFoll[i] = nullptr;
		for(auto accessUnitIt = pAccessUnits.rbegin();accessUnitIt != pAccessUnits.rend() && !RefPicSetStFoll[i];++accessUnitIt){
			if((*accessUnitIt)->PicOrderCntVal == h265Slice.PocStFoll[i] && (*accessUnitIt)->slice()->nuh_layer_id == h265Slice.nuh_layer_id){
				RefPicSetStFoll[i] = *accessUnitIt;
				RefPicSetStFoll[i]->status = H265AccessUnit::ReferenceStatus_ShortTerm;
			}
		}
	}

	H265PPS* h265PPS = h265Slice.getPPS();
	// 8.3.4 Decoding process for reference picture lists construction
	uint32_t NumRpsCurrTempList0 = std::max(h265Slice.num_ref_idx_l0_active_minus1+1, h265Slice.NumPicTotalCurr);
	uint32_t rIdx = 0;
	std::vector<H265AccessUnit*> RefPicListTemp0;
	if(h265Slice.slice_type == H265Slice::SliceType_P || h265Slice.slice_type == H265Slice::SliceType_B) {
		while (rIdx < NumRpsCurrTempList0) {
			if(h265Slice.PocStCurrBefore.size() + h265Slice.PocStCurrAfter.size() + h265Slice.PocLtCurr.size() == 0 && !h265PPS->pps_scc_extension.pps_curr_pic_ref_enabled_flag) {
				break;
			}
			for (uint32_t i = 0; i < h265Slice.PocStCurrBefore.size() && rIdx < NumRpsCurrTempList0; rIdx++, i++) {
				RefPicListTemp0.push_back(RefPicSetStCurrBefore[i]);
			}
			for (uint32_t i = 0; i < h265Slice.PocStCurrAfter.size() && rIdx < NumRpsCurrTempList0; rIdx++, i++) {
				RefPicListTemp0.push_back(RefPicSetStCurrAfter[i]);
			}
			for (uint32_t i = 0; i < h265Slice.PocLtCurr.size() && rIdx < NumRpsCurrTempList0; rIdx++, i++) {
				RefPicListTemp0.push_back(RefPicSetLtCurr[i]);
			}
			if (h265PPS->pps_scc_extension.pps_curr_pic_ref_enabled_flag) {
				RefPicListTemp0.push_back(pSliceAccessUnit);
				++rIdx;
			}
		}
		for (rIdx = 0; rIdx <= h265Slice.num_ref_idx_l0_active_minus1 && !RefPicListTemp0.empty(); rIdx++) {
			H265AccessUnit* val = h265Slice.ref_pic_lists_modification.ref_pic_list_modification_flag_l0
					  ? RefPicListTemp0[h265Slice.ref_pic_lists_modification.list_entry_l0[rIdx]] : RefPicListTemp0[rIdx];
			h265Slice.RefPicList0.push_back(val);
		}
		if( h265PPS->pps_scc_extension.pps_curr_pic_ref_enabled_flag && !h265Slice.ref_pic_lists_modification.ref_pic_list_modification_flag_l0 &&
			NumRpsCurrTempList0 > (h265Slice.num_ref_idx_l0_active_minus1 + 1 ) ) {
			h265Slice.RefPicList0[h265Slice.num_ref_idx_l0_active_minus1] = pSliceAccessUnit;
		}
	}

	std::vector<H265AccessUnit*> RefPicListTemp1;
	uint32_t NumRpsCurrTempList1 = std::max(h265Slice.num_ref_idx_l1_active_minus1+1, h265Slice.NumPicTotalCurr);
	if(h265Slice.slice_type == H265Slice::SliceType_B) {
		rIdx = 0;
		while (rIdx < NumRpsCurrTempList1) {
			if(h265Slice.PocStCurrAfter.size() + h265Slice.PocStCurrBefore.size() + h265Slice.PocLtCurr.size() == 0 && !h265PPS->pps_scc_extension.pps_curr_pic_ref_enabled_flag) break;
			for (uint32_t i = 0; i < h265Slice.PocStCurrAfter.size() && rIdx < NumRpsCurrTempList1; rIdx++, i++) {
				RefPicListTemp1.push_back(RefPicSetStCurrAfter[i]);
			}
			for (uint32_t i = 0; i < h265Slice.PocStCurrBefore.size() && rIdx < NumRpsCurrTempList1; rIdx++, i++) {
				RefPicListTemp1.push_back(RefPicSetStCurrBefore[i]);
			}
			for (uint32_t i = 0; i < h265Slice.PocLtCurr.size() && rIdx < NumRpsCurrTempList1; rIdx++, i++) {
				RefPicListTemp1.push_back(RefPicSetLtCurr[i]);
			}
			if (h265PPS->pps_scc_extension.pps_curr_pic_ref_enabled_flag) {
				RefPicListTemp1.push_back(pSliceAccessUnit);
				++rIdx;
			}
		}
		for (rIdx = 0; rIdx <= h265Slice.num_ref_idx_l1_active_minus1 && !RefPicListTemp1.empty(); rIdx++)
		{
			H265AccessUnit* val = h265Slice.ref_pic_lists_modification.ref_pic_list_modification_flag_l1 ? RefPicListTemp1[h265Slice.ref_pic_lists_modification.list_entry_l1[rIdx]] : RefPicListTemp1[rIdx];
			h265Slice.RefPicList1.push_back(val);
		}
	}
}

H265PredWeightTable H265BitstreamReader::readSlicePredWeightTable(const H265Slice& h265Slice){
	H265PredWeightTable h265PredWeightTable;
	h265PredWeightTable.luma_log2_weight_denom = readGolombUE();
	H265SPS* h265SPS = h265Slice.getSPS();
	if(!h265SPS){
		h265PredWeightTable.errors.add(H26XError::Major, "[Slice PWT] missing SPS unit : unable to parse pred_weight_table");
		return h265PredWeightTable;
	}
	if(h265SPS->ChromaArrayType != 0) h265PredWeightTable.delta_chroma_log2_weight_denom = readGolombSE();
	for(uint32_t i = 0;i <= h265Slice.num_ref_idx_l0_active_minus1;++i){
		if(!h265Slice.RefPicList0[i]){
			continue;
		}
		if((h265Slice.RefPicList0[i]->slice() && h265Slice.RefPicList0[i]->slice()->nuh_layer_id != h265Slice.nuh_layer_id) || (h265Slice.RefPicList0[i]->PicOrderCntVal != h265Slice.PicOrderCntVal))
		{
			h265PredWeightTable.luma_weight_l0_flag[i] = readBits(1);
		}
	}
	if(h265SPS->ChromaArrayType != 0){
		for(uint32_t i = 0;i <= h265Slice.num_ref_idx_l0_active_minus1;++i){
			if(!h265Slice.RefPicList0[i]){
				continue;
			}
			if((h265Slice.RefPicList0[i]->slice() && h265Slice.RefPicList0[i]->slice()->nuh_layer_id != h265Slice.nuh_layer_id) || (h265Slice.RefPicList0[i]->PicOrderCntVal != h265Slice.PicOrderCntVal)){
				h265PredWeightTable.chroma_weight_l0_flag[i] = readBits(1);
			}
		}
	}
	for(uint32_t i = 0;i <= h265Slice.num_ref_idx_l0_active_minus1;++i){
		if(h265PredWeightTable.luma_weight_l0_flag[i]){
			h265PredWeightTable.delta_luma_weight_l0[i] = readGolombSE();
			h265PredWeightTable.luma_offset_l0[i] = readGolombSE();
		}
		if(h265PredWeightTable.chroma_weight_l0_flag[i]){
			for(int j = 0;j < 2;++j){
				h265PredWeightTable.delta_chroma_weight_l0[i][j] = readGolombSE();
				h265PredWeightTable.delta_chroma_offset_l0[i][j] = readGolombSE();
			}
		}
	}
	if(h265Slice.slice_type == H265Slice::SliceType_B){
		for(uint32_t i = 0;i <= h265Slice.num_ref_idx_l1_active_minus1;++i){
			if(!h265Slice.RefPicList0[i]){
				continue;
			}
			if((h265Slice.RefPicList0[i]->slice() && h265Slice.RefPicList0[i]->slice()->nuh_layer_id != h265Slice.nuh_layer_id) || (h265Slice.RefPicList1[i] && h265Slice.RefPicList1[i]->PicOrderCntVal != h265Slice.PicOrderCntVal)){
				h265PredWeightTable.luma_weight_l1_flag[i] = readBits(1);
			}
		}
		
		if(h265SPS->ChromaArrayType != 0){
			for(uint32_t i = 0;i <= h265Slice.num_ref_idx_l1_active_minus1;++i){
				if(!h265Slice.RefPicList0[i]){
					continue;
				}
				if((h265Slice.RefPicList0[i]->slice() && h265Slice.RefPicList0[i]->slice()->nuh_layer_id != h265Slice.nuh_layer_id) || (h265Slice.RefPicList1[i] && h265Slice.RefPicList1[i]->PicOrderCntVal != h265Slice.PicOrderCntVal)){
					h265PredWeightTable.chroma_weight_l1_flag[i] = readBits(1);
				}
			}
		}
		for(uint32_t i = 0;i <= h265Slice.num_ref_idx_l1_active_minus1;++i){
			if(h265PredWeightTable.luma_weight_l1_flag[i]){
				h265PredWeightTable.delta_luma_weight_l1[i] = readGolombSE();
				h265PredWeightTable.luma_offset_l1[i] = readGolombSE();
			}
			if(h265PredWeightTable.chroma_weight_l1_flag[i]){
				for(int j = 0;j < 2;++j){
					h265PredWeightTable.delta_chroma_weight_l1[i][j] = readGolombSE();
					h265PredWeightTable.delta_chroma_offset_l1[i][j] = readGolombSE();
				}
			}
		}
	}
	return h265PredWeightTable;
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

void H265BitstreamReader::readSEI(H265SEI& h265SEI){
	do {
		uint64_t payloadType = 0;
		uint8_t last_payload_type_byte = readBits(8);
		while(last_payload_type_byte == 0xFF){
			payloadType += last_payload_type_byte;
			last_payload_type_byte = readBits(8);
		}
		payloadType += last_payload_type_byte;
		uint64_t payloadSize = 0;
		uint8_t last_payload_size_byte = readBits(8);
		while(last_payload_size_byte == 0xFF){
			payloadSize += last_payload_size_byte;
			last_payload_size_byte = readBits(8);
		}
		payloadSize += last_payload_size_byte;
		if(payloadSize*8 > (uint64_t)m_iRemainingBits) throw std::runtime_error("Payload size exceeds remaining bitstream length left");
		switch(payloadType){
			default:
				std::cerr << "Unsupported SEI message type : " << payloadType << "\n";
				skipBits(8*payloadSize);
		}
		uint8_t byte_bit_offset = m_iBitsOffset%8;
		if(byte_bit_offset != 0) skipBits(8-byte_bit_offset);
	} while(hasMoreRBSPData());
}
