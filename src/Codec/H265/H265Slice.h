#ifndef TOOLKIT_CODEC_UTILS_H265_SLICE_H_
#define TOOLKIT_CODEC_UTILS_H265_SLICE_H_

#include <list>

#include "H265ShortTermRefPicSet.h"
#include "H265NAL.h"
#include "H265PPS.h"
#include "H265SPS.h"
#include "H265VPS.h"

struct H265Slice;
struct H265AccessUnit;

struct RefPicListsModification {

	RefPicListsModification();

	uint8_t ref_pic_list_modification_flag_l0;
	std::vector<uint32_t> list_entry_l0;
	uint8_t ref_pic_list_modification_flag_l1;
	std::vector<uint32_t> list_entry_l1;

	std::vector<std::string> dump_fields(const H265Slice& slice) const;
	std::vector<std::string> minorErrors;
	std::vector<std::string> majorErrors;
	void validate(const H265Slice& h265Slice);
};

struct H265PredWeightTable {
	H265PredWeightTable();

	uint8_t luma_log2_weight_denom;
	int8_t delta_chroma_log2_weight_denom;
	std::array<uint8_t, 15> luma_weight_l0_flag;
	std::array<uint8_t, 15> chroma_weight_l0_flag;
	std::array<int16_t, 15> delta_luma_weight_l0;
	std::array<int16_t, 15> luma_offset_l0;
	std::array<std::array<int16_t, 2>, 15> delta_chroma_weight_l0;
	std::array<std::array<int16_t, 2>, 15> delta_chroma_offset_l0;
	std::array<uint8_t, 15> luma_weight_l1_flag;
	std::array<uint8_t, 15> chroma_weight_l1_flag;
	std::array<int16_t, 15> delta_luma_weight_l1;
	std::array<int16_t, 15> luma_offset_l1;
	std::array<std::array<int16_t, 2>, 15> delta_chroma_weight_l1;
	std::array<std::array<int16_t, 2>, 15> delta_chroma_offset_l1;

	std::vector<std::string> minorErrors;
	std::vector<std::string> majorErrors;
	std::vector<std::string> dump_fields(const H265Slice& h265Slice);
	void validate(const H265Slice& h265Slice);
};

struct H265Slice : public H265NAL {
	enum SliceType {
		SliceType_B = 0,
		SliceType_P = 1,
		SliceType_I = 2,
		SliceType_None = 3
	};

	H265Slice();
	H265Slice(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, uint8_t* nal_data);

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
	H265PredWeightTable pred_weight_table;
	uint8_t five_minus_max_num_merge_cand;
	uint8_t use_integer_mv_flag;
	int32_t slice_qp_delta;
	int32_t slice_cb_qp_offset;
	int32_t slice_cr_qp_offset;
	int32_t slice_act_y_qp_offset;
	int32_t slice_act_cb_qp_offset;
	int32_t slice_act_cr_qp_offset;
	uint8_t cu_chroma_qp_offset_enabled_flag;
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
	int32_t PicOrderCntVal;
	uint32_t PicOrderCntMsb;
	uint32_t SliceQpY;
	uint8_t IdrPicFlag;
	uint8_t IRAPPicture;
	uint8_t NoRaslOutputFlag;
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

	// Reference picture lists
	std::vector<int32_t> PocStCurrBefore;
	std::vector<int32_t> PocStCurrAfter; 
	std::vector<int32_t> PocStFoll; 
	std::vector<int32_t> PocLtCurr; 
	std::vector<int32_t> PocLtFoll; 
	std::vector<uint8_t> CurrDeltaPocMsbPresentFlag;
	std::vector<uint8_t> FollDeltaPocMsbPresentFlag;

	std::vector<H265AccessUnit*> RefPicList0; // 8.3.4 (For P-slice)
	std::vector<H265AccessUnit*> RefPicList1; // 8.3.4 (For B-slice)

	std::vector<std::string> dump_fields() override;
	H265PPS* getPPS() const;
	H265SPS* getSPS() const;
	H265VPS* getVPS() const;
	void validate() override;
};


#endif // TOOLKIT_CODEC_UTILS_H265_SLICE_H_
