#ifndef TOOLKIT_CODEC_UTILS_H264Slice_H_
#define TOOLKIT_CODEC_UTILS_H264Slice_H_

#include <cstdint>
#include <vector>
#include <string>
#include <optional>

#include "H264NAL.h"

struct H264PPS;
struct H264SPS2;

struct H264Slice : public H264NAL {
	enum SliceType {
		SliceType_Unspecified,
		SliceType_P,
		SliceType_B,
		SliceType_I,
		SliceType_SP,
		SliceType_SI,
	};

	H264Slice();

	static bool isSlice(H264NAL* NALUnit);

	uint32_t first_mb_in_slice;
	SliceType slice_type;
	uint32_t pic_parameter_set_id;
	uint8_t colour_plane_id;
	uint16_t frame_num;
	uint8_t field_pic_flag;
	uint8_t bottom_field_flag;
	uint16_t idr_pic_id;
	uint16_t pic_order_cnt_lsb;
	int32_t delta_pic_order_cnt_bottom;
	int32_t delta_pic_order_cnt[2];
	uint8_t redundant_pic_cnt;
	uint8_t direct_spatial_mv_pred_flag;
	uint8_t num_ref_idx_active_override_flag;
	uint8_t num_ref_idx_l0_active_minus1;
	uint8_t num_ref_idx_l1_active_minus1;
	uint8_t cabac_init_idc;
	int8_t slice_qp_delta;
	uint8_t sp_for_switch_flag;
	int8_t slice_qs_delta;
	uint8_t disable_deblocking_filter_idc;
	int8_t slice_alpha_c0_offset_div2;
	int8_t slice_beta_offset_div2;
	uint16_t slice_group_change_cycle;

	struct
	{
		uint8_t luma_log2_weight_denom;
		uint8_t chroma_log2_weight_denom;
		uint8_t luma_weight_l0_flag[64];
		int16_t luma_weight_l0[64];
		int8_t luma_offset_l0[64];
		uint8_t chroma_weight_l0_flag[64];
		int16_t chroma_weight_l0[64][2];
		int8_t chroma_offset_l0[64][2];
		uint8_t luma_weight_l1_flag[64];
		int16_t luma_weight_l1[64];
		int8_t luma_offset_l1[64];
		uint8_t chroma_weight_l1_flag[64];
		int16_t chroma_weight_l1[64][2];
		int8_t chroma_offset_l1[64][2];
	} pwt; // predictive weight table

	struct
	{
		uint8_t ref_pic_list_reordering_flag_l0;
		struct
		{
			uint32_t reordering_of_pic_nums_idc;
			uint32_t abs_diff_pic_num_minus1;
			uint32_t long_term_pic_num;
		} reorder_l0[64];

		uint8_t ref_pic_list_reordering_flag_l1;
		struct
		{
			uint32_t reordering_of_pic_nums_idc;
			uint32_t abs_diff_pic_num_minus1;
			uint32_t long_term_pic_num;
		} reorder_l1[64];
	} rplr; // ref pic list reorder

	struct
	{
		uint8_t no_output_of_prior_pics_flag;
		uint8_t long_term_reference_flag;
		uint8_t adaptive_ref_pic_marking_mode_flag;
		uint8_t memory_management_control_operation[64];
		uint32_t difference_of_pic_nums_minus1[64];
		uint32_t long_term_pic_num[64];
		uint32_t long_term_frame_idx[64];
		uint32_t max_long_term_frame_idx_plus1[64];
	} drpm; // decoded ref pic marking

	uint8_t IdrPicFlag;

	std::vector<std::string> errors;
	std::vector<std::string> dump_fields() override;
	std::optional<H264PPS> getPPS() const;
	std::optional<H264SPS2> getSPS() const;
};

#endif // TOOLKIT_CODEC_UTILS_H264Slice_H_
