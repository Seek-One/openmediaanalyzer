#include <cstring>
#include <sstream>
#include <iostream>

#include "H264SPS.h"
#include "H264NAL.h"

H264SPS::H264SPS():
	H264SPS(0, 0, 0, nullptr)
{}

H264SPS::H264SPS(uint8_t forbidden_zero_bit, uint8_t nal_ref_idc, uint32_t nal_size, uint8_t* nal_data):
	H264NAL(forbidden_zero_bit, nal_ref_idc, nal_size, nal_data)
{
	nal_unit_type = UnitType_SPS;
	profile_idc = 0;
	constraint_set0_flag = 0;
	constraint_set1_flag = 0;
	constraint_set2_flag = 0;
	constraint_set3_flag = 0;
	constraint_set4_flag = 0;
	constraint_set5_flag = 0;
	reserved_zero_2bits = 0;
	level_idc = 0;
	seq_parameter_set_id = 0;
	chroma_format_idc = 1;
	separate_colour_plane_flag = 0;
	bit_depth_luma_minus8 = 0;
	bit_depth_chroma_minus8 = 0;
	qpprime_y_zero_transform_bypass_flag = 0;
	seq_scaling_matrix_present_flag = 0;
	memset(seq_scaling_list_present_flag, 0, sizeof(uint8_t) * 12);

	// Set scaling list to Flat_4x4_16 and Flat_8x8_16
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 16; ++j) {
			scaling_lists_4x4[i][j] = 16;
		}
		for (int j = 0; j < 64; ++j) {
			scaling_lists_8x8[i][j] = 16;
		}
	}

	log2_max_frame_num_minus4 = 0;
	pic_order_cnt_type = 0;
	log2_max_pic_order_cnt_lsb_minus4 = 0;
	delta_pic_order_always_zero_flag = 0;
	offset_for_non_ref_pic = 0;
	offset_for_top_to_bottom_field = 0;
	num_ref_frames_in_pic_order_cnt_cycle = 0;

	for (int i = 0; i < 256; ++i) {
		offset_for_ref_frame[i] = 0;
	}

	max_num_ref_frames = 0;
	gaps_in_frame_num_value_allowed_flag = 0;
	pic_width_in_mbs_minus1 = 0;
	pic_height_in_map_units_minus1 = 0;
	frame_mbs_only_flag = 0;
	mb_adaptive_frame_field_flag = 0;
	direct_8x8_inference_flag = 0;
 	QpBdOffsetY = 6;  
	PicHeightInMapUnits = 0;   
	PicWidthInSamplesL = 0;   
	FrameHeightInMbs = 0;  
	PicWidthInMbs = 0;  
	PicSizeInMapUnits = 0;  
	MaxPicOrderCntLsb = 0;  
	MaxDpbFrames = 0;  
	ChromaArrayType = 0;
	CropUnitX = 0;
	CropUnitY = 0;
	frame_cropping_flag = 0;
	frame_crop_left_offset = 0;
	frame_crop_right_offset = 0;
	frame_crop_top_offset = 0;
	frame_crop_bottom_offset = 0;
	vui_parameters_present_flag = 0;
	aspect_ratio_idc = 0;
	video_format = 5;
	video_full_range_flag = 0;
	colour_primaries = 2;
	transfer_characteristics = 2;
	matrix_coefficients = 2;
	chroma_sample_loc_type_top_field = 0;
	chroma_sample_loc_type_bottom_field = 0;
	fixed_frame_rate_flag = 0;
	nal_cpb_cnt_minus1 = 0;
	aspect_ratio_info_present_flag = 0;   
	aspect_ratio_idc = 1;   
	sar_width = 1;   
	sar_height = 1; 
	overscan_info_present_flag = 0;   
	overscan_appropriate_flag = 0;   
	video_signal_type_present_flag = 0;   
	video_full_range_flag = 0;   
	colour_description_present_flag = 0;   
	chroma_loc_info_present_flag  = 0;   
	timing_info_present_flag = 0;   
	num_units_tick = 0;    
	time_scale = 0;  
	fixed_frame_rate_flag = 0;   

	nal_hrd_parameters_present_flag = 0; 
	nal_cpb_cnt_minus1 = 0;    
	nal_bit_rate_scale = 0;   
	nal_cpb_size_scale = 0;   
	nal_initial_cpb_removal_delay_length_minus1 = 23;
	nal_cpb_removal_delay_length_minus1 = 23;
	nal_dpb_output_delay_length_minus1 = 23;
	nal_time_offset_length = 0;        
	vcl_hrd_parameters_present_flag = 0;   
	vcl_cpb_cnt_minus1 = 0; 
	vcl_bit_rate_scale = 0;
	vcl_cpb_size_scale = 0;
	vcl_initial_cpb_removal_delay_length_minus1 = 23; 
	vcl_cpb_removal_delay_length_minus1 = 23;
	vcl_dpb_output_delay_length_minus1 = 23;
	vcl_time_offset_length = 0;  
	low_delay_hrd_flag = 0;
	pic_struct_present_flag = 0;
	bitstream_restriction_flag = 0;
	motion_vectors_over_pic_boundaries_flag = 1; 
	max_bytes_per_pic_denom = 0;  
	max_bits_per_mb_denom = 0;  
	log2_max_mv_length_horizontal = 0;   
	log2_max_mv_length_vertical = 0;   
	max_num_reorder_frames = 0;   
	max_dec_frame_buffering = 0;  
	for(int i = 0;i < 32;++i){
		nal_bit_rate_value_minus1[i] = 0; 
		nal_cpb_size_value_minus1[i] = 0; 
		nal_cbr_flag[i] = 0; 
		vcl_bit_rate_value_minus1[i] = 0; 
		vcl_cpb_size_value_minus1[i] = 0; 
		vcl_cbr_flag[i] = 0;
	}
}

