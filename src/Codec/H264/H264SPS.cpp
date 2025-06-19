#include <cstring>
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
	MaxFrameNumber = 16;
	MaxDpbFrames = 0;  
	ChromaArrayType = 0;
	ExpectedDeltaPerPicOrderCntCycle = 0;
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

UnitFieldList H264SPS::dump_fields(){
	UnitFieldList fields = UnitFieldList("Sequence Parameter Set", H264NAL::dump_fields());
	if(!completelyParsed) return fields;
	ValueUnitFieldList profile_idcField = ValueUnitFieldList("profile_idc", profile_idc);
	fields.addItem(UnitField("constraint_set0_flag", constraint_set0_flag));
	fields.addItem(UnitField("constraint_set1_flag", constraint_set1_flag));
	fields.addItem(UnitField("constraint_set2_flag", constraint_set2_flag));
	fields.addItem(UnitField("constraint_set3_flag", constraint_set3_flag));
	fields.addItem(UnitField("constraint_set4_flag", constraint_set4_flag));
	fields.addItem(UnitField("constraint_set5_flag", constraint_set5_flag));
	fields.addItem(UnitField("reserved_zero_2bits", reserved_zero_2bits));
	fields.addItem(UnitField("level_idc", level_idc));
	
	fields.addItem(UnitField("seq_parameter_set_id", seq_parameter_set_id));
	switch(profile_idc){
		default: break;
		case 100: case 110: case 122: case 244: case 44:
		case 83: case 86: case 118: case 128: case 138:
		case 139: case 134: case 135:
		profile_idcField.addItem(UnitField("chroma_format_idc", chroma_format_idc));
		if(chroma_format_idc == 3) profile_idcField.addItem(UnitField("separate_colour_plane_flag", separate_colour_plane_flag));
		profile_idcField.addItem(UnitField("bit_depth_luma_minus8", bit_depth_luma_minus8));
		profile_idcField.addItem(UnitField("bit_depth_chroma_minus8", bit_depth_chroma_minus8));
		profile_idcField.addItem(UnitField("qpprime_y_zero_transform_bypass_flag", qpprime_y_zero_transform_bypass_flag));
		profile_idcField.addItem(UnitField("seq_scaling_matrix_present_flag", seq_scaling_matrix_present_flag));
		for(int i = 0;i < (chroma_format_idc != 3 ? 8 : 12);++i) profile_idcField.addItem(IdxUnitField("seq_scaling_list_present_flag", seq_scaling_list_present_flag[i], i));
		// uint8_t scaling_lists_4x4[6][16];
		// uint8_t scaling_lists_8x8[6][64];
		break;
	}
	fields.addItem(std::move(profile_idcField));
	
	fields.addItem(UnitField("log2_max_frame_num_minus4", log2_max_frame_num_minus4));
	ValueUnitFieldList pic_order_cnt_typeField = ValueUnitFieldList("pic_order_cnt_type", pic_order_cnt_type);
	if(pic_order_cnt_type == 0) pic_order_cnt_typeField.addItem(std::move(UnitField("log2_max_pic_order_cnt_lsb_minus4", log2_max_pic_order_cnt_lsb_minus4)));
	else if(pic_order_cnt_type == 1){
		pic_order_cnt_typeField.addItem(UnitField("delta_pic_order_always_zero_flag", delta_pic_order_always_zero_flag));
		pic_order_cnt_typeField.addItem(UnitField("offset_for_non_ref_pic", offset_for_non_ref_pic));
		pic_order_cnt_typeField.addItem(UnitField("offset_for_top_to_bottom_field", offset_for_top_to_bottom_field));
		pic_order_cnt_typeField.addItem(UnitField("num_ref_frames_in_pic_order_cnt_cycle", num_ref_frames_in_pic_order_cnt_cycle));
		for(int i = 0;i < num_ref_frames_in_pic_order_cnt_cycle;++i) pic_order_cnt_typeField.addItem(IdxUnitField("offset_for_ref_frame", offset_for_ref_frame[i], i));
	}
	fields.addItem(std::move(pic_order_cnt_typeField));

	fields.addItem(UnitField("max_num_ref_frames", max_num_ref_frames));
	fields.addItem(UnitField("gaps_in_frame_num_value_allowed_flag", gaps_in_frame_num_value_allowed_flag));
	fields.addItem(UnitField("pic_width_in_mbs_minus1", pic_width_in_mbs_minus1));
	fields.addItem(UnitField("pic_height_in_map_units_minus1", pic_height_in_map_units_minus1));
	ValueUnitFieldList frame_mbs_only_flagField = ValueUnitFieldList("frame_mbs_only_flag", frame_mbs_only_flag);
	if(!frame_mbs_only_flag) frame_mbs_only_flagField.addItem(UnitField("mb_adaptive_frame_field_flag", mb_adaptive_frame_field_flag));
	fields.addItem(std::move(frame_mbs_only_flagField));

	fields.addItem(UnitField("direct_8x8_inference_flag", direct_8x8_inference_flag));

	ValueUnitFieldList frame_cropping_flagField = ValueUnitFieldList("frame_cropping_flag", frame_cropping_flag);
	if(frame_cropping_flag){  
		frame_cropping_flagField.addItem(UnitField("frame_crop_left_offset", frame_crop_left_offset));
		frame_cropping_flagField.addItem(UnitField("frame_crop_right_offset", frame_crop_right_offset));
		frame_cropping_flagField.addItem(UnitField("frame_crop_top_offset", frame_crop_top_offset));
		frame_cropping_flagField.addItem(UnitField("frame_crop_bottom_offset", frame_crop_bottom_offset));
	}  
	fields.addItem(std::move(frame_cropping_flagField));

	ValueUnitFieldList vui_parameters_present_flagField = ValueUnitFieldList("vui_parameters_present_flag", vui_parameters_present_flag);
	if(!vui_parameters_present_flag) return fields;
	ValueUnitFieldList aspect_ratio_info_present_flagField = ValueUnitFieldList("aspect_ratio_info_present_flag", aspect_ratio_info_present_flag);
	vui_parameters_present_flagField.addItem(std::move(aspect_ratio_info_present_flagField));
	if(aspect_ratio_info_present_flag){
		ValueUnitFieldList aspect_ratio_idcField = ValueUnitFieldList("aspect_ratio_idc", aspect_ratio_idc);
		fields.addItem(StringFormatter::formatString("aspect_ratio_idc", aspect_ratio_idc));
		if(aspect_ratio_idc == EXTENDED_SAR){
			aspect_ratio_idcField.addItem(UnitField("sar_width", sar_width));
			aspect_ratio_idcField.addItem(UnitField("sar_height", sar_height));
		}
		aspect_ratio_info_present_flagField.addItem(std::move(aspect_ratio_idcField));
	}
	ValueUnitFieldList overscan_info_present_flagField = ValueUnitFieldList("overscan_info_present_flag", overscan_info_present_flag);
	if(overscan_info_present_flag) overscan_info_present_flagField.addItem(UnitField("overscan_appropriate_flag", overscan_appropriate_flag));
	vui_parameters_present_flagField.addItem(std::move(overscan_info_present_flagField));
	ValueUnitFieldList video_signal_type_present_flagField = ValueUnitFieldList("video_signal_type_present_flag", video_signal_type_present_flag);
	if(video_signal_type_present_flag){
		video_signal_type_present_flagField.addItem(UnitField("video_format", video_format));
		video_signal_type_present_flagField.addItem(UnitField("video_full_range_flag", video_full_range_flag));
		ValueUnitFieldList colour_description_present_flagField = ValueUnitFieldList("colour_description_present_flag", colour_description_present_flag);
		video_signal_type_present_flagField.addItem(std::move(colour_description_present_flagField));
		if(colour_description_present_flag){
			colour_description_present_flagField.addItem(UnitField("colour_primaries", colour_primaries));
			colour_description_present_flagField.addItem(UnitField("transfer_characteristics", transfer_characteristics));
			colour_description_present_flagField.addItem(UnitField("matrix_coefficients", matrix_coefficients));
		}
	}
	vui_parameters_present_flagField.addItem(std::move(video_signal_type_present_flagField));
	ValueUnitFieldList chroma_loc_info_present_flagField = ValueUnitFieldList("chroma_loc_info_present_flag", chroma_loc_info_present_flag);
	if(chroma_loc_info_present_flag){
		chroma_loc_info_present_flagField.addItem(UnitField("chroma_sample_loc_type_top_field", chroma_sample_loc_type_top_field));
		chroma_loc_info_present_flagField.addItem(UnitField("chroma_sample_loc_type_bottom_field", chroma_sample_loc_type_bottom_field));
	}
	vui_parameters_present_flagField.addItem(std::move(chroma_loc_info_present_flagField));
	ValueUnitFieldList timing_info_present_flagField = ValueUnitFieldList("timing_info_present_flag", timing_info_present_flag);
	if(timing_info_present_flag){
		timing_info_present_flagField.addItem(UnitField("num_units_tick", num_units_tick));
		timing_info_present_flagField.addItem(UnitField("time_scale", time_scale));
		timing_info_present_flagField.addItem(UnitField("fixed_frame_rate_flag", fixed_frame_rate_flag));
	}
	vui_parameters_present_flagField.addItem(std::move(timing_info_present_flagField));
	ValueUnitFieldList nal_hrd_parameters_present_flagField = ValueUnitFieldList("nal_hrd_parameters_present_flag", nal_hrd_parameters_present_flag);
	if(nal_hrd_parameters_present_flag){
		nal_hrd_parameters_present_flagField.addItem(UnitField("nal_cpb_cnt_minus1", nal_cpb_cnt_minus1));
		nal_hrd_parameters_present_flagField.addItem(UnitField("nal_bit_rate_scale", nal_bit_rate_scale));
		nal_hrd_parameters_present_flagField.addItem(UnitField("nal_cpb_size_scale", nal_cpb_size_scale));
		for(int i = 0;i <= nal_cpb_cnt_minus1;++i) {
			nal_hrd_parameters_present_flagField.addItem(IdxUnitField("nal_bit_rate_value_minus1", nal_bit_rate_value_minus1[i], i));
			nal_hrd_parameters_present_flagField.addItem(IdxUnitField("nal_cpb_size_value_minus1", nal_cpb_size_value_minus1[i], i));
			nal_hrd_parameters_present_flagField.addItem(IdxUnitField("nal_cbr_flag", nal_cbr_flag[i], i));
		}
		nal_hrd_parameters_present_flagField.addItem(UnitField("nal_initial_cpb_removal_delay_length_minus1", nal_initial_cpb_removal_delay_length_minus1));
		nal_hrd_parameters_present_flagField.addItem(UnitField("nal_cpb_removal_delay_length_minus1", nal_cpb_removal_delay_length_minus1));
		nal_hrd_parameters_present_flagField.addItem(UnitField("nal_dpb_output_delay_length_minus1", nal_dpb_output_delay_length_minus1));
		nal_hrd_parameters_present_flagField.addItem(UnitField("nal_time_offset_length", nal_time_offset_length));
	}
	vui_parameters_present_flagField.addItem(std::move(nal_hrd_parameters_present_flagField));
	ValueUnitFieldList vcl_hrd_parameters_present_flagField = ValueUnitFieldList("vcl_hrd_parameters_present_flag", vcl_hrd_parameters_present_flag);
	if(vcl_hrd_parameters_present_flag){
		vcl_hrd_parameters_present_flagField.addItem(UnitField("vcl_cpb_cnt_minus1", vcl_cpb_cnt_minus1));
		vcl_hrd_parameters_present_flagField.addItem(UnitField("vcl_bit_rate_scale", vcl_bit_rate_scale));
		vcl_hrd_parameters_present_flagField.addItem(UnitField("vcl_cpb_size_scale", vcl_cpb_size_scale));
		for(int i = 0;i <= vcl_cpb_cnt_minus1;++i) {
			vcl_hrd_parameters_present_flagField.addItem(IdxUnitField("vcl_bit_rate_value_minus1", vcl_bit_rate_value_minus1[i], i));
			vcl_hrd_parameters_present_flagField.addItem(IdxUnitField("vcl_cpb_size_value_minus1", vcl_cpb_size_value_minus1[i], i));
			vcl_hrd_parameters_present_flagField.addItem(IdxUnitField("vcl_cbr_flag", vcl_cbr_flag[i], i));
		}
		vcl_hrd_parameters_present_flagField.addItem(UnitField("vcl_initial_cpb_removal_delay_length_minus1", vcl_initial_cpb_removal_delay_length_minus1));
		vcl_hrd_parameters_present_flagField.addItem(UnitField("vcl_cpb_removal_delay_length_minus1", vcl_cpb_removal_delay_length_minus1));
		vcl_hrd_parameters_present_flagField.addItem(UnitField("vcl_dpb_output_delay_length_minus1", vcl_dpb_output_delay_length_minus1));
		vcl_hrd_parameters_present_flagField.addItem(UnitField("vcl_time_offset_length", vcl_time_offset_length));
	}
	vui_parameters_present_flagField.addItem(std::move(vcl_hrd_parameters_present_flagField));
	if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag) vui_parameters_present_flagField.addItem(UnitField("low_delay_hrd_flag", low_delay_hrd_flag));
	vui_parameters_present_flagField.addItem(UnitField("pic_struct_present_flag", pic_struct_present_flag));
	ValueUnitFieldList bitstream_restriction_flagField = ValueUnitFieldList("bitstream_restriction_flag", bitstream_restriction_flag);
	if(bitstream_restriction_flag){
		bitstream_restriction_flagField.addItem(UnitField("motion_vectors_over_pic_boundaries_flag", motion_vectors_over_pic_boundaries_flag));
		bitstream_restriction_flagField.addItem(UnitField("max_bytes_per_pic_denom", max_bytes_per_pic_denom));
		bitstream_restriction_flagField.addItem(UnitField("max_bits_per_mb_denom", max_bits_per_mb_denom));
		bitstream_restriction_flagField.addItem(UnitField("log2_max_mv_length_horizontal", log2_max_mv_length_horizontal));
		bitstream_restriction_flagField.addItem(UnitField("log2_max_mv_length_vertical", log2_max_mv_length_vertical));
		bitstream_restriction_flagField.addItem(UnitField("max_num_reorder_frames", max_num_reorder_frames));
		bitstream_restriction_flagField.addItem(UnitField("max_dec_frame_buffering", max_dec_frame_buffering));
	}
	vui_parameters_present_flagField.addItem(std::move(bitstream_restriction_flagField));
	fields.addItem(std::move(vui_parameters_present_flagField));
	return fields;
}

