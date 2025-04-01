#include "H265PPS.h"

H265PPS::H265PPS()
{
	pps_pic_parameter_set_id = 0;
	pps_seq_parameter_set_id = 0;
	dependent_slice_segments_enabled_flag = 0;
	output_flag_present_flag = 0;
	num_extra_slice_header_bits = 0;
	sign_data_hiding_enabled_flag = 0;
	cabac_init_present_flag = 0;
	num_ref_idx_l0_default_active_minus1 = 0;
	num_ref_idx_l1_default_active_minus1 = 0;
	init_qp_minus26 = 0;
	constrained_intra_pred_flag = 0;
	transform_skip_enabled_flag = 0;
	cu_qp_delta_enabled_flag = 0;
	diff_cu_qp_delta_depth = 0;
	pps_cb_qp_offset = 0;
	pps_cr_qp_offset = 0;
	pps_slice_chroma_qp_offsets_present_flag = 0;
	weighted_pred_flag = 0;
	weighted_bipred_flag = 0;
	transquant_bypass_enabled_flag = 0;
	tiles_enabled_flag = 0;
	entropy_coding_sync_enabled_flag = 0;
	num_tile_columns_minus1 = 0;
	num_tile_rows_minus1 = 0;
	uniform_spacing_flag = 1;
	loop_filter_across_tiles_enabled_flag = 1;
	pps_loop_filter_across_slices_enabled_flag = 0;
	deblocking_filter_control_present_flag = 0;
	deblocking_filter_override_enabled_flag = 0;
	pps_deblocking_filter_disabled_flag = 0;
	pps_beta_offset_div2 = 0;
	pps_tc_offset_div2 = 0;
	pps_scaling_list_data_present_flag = 0;
	lists_modification_present_flag = 0;
	log2_parallel_merge_level_minus2 = 0;
	slice_segment_header_extension_present_flag = 0;
	pps_extension_present_flag = 0;
}
