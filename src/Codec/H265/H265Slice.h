#ifndef TOOLKIT_CODEC_UTILS_H265_SLICE_H_
#define TOOLKIT_CODEC_UTILS_H265_SLICE_H_

#include "H265ShortTermRefPicSet.h"

struct RefPicListsModification {

	RefPicListsModification();

	uint8_t ref_pic_list_modification_flag_l0;
	std::vector<uint32_t> list_entry_l0;
	uint8_t ref_pic_list_modification_flag_l1;
	std::vector<uint32_t> list_entry_l1;
};

struct H265Slice {
	enum SliceType {
		SliceType_B = 0,
		SliceType_P = 1,
		SliceType_I = 2,
		SliceType_None = 3
	};

	H265Slice();

	uint8_t first_slice_segment_in_pic_flag;
	uint8_t no_output_of_prior_pics_flag;
	uint32_t slice_pic_parameter_set_id;
	uint8_t dependent_slice_segment_flag;
	uint32_t slice_segment_address;
	SliceType slice_type;
	uint8_t pic_output_flag;
	uint8_t colour_plane_id;
	uint32_t slice_pic_order_cnt_lsb;
	uint8_t short_term_ref_pic_set_sps_flag;
	uint8_t short_term_ref_pic_set_idx;

	uint32_t num_long_term_sps;
	uint32_t num_long_term_pics;
	uint32_t NumLongTerm;
	std::vector<uint32_t> lt_idx_sps;
	std::vector<uint32_t> poc_lsb_lt;
	std::vector<uint8_t> used_by_curr_pic_lt_flag;
	std::vector<uint8_t> delta_poc_msb_present_flag;
	std::vector<uint32_t> delta_poc_msb_cycle_lt;

	uint8_t slice_temporal_mvp_enabled_flag;
	uint8_t slice_sao_luma_flag;
	uint8_t slice_sao_chroma_flag;
	uint8_t num_ref_idx_active_override_flag;
	uint32_t num_ref_idx_l0_active_minus1;
	uint32_t num_ref_idx_l1_active_minus1;
	RefPicListsModification ref_pic_lists_modification;
	uint8_t mvd_l1_zero_flag;
	uint8_t cabac_init_flag;
	uint8_t collocated_from_l0_flag;
	uint32_t collocated_ref_idx;
	// PredWeightTable          pred_weight_table;
	uint32_t five_minus_max_num_merge_cand;
	int32_t slice_qp_delta;
	int32_t slice_cb_qp_offset;
	int32_t slice_cr_qp_offset;
	int32_t slice_act_y_qp_offset;
	int32_t slice_act_cb_qp_offset;
	int32_t slice_act_cr_qp_offset;
	uint8_t deblocking_filter_override_flag;
	uint8_t slice_deblocking_filter_disabled_flag;
	int32_t slice_beta_offset_div2;
	int32_t slice_tc_offset_div2;
	int32_t slice_loop_filter_across_slices_enabled_flag;
	uint32_t num_entry_point_offsets;
	uint32_t offset_len_minus1;
	std::vector<uint32_t> entry_point_offset_minus1;
	uint32_t slice_segment_header_extension_length;
	std::vector<uint8_t> slice_segment_header_extension_data_byte;

	// Derived variables
	uint8_t CurrRpsIdx;
	uint32_t NumPicTotalCurr;
	std::vector<uint32_t> PocLsbLt;
	std::vector<uint32_t> UsedByCurrPicLt;
	std::vector<uint32_t> DeltaPocMsbCycleLt;
	uint32_t NumRpsCurrTempList0;
	uint32_t NumRpsCurrTempList1;

	// Specific variables for VDPAU
	uint64_t NumShortTermPictureSliceHeaderBits;
	uint64_t NumLongTermPictureSliceHeaderBits;

	uint8_t IdrPicFlag;
};


#endif // TOOLKIT_CODEC_UTILS_H265_SLICE_H_
