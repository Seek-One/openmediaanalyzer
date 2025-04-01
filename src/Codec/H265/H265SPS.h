#ifndef TOOLKIT_CODEC_UTILS_H265SPS_H_
#define TOOLKIT_CODEC_UTILS_H265SPS_H_

#include <vector>
#include <cstdint>

#include "H265ProfileTierLevel.h"
#include "H265ScalingList.h"
#include "H265ShortTermRefPicSet.h"

struct H265SPS {
	H265SPS();

	uint8_t sps_video_parameter_set_id;
	uint8_t sps_max_sub_layers_minus1;
	uint8_t sps_temporal_id_nesting_flag;
	H265ProfileTierLevel profile_tier_level;
	uint32_t sps_seq_parameter_set_id;
	uint32_t chroma_format_idc;
	uint8_t separate_colour_plane_flag;
	uint32_t pic_width_in_luma_samples;
	uint32_t pic_height_in_luma_samples;
	uint8_t conformance_window_flag;
	uint32_t conf_win_left_offset;
	uint32_t conf_win_right_offset;
	uint32_t conf_win_top_offset;
	uint32_t conf_win_bottom_offset;
	uint32_t bit_depth_luma_minus8;
	uint32_t bit_depth_chroma_minus8;
	uint32_t log2_max_pic_order_cnt_lsb_minus4;
	uint8_t sps_sub_layer_ordering_info_present_flag;
	std::vector<uint32_t> sps_max_dec_pic_buffering_minus1;
	std::vector<uint32_t> sps_max_num_reorder_pics;
	std::vector<uint32_t> sps_max_latency_increase_plus1;
	uint32_t log2_min_luma_coding_block_size_minus3;
	uint32_t log2_diff_max_min_luma_coding_block_size;
	uint32_t log2_min_luma_transform_block_size_minus2;
	uint32_t log2_diff_max_min_luma_transform_block_size;
	uint32_t max_transform_hierarchy_depth_inter;
	uint32_t max_transform_hierarchy_depth_intra;
	uint8_t scaling_list_enabled_flag;
	uint8_t sps_scaling_list_data_present_flag;
	H265ScalingList scaling_list_data;
	uint8_t amp_enabled_flag;
	uint8_t sample_adaptive_offset_enabled_flag;
	uint8_t pcm_enabled_flag;
	uint8_t pcm_sample_bit_depth_luma_minus1;
	uint8_t pcm_sample_bit_depth_chroma_minus1;
	uint32_t log2_min_pcm_luma_coding_block_size_minus3;
	uint32_t log2_diff_max_min_pcm_luma_coding_block_size;
	uint8_t pcm_loop_filter_disabled_flag;
	uint32_t num_short_term_ref_pic_sets;
	std::vector<H265ShortTermRefPicSet> short_term_ref_pic_set;
	uint8_t long_term_ref_pics_present_flag;
	uint32_t num_long_term_ref_pics_sps;
	std::vector<uint32_t> lt_ref_pic_poc_lsb_sps;
	std::vector<uint8_t> used_by_curr_pic_lt_sps_flag;
	uint8_t sps_temporal_mvp_enabled_flag;
	uint8_t strong_intra_smoothing_enabled_flag;
	// TODO
	// uint8_t                  vui_parameters_present_flag;
	// VuiParameters            vui_parameters;
	// uint8_t                  sps_extension_flag;

	// Computed variables
	uint8_t SubWidthC;
	uint8_t SubHeightC;
	uint32_t MinCbLog2SizeY;
	uint32_t CtbLog2SizeY;
	uint32_t MinCbSizeY;
	uint32_t CtbSizeY;
	uint32_t PicWidthInMinCbsY;
	uint32_t PicWidthInCtbsY;
	uint32_t PicHeightInMinCbsY;
	uint32_t PicHeightInCtbsY;
	uint32_t PicSizeInMinCbsY;
	uint32_t PicSizeInCtbsY;
	uint32_t PicSizeInSamplesY;
};

#endif // TOOLKIT_CODEC_UTILS_H265SPS_H_
