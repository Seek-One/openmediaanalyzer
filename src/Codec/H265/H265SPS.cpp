#include "H265SPS.h"

H265SPS::H265SPS()
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
}
