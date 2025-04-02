#ifndef TOOLKIT_CODEC_UTILS_H264PPS_H_
#define TOOLKIT_CODEC_UTILS_H264PPS_H_

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "H264NAL.h"

struct H264PPS : public H264NAL {
	H264PPS();
	H264PPS(uint8_t forbidden_zero_bit, uint8_t nal_ref_idc, uint32_t nal_size, uint8_t* nal_data);
	~H264PPS();

	uint8_t pic_parameter_set_id;
	uint8_t seq_parameter_set_id;

	uint8_t entropy_coding_mode_flag;
	uint8_t bottom_field_pic_order_in_frame_present_flag;

	uint32_t num_slice_groups_minus1;

	uint8_t slice_group_map_type;

	uint32_t run_length_minus1[8];

	uint32_t top_left[8];
	uint32_t bottom_right[8];

	uint8_t slice_group_change_direction_flag;
	uint32_t slice_group_change_rate_minus1;

	uint32_t pic_size_in_map_units_minus1;
	uint8_t slice_group_id[256]; // FIXME what size?

	uint8_t num_ref_idx_l0_active_minus1;
	uint8_t num_ref_idx_l1_active_minus1;
	uint8_t weighted_pred_flag;
	uint8_t weighted_bipred_idc;
	int8_t pic_init_qp_minus26;
	int8_t pic_init_qs_minus26;
	int8_t chroma_qp_index_offset;
	uint8_t deblocking_filter_control_present_flag;
	uint8_t constrained_intra_pred_flag;
	uint8_t redundant_pic_cnt_present_flag;

	uint8_t transform_8x8_mode_flag;

	uint8_t pic_scaling_matrix_present_flag;
	uint8_t pic_scaling_list_present_flag[12];
	uint8_t scaling_lists_4x4[6][16];
	uint8_t scaling_lists_8x8[6][64];

	int8_t second_chroma_qp_index_offset;

	std::vector<std::string> errors;

	std::vector<std::string> dump_fields() override;
	static inline std::unordered_map<uint8_t, H264PPS*> PPSMap;
};

#endif // TOOLKIT_CODEC_UTILS_H264PPS_H_
