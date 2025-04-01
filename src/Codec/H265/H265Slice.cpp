#include "H265Slice.h"

RefPicListsModification::RefPicListsModification()
{
	ref_pic_list_modification_flag_l0 = 0;
	ref_pic_list_modification_flag_l1 = 0;
}

H265Slice::H265Slice()
{
	first_slice_segment_in_pic_flag = 0;
	no_output_of_prior_pics_flag = 0;
	slice_pic_parameter_set_id = 0;
	dependent_slice_segment_flag = 0;
	slice_segment_address = 0;
	slice_type = SliceType_None;
	pic_output_flag = 1;
	colour_plane_id = 0;
	slice_pic_order_cnt_lsb = 0;
	short_term_ref_pic_set_sps_flag = 0;
	short_term_ref_pic_set_idx = 0;
	num_long_term_sps = 0;
	num_long_term_pics = 0;
	NumLongTerm = 0;
	slice_temporal_mvp_enabled_flag = 0;
	slice_sao_luma_flag = 0;
	slice_sao_chroma_flag = 0;
	num_ref_idx_active_override_flag = 0;
	num_ref_idx_l0_active_minus1 = 0;
	num_ref_idx_l1_active_minus1 = 0;
	mvd_l1_zero_flag = 0;
	cabac_init_flag = 0;
	collocated_from_l0_flag = 0;
	collocated_ref_idx = 0;
	five_minus_max_num_merge_cand = 0;
	slice_qp_delta = 0;
	slice_cb_qp_offset = 0;
	slice_cr_qp_offset = 0;
	slice_act_y_qp_offset = 0;
	slice_act_cb_qp_offset = 0;
	slice_act_cr_qp_offset = 0;
	deblocking_filter_override_flag = 0;
	slice_deblocking_filter_disabled_flag = 0;
	slice_beta_offset_div2 = 0;
	slice_tc_offset_div2 = 0;
	slice_loop_filter_across_slices_enabled_flag = 0;
	num_entry_point_offsets = 0;
	offset_len_minus1 = 0;
	slice_segment_header_extension_length = 0;
	CurrRpsIdx = 0;
	NumPicTotalCurr = 0;
	NumShortTermPictureSliceHeaderBits = 0;
	NumLongTermPictureSliceHeaderBits = 0;
	NumRpsCurrTempList0 = 0;
	NumRpsCurrTempList1 = 0;
}
