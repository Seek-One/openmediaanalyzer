#ifndef TOOLKIT_CODEC_UTILS_H265PPS_H_
#define TOOLKIT_CODEC_UTILS_H265PPS_H_

#include "H265ScalingList.h"

struct H265PPS {
	H265PPS();

	uint32_t pps_pic_parameter_set_id;
	uint32_t pps_seq_parameter_set_id;
	uint8_t dependent_slice_segments_enabled_flag;
	uint8_t output_flag_present_flag;
	uint8_t num_extra_slice_header_bits;
	uint8_t sign_data_hiding_enabled_flag;
	uint8_t cabac_init_present_flag;
	uint32_t num_ref_idx_l0_default_active_minus1;
	uint32_t num_ref_idx_l1_default_active_minus1;
	int32_t init_qp_minus26;
	uint8_t constrained_intra_pred_flag;
	uint8_t transform_skip_enabled_flag;
	uint8_t cu_qp_delta_enabled_flag;
	uint32_t diff_cu_qp_delta_depth;
	int32_t pps_cb_qp_offset;
	int32_t pps_cr_qp_offset;
	uint8_t pps_slice_chroma_qp_offsets_present_flag;
	uint8_t weighted_pred_flag;
	uint8_t weighted_bipred_flag;
	uint8_t transquant_bypass_enabled_flag;
	uint8_t tiles_enabled_flag;
	uint8_t entropy_coding_sync_enabled_flag;
	uint32_t num_tile_columns_minus1;
	uint32_t num_tile_rows_minus1;
	uint8_t uniform_spacing_flag;
	std::vector<uint32_t> column_width_minus1;
	std::vector<uint32_t> row_height_minus1;
	uint8_t loop_filter_across_tiles_enabled_flag;
	uint8_t pps_loop_filter_across_slices_enabled_flag;
	uint8_t deblocking_filter_control_present_flag;
	uint8_t deblocking_filter_override_enabled_flag;
	uint8_t pps_deblocking_filter_disabled_flag;
	int32_t pps_beta_offset_div2;
	int32_t pps_tc_offset_div2;
	uint8_t pps_scaling_list_data_present_flag;
	H265ScalingList scaling_list_data;
	uint8_t lists_modification_present_flag;
	int32_t log2_parallel_merge_level_minus2;
	uint8_t slice_segment_header_extension_present_flag;
	uint8_t pps_extension_present_flag;
};


#endif // TOOLKIT_CODEC_UTILS_H265PPS_H_