std::vector<std::string> H264SPS::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "profile_idc:" << (int)profile_idc).str());
	fields.push_back((std::ostringstream() << "constraint_set0_flag:" << (int)constraint_set0_flag).str());
	fields.push_back((std::ostringstream() << "constraint_set1_flag:" << (int)constraint_set1_flag).str());
	fields.push_back((std::ostringstream() << "constraint_set2_flag:" << (int)constraint_set2_flag).str());
	fields.push_back((std::ostringstream() << "constraint_set3_flag:" << (int)constraint_set3_flag).str());
	fields.push_back((std::ostringstream() << "constraint_set4_flag:" << (int)constraint_set4_flag).str());
	fields.push_back((std::ostringstream() << "constraint_set5_flag:" << (int)constraint_set5_flag).str());
	fields.push_back((std::ostringstream() << "reserved_zero_2bits:" << (int)reserved_zero_2bits).str());
	fields.push_back((std::ostringstream() << "level_idc:" << (int)level_idc).str());

	fields.push_back((std::ostringstream() << "seq_parameter_set_id:" << (int)seq_parameter_set_id).str());
	switch(profile_idc){
		default: break;
		case 100: case 110: case 122: case 244: case 44:
		case 83: case 86: case 118: case 128: case 138:
		case 139: case 134: case 135:
			fields.push_back((std::ostringstream() << "  chroma_format_idc:" << (int)chroma_format_idc).str());
			if(chroma_format_idc == 3) fields.push_back((std::ostringstream() << "    separate_colour_plane_flag:" << (int)separate_colour_plane_flag).str());
			fields.push_back((std::ostringstream() << "  bit_depth_luma_minus8:" << (int)bit_depth_luma_minus8).str());
			fields.push_back((std::ostringstream() << "  bit_depth_chroma_minus8:" << (int)bit_depth_chroma_minus8).str());
			fields.push_back((std::ostringstream() << "  qpprime_y_zero_transform_bypass_flag:" << (int)qpprime_y_zero_transform_bypass_flag).str());
			fields.push_back((std::ostringstream() << "  seq_scaling_matrix_present_flag:" << (int)seq_scaling_matrix_present_flag).str());
			for(int i = 0;i < (chroma_format_idc != 3 ? 8 : 12);++i) fields.push_back((std::ostringstream() << "    seq_scaling_list_present_flag[" << i << "]:" << (int)seq_scaling_list_present_flag[i]).str());
			// uint8_t scaling_lists_4x4[6][16];
			// uint8_t scaling_lists_8x8[6][64];
			break;
	}

	fields.push_back((std::ostringstream() << "log2_max_frame_num_minus4:" << (int)log2_max_frame_num_minus4).str());
	fields.push_back((std::ostringstream() << "pic_order_cnt_type:" << (int)pic_order_cnt_type).str());
	if(pic_order_cnt_type == 0)fields.push_back((std::ostringstream() << "  log2_max_pic_order_cnt_lsb_minus4:" << (int)log2_max_pic_order_cnt_lsb_minus4).str());
	else if(pic_order_cnt_type == 1){
		fields.push_back((std::ostringstream() << "  delta_pic_order_always_zero_flag:" << (int)delta_pic_order_always_zero_flag).str());
		fields.push_back((std::ostringstream() << "  offset_for_non_ref_pic:" << offset_for_non_ref_pic).str());
		fields.push_back((std::ostringstream() << "  offset_for_top_to_bottom_field:" << offset_for_top_to_bottom_field).str());
		fields.push_back((std::ostringstream() << "  num_ref_frames_in_pic_order_cnt_cycle:" << num_ref_frames_in_pic_order_cnt_cycle).str());
		for(int i = 0;i < num_ref_frames_in_pic_order_cnt_cycle;++i) fields.push_back((std::ostringstream() << "    offset_for_ref_frame[" << i << "]:" << offset_for_ref_frame[i]).str());
	}

	fields.push_back((std::ostringstream() << "max_num_ref_frames:" << (int)max_num_ref_frames).str());
	fields.push_back((std::ostringstream() << "gaps_in_frame_num_value_allowed_flag:" << (int)gaps_in_frame_num_value_allowed_flag).str());
	fields.push_back((std::ostringstream() << "pic_width_in_mbs_minus1:" << (int)pic_width_in_mbs_minus1).str());
	fields.push_back((std::ostringstream() << "pic_height_in_map_units_minus1:" << (int)pic_height_in_map_units_minus1).str());
	fields.push_back((std::ostringstream() << "frame_mbs_only_flag:" << (int)frame_mbs_only_flag).str());
	if(!frame_mbs_only_flag) fields.push_back((std::ostringstream() << "  mb_adaptive_frame_field_flag:" << (int)mb_adaptive_frame_field_flag).str());

	fields.push_back((std::ostringstream() << "direct_8x8_inference_flag:" << (int)direct_8x8_inference_flag).str());

	fields.push_back((std::ostringstream() << "frame_cropping_flag:" << (int)frame_cropping_flag).str());
	if(frame_cropping_flag){  
		fields.push_back((std::ostringstream() << "  frame_crop_left_offset:" << frame_crop_left_offset).str());
		fields.push_back((std::ostringstream() << "  frame_crop_right_offset:" << frame_crop_right_offset).str());
		fields.push_back((std::ostringstream() << "  frame_crop_top_offset:" << frame_crop_top_offset).str());
		fields.push_back((std::ostringstream() << "  frame_crop_bottom_offset:" << frame_crop_bottom_offset).str());
	}  

	fields.push_back((std::ostringstream() << "vui_parameters_present_flag:" << (int)vui_parameters_present_flag).str());
	if(!vui_parameters_present_flag) return fields;
	fields.push_back((std::ostringstream() << "  aspect_ratio_info_present_flag:" << (int)aspect_ratio_info_present_flag).str());
	if(aspect_ratio_info_present_flag){
		fields.push_back((std::ostringstream() << "    aspect_ratio_idc:" << (int)aspect_ratio_idc).str());
		if(aspect_ratio_idc == EXTENDED_SAR){
			fields.push_back((std::ostringstream() << "      sar_width:" << (int)sar_width).str());
			fields.push_back((std::ostringstream() << "      sar_height:" << (int)sar_height).str());
		}
	}
	fields.push_back((std::ostringstream() << "  overscan_info_present_flag:" << (int)overscan_info_present_flag).str());
	if(overscan_info_present_flag) fields.push_back((std::ostringstream() << "    overscan_appropriate_flag:" << (int)overscan_appropriate_flag).str());
	fields.push_back((std::ostringstream() << "  video_signal_type_present_flag:" << (int)video_signal_type_present_flag).str());
	if(video_signal_type_present_flag){
		fields.push_back((std::ostringstream() << "    video_format:" << (int)video_format).str());
		fields.push_back((std::ostringstream() << "    video_full_range_flag:" << (int)video_full_range_flag).str());
		fields.push_back((std::ostringstream() << "    colour_description_present_flag:" << (int)colour_description_present_flag).str());
		if(colour_description_present_flag){
			fields.push_back((std::ostringstream() << "      colour_primaries:" << (int)colour_primaries).str());
			fields.push_back((std::ostringstream() << "      transfer_characteristics:" << (int)transfer_characteristics).str());
			fields.push_back((std::ostringstream() << "      matrix_coefficients:" << (int)matrix_coefficients).str());
		}
	}
	fields.push_back((std::ostringstream() << "  chroma_loc_info_present_flag:" << (int)chroma_loc_info_present_flag).str());
	if(chroma_loc_info_present_flag){
		fields.push_back((std::ostringstream() << "    chroma_sample_loc_type_top_field:" << (int)chroma_sample_loc_type_top_field).str());
		fields.push_back((std::ostringstream() << "    chroma_sample_loc_type_bottom_field:" << (int)chroma_sample_loc_type_bottom_field).str());
	}
	fields.push_back((std::ostringstream() << "  timing_info_present_flag:" << (int)timing_info_present_flag).str());
	if(timing_info_present_flag){
		fields.push_back((std::ostringstream() << "    num_units_tick:" << num_units_tick).str());
		fields.push_back((std::ostringstream() << "    time_scale:" << time_scale).str());
		fields.push_back((std::ostringstream() << "    fixed_frame_rate_flag:" << (int)fixed_frame_rate_flag).str());
	}
	fields.push_back((std::ostringstream() << "  nal_hrd_parameters_present_flag:" << (int)nal_hrd_parameters_present_flag).str());
	if(nal_hrd_parameters_present_flag){
		fields.push_back((std::ostringstream() << "    nal_cpb_cnt_minus1:" << (int)nal_cpb_cnt_minus1).str());
		fields.push_back((std::ostringstream() << "    nal_bit_rate_scale:" << (int)nal_bit_rate_scale).str());
		fields.push_back((std::ostringstream() << "    nal_cpb_size_scale:" << (int)nal_cpb_size_scale).str());
		for(int i = 0;i <= nal_cpb_cnt_minus1;++i) {
			fields.push_back((std::ostringstream() << "      nal_bit_rate_value_minus1[" << i << "]:" << nal_bit_rate_value_minus1[i]).str());
			fields.push_back((std::ostringstream() << "      nal_cpb_size_value_minus1[" << i << "]:" << nal_cpb_size_value_minus1[i]).str());
			fields.push_back((std::ostringstream() << "      nal_cbr_flag[" << i << "]:" << (int)nal_cbr_flag[i]).str());
		}
		fields.push_back((std::ostringstream() << "    nal_initial_cpb_removal_delay_length_minus1:" << (int)nal_initial_cpb_removal_delay_length_minus1).str());
		fields.push_back((std::ostringstream() << "    nal_cpb_removal_delay_length_minus1:" << (int)nal_cpb_removal_delay_length_minus1).str());
		fields.push_back((std::ostringstream() << "    nal_dpb_output_delay_length_minus1:" << (int)nal_dpb_output_delay_length_minus1).str());
		fields.push_back((std::ostringstream() << "    nal_time_offset_length:" << (int)nal_time_offset_length).str());
	}
	fields.push_back((std::ostringstream() << "  vcl_hrd_parameters_present_flag:" << (int)vcl_hrd_parameters_present_flag).str());
	if(vcl_hrd_parameters_present_flag){
		fields.push_back((std::ostringstream() << "    vcl_cpb_cnt_minus1:" << (int)vcl_cpb_cnt_minus1).str());
		fields.push_back((std::ostringstream() << "    vcl_bit_rate_scale:" << (int)vcl_bit_rate_scale).str());
		fields.push_back((std::ostringstream() << "    vcl_cpb_size_scale:" << (int)vcl_cpb_size_scale).str());
		for(int i = 0;i <= vcl_cpb_cnt_minus1;++i) {
			fields.push_back((std::ostringstream() << "      vcl_bit_rate_value_minus1[" << i << "]:" << vcl_bit_rate_value_minus1[i]).str());
			fields.push_back((std::ostringstream() << "      vcl_cpb_size_value_minus1[" << i << "]:" << vcl_cpb_size_value_minus1[i]).str());
			fields.push_back((std::ostringstream() << "      vcl_cbr_flag[" << i << "]:" << (int)vcl_cbr_flag[i]).str());
		}
		fields.push_back((std::ostringstream() << "    vcl_initial_cpb_removal_delay_length_minus1:" << (int)vcl_initial_cpb_removal_delay_length_minus1).str());
		fields.push_back((std::ostringstream() << "    vcl_cpb_removal_delay_length_minus1:" << (int)vcl_cpb_removal_delay_length_minus1).str());
		fields.push_back((std::ostringstream() << "    vcl_dpb_output_delay_length_minus1:" << (int)vcl_dpb_output_delay_length_minus1).str());
		fields.push_back((std::ostringstream() << "    vcl_time_offset_length:" << (int)vcl_time_offset_length).str());
	}
	if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag) fields.push_back((std::ostringstream() << "  low_delay_hrd_flag:" << (int)low_delay_hrd_flag).str());
	fields.push_back((std::ostringstream() << "  pic_struct_present_flag:" << (int)pic_struct_present_flag).str());
	fields.push_back((std::ostringstream() << "  bitstream_restriction_flag:" << (int)bitstream_restriction_flag).str());
	if(bitstream_restriction_flag){
		fields.push_back((std::ostringstream() << "    motion_vectors_over_pic_boundaries_flag:" << (int)motion_vectors_over_pic_boundaries_flag).str());
		fields.push_back((std::ostringstream() << "    max_bytes_per_pic_denom:" << (int)max_bytes_per_pic_denom).str());
		fields.push_back((std::ostringstream() << "    max_bits_per_mb_denom:" << (int)max_bits_per_mb_denom).str());
		fields.push_back((std::ostringstream() << "    log2_max_mv_length_horizontal:" << (int)log2_max_mv_length_horizontal).str());
		fields.push_back((std::ostringstream() << "    log2_max_mv_length_vertical:" << (int)log2_max_mv_length_vertical).str());
		fields.push_back((std::ostringstream() << "    max_num_reorder_frames:" << (int)max_num_reorder_frames).str());
		fields.push_back((std::ostringstream() << "    max_dec_frame_buffering:" << (int)max_dec_frame_buffering).str());
	}
	return fields;
}

uint8_t H264SPS::level_limit_index(){
	if(level_idc == 11 && constraint_set3_flag == 1) return 1; // 1b
	switch(level_idc){
		case 10: return 0;
		case 11: return 2;
		case 12: return 3;
		case 13: return 4;
		case 20: return 5;
		case 21: return 6;
		case 22: return 7;
		case 30: return 8;
		case 31: return 9;
		case 32: return 10;
		case 40: return 11;
		case 41: return 12;
		case 42: return 13;
		case 50: return 14;
		case 51: return 15;
		case 52: return 16;
		case 60: return 17;
		case 61: return 18;
		case 62: return 19;
	}
	return 0;
}

uint16_t H264SPS::computeMaxFrameNumber(){
	if(log2_max_frame_num_minus4 == 12){ // overflow
		return UINT16_MAX; 
	}
	return 1 << (log2_max_frame_num_minus4+4);
}
