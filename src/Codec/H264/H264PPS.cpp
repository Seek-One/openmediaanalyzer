#include <cstring>
#include <sstream>

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
	fields.push_back((std::ostringstream() << "pic_parameter_set_id:" << (int)pic_parameter_set_id).str());
	fields.push_back((std::ostringstream() << "seq_parameter_set_id:" << (int)seq_parameter_set_id).str());
	fields.push_back((std::ostringstream() << "entropy_coding_mode_flag:" << (int)entropy_coding_mode_flag).str());
	fields.push_back((std::ostringstream() << "bottom_field_pic_order_in_frame_present_flag:" << (int)bottom_field_pic_order_in_frame_present_flag).str());
	fields.push_back((std::ostringstream() << "num_slice_groups_minus1:" << num_slice_groups_minus1).str());
	if(num_slice_groups_minus1 > 0){
		fields.push_back((std::ostringstream() << "  slice_group_map_type:" << (int)slice_group_map_type).str());
		if(slice_group_map_type == 0){
			for(int i = 0;i <= num_slice_groups_minus1;++i) fields.push_back((std::ostringstream() << "      run_length_minus1[" << i << "]:" << run_length_minus1[i]).str());
		} else if(slice_group_map_type == 2){
			for(int i = 0;i < num_slice_groups_minus1;++i) {
				fields.push_back((std::ostringstream() << "      top_left[" << i << "]:" << top_left[i]).str());
				fields.push_back((std::ostringstream() << "      bottom_right[" << i << "]:" << bottom_right[i]).str());
			}
		} else if(slice_group_map_type >= 3 && slice_group_map_type <= 5){
			fields.push_back((std::ostringstream() << "      slice_group_change_direction_flag:" << (int)slice_group_change_direction_flag).str());
			fields.push_back((std::ostringstream() << "      slice_group_change_rate_minus1:" << slice_group_change_rate_minus1).str());
		} else if(slice_group_map_type == 6) {
			fields.push_back((std::ostringstream() << "      pic_size_in_map_units_minus1:" << pic_size_in_map_units_minus1).str());
			for(int i = 0;i <= pic_size_in_map_units_minus1;++i) fields.push_back((std::ostringstream() << "        slice_group_id[" << i << "]:" << (int)slice_group_id[i]).str());
		}
	}

	fields.push_back((std::ostringstream() << "num_ref_idx_l0_active_minus1:" << (int)num_ref_idx_l0_active_minus1).str());
	fields.push_back((std::ostringstream() << "num_ref_idx_l1_active_minus1:" << (int)num_ref_idx_l1_active_minus1).str());
	fields.push_back((std::ostringstream() << "weighted_pred_flag:" << (int)weighted_pred_flag).str());
	fields.push_back((std::ostringstream() << "weighted_bipred_idc:" << (int)weighted_bipred_idc).str());
	fields.push_back((std::ostringstream() << "pic_init_qp_minus26:" << (int)pic_init_qp_minus26).str());
	fields.push_back((std::ostringstream() << "pic_init_qs_minus26:" << (int)pic_init_qs_minus26).str());
	fields.push_back((std::ostringstream() << "chroma_qp_index_offset:" << (int)chroma_qp_index_offset).str());
	fields.push_back((std::ostringstream() << "deblocking_filter_control_present_flag:" << (int)deblocking_filter_control_present_flag).str());
	fields.push_back((std::ostringstream() << "constrained_intra_pred_flag:" << (int)constrained_intra_pred_flag).str());
	fields.push_back((std::ostringstream() << "redundant_pic_cnt_present_flag:" << (int)redundant_pic_cnt_present_flag).str());

	// fields.push_back((std::ostringstream() << "transform_8x8_mode_flag:" << (int)transform_8x8_mode_flag).str());

	// fields.push_back((std::ostringstream() << "pic_scaling_matrix_present_flag:" << (int)pic_scaling_matrix_present_flag).str());
	// uint8_t pic_scaling_list_present_flag[12];
	// uint8_t scaling_lists_4x4[6][16];
	// uint8_t scaling_lists_8x8[6][64];

	// fields.push_back((std::ostringstream() << "second_chroma_qp_index_offset:" << (int)second_chroma_qp_index_offset).str());
	return fields;
}

