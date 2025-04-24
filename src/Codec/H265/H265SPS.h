#ifndef TOOLKIT_CODEC_UTILS_H265SPS_H_
#define TOOLKIT_CODEC_UTILS_H265SPS_H_

#include <cstdint>
#include <unordered_map>

#include "H265NAL.h"
#include "H265ProfileTierLevel.h"
#include "H265ScalingList.h"
#include "H265ShortTermRefPicSet.h"
#include "H265HrdParameters.h"

#define EXTENDED_SAR 255

struct H265VuiParameters {
	H265VuiParameters();

	uint8_t aspect_ratio_info_present_flag;
	uint8_t aspect_ratio_idc;
	uint16_t sar_width;
	uint16_t sar_height;
	
	uint8_t overscan_info_present_flag;
	uint8_t overscan_appropriate_flag;
	uint8_t video_signal_type_present_flag;
	uint8_t video_format;
	uint8_t video_full_range_flag;
	uint8_t colour_description_present_flag;
	uint8_t colour_primaries;
	uint8_t transfer_characteristics;
	uint8_t matrix_coeffs;

	uint8_t chroma_loc_info_present_flag;
	uint8_t chroma_sample_loc_type_top_field;
	uint8_t chroma_sample_loc_type_bottom_field;

	uint8_t neutral_chroma_indication_flag;
	uint8_t field_seq_flag;
	uint8_t frame_field_info_present_flag;
	uint8_t default_display_window_flag;
	uint32_t def_disp_win_left_offset;
	uint32_t def_disp_win_right_offset;
	uint32_t def_disp_win_top_offset;
	uint32_t def_disp_win_bottom_offset;

	uint8_t vui_timing_info_present_flag;
	uint32_t vui_num_units_in_tick;
	uint32_t vui_time_scale;
	uint8_t vui_poc_proportional_to_timing_flag;
	uint32_t vui_num_ticks_poc_diff_one_minus1;

	uint8_t vui_hrd_parameters_present_flag;
	H265HrdParameters hrd_parameters;

	uint8_t bitstream_restriction_flag;
	uint8_t tiles_fixed_structure_flag;
	uint8_t motion_vectors_over_pic_boundaries_flag;
	uint8_t restricted_ref_pic_lists_flag;
	uint16_t min_spatial_segmentation_idc;
	uint8_t max_bytes_per_pic_denom;
	uint8_t max_bits_per_min_cu_denom;
	uint8_t log2_max_mv_length_horizontal;
	uint8_t log2_max_mv_length_vertical;

	std::vector<std::string> dump_fields();
};

struct H265SPSRangeExtension {
	H265SPSRangeExtension();

	uint8_t transform_skip_rotation_enabled_flag;
	uint8_t transform_skip_context_enabled_flag;
	uint8_t implicit_rdpcm_enabled_flag;
	uint8_t explicit_rdpcm_enabled_flag;
	uint8_t extended_precision_processing_flag;
	uint8_t intra_smoothing_disabled_flag;
	uint8_t high_precision_offsets_enabled_flag;
	uint8_t persistent_rice_adaptation_enabled_flag;
	uint8_t cabac_bypass_alignment_enabled_flag;

	std::vector<std::string> dump_fields();
};

struct H265SPSMultilayerExtension {
	H265SPSMultilayerExtension();

	uint8_t inter_view_mv_vert_constraint_flag;

	std::vector<std::string> dump_fields();
};

struct H265SPS3DExtension {
	H265SPS3DExtension();

	std::array<uint8_t, 2> iv_di_mc_enabled_flag;
	std::array<uint8_t, 2> iv_mv_scal_enabled_flag;
	std::array<uint32_t, 2> log2_ivmc_sub_pb_size_minus3;
	std::array<uint8_t, 2> iv_res_pred_enabled_flag;
	std::array<uint8_t, 2> depth_ref_enabled_flag;
	std::array<uint8_t, 2> vsp_mc_enabled_flag;
	std::array<uint8_t, 2> dbbp_enabled_flag;

	std::array<uint8_t, 2> tex_mc_enabled_flag;
	std::array<uint32_t, 2> log2_texmc_sub_pb_size_minus3;
	std::array<uint8_t, 2> intra_contour_enabled_flag;
	std::array<uint8_t, 2> intra_dc_only_wedge_enabled;
	std::array<uint8_t, 2> cqt_cu_part_pred_enabled_flag;
	std::array<uint8_t, 2> inter_dc_only_enabled_flag;
	std::array<uint8_t, 2> skip_intra_enabled_flag;

	std::vector<std::string> dump_fields();
};

struct H265SPSSCCExtension {
	H265SPSSCCExtension();

	uint8_t sps_curr_pic_ref_enabled_flag;
	uint8_t palette_mode_enabled_flag;
	uint32_t palette_max_size;
	uint32_t delta_palette_max_predictor_size;
	uint8_t sps_palette_predictor_initializers_present_flag;
	uint16_t sps_num_palette_predictor_initializers_minus1;
	std::array<std::vector<uint16_t>, 3> sps_palette_predictor_initializer;
	uint8_t motion_vector_resolution_control_idc;
	uint8_t intra_boundary_filtering_disabled_flag;

	std::vector<std::string> dump_fields(uint32_t chroma_format_idc);
};

struct H265SPS : public H265NAL {
	H265SPS();
	H265SPS(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, uint8_t* nal_data);
	~H265SPS();

	uint8_t sps_video_parameter_set_id;
	uint8_t sps_max_sub_layers_minus1;
	uint8_t sps_temporal_id_nesting_flag;
	H265ProfileTierLevel profile_tier_level;
	uint8_t sps_seq_parameter_set_id;
	uint32_t chroma_format_idc;
	uint8_t separate_colour_plane_flag;
	uint32_t pic_width_in_luma_samples;
	uint32_t pic_height_in_luma_samples;
	uint8_t conformance_window_flag;
	uint32_t conf_win_left_offset;
	uint32_t conf_win_right_offset;
	uint32_t conf_win_top_offset;
	uint32_t conf_win_bottom_offset;
	uint8_t bit_depth_luma_minus8;
	uint8_t bit_depth_chroma_minus8;
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

	uint8_t vui_parameters_present_flag;
	H265VuiParameters vui_parameters;

	uint8_t sps_extension_flag;
	uint8_t sps_range_extension_flag;
	uint8_t sps_multilayer_extension_flag;
	uint8_t sps_3d_extension_flag;
	uint8_t sps_scc_extension_flag;

	H265SPSRangeExtension sps_range_extension;
	H265SPSMultilayerExtension sps_multilayer_extension;
	H265SPS3DExtension sps_3d_extension;
	H265SPSSCCExtension sps_scc_extension;

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
	uint8_t BitDepthY;
	uint16_t QpBdOffsetY;
	uint8_t BitDepthC;
	uint16_t QpBdOffsetC;
	uint8_t ChromaArrayType;

	static inline std::unordered_map<uint8_t, H265SPS*> SPSMap;

	std::vector<std::string> dump_fields() override;
};

#endif // TOOLKIT_CODEC_UTILS_H265SPS_H_
