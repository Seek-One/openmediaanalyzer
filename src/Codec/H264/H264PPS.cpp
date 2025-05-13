#include <cstring>
#include <fmt/core.h>

#include "H264SPS.h"

#include "H264PPS.h"

H264PPS::H264PPS():
	H264PPS(0, 0, 0, nullptr)
{}

H264PPS::H264PPS(uint8_t forbidden_zero_bit, uint8_t nal_ref_idc, uint32_t nal_size, uint8_t* nal_data):
	H264NAL(forbidden_zero_bit, nal_ref_idc, nal_size, nal_data)
{
	nal_unit_type = UnitType_PPS;
	pic_parameter_set_id = 0;
	seq_parameter_set_id = 0;
	entropy_coding_mode_flag = 0;
	bottom_field_pic_order_in_frame_present_flag = 0;
	num_slice_groups_minus1 = 0;
	slice_group_map_type = 0;

	for (int i = 0; i < 8; ++i) {
		run_length_minus1[i] = 0;
	}

	for (int i = 0; i < 8; ++i) {
		top_left[i] = 0;
	}

	for (int i = 0; i < 8; ++i) {
		bottom_right[i] = 0;
	}

	slice_group_change_direction_flag = 0;
	slice_group_change_rate_minus1 = 0;
	pic_size_in_map_units_minus1 = 0;

	for (int i = 0; i < 256; ++i) {
		slice_group_id[i] = 0;
	}

	num_ref_idx_l0_active_minus1 = 0;
	num_ref_idx_l1_active_minus1 = 0;
	weighted_pred_flag = 0;
	weighted_bipred_idc = 0;
	pic_init_qp_minus26 = 0;
	pic_init_qs_minus26 = 0;
	chroma_qp_index_offset = 0;
	deblocking_filter_control_present_flag = 0;
	constrained_intra_pred_flag = 0;
	redundant_pic_cnt_present_flag = 0;
	transform_8x8_mode_flag = 0;
	pic_scaling_matrix_present_flag = 0;
	memset(pic_scaling_list_present_flag, 0, sizeof(uint8_t) * 12);

	// Set scaling list to Flat_4x4_16 and Flat_8x8_16
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 16; ++j) {
			scaling_lists_4x4[i][j] = 16;
		}
		for (int j = 0; j < 64; ++j) {
			scaling_lists_8x8[i][j] = 16;
		}
	}

	second_chroma_qp_index_offset = 0;
}

std::vector<std::string> H264PPS::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back(fmt::format("pic_parameter_set_id:{}", pic_parameter_set_id));
	fields.push_back(fmt::format("seq_parameter_set_id:{}", seq_parameter_set_id));
	fields.push_back(fmt::format("entropy_coding_mode_flag:{}", entropy_coding_mode_flag));
	fields.push_back(fmt::format("bottom_field_pic_order_in_frame_present_flag:{}", bottom_field_pic_order_in_frame_present_flag));
	fields.push_back(fmt::format("num_slice_groups_minus1:{}", num_slice_groups_minus1));
	if(num_slice_groups_minus1 > 0){
		fields.push_back(fmt::format("  slice_group_map_type:{}", slice_group_map_type));
		if(slice_group_map_type == 0){
			for(int i = 0;i <= num_slice_groups_minus1;++i) fields.push_back(fmt::format("      run_length_minus1[{}]:{}", i, run_length_minus1[i]));
		} else if(slice_group_map_type == 2){
			for(int i = 0;i < num_slice_groups_minus1;++i) {
				fields.push_back(fmt::format("      top_left[{}]:{}", i, top_left[i]));
				fields.push_back(fmt::format("      bottom_right[{}]:{}", i, bottom_right[i]));
			}
		} else if(slice_group_map_type >= 3 && slice_group_map_type <= 5){
			fields.push_back(fmt::format("      slice_group_change_direction_flag:{}", slice_group_change_direction_flag));
			fields.push_back(fmt::format("      slice_group_change_rate_minus1:{}", slice_group_change_rate_minus1));
		} else if(slice_group_map_type == 6) {
			fields.push_back(fmt::format("      pic_size_in_map_units_minus1:{}", pic_size_in_map_units_minus1));
			for(int i = 0;i <= pic_size_in_map_units_minus1;++i) fields.push_back(fmt::format("        slice_group_id[{}]:{}", i, slice_group_id[i]));
		}
	}

	fields.push_back(fmt::format("num_ref_idx_l0_active_minus1:{}", num_ref_idx_l0_active_minus1));
	fields.push_back(fmt::format("num_ref_idx_l1_active_minus1:{}", num_ref_idx_l1_active_minus1));
	fields.push_back(fmt::format("weighted_pred_flag:{}", weighted_pred_flag));
	fields.push_back(fmt::format("weighted_bipred_idc:{}", weighted_bipred_idc));
	fields.push_back(fmt::format("pic_init_qp_minus26:{}", pic_init_qp_minus26));
	fields.push_back(fmt::format("pic_init_qs_minus26:{}", pic_init_qs_minus26));
	fields.push_back(fmt::format("chroma_qp_index_offset:{}", chroma_qp_index_offset));
	fields.push_back(fmt::format("deblocking_filter_control_present_flag:{}", deblocking_filter_control_present_flag));
	fields.push_back(fmt::format("constrained_intra_pred_flag:{}", constrained_intra_pred_flag));
	fields.push_back(fmt::format("redundant_pic_cnt_present_flag:{}", redundant_pic_cnt_present_flag));

	// fields.push_back(fmt::format("transform_8x8_mode_flag:{}", transform_8x8_mode_flag));

	// fields.push_back(fmt::format("pic_scaling_matrix_present_flag:{}", pic_scaling_matrix_present_flag));
	// uint8_t pic_scaling_list_present_flag[12];
	// uint8_t scaling_lists_4x4[6][16];
	// uint8_t scaling_lists_8x8[6][64];

	// fields.push_back(fmt::format("second_chroma_qp_index_offset:{}", second_chroma_qp_index_offset));
	return fields;
}