void H264PPS::validate(){
	H264NAL::validate();
	if(seq_parameter_set_id > 31){
		minorErrors.push_back((std::ostringstream() << "[PPS] seq_parameter_set_id value (" << (int)seq_parameter_set_id << ") not in valid range (0..31)").str());
	}
	H264SPS* h264SPS;
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()){
		majorErrors.push_back((std::ostringstream() << "[PPS] reference to unknown SPS (" << (int)seq_parameter_set_id << ")").str());
		return;
	}
	h264SPS = referencedSPS->second;

	if (num_slice_groups_minus1 > 0) {
		switch(slice_group_map_type){
			case 0:
				for (int iGroup = 0; iGroup <= num_slice_groups_minus1; iGroup++) {
					if(run_length_minus1[iGroup] > h264SPS->PicSizeInMapUnits-1){
						minorErrors.push_back((std::ostringstream() << "[PPS] run_length_minus1[" << iGroup << "] value (" << run_length_minus1[iGroup] << ") not in valid range (0.." << h264SPS->PicSizeInMapUnits-1 << ")").str());
					}
				}
				break;
			case 1: break;
			case 2:
				for (int iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++) {
					if(top_left[iGroup] > bottom_right[iGroup]){
						minorErrors.push_back((std::ostringstream() << "[PPS] top_left[" << iGroup << "] value (" << top_left[iGroup] << ") should be less than or equal to bottom_right[" << iGroup << "] value (" << bottom_right[iGroup] << ")").str());
					}
					if(top_left[iGroup] % h264SPS->PicWidthInMbs > bottom_right[iGroup] % h264SPS->PicWidthInMbs){
						minorErrors.push_back((std::ostringstream() << "[PPS] top_left[" << iGroup << "]%PicWidthInMbs value (" << top_left[iGroup]%h264SPS->PicWidthInMbs << ") should be less than or equal to bottom_right[" << iGroup << "]%PicWidthInMbs value (" << bottom_right[iGroup]%h264SPS->PicWidthInMbs << ")").str());
					}
				}
				break;
			case 3:
			case 4:
			case 5:
				if(slice_group_change_rate_minus1 > h264SPS->PicSizeInMapUnits-1){
					minorErrors.push_back((std::ostringstream() << "[PPS] slice_group_change_rate_minus1 value (" << slice_group_change_rate_minus1 << ") not in valid range (0.." << h264SPS->PicSizeInMapUnits-1 << ")").str());
				}
				break;
			case 6:
				if(pic_size_in_map_units_minus1 != h264SPS->PicSizeInMapUnits-1){
					minorErrors.push_back((std::ostringstream() << "[PPS] pic_size_in_map_units_minus1 value (" << pic_size_in_map_units_minus1 << ") should be equal to " << h264SPS->PicSizeInMapUnits-1).str());
				}
				for (unsigned i = 0; i <= pic_size_in_map_units_minus1; i++) {
					if(slice_group_id[i] > num_slice_groups_minus1){
						minorErrors.push_back((std::ostringstream() << "[PPS] slice_group_id[" << i << "] value (" << (int)slice_group_id[i] << ") not in valid range (0.." << num_slice_groups_minus1 << ")").str());
					}
				}
				break;
			default:
				minorErrors.push_back((std::ostringstream() << "[PPS] slice_group_map_type value (" << (int)slice_group_map_type << ") not in valid range (0..6)").str());
				break;
		}
	}
	if(num_ref_idx_l0_active_minus1 > 31){
		minorErrors.push_back((std::ostringstream() << "[PPS] num_ref_idx_l0_active_minus1 value (" << (int)num_ref_idx_l0_active_minus1 << ") not in valid range (0..31)").str());
	}
	if(num_ref_idx_l1_active_minus1 > 31){
		minorErrors.push_back((std::ostringstream() << "[PPS] num_ref_idx_l1_active_minus1 value (" << (int)num_ref_idx_l1_active_minus1 << ") not in valid range (0..31)").str());
	}

	if(weighted_bipred_idc > 2){
		minorErrors.push_back((std::ostringstream() << "[PPS] weighted_bipred_idc value (" << (int)weighted_bipred_idc << ") not in valid range (0..2)").str());
	}
	if(pic_init_qp_minus26 < -26 - h264SPS->QpBdOffsetY || pic_init_qp_minus26 > 25){
		minorErrors.push_back((std::ostringstream() << "[PPS] pic_init_qp_minus26 value (" << (int)pic_init_qp_minus26 << ") not in valid range (" << (int)(-26 - h264SPS->QpBdOffsetY ) << "..25)").str());
	}
	if(pic_init_qs_minus26 < -26 || pic_init_qs_minus26 > 25){
		minorErrors.push_back((std::ostringstream() << "[PPS] pic_init_qs_minus26 value (" << (int)pic_init_qs_minus26 << ") not in valid range (-26..25)").str());
	}
	if(chroma_qp_index_offset < -12 || chroma_qp_index_offset > 12){
		minorErrors.push_back((std::ostringstream() << "[PPS] chroma_qp_index_offset value (" << (int)chroma_qp_index_offset << ") not in valid range (-12..12)").str());
	}
}