void H264SPS::validate(){
	H264NAL::validate();
	if(!completelyParsed) return;
	if(seq_parameter_set_id > 31){
		minorErrors.push_back(StringFormatter::formatString("[SPS] seq_parameter_set_id value (%ld) not in valid range (0..31)", seq_parameter_set_id));
	}
	switch(profile_idc) {
		case 100:
		case 110:
		case 122:
		case 244:
		case 44:
		case 83:
		case 86:
		case 118:
		case 128:
		case 138:
		case 139:
		case 134:
			if(bit_depth_luma_minus8 > 6){
				minorErrors.push_back(StringFormatter::formatString("[SPS] bit_depth_luma_minus8 value (%ld) not in valid range (0..6)", bit_depth_luma_minus8));
			}
			if(bit_depth_chroma_minus8 > 6){
				minorErrors.push_back(StringFormatter::formatString("[SPS] bit_depth_chroma_minus8 value (%ld) not in valid range (0..6)", bit_depth_chroma_minus8));
			}
			break;
		default:
			break;
	}

	if(log2_max_frame_num_minus4 > 12){
		minorErrors.push_back(StringFormatter::formatString("[SPS] log2_max_frame_num_minus4 value (%ld) not in valid range (0..12)", log2_max_frame_num_minus4));
	}
	if(pic_order_cnt_type > 2){
		minorErrors.push_back(StringFormatter::formatString("[SPS] pic_order_cnt_type (%ld) not in valid range (0..2)", pic_order_cnt_type));
	}
	if (pic_order_cnt_type == 0) {
		if(log2_max_pic_order_cnt_lsb_minus4 > 12){
			minorErrors.push_back(StringFormatter::formatString("[SPS] log2_max_pic_order_cnt_lsb_minus4 value (%ld) not in valid range (0..12)", log2_max_frame_num_minus4));
		}
	} else if (pic_order_cnt_type == 1) {
		if(num_ref_frames_in_pic_order_cnt_cycle > 255){
			minorErrors.push_back(StringFormatter::formatString("[SPS] num_ref_frames_in_pic_order_cnt_cycle value (%ld) not in valid range (0..255)", num_ref_frames_in_pic_order_cnt_cycle));
		}
	}
	if(max_num_ref_frames > MaxDpbFrames){
		minorErrors.push_back(StringFormatter::formatString("[SPS] max_num_ref_frames value (%ld) not in valid range (0..{})", max_num_ref_frames, MaxDpbFrames));
	}

	if (ChromaArrayType >= 1 && ChromaArrayType <= 3) {
		if (separate_colour_plane_flag == 0) {
			if (chroma_format_idc > 3) {
				minorErrors.push_back(StringFormatter::formatString("[SPS] chroma_format_idc value (%ld) not in valid range (0..3)", chroma_format_idc));
			}
		}
	}

	if(frame_cropping_flag){
		uint32_t frame_crop_left_limit = PicWidthInSamplesL/CropUnitX - frame_crop_right_offset+1; 
		if(frame_crop_left_offset > frame_crop_left_limit){
			minorErrors.push_back(StringFormatter::formatString("[SPS] frame_crop_left_offset value (%ld) not in valid range (0..{})", frame_crop_left_offset, frame_crop_left_limit));
		};
		uint32_t frame_crop_top_limit = 16*FrameHeightInMbs/CropUnitY - frame_crop_bottom_offset+1;
		if(frame_crop_top_offset > frame_crop_top_limit){
			minorErrors.push_back(StringFormatter::formatString("[SPS] frame_crop_top_offset value (%ld) not in valid range (0..{})", frame_crop_top_offset, frame_crop_top_limit));
		}
	}
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