void H264PPS::validate(){
	H264NAL::validate();
	if(seq_parameter_set_id > 31){
		minorErrors.push_back(fmt::format("[PPS] seq_parameter_set_id value ({}) not in valid range (0..31)", seq_parameter_set_id));
	}
	H264SPS* h264SPS;
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()){
		majorErrors.push_back(fmt::format("[PPS] reference to unknown SPS ({})", seq_parameter_set_id));
		return;
	}
	h264SPS = referencedSPS->second;

	if (num_slice_groups_minus1 > 0) {
		switch(slice_group_map_type){
			case 0:
				for (int iGroup = 0; iGroup <= num_slice_groups_minus1; iGroup++) {
					if(run_length_minus1[iGroup] > h264SPS->PicSizeInMapUnits-1){
						minorErrors.push_back(fmt::format("[PPS] run_length_minus1[{}] value ({}) not in valid range (0..{})", iGroup, run_length_minus1[iGroup], h264SPS->PicSizeInMapUnits-1));
					}
				}
				break;
			case 1: break;
			case 2:
				for (int iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++) {
					if(top_left[iGroup] > bottom_right[iGroup]){
						minorErrors.push_back(fmt::format("[PPS] top_left[{}] value ({}) should be less than or equal to bottom_right[{}] value ({})", iGroup, top_left[iGroup], iGroup, bottom_right[iGroup]));
					}
					if(top_left[iGroup] % h264SPS->PicWidthInMbs > bottom_right[iGroup] % h264SPS->PicWidthInMbs){
						minorErrors.push_back(fmt::format("[PPS] top_left[{}]%PicWidthInMbs value ({}) should be less than or equal to bottom_right[{}]%PicWidthInMbs value ({})", iGroup, top_left[iGroup]%h264SPS->PicWidthInMbs, iGroup, bottom_right[iGroup]%h264SPS->PicWidthInMbs));
					}
				}
				break;
			case 3:
			case 4:
			case 5:
				if(slice_group_change_rate_minus1 > h264SPS->PicSizeInMapUnits-1){
					minorErrors.push_back(fmt::format("[PPS] slice_group_change_rate_minus1 value ({}) not in valid range (0..{})", slice_group_change_rate_minus1, h264SPS->PicSizeInMapUnits-1));
				}
				break;
			case 6:
				if(pic_size_in_map_units_minus1 != h264SPS->PicSizeInMapUnits-1){
					minorErrors.push_back(fmt::format("[PPS] pic_size_in_map_units_minus1 value ({}) should be equal to {}", pic_size_in_map_units_minus1, h264SPS->PicSizeInMapUnits-1));
				}
				for (unsigned i = 0; i <= pic_size_in_map_units_minus1; i++) {
					if(slice_group_id[i] > num_slice_groups_minus1){
						minorErrors.push_back(fmt::format("[PPS] slice_group_id[{}] value ({}) not in valid range (0..{})", i, slice_group_id[i], num_slice_groups_minus1));
					}
				}
				break;
			default:
				minorErrors.push_back(fmt::format("[PPS] slice_group_map_type value ({}) not in valid range (0..6)", slice_group_map_type));
				break;
		}
	}
	if(num_ref_idx_l0_active_minus1 > 31){
		minorErrors.push_back(fmt::format("[PPS] num_ref_idx_l0_active_minus1 value ({}) not in valid range (0..31)", num_ref_idx_l0_active_minus1));
	}
	if(num_ref_idx_l1_active_minus1 > 31){
		minorErrors.push_back(fmt::format("[PPS] num_ref_idx_l1_active_minus1 value ({}) not in valid range (0..31)", num_ref_idx_l1_active_minus1));
	}

	if(weighted_bipred_idc > 2){
		minorErrors.push_back(fmt::format("[PPS] weighted_bipred_idc value ({}) not in valid range (0..2)", weighted_bipred_idc));
	}
	if(pic_init_qp_minus26 < -26 - h264SPS->QpBdOffsetY || pic_init_qp_minus26 > 25){
		minorErrors.push_back(fmt::format("[PPS] pic_init_qp_minus26 value ({}) not in valid range ({}..25)", pic_init_qp_minus26, -26 - h264SPS->QpBdOffsetY));
	}
	if(pic_init_qs_minus26 < -26 || pic_init_qs_minus26 > 25){
		minorErrors.push_back(fmt::format("[PPS] pic_init_qs_minus26 value ({}) not in valid range (-26..25)", pic_init_qs_minus26));
	}
	if(chroma_qp_index_offset < -12 || chroma_qp_index_offset > 12){
		minorErrors.push_back(fmt::format("[PPS] chroma_qp_index_offset value ({}) not in valid range (-12..12)", chroma_qp_index_offset));
	}
}
