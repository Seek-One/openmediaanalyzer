#include <cstring>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <iostream>

#include "H264BitstreamReader.h"
#include "H264NAL.h"
#include "H264PPS.h"
#include "H264Slice.h"
#include "H264SPS.h"
#include "H264AUD.h"
#include "H264SEI.h"
#include "H264Utils.h"

// Default scaling_lists according to Table 7-2
static const uint8_t default_4x4_intra[16] = {
	 6,13,20,28,
	13,20,28,32,
	20,28,32,37,
	28,32,37,42
};

static const uint8_t default_4x4_inter[16] = {
	10,14,20,24,
	14,20,24,27,
	20,24,27,30,
	24,27,30,34
};

static const uint8_t default_8x8_intra[64] = {
	 6,10,13,16,18,23,25,27,
	10,11,16,18,23,25,27,29,
	13,16,18,23,25,27,29,31,
	16,18,23,25,27,29,31,33,
	18,23,25,27,29,31,33,36,
	23,25,27,29,31,33,36,38,
	25,27,29,31,33,36,38,40,
	27,29,31,33,36,38,40,42
};

static const uint8_t default_8x8_inter[64] = {
	 9,13,15,17,19,21,22,24,
	13,13,17,19,21,22,24,25,
	15,17,19,21,22,24,25,27,
	17,19,21,22,24,25,27,28,
	19,21,22,24,25,27,28,30,
	21,22,24,25,27,28,30,32,
	22,24,25,27,28,30,32,33,
	24,25,27,28,30,32,33,35
};

H264BitstreamReader::H264BitstreamReader(uint8_t* pNALData, uint32_t iNALLength)
	: H26XBitstreamReader(pNALData, iNALLength)
{

}

void H264BitstreamReader::readNALHeader(H264NAL& h264NAL)
{
	// Ref sec. 7.3.1
	h264NAL.forbidden_zero_bit = readBits(1);
	h264NAL.nal_ref_idc = readBits(2);
	h264NAL.nal_unit_type = (H264NAL::UnitType)readBits(5);
}

void H264BitstreamReader::readSPS(H264SPS& h264SPS)
{
	h264SPS.errors.clear();
	// Ref sec. 7.3.2.1.1
	h264SPS.profile_idc = readBits(8);
	
	h264SPS.constraint_set0_flag = readBits(1);
	h264SPS.constraint_set1_flag = readBits(1);
	h264SPS.constraint_set2_flag = readBits(1);
	h264SPS.constraint_set3_flag = readBits(1);
	h264SPS.constraint_set4_flag = readBits(1);
	h264SPS.constraint_set5_flag = readBits(1);
	h264SPS.reserved_zero_2bits = readBits(2);
	
	if (h264SPS.reserved_zero_2bits != 0) {
		h264SPS.errors.push_back("[H264 SPS content] reserved_zero_2bits not equal to 0");
	}
	
	h264SPS.level_idc = readBits(8);
	h264SPS.seq_parameter_set_id = readGolombUE();
	if(h264SPS.seq_parameter_set_id > 31){
		h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] seq_parameter_set_id value (" << (int)h264SPS.seq_parameter_set_id << ") not in valid range (0..31)").str());
	}

	switch(h264SPS.profile_idc) {
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
		h264SPS.chroma_format_idc = readGolombUE();
		if(h264SPS.chroma_format_idc == 3){
			h264SPS.separate_colour_plane_flag = readBits(1);
		}
		h264SPS.bit_depth_luma_minus8 = readGolombUE();
		if(h264SPS.bit_depth_luma_minus8 > 6){
			h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] bit_depth_luma_minus8  value (" << (int)h264SPS.bit_depth_luma_minus8  << ") not in valid range (0..6)").str());
		}
		h264SPS.bit_depth_chroma_minus8 = readGolombUE();
		if(h264SPS.bit_depth_chroma_minus8 > 6){
			h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] bit_depth_chroma_minus8  value (" << (int)h264SPS.bit_depth_chroma_minus8  << ") not in valid range (0..6)").str());
		}
		h264SPS.qpprime_y_zero_transform_bypass_flag = readBits(1);

		h264SPS.seq_scaling_matrix_present_flag = readBits(1);
		if(h264SPS.seq_scaling_matrix_present_flag){
			int iListNumber = ((h264SPS.chroma_format_idc != 3) ? 8 : 12);

			// Set fallback rules set A - cf table 7-2 from ref H264
			const uint8_t* fallbackRules[12] = {
				default_4x4_intra,
				h264SPS.scaling_lists_4x4[0],
				h264SPS.scaling_lists_4x4[1],
				default_4x4_inter,
				h264SPS.scaling_lists_4x4[3],
				h264SPS.scaling_lists_4x4[4],
				default_8x8_intra,
				default_8x8_inter,
				h264SPS.scaling_lists_8x8[0],
				h264SPS.scaling_lists_8x8[1],
				h264SPS.scaling_lists_8x8[2],
				h264SPS.scaling_lists_8x8[3]
			};

			for (int i = 0; i < iListNumber; ++i) {
				h264SPS.seq_scaling_list_present_flag[i] = readBits(1);
				bool bUseDefaultScalingList = false;

				if (h264SPS.seq_scaling_list_present_flag[i]) {
					if (i < 6) {
						readScalingList(h264SPS.scaling_lists_4x4[i], 16, bUseDefaultScalingList);
					} else {
						readScalingList(h264SPS.scaling_lists_8x8[i - 6], 64, bUseDefaultScalingList);
					}
				}

				if (!h264SPS.seq_scaling_list_present_flag[i] || bUseDefaultScalingList) {
					if (i < 6) {
						std::memcpy(h264SPS.scaling_lists_4x4[i], fallbackRules[i], sizeof(uint8_t) * 16);
					} else {
						std::memcpy(h264SPS.scaling_lists_8x8[i - 6], fallbackRules[i], sizeof(uint8_t) * 64);
					}
				}
			}
		}

		break;
	default:
		break;
	}
	h264SPS.QpBdOffsetY = 6*h264SPS.bit_depth_luma_minus8;

	h264SPS.log2_max_frame_num_minus4 = readGolombUE();
	if(h264SPS.log2_max_frame_num_minus4 > 12){
		h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] log2_max_frame_num_minus4 value (" << (int)h264SPS.log2_max_frame_num_minus4 << ") not in valid range (0..12)").str());
	}
	h264SPS.pic_order_cnt_type = readGolombUE();
	if(h264SPS.pic_order_cnt_type > 2){
		h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] pic_order_cnt_type (" << (int)h264SPS.pic_order_cnt_type << ") not in valid range (0..2)").str());
	}
	if (h264SPS.pic_order_cnt_type == 0) {
		h264SPS.log2_max_pic_order_cnt_lsb_minus4 = readGolombUE();
		if(h264SPS.log2_max_pic_order_cnt_lsb_minus4 > 12){
			h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] log2_max_pic_order_cnt_lsb_minus4 value (" << (int)h264SPS.log2_max_frame_num_minus4 << ") not in valid range (0..12)").str());
		}
		h264SPS.MaxPicOrderCntLsb = 1 << (h264SPS.log2_max_pic_order_cnt_lsb_minus4+4);
	} else if (h264SPS.pic_order_cnt_type == 1) {
		h264SPS.delta_pic_order_always_zero_flag = readBits(1);
		h264SPS.offset_for_non_ref_pic = readGolombSE();
		h264SPS.offset_for_top_to_bottom_field = readGolombSE();
		h264SPS.num_ref_frames_in_pic_order_cnt_cycle = readGolombUE();
		if(h264SPS.num_ref_frames_in_pic_order_cnt_cycle > 255){
			h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] num_ref_frames_in_pic_order_cnt_cycle value (" << (int)h264SPS.num_ref_frames_in_pic_order_cnt_cycle << ") not in valid range (0..255)").str());
		}
		for (uint32_t i=0; i < h264SPS.num_ref_frames_in_pic_order_cnt_cycle; i++) {
			h264SPS.offset_for_ref_frame[i] = readGolombSE();
		}
	}
	h264SPS.max_num_ref_frames = readGolombUE();
	h264SPS.gaps_in_frame_num_value_allowed_flag = readBits(1);
	h264SPS.pic_width_in_mbs_minus1 = readGolombUE();
	h264SPS.PicWidthInMbs = h264SPS.pic_width_in_mbs_minus1 + 1;
	h264SPS.PicWidthInSamplesL = h264SPS.PicWidthInMbs * 16;
	h264SPS.pic_height_in_map_units_minus1 = readGolombUE();
	h264SPS.PicHeightInMapUnits = h264SPS.pic_height_in_map_units_minus1 + 1; 
	h264SPS.FrameHeightInMbs = (2 - h264SPS.frame_mbs_only_flag) * h264SPS.PicHeightInMapUnits;
	h264SPS.MaxDpbFrames = std::min(H264SPS::MaxDpbMbs[h264SPS.level_limit_index()]/(h264SPS.PicWidthInMbs*h264SPS.FrameHeightInMbs), 16u);
	if(h264SPS.max_num_ref_frames > h264SPS.MaxDpbFrames){
		h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] max_num_ref_frames value (" << (int)h264SPS.max_num_ref_frames << ") not in valid range (0.." << (int)h264SPS.MaxDpbFrames << ")").str());
	}
	h264SPS.PicSizeInMapUnits = h264SPS.PicWidthInMbs * h264SPS.PicHeightInMapUnits;


	h264SPS.frame_mbs_only_flag = readBits(1);
	if(!h264SPS.frame_mbs_only_flag){
		h264SPS.mb_adaptive_frame_field_flag = readBits(1);
	}

	h264SPS.direct_8x8_inference_flag = readBits(1);

	// Computing ChromaArrayType (separate_colour_plane_flag page 74)
	h264SPS.ChromaArrayType = 0;
	if (!h264SPS.separate_colour_plane_flag) {
		h264SPS.ChromaArrayType = h264SPS.chroma_format_idc;
	}

	h264SPS.CropUnitX = 1; // (7-19)
	h264SPS.CropUnitY = 2 - h264SPS.frame_mbs_only_flag; // (7-20)

	if (h264SPS.ChromaArrayType >= 1 && h264SPS.ChromaArrayType <= 3) {
		uint8_t SubWidthC = 0;
		uint8_t SubHeightC = 0;

		if (h264SPS.separate_colour_plane_flag == 0) {
			if (h264SPS.chroma_format_idc == 3) {
				SubWidthC = 1;
			} else if (h264SPS.chroma_format_idc == 1 || h264SPS.chroma_format_idc == 2) {
				SubWidthC = 2;
			} else {
				// std::cerr << "[H264::Bitstream] invalid chroma_format_idc\n"; 
				h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] chroma_format_idc value (" << (int)h264SPS.chroma_format_idc << ") not in valid range (0..3)").str());
			}

			if (h264SPS.chroma_format_idc == 1) {
				SubHeightC = 2;
			} else if (h264SPS.chroma_format_idc == 2 || h264SPS.chroma_format_idc == 3) {
				SubHeightC = 1;
			} else {
				// std::cerr << "[H264::Bitstream] invalid chroma_format_idc\n";
				h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] chroma_format_idc value (" << (int)h264SPS.chroma_format_idc << ") not in valid range (0..3)").str());
			}
		} else {
			// std::cerr << "[H264::Bitstream] invalid separate_colour_plane_flag\n";
		}

		h264SPS.CropUnitX = SubWidthC; // (7-21)
		h264SPS.CropUnitY = SubHeightC * (2 - h264SPS.frame_mbs_only_flag); // (7-22)
	}

	h264SPS.frame_cropping_flag = readBits(1);
	if(h264SPS.frame_cropping_flag){
		h264SPS.frame_crop_left_offset = readGolombUE();
		h264SPS.frame_crop_right_offset = readGolombUE();
		uint32_t frame_crop_left_limit = h264SPS.PicWidthInSamplesL/h264SPS.CropUnitX - h264SPS.frame_crop_right_offset+1; 
		if(h264SPS.frame_crop_left_offset > frame_crop_left_limit){
			h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] frame_crop_left_offset value (" << (int)h264SPS.frame_crop_left_offset << ") not in valid range (0.." << frame_crop_left_limit << ")").str());
		}
		h264SPS.frame_crop_top_offset = readGolombUE();
		h264SPS.frame_crop_bottom_offset = readGolombUE();
		uint32_t frame_crop_top_limit = 16*h264SPS.FrameHeightInMbs/h264SPS.CropUnitY - h264SPS.frame_crop_bottom_offset+1;
		if(h264SPS.frame_crop_top_offset > frame_crop_top_limit){
			h264SPS.errors.push_back((std::ostringstream() << "[H264 SPS content] frame_crop_top_offset value (" << (int)h264SPS.frame_crop_top_offset << ") not in valid range (0.." << frame_crop_top_limit << ")").str());
		}
	}

	h264SPS.vui_parameters_present_flag = readBits(1);
	if (h264SPS.vui_parameters_present_flag) {
		h264SPS.aspect_ratio_info_present_flag = readBits(1);
		if(h264SPS.aspect_ratio_info_present_flag){
			h264SPS.aspect_ratio_idc = readBits(8);
			if(h264SPS.aspect_ratio_idc == EXTENDED_SAR){
				h264SPS.sar_width = readBits(16);
				h264SPS.sar_height = readBits(16);
			}
		}
	}
	h264SPS.overscan_info_present_flag = readBits(1);
	if(h264SPS.overscan_info_present_flag) h264SPS.overscan_appropriate_flag = readBits(1);
	h264SPS.video_signal_type_present_flag = readBits(1);
	if(h264SPS.video_signal_type_present_flag){
		h264SPS.video_format = readBits(3);
		h264SPS.video_full_range_flag = readBits(1);
		h264SPS.colour_description_present_flag = readBits(1);
		if(h264SPS.colour_description_present_flag){
			h264SPS.colour_primaries = readBits(8);
			h264SPS.transfer_characteristics = readBits(8);
			h264SPS.matrix_coefficients	= readBits(8);
		}
	}
	h264SPS.chroma_loc_info_present_flag = readBits(1);
	if(h264SPS.chroma_loc_info_present_flag){
		h264SPS.chroma_sample_loc_type_top_field = readGolombUE();
		h264SPS.chroma_sample_loc_type_bottom_field = readGolombUE();
	}
	h264SPS.timing_info_present_flag = readBits(1);
	if(h264SPS.timing_info_present_flag){
		h264SPS.num_units_tick = readBits(32);
		h264SPS.time_scale = readBits(32);
		h264SPS.fixed_frame_rate_flag = readBits(1);
	}
	h264SPS.nal_hrd_parameters_present_flag = readBits(1);
	if(h264SPS.nal_hrd_parameters_present_flag){
		h264SPS.nal_cpb_cnt_minus1 = readGolombUE();
		h264SPS.nal_bit_rate_scale = readBits(4);
		h264SPS.nal_cpb_size_scale = readBits(4);
		for(int SchedSelIdx = 0;SchedSelIdx <= h264SPS.nal_cpb_cnt_minus1;++SchedSelIdx){
			h264SPS.nal_bit_rate_value_minus1[SchedSelIdx] = readGolombUE();
			h264SPS.nal_cpb_size_value_minus1[SchedSelIdx] = readGolombUE();
			h264SPS.nal_cbr_flag[SchedSelIdx] = readBits(1);
		}
		h264SPS.nal_initial_cpb_removal_delay_length_minus1 = readBits(5);
		h264SPS.nal_cpb_removal_delay_length_minus1 = readBits(5);
		h264SPS.nal_dpb_output_delay_length_minus1 = readBits(5);
		h264SPS.nal_time_offset_length = readBits(5);
	}
	h264SPS.vcl_hrd_parameters_present_flag = readBits(1);
	if(h264SPS.vcl_hrd_parameters_present_flag){
		h264SPS.vcl_cpb_cnt_minus1 = readGolombUE();
		h264SPS.vcl_bit_rate_scale = readBits(4);
		h264SPS.vcl_cpb_size_scale = readBits(4);
		for(int SchedSelIdx = 0;SchedSelIdx <= h264SPS.vcl_cpb_cnt_minus1;++SchedSelIdx){
			h264SPS.vcl_bit_rate_value_minus1[SchedSelIdx] = readGolombUE();
			h264SPS.vcl_cpb_size_value_minus1[SchedSelIdx] = readGolombUE();
			h264SPS.vcl_cbr_flag[SchedSelIdx] = readBits(1);
		}
		h264SPS.vcl_initial_cpb_removal_delay_length_minus1 = readBits(5);
		h264SPS.vcl_cpb_removal_delay_length_minus1 = readBits(5);
		h264SPS.vcl_dpb_output_delay_length_minus1 = readBits(5);
		h264SPS.vcl_time_offset_length = readBits(5);
	}
	if(h264SPS.nal_hrd_parameters_present_flag || h264SPS.vcl_hrd_parameters_present_flag){
		h264SPS.low_delay_hrd_flag = readBits(1);
		if(h264SPS.low_delay_hrd_flag){
			h264SPS.nal_cpb_cnt_minus1 = h264SPS.vcl_cpb_cnt_minus1 = 0;
		}
	}
	h264SPS.pic_struct_present_flag = readBits(1);
	h264SPS.bitstream_restriction_flag = readBits(1);
	if(h264SPS.bitstream_restriction_flag){
		h264SPS.motion_vectors_over_pic_boundaries_flag = readBits(1);
		h264SPS.max_bytes_per_pic_denom = readGolombUE();
		h264SPS.max_bits_per_mb_denom = readGolombUE();
		h264SPS.log2_max_mv_length_horizontal = readGolombUE();
		h264SPS.log2_max_mv_length_vertical = readGolombUE();
		h264SPS.max_num_reorder_frames = readGolombUE();
		h264SPS.max_dec_frame_buffering = readGolombUE();
	}
}

void H264BitstreamReader::readPPS(H264PPS& h264PPS)
{
	h264PPS.errors.clear();
	// Ref sec. 7.3.2.2
	h264PPS.pic_parameter_set_id = readGolombUE();
	h264PPS.seq_parameter_set_id = readGolombUE();
	if(h264PPS.seq_parameter_set_id > 31){
		h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] seq_parameter_set_id value (" << (int)h264PPS.seq_parameter_set_id << ") not in valid range (0..31)").str());
	}
	H264SPS* h264SPS;
	auto referencedSPS = H264SPS::SPSMap.find(h264PPS.seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()){
		// std::cerr << "PPS unit is referencing an unknown SPS unit\n";
		h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] reference to unknown SPS (" << (int)h264PPS.seq_parameter_set_id << ")").str());
		return;
	}
	h264SPS = referencedSPS->second;
	h264PPS.entropy_coding_mode_flag = readBits(1);
	h264PPS.bottom_field_pic_order_in_frame_present_flag = readBits(1);
	h264PPS.num_slice_groups_minus1 = readGolombUE();

	if (h264PPS.num_slice_groups_minus1 > 0) {
		h264PPS.slice_group_map_type = readGolombUE();
		if (h264PPS.slice_group_map_type == 0) {
			for (int iGroup = 0; iGroup <= h264PPS.num_slice_groups_minus1; iGroup++) {
				h264PPS.run_length_minus1[iGroup] = readGolombUE();
				if(h264PPS.run_length_minus1[iGroup] > h264SPS->PicSizeInMapUnits-1){
					h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] run_length_minus1[" << iGroup << "] value (" << h264PPS.run_length_minus1[iGroup] << ") not in valid range (0.." << h264SPS->PicSizeInMapUnits-1 << ")").str());
				}
			}
		}
		else if (h264PPS.slice_group_map_type == 2) {
			for (int iGroup = 0; iGroup < h264PPS.num_slice_groups_minus1; iGroup++) {
				h264PPS.top_left[iGroup] = readGolombUE();
				h264PPS.bottom_right[iGroup] = readGolombUE();
				if(h264PPS.top_left[iGroup] > h264PPS.bottom_right[iGroup]){
					h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] top_left[" << iGroup << "] value (" << h264PPS.top_left[iGroup] << ") should be less than or equal to bottom_right[" << iGroup << "] value (" << h264PPS.bottom_right[iGroup] << ")").str());
				}
				if(h264PPS.top_left[iGroup] % h264SPS->PicWidthInMbs > h264PPS.bottom_right[iGroup] % h264SPS->PicWidthInMbs){
					h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] top_left[" << iGroup << "]%PicWidthInMbs value (" << h264PPS.top_left[iGroup]%h264SPS->PicWidthInMbs << ") should be less than or equal to bottom_right[" << iGroup << "]%PicWidthInMbs value (" << h264PPS.bottom_right[iGroup]%h264SPS->PicWidthInMbs << ")").str());
				}
			}
		}
		else if( h264PPS.slice_group_map_type == 3 ||
				 h264PPS.slice_group_map_type == 4 ||
				 h264PPS.slice_group_map_type == 5 ) {
			h264PPS.slice_group_change_direction_flag = readBits(1);
			h264PPS.slice_group_change_rate_minus1 = readGolombUE();
			if(h264PPS.slice_group_change_rate_minus1 > h264SPS->PicSizeInMapUnits-1){
				h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] slice_group_change_rate_minus1 value (" << h264PPS.slice_group_change_rate_minus1 << ") not in valid range (0.." << h264SPS->PicSizeInMapUnits-1 << ")").str());
			}
		}
		else if (h264PPS.slice_group_map_type == 6) {
			h264PPS.pic_size_in_map_units_minus1 = readGolombUE();
			if(h264PPS.pic_size_in_map_units_minus1 != h264SPS->PicSizeInMapUnits-1){
				h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] pic_size_in_map_units_minus1 value (" << h264PPS.pic_size_in_map_units_minus1 << ") should be equal to " << h264SPS->PicSizeInMapUnits-1).str());
			}
			for (unsigned i = 0; i <= h264PPS.pic_size_in_map_units_minus1; i++) {
				int v = (int)log2(h264PPS.num_slice_groups_minus1 + 1);
				h264PPS.slice_group_id[i] = readBits(v);
				if(h264PPS.slice_group_id[i] > h264PPS.num_slice_groups_minus1){
					h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] slice_group_id[" << i << "] value (" << (int)h264PPS.slice_group_id[i] << ") not in valid range (0.." << h264PPS.num_slice_groups_minus1 << ")").str());
				}
			}
		} else {
			h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] slice_group_map_type value (" << (int)h264PPS.slice_group_map_type << ") not in valid range (0..6)").str());
		}
	}
	h264PPS.num_ref_idx_l0_active_minus1 = readGolombUE();
	if(h264PPS.num_ref_idx_l0_active_minus1 > 31){
		h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] num_ref_idx_l0_active_minus1 value (" << (int)h264PPS.num_ref_idx_l0_active_minus1 << ") not in valid range (0..31)").str());
	}
	h264PPS.num_ref_idx_l1_active_minus1 = readGolombUE();
	if(h264PPS.num_ref_idx_l1_active_minus1 > 31){
		h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] num_ref_idx_l1_active_minus1 value (" << (int)h264PPS.num_ref_idx_l1_active_minus1 << ") not in valid range (0..31)").str());
	}
	h264PPS.weighted_pred_flag = readBits(1);
	h264PPS.weighted_bipred_idc = readBits(2);
	if(h264PPS.weighted_bipred_idc > 2){
		h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] weighted_bipred_idc value (" << (int)h264PPS.weighted_bipred_idc << ") not in valid range (0..2)").str());
	}
	h264PPS.pic_init_qp_minus26 = readGolombSE();
	if(h264PPS.pic_init_qp_minus26 < -26 - h264SPS->QpBdOffsetY || h264PPS.pic_init_qp_minus26 > 25){
		h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] pic_init_qp_minus26 value (" << (int)h264PPS.pic_init_qp_minus26 << ") not in valid range (" << (int)(-26 - h264SPS->QpBdOffsetY ) << "..25)").str());
	}
	h264PPS.pic_init_qs_minus26 = readGolombSE();
	if(h264PPS.pic_init_qs_minus26 < -26 || h264PPS.pic_init_qs_minus26 > 25){
		h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] pic_init_qs_minus26 value (" << (int)h264PPS.pic_init_qs_minus26 << ") not in valid range (-26..25)").str());
	}
	h264PPS.chroma_qp_index_offset = readGolombSE();
	if(h264PPS.chroma_qp_index_offset < -12 || h264PPS.chroma_qp_index_offset > 12){
		h264PPS.errors.push_back((std::ostringstream() << "[H264 PPS content] chroma_qp_index_offset value (" << (int)h264PPS.chroma_qp_index_offset << ") not in valid range (-12..12)").str());
	}
	h264PPS.deblocking_filter_control_present_flag = readBits(1);
	h264PPS.constrained_intra_pred_flag = readBits(1);
	h264PPS.redundant_pic_cnt_present_flag = readBits(1);

	if (hasMoreRBSPData()) {
		h264PPS.transform_8x8_mode_flag = readBits(1);
		h264PPS.pic_scaling_matrix_present_flag = readBits(1);
		if (h264PPS.pic_scaling_matrix_present_flag) {
			// Set fallback rules set A - cf table 7-2 from ref H264
			const uint8_t* fallbackRules[12] = {
				default_4x4_intra,
				h264PPS.scaling_lists_4x4[0],
				h264PPS.scaling_lists_4x4[1],
				default_4x4_inter,
				h264PPS.scaling_lists_4x4[3],
				h264PPS.scaling_lists_4x4[4],
				default_8x8_intra,
				default_8x8_inter,
				h264PPS.scaling_lists_8x8[0],
				h264PPS.scaling_lists_8x8[1],
				h264PPS.scaling_lists_8x8[2],
				h264PPS.scaling_lists_8x8[3]
			};

			for (int i = 0; i < 6 + 2* h264PPS.transform_8x8_mode_flag; i++) {
				h264PPS.pic_scaling_list_present_flag[i] = readBits(1);
				bool bUseDefaultScalingList = false;

				if (h264PPS.pic_scaling_list_present_flag[i]) {
					if (i < 6) {
						readScalingList(h264PPS.scaling_lists_4x4[i], 16, bUseDefaultScalingList);
					}
					else {
						readScalingList(h264PPS.scaling_lists_8x8[i - 6], 64, bUseDefaultScalingList);
					}
				}

				if (!h264PPS.pic_scaling_list_present_flag[i] || bUseDefaultScalingList) {
					if (i < 6) {
						std::memcpy(h264PPS.scaling_lists_4x4[i], fallbackRules[i], sizeof(uint8_t) * 16);
					} else {
						std::memcpy(h264PPS.scaling_lists_8x8[i - 6], fallbackRules[i], sizeof(uint8_t) * 64);
					}
				}
			}
		}
		h264PPS.second_chroma_qp_index_offset = readGolombSE();
	}
	else {
		h264PPS.second_chroma_qp_index_offset = h264PPS.chroma_qp_index_offset;
	}
}

void H264BitstreamReader::readSlice(H264Slice& h264Slice)
{
	h264Slice.IdrPicFlag = h264Slice.nal_unit_type == H264NAL::UnitType_IDRFrame;
	h264Slice.first_mb_in_slice = readGolombUE();
	h264Slice.slice_type = H264Utils::getSliceType(readGolombUE());
	if(h264Slice.slice_type == H264Slice::SliceType_Unspecified){
		h264Slice.errors.push_back("[H264 Slice content] Invalid slice type");
	}
	h264Slice.pic_parameter_set_id = readGolombUE();
	if(h264Slice.pic_parameter_set_id > 255){
		h264Slice.errors.push_back((std::ostringstream() << "[H264 Slice content] pic_parameter_set_id value (" << (int)h264Slice.pic_parameter_set_id << ") not in valid range (0.255)").str());
	}

	H264PPS* pH264PPS;
	auto referencedPPS = H264PPS::PPSMap.find(h264Slice.pic_parameter_set_id);
	if(referencedPPS == H264PPS::PPSMap.end()){
		h264Slice.errors.push_back((std::ostringstream() << "[H264 Slice content] reference to unknown PPS (" << (int)h264Slice.pic_parameter_set_id << ")").str());
		return;
	}
	pH264PPS = referencedPPS->second;
	H264SPS* pH264SPS;
	auto referencedSPS = H264SPS::SPSMap.find(pH264PPS->seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()){
		h264Slice.errors.push_back((std::ostringstream() << "[H264 Slice content] This unit's PPS is referencing an unknown SPS (" << (int)pH264PPS->seq_parameter_set_id << ")").str());
		return;
	}
	pH264SPS = referencedSPS->second;
	if(h264Slice.nal_unit_type == H264NAL::UnitType_IDRFrame || pH264SPS->max_num_ref_frames == 0){
		switch(h264Slice.slice_type){
			case H264Slice::SliceType_I: case H264Slice::SliceType_SI: break;
			default:
				h264Slice.errors.push_back((std::ostringstream() << "[H264 Slice content] slice_type value (" << (int)h264Slice.slice_type-1 << ") of IDR should be in {2, 4, 7, 9}").str());
				break;
		}
	}

	if (pH264SPS->separate_colour_plane_flag)
	{
		h264Slice.colour_plane_id = readBits(2);
		if(h264Slice.colour_plane_id > 2){
			h264Slice.errors.push_back((std::ostringstream() << "[H264 Slice content] colour_plane_id value (" << (int)h264Slice.colour_plane_id << ") not in valid range (0..2)").str());
		}
	}

	h264Slice.frame_num = readBits(pH264SPS->log2_max_frame_num_minus4 + 4 );
	if(h264Slice.slice_type == H264Slice::SliceType_I && h264Slice.frame_num != 0){
		h264Slice.errors.push_back((std::ostringstream() << "[H264 Slice content] frame_num of an IDR picture (" << (int)h264Slice.frame_num << ") should be 0").str());
	}
	if (!pH264SPS->frame_mbs_only_flag)
	{
		h264Slice.field_pic_flag = readBits(1);
		if (h264Slice.field_pic_flag)
		{
			h264Slice.bottom_field_flag = readBits(1);
		}
	}
	if (h264Slice.IdrPicFlag)
	{
		h264Slice.idr_pic_id = readGolombUE();
	}
	if (pH264SPS->pic_order_cnt_type == 0)
	{
		h264Slice.pic_order_cnt_lsb = readBits(pH264SPS->log2_max_pic_order_cnt_lsb_minus4 + 4 ); // was u(v)
		if(h264Slice.pic_order_cnt_lsb > pH264SPS->MaxPicOrderCntLsb-1){
			h264Slice.errors.push_back((std::ostringstream() << "[H264 Slice content] pic_order_cnt_lsb value (" << (int)h264Slice.pic_order_cnt_lsb << ") not in valid range (0.." << pH264SPS->MaxPicOrderCntLsb-1 << ")").str());
		}
		if (pH264PPS->bottom_field_pic_order_in_frame_present_flag && !h264Slice.field_pic_flag)
		{
			h264Slice.delta_pic_order_cnt_bottom = readGolombSE();
		}
	}
	if (pH264SPS->pic_order_cnt_type == 1 && !pH264SPS->delta_pic_order_always_zero_flag)
	{
		h264Slice.delta_pic_order_cnt[0] = readGolombSE();
		if (pH264PPS->bottom_field_pic_order_in_frame_present_flag && !h264Slice.field_pic_flag)
		{
			h264Slice.delta_pic_order_cnt[1] = readGolombSE();
		}
	}
	if (pH264PPS->redundant_pic_cnt_present_flag)
	{
		h264Slice.redundant_pic_cnt = readGolombUE();
		if(h264Slice.redundant_pic_cnt > 127){
			h264Slice.errors.push_back((std::ostringstream() << "[H264 Slice content] redundant_pic_cnt (" << (int)h264Slice.redundant_pic_cnt << ") not in valid range (0..127)").str());
		}
	}
	if (h264Slice.slice_type == H264Slice::SliceType_B)
	{
		h264Slice.direct_spatial_mv_pred_flag = readBits(1);
	}
	if ((h264Slice.slice_type == H264Slice::SliceType_P) || (h264Slice.slice_type == H264Slice::SliceType_SP) || (h264Slice.slice_type == H264Slice::SliceType_B))
	{
		h264Slice.num_ref_idx_active_override_flag = readBits(1);
		if (h264Slice.num_ref_idx_active_override_flag)
		{
			h264Slice.num_ref_idx_l0_active_minus1 = readGolombUE(); // FIXME does this modify the pps?
			if (h264Slice.slice_type == H264Slice::SliceType_B)
			{
				h264Slice.num_ref_idx_l1_active_minus1 = readGolombUE();
			}
		}
	}

	readRefPicListReordering(h264Slice);

	if (( pH264PPS->weighted_pred_flag && ( (h264Slice.slice_type == H264Slice::SliceType_P) || (h264Slice.slice_type == H264Slice::SliceType_SP) )) ||
		( pH264PPS->weighted_bipred_idc == 1 && (h264Slice.slice_type == H264Slice::SliceType_B) ) )
	{
		readPredWeightTable(*pH264SPS, *pH264PPS, h264Slice);
	}
	if (h264Slice.nal_ref_idc != 0)
	{
		readDecRefPicMarking(h264Slice);
	}
	if (pH264PPS->entropy_coding_mode_flag && (h264Slice.slice_type != H264Slice::SliceType_I) && (h264Slice.slice_type != H264Slice::SliceType_SI))
	{
		h264Slice.cabac_init_idc = readGolombUE();
	}
	h264Slice.slice_qp_delta = readGolombSE();
	if ((h264Slice.slice_type == H264Slice::SliceType_SP) || (h264Slice.slice_type == H264Slice::SliceType_SI))
	{
		if (h264Slice.slice_type == H264Slice::SliceType_SP)
		{
			h264Slice.sp_for_switch_flag = readBits(1);
		}
		h264Slice.slice_qs_delta = readGolombSE();
	}
	if (pH264PPS->deblocking_filter_control_present_flag)
	{
		h264Slice.disable_deblocking_filter_idc = readGolombUE();
		if (h264Slice.disable_deblocking_filter_idc != 1)
		{
			h264Slice.slice_alpha_c0_offset_div2 = readGolombSE();
			h264Slice.slice_beta_offset_div2 = readGolombSE();
		}
	}
	if( pH264PPS->num_slice_groups_minus1 > 0 &&
		pH264PPS->slice_group_map_type >= 3 && pH264PPS->slice_group_map_type <= 5)
	{
		int v = (int)log2(pH264PPS->pic_size_in_map_units_minus1 + pH264PPS->slice_group_change_rate_minus1 + 1);
		h264Slice.slice_group_change_cycle = readBits(v); // FIXME add 2?
	}
}

void H264BitstreamReader::readScalingList(uint8_t* pScalingList, uint8_t iScalingListSize, bool bUseDefaultScalingList)
{
	uint32_t lastScale = 8;
	uint32_t nextScale = 8;

	const uint8_t *pRasterScan = ((iScalingListSize == 16) ? g_rasterScan4x4 : g_rasterScan8x8);

	for (int j = 0; j < iScalingListSize; ++j) {
		if (nextScale != 0) {
			int32_t delta_scale = readGolombSE();
			nextScale = (lastScale + delta_scale + 256) % 256;

			bUseDefaultScalingList = ((j == 0) && (nextScale == 0));
		}

		pScalingList[pRasterScan[j]] = ((nextScale == 0) ? lastScale : nextScale);
		lastScale = pScalingList[pRasterScan[j]];
	}
}

void H264BitstreamReader::readRefPicListReordering(H264Slice& h264Slice)
{
	if ((h264Slice.slice_type != H264Slice::SliceType_I) && (h264Slice.slice_type != H264Slice::SliceType_SI) )
	{
		h264Slice.rplr.ref_pic_list_reordering_flag_l0 = readBits(1);
		if (h264Slice.rplr.ref_pic_list_reordering_flag_l0)
		{
			int n = -1;
			do
			{
				n++;
				h264Slice.rplr.reorder_l0[n].reordering_of_pic_nums_idc = readGolombUE();
				if( h264Slice.rplr.reorder_l0[n].reordering_of_pic_nums_idc == 0 ||
					h264Slice.rplr.reorder_l0[n].reordering_of_pic_nums_idc == 1 )
				{
					h264Slice.rplr.reorder_l0[n].abs_diff_pic_num_minus1 = readGolombUE();
				}
				else if (h264Slice.rplr.reorder_l0[n].reordering_of_pic_nums_idc == 2)
				{
					h264Slice.rplr.reorder_l0[n].long_term_pic_num = readGolombUE();
				}
			} while( h264Slice.rplr.reorder_l0[n].reordering_of_pic_nums_idc != 3 && (m_iRemainingBits > 0));
		}
	}
	if (h264Slice.slice_type == H264Slice::SliceType_B)
	{
		h264Slice.rplr.ref_pic_list_reordering_flag_l1 = readBits(1);
		if (h264Slice.rplr.ref_pic_list_reordering_flag_l1)
		{
			int n = -1;
			do
			{
				n++;
				h264Slice.rplr.reorder_l1[n].reordering_of_pic_nums_idc = readGolombUE();
				if( h264Slice.rplr.reorder_l1[n].reordering_of_pic_nums_idc == 0 ||
					h264Slice.rplr.reorder_l1[n].reordering_of_pic_nums_idc == 1 )
				{
					h264Slice.rplr.reorder_l1[n].abs_diff_pic_num_minus1 = readGolombUE();
				}
				else if (h264Slice.rplr.reorder_l1[n].reordering_of_pic_nums_idc == 2)
				{
					h264Slice.rplr.reorder_l1[n].long_term_pic_num = readGolombUE();
				}
			} while( h264Slice.rplr.reorder_l1[n].reordering_of_pic_nums_idc != 3 && (m_iRemainingBits > 0));
		}
	}
}

void H264BitstreamReader::readPredWeightTable(const H264SPS& h264SPS, const H264PPS& h264PPS, H264Slice& h264Slice)
{
	h264Slice.pwt.luma_log2_weight_denom = readGolombUE();
	if (h264SPS.chroma_format_idc != 0)
	{
		h264Slice.pwt.chroma_log2_weight_denom = readGolombUE();
	}
	for (int i = 0; i <= h264PPS.num_ref_idx_l0_active_minus1; i++)
	{
		h264Slice.pwt.luma_weight_l0_flag[i] = readBits(1);
		if (h264Slice.pwt.luma_weight_l0_flag[i])
		{
			h264Slice.pwt.luma_weight_l0[i] = readGolombSE();
			h264Slice.pwt.luma_offset_l0[i] = readGolombSE();
		}
		if ( h264SPS.chroma_format_idc != 0 )
		{
			h264Slice.pwt.chroma_weight_l0_flag[i] = readBits(1);
			if (h264Slice.pwt.chroma_weight_l0_flag[i])
			{
				for (int j =0; j < 2; j++)
				{
					h264Slice.pwt.chroma_weight_l0[i][j] = readGolombSE();
					h264Slice.pwt.chroma_offset_l0[i][j] = readGolombSE();
				}
			}
		}
	}
	if (h264Slice.slice_type != H264Slice::SliceType_B)
	{
		for (int i = 0; i <= h264PPS.num_ref_idx_l1_active_minus1; i++)
		{
			h264Slice.pwt.luma_weight_l1_flag[i] = readBits(1);
			if (h264Slice.pwt.luma_weight_l1_flag[i])
			{
				h264Slice.pwt.luma_weight_l1[i] = readGolombSE();
				h264Slice.pwt.luma_offset_l1[i] = readGolombSE();
			}
			if (h264SPS.chroma_format_idc != 0)
			{
				h264Slice.pwt.chroma_weight_l1_flag[i] = readBits(1);
				if (h264Slice.pwt.chroma_weight_l1_flag[i])
				{
					for (int j = 0; j < 2; j++)
					{
						h264Slice.pwt.chroma_weight_l1[i][j] = readGolombSE();
						h264Slice.pwt.chroma_offset_l1[i][j] = readGolombSE();
					}
				}
			}
		}
	}
}

void H264BitstreamReader::readDecRefPicMarking(H264Slice& h264Slice)
{
	if (h264Slice.nal_unit_type == H264NAL::UnitType_IDRFrame)
	{
		h264Slice.drpm.no_output_of_prior_pics_flag = readBits(1);
		h264Slice.drpm.long_term_reference_flag = readBits(1);
	}
	else
	{
		h264Slice.drpm.adaptive_ref_pic_marking_mode_flag = readBits(1);
		if (h264Slice.drpm.adaptive_ref_pic_marking_mode_flag)
		{
			int n = -1;
			do
			{
				n++;
				h264Slice.drpm.memory_management_control_operation[n] = readGolombUE();
				if( h264Slice.drpm.memory_management_control_operation[n] == 1 ||
					h264Slice.drpm.memory_management_control_operation[n] == 3 )
				{
					h264Slice.drpm.difference_of_pic_nums_minus1[n] = readGolombUE();
				}
				if(h264Slice.drpm.memory_management_control_operation[n] == 2 )
				{
					h264Slice.drpm.long_term_pic_num[n] = readGolombUE();
				}
				if( h264Slice.drpm.memory_management_control_operation[n] == 3 ||
					h264Slice.drpm.memory_management_control_operation[n] == 6 )
				{
					h264Slice.drpm.long_term_frame_idx[n] = readGolombUE();
				}
				if (h264Slice.drpm.memory_management_control_operation[n] == 4)
				{
					h264Slice.drpm.max_long_term_frame_idx_plus1[n] = readGolombUE();
				}
			} while (h264Slice.drpm.memory_management_control_operation[n] != 0 && m_iRemainingBits > 0);
		}
	}
}

void H264BitstreamReader::readAUD(H264AUD& h264AUD){
	h264AUD.primary_pic_type = readBits(3);
}

void H264BitstreamReader::readSEI(H264SEI& h264SEI, const H264SPS& activeSPS){
	do {
		uint64_t payloadType = 0;
		uint8_t last_payload_type_byte = readBits(8);
		while(last_payload_type_byte == 0xFF){
			payloadType += last_payload_type_byte;
			last_payload_type_byte = readBits(8);
		}
		payloadType += last_payload_type_byte;
		uint64_t payloadSize = 0;
		uint8_t last_payload_size_byte = readBits(8);
		while(last_payload_size_byte == 0xFF){
			payloadSize += last_payload_size_byte;
			last_payload_size_byte = readBits(8);
		}
		payloadSize += last_payload_size_byte;
		if(payloadSize*8 > m_iRemainingBits) return;
		switch(payloadType){
			case SEI_BUFFERING_PERIOD:
				readSEIBufferingPeriod(h264SEI);
				break;
			case SEI_PIC_TIMING:
				readSEIPicTiming(h264SEI, activeSPS);
				break;
			case SEI_FILLER_PAYLOAD:
				readSEIFillerPayload(h264SEI, payloadSize);
				break;
			case SEI_USER_DATA_UNREGISTERED:
				readSEIUserDataUnregistered(h264SEI, payloadSize);
				break;
			case SEI_RECOVERY_POINT:
				readSEIRecoveryPoint(h264SEI, activeSPS);
				break;
			case SEI_FULL_FRAME_FREEZE:
				readSEIFullFrameFreeze(h264SEI);
				break;
			case SEI_MVCD_VIEW_SCALABILITY_INFO:
				readSEIMvcdViewScalabilityInfo(h264SEI, activeSPS);
				break;
			default:
				std::cerr << "Unsupported SEI type : " << payloadType << "\n";
				skipBits(8*payloadSize);
		}
		uint8_t byte_bit_offset = m_iBitsOffset%8;
		if(byte_bit_offset != 0) skipBits(8-byte_bit_offset);
	} while(hasMoreRBSPData());
}

void H264BitstreamReader::readSEIBufferingPeriod(H264SEI& h264SEI){
	H264SEIBufferingPeriod* h264SEImsg = new H264SEIBufferingPeriod();
	h264SEI.messages.push_back(h264SEImsg);
	h264SEImsg->payloadType = SEI_BUFFERING_PERIOD;
	h264SEImsg->seq_parameter_set_id = readGolombUE();
	auto referencedSPS = H264SPS::SPSMap.find(h264SEImsg->seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()){
		h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Buffering period] unknown reference to a SPS unit (" << (int)h264SEImsg->seq_parameter_set_id << ")").str());
		return;
	}
	H264SPS* pH264SPS = referencedSPS->second;
	if(h264SEImsg->seq_parameter_set_id > 31){
		h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Buffering period] seq_parameter_set_id value (" << (int)h264SEImsg->seq_parameter_set_id << ") not in valid range (0..31)").str());
	}
	if(pH264SPS->nal_hrd_parameters_present_flag){
		for(int SchedSelIdx = 0;SchedSelIdx <= pH264SPS->nal_cpb_cnt_minus1;++SchedSelIdx){
			h264SEImsg->nal_initial_cpb_removal_delay[SchedSelIdx] = readBits(pH264SPS->nal_initial_cpb_removal_delay_length_minus1+1);
			int CpbSize = (pH264SPS->nal_cpb_size_value_minus1[SchedSelIdx]+1)*(pow(2, 4+pH264SPS->nal_cpb_size_scale));
			int BitRate = (pH264SPS->nal_bit_rate_value_minus1[SchedSelIdx]+1)*(pow(2, 6+pH264SPS->nal_bit_rate_scale));
			int delay_limit = 90000 * (CpbSize/BitRate);
			if(h264SEImsg->nal_initial_cpb_removal_delay[SchedSelIdx] == 0 || h264SEImsg->nal_initial_cpb_removal_delay[SchedSelIdx] > delay_limit){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Buffering period] nal_initial_cpb_removal_delay[" << SchedSelIdx << "] value (" << h264SEImsg->nal_initial_cpb_removal_delay[SchedSelIdx] << ") not in valid range (1.." << delay_limit << ")").str());
			}
			h264SEImsg->nal_initial_cpb_removal_delay_offset[SchedSelIdx] = readBits(pH264SPS->nal_initial_cpb_removal_delay_length_minus1+1);
		}
	}
	if(pH264SPS->vcl_hrd_parameters_present_flag){
		for(int SchedSelIdx = 0;SchedSelIdx <= pH264SPS->vcl_cpb_cnt_minus1;++SchedSelIdx){
			h264SEImsg->vcl_initial_cpb_removal_delay[SchedSelIdx] = readBits(pH264SPS->vcl_initial_cpb_removal_delay_length_minus1+1);
			int CpbSize = (pH264SPS->vcl_cpb_size_value_minus1[SchedSelIdx]+1)*(pow(2, 4+pH264SPS->vcl_cpb_size_scale));
			int BitRate = (pH264SPS->vcl_bit_rate_value_minus1[SchedSelIdx]+1)*(pow(2, 6+pH264SPS->vcl_bit_rate_scale));
			int delay_limit = 90000 * (CpbSize/BitRate);
			if(h264SEImsg->vcl_initial_cpb_removal_delay[SchedSelIdx] == 0 || h264SEImsg->vcl_initial_cpb_removal_delay[SchedSelIdx] > delay_limit){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Buffering period] vcl_initial_cpb_removal_delay[" << SchedSelIdx << "] value (" << h264SEImsg->vcl_initial_cpb_removal_delay[SchedSelIdx] << ") not in valid range (1.." << delay_limit << ")").str());
			}
			h264SEImsg->vcl_initial_cpb_removal_delay_offset[SchedSelIdx] = readBits(pH264SPS->vcl_initial_cpb_removal_delay_length_minus1+1);
		}
	}
}

void H264BitstreamReader::readSEIPicTiming(H264SEI& h264SEI, const H264SPS& activeSPS){
	H264SEIPicTiming* h264SEImsg = new H264SEIPicTiming();
	h264SEI.messages.push_back(h264SEImsg);
	h264SEImsg->payloadType = SEI_PIC_TIMING;
	h264SEImsg->seq_parameter_set_id = activeSPS.seq_parameter_set_id;
	if(activeSPS.nal_hrd_parameters_present_flag || activeSPS.vcl_hrd_parameters_present_flag){
		h264SEImsg->cpb_removal_delay = readBits(std::max(activeSPS.nal_cpb_removal_delay_length_minus1+1, activeSPS.vcl_cpb_removal_delay_length_minus1+1));
		h264SEImsg->dpb_output_delay = readBits(std::max(activeSPS.nal_dpb_output_delay_length_minus1+1, activeSPS.vcl_dpb_output_delay_length_minus1+1));
		if(activeSPS.max_dec_frame_buffering == 0) h264SEImsg->dpb_output_delay = 0;
	}
	if(activeSPS.pic_struct_present_flag){
		h264SEImsg->pic_struct = readBits(4);
		if(h264SEImsg->pic_struct > 8) h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Pic timing] pic_struct value (" << h264SEImsg->pic_struct << ") not in valid range (0..8)").str());
		int NumClockTS = 0;
		switch(h264SEImsg->pic_struct){
			case 0: case 1: case 2: 
				NumClockTS = 1;
				break;
			case 3: case 4: case 7:
				NumClockTS = 2;
				break;
			case 5: case 6: case 8:
				NumClockTS = 3;
				break;
		}
		for(int i = 0;i < NumClockTS;++i){
			h264SEImsg->clock_timestamp_flag[i] = readBits(1);
			if(h264SEImsg->clock_timestamp_flag[i]){
				h264SEImsg->ct_type[i] = readBits(2);
				if(h264SEImsg->ct_type[i] > 2){
					h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Pic timing] ct_type[" << i << "] value (" << h264SEImsg->ct_type[i] << ") not in valid range (0..2)").str());
				}
				h264SEImsg->nuit_field_based_flag[i] = readBits(1);
				h264SEImsg->counting_type[i] = readBits(5);
				if(h264SEImsg->counting_type[i] > 6){
					h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Pic timing] counting_type[" << i << "] value (" << h264SEImsg->counting_type[i] << ") not in valid range (0..6)").str());
				}
				h264SEImsg->full_timestamp_flag[i] = readBits(1);
				h264SEImsg->discontinuity_flag[i] = readBits(1);
				h264SEImsg->cnt_dropped_flag[i] = readBits(1);
				h264SEImsg->n_frames[i] = readBits(8);
				if(h264SEImsg->full_timestamp_flag){
					h264SEImsg->seconds_value[i] = readBits(6);
					h264SEImsg->minutes_value[i] = readBits(6);
					h264SEImsg->hours_value[i] = readBits(5);
				} else {
					h264SEImsg->seconds_flag[i] = readBits(1);
					if(h264SEImsg->seconds_flag[i]){
						h264SEImsg->seconds_value[i] = readBits(6);
						h264SEImsg->minutes_flag[i] = readBits(1);
						if(h264SEImsg->minutes_flag[i]){
							h264SEImsg->minutes_value[i] = readBits(6);
							h264SEImsg->hours_flag[i] = readBits(1);
							if(h264SEImsg->hours_value) h264SEImsg->hours_value[i] = readBits(5);
						}
					}
				}
				if(h264SEImsg->seconds_value[i] > 59){
					h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Pic timing] seconds_value[" << i << "] value (" << h264SEImsg->seconds_value[i] << ") not in valid range (0..59)").str());
				}
				if(h264SEImsg->minutes_value[i] > 59){
					h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Pic timing] minutes_value[" << i << "] value (" << h264SEImsg->minutes_value[i] << ") not in valid range (0..59)").str());
				}
				if(h264SEImsg->hours_value[i] > 23){
					h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Pic timing] hours_value[" << i << "] value (" << h264SEImsg->hours_value[i] << ") not in valid range (0..23)").str());
				}
				if(activeSPS.nal_time_offset_length > 0 || activeSPS.vcl_time_offset_length > 0){
					h264SEImsg->time_offset[i] = readBits(std::max(activeSPS.nal_time_offset_length, activeSPS.vcl_time_offset_length));
				}
			}
		}
	}
}

void H264BitstreamReader::readSEIFillerPayload(H264SEI& h264SEI, uint payloadSize){
	H264SEIFillerPayload* h264SEImsg = new H264SEIFillerPayload();
	h264SEI.messages.push_back(h264SEImsg);
	h264SEImsg->payloadType = SEI_FILLER_PAYLOAD;
	skipBits(8*payloadSize);
}

void H264BitstreamReader::readSEIUserDataUnregistered(H264SEI& h264SEI, uint payloadSize){
	H264SEIUserDataUnregistered* h264SEImsg = new H264SEIUserDataUnregistered();
	h264SEI.messages.push_back(h264SEImsg);
	h264SEImsg->payloadType = SEI_USER_DATA_UNREGISTERED;
	h264SEImsg->uuid_iso_iec_11578 = (__uint128_t)readBits(32) << 96;
	h264SEImsg->uuid_iso_iec_11578 = h264SEImsg->uuid_iso_iec_11578 | ((__uint128_t)readBits(32) << 64);
	h264SEImsg->uuid_iso_iec_11578 = h264SEImsg->uuid_iso_iec_11578 | ((__uint128_t)readBits(32) << 32);
	h264SEImsg->uuid_iso_iec_11578 = h264SEImsg->uuid_iso_iec_11578 | readBits(32);
	for(int i = 16;i < payloadSize;++i) h264SEImsg->user_data_payload_byte.push_back(readBits(8));
}

void H264BitstreamReader::readSEIRecoveryPoint(H264SEI& h264SEI, const H264SPS& activeSPS){
	H264SEIRecoveryPoint* h264SEImsg = new H264SEIRecoveryPoint();
	h264SEI.messages.push_back(h264SEImsg);
	h264SEImsg->payloadType = SEI_RECOVERY_POINT;
	h264SEImsg->recovery_frame_cnt = readGolombUE();
	int MaxNumFrames = pow(2, 4+activeSPS.log2_max_frame_num_minus4);
	if(h264SEImsg->recovery_frame_cnt > MaxNumFrames-1){
		h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Recovery point] recovery_frame_cnt value (" << h264SEImsg->recovery_frame_cnt << ") not in valid range (0.." << MaxNumFrames-1 << ")").str());
	}
	h264SEImsg->exact_match_flag = readBits(1);
	h264SEImsg->broken_link_flag = readBits(1);
	h264SEImsg->changing_slice_group_idc = readBits(2);
}

void H264BitstreamReader::readSEIFullFrameFreeze(H264SEI& h264SEI){
	H264SEIFullFrameFreeze* h264SEImsg = new H264SEIFullFrameFreeze();
	h264SEI.messages.push_back(h264SEImsg);
	h264SEImsg->payloadType = SEI_FULL_FRAME_FREEZE;
	h264SEImsg->full_frame_freeze_repetition_period = readGolombUE();
	if(h264SEImsg->full_frame_freeze_repetition_period > 16384){
		h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Full frame freeze] full_frame_freeze_repetition_period value (" << h264SEImsg->full_frame_freeze_repetition_period << ") not in valid range (0..16384)").str());
	}
}

void H264BitstreamReader::readSEIMvcdViewScalabilityInfo(H264SEI& h264SEI, const H264SPS& activeSPS){
	H264SEIMvcdViewScalabilityInfo* h264SEImsg = new H264SEIMvcdViewScalabilityInfo();
	h264SEI.messages.push_back(h264SEImsg);
	h264SEImsg->payloadType = SEI_MVCD_VIEW_SCALABILITY_INFO;
	h264SEImsg->num_operation_points_minus1 = readGolombUE();
	if(h264SEImsg->num_operation_points_minus1 > 1023){
		h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] num_operation_points_minus1 value (" << h264SEImsg->num_operation_points_minus1 << ") not in valid range (0..1023)").str());
	}
	for(int i = 0;i <= h264SEImsg->num_operation_points_minus1;++i){
		h264SEImsg->operation_point_id[i] = readGolombUE();
		h264SEImsg->priority_id[i] = readBits(5);
		h264SEImsg->temporal_id[i] = readBits(3);
		h264SEImsg->num_target_output_views_minus1[i] = readGolombUE();
		if(h264SEImsg->num_target_output_views_minus1[i] > 1023){
			h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] num_target_output_views_minus1[" << i <<"] value (" << h264SEImsg->num_target_output_views_minus1[i] << ") not in valid range (0..1023)").str());
		}
		for(int j = 0;j <= h264SEImsg->num_target_output_views_minus1[i];++j){
			h264SEImsg->view_id[i][j] = readGolombUE();
			if(h264SEImsg->view_id[i][j] > 1023){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] view_id[" << i <<"][" << j << "] value (" << h264SEImsg->view_id[i][j] << ") not in valid range (0..1023)").str());
			}
			readSEIMvcdOpViewInfo(h264SEImsg->view_movi[i][j]);
		}
		h264SEImsg->profile_level_info_present_flag[i] = readBits(1);
		h264SEImsg->bitrate_info_present_flag[i] = readBits(1);
		h264SEImsg->frm_rate_info_present_flag[i] = readBits(1);
		if(!h264SEImsg->num_target_output_views_minus1[i]) h264SEImsg->view_dependency_info_present_flag[i] = readBits(1);
		h264SEImsg->parameter_sets_info_src_op_id[i] = readBits(1);
		h264SEImsg->bitstream_restriction_info_present_flag[i] = readBits(1);
		if(h264SEImsg->profile_level_info_present_flag[i]) h264SEImsg->op_profile_level_idc[i] = readBits(24);
		if(h264SEImsg->bitrate_info_present_flag[i]){
			h264SEImsg->avg_bitrate[i] = readBits(16);
			h264SEImsg->max_bitrate[i] = readBits(16);
			h264SEImsg->max_bitrate_calc_window[i] = readBits(16);
		}
		if(h264SEImsg->frm_rate_info_present_flag[i]){
			h264SEImsg->constant_frm_rate_idc[i] = readBits(2);
			if(h264SEImsg->constant_frm_rate_idc[i] > 2){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] constant_frm_rate_idc[" << i <<"] value (" << (int)h264SEImsg->constant_frm_rate_idc[i] << ") not in valid range (0..2)").str());
			}
			h264SEImsg->avg_frm_rate[i] = readBits(16);
		}
		if(h264SEImsg->view_dependency_info_present_flag[i]){
			h264SEImsg->num_directly_dependant_views[i] = readGolombUE();
			if(h264SEImsg->num_directly_dependant_views[i] > 16){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] num_directly_dependant_views[" << i <<"] value (" << (int)h264SEImsg->num_directly_dependant_views[i] << ") not in valid range (0..16)").str());
			}
			for(int j = 0;j < h264SEImsg->num_directly_dependant_views[i];++j){
				h264SEImsg->directly_dependant_view_id[i][j] = readGolombUE();
				if(h264SEImsg->directly_dependant_view_id[i][j] > 1023){
					h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] directly_dependant_view_id[" << i <<"][" << j << "] value (" << (int)h264SEImsg->directly_dependant_view_id[i][j] << ") not in valid range (0..1023)").str());
				}
				readSEIMvcdOpViewInfo(h264SEImsg->directely_dependant_view_movi[i][j]);
			}
		} else h264SEImsg->view_dependency_info_src_op_id[i] = readGolombUE();
		if(h264SEImsg->parameters_sets_info_present_flag[i]) {
			h264SEImsg->num_seq_parameter_sets[i] = readGolombUE();
			if(h264SEImsg->num_seq_parameter_sets[i] > 32){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] num_seq_parameter_sets[" << i <<"] value (" << (int)h264SEImsg->num_seq_parameter_sets[i] << ") not in valid range (0..32)").str());
			}
			for(int j = 0;j <= h264SEImsg->num_seq_parameter_sets[i];++j){
				h264SEImsg->seq_parameter_set_id_delta[i][j] = readGolombUE();
				if(h264SEImsg->seq_parameter_set_id_delta[i][j] > 31){
					h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] seq_parameter_set_id_delta[" << i <<"][" << j << "] value (" << (int)h264SEImsg->seq_parameter_set_id_delta[i][j] << ") not in valid range (0..31)").str());
				}
			}
			h264SEImsg->num_subset_seq_parameter_set_minus1[i] = readGolombUE();
			if(h264SEImsg->num_subset_seq_parameter_set_minus1[i] > 32){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] num_subset_seq_parameter_set_minus1[" << i <<"] value (" << (int)h264SEImsg->num_subset_seq_parameter_set_minus1[i] << ") not in valid range (0..32)").str());
			}
			for(int j = 0;j <= h264SEImsg->num_subset_seq_parameter_set_minus1[i];++j){
				h264SEImsg->subset_seq_parameter_set_id_delta[i][j] = readGolombUE();
				if(h264SEImsg->seq_parameter_set_id_delta[i][j] > 31){
					h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] seq_parameter_set_id_delta[" << i <<"][" << j << "] value (" << (int)h264SEImsg->seq_parameter_set_id_delta[i][j] << ") not in valid range (0..31)").str());
				}
			}
			h264SEImsg->num_pic_parameter_set_minus1[i] = readGolombUE();
			for(int j = 0;j <= h264SEImsg->num_pic_parameter_set_minus1[i];++j){
				h264SEImsg->pic_parameter_set_id_delta[i][j] = readGolombUE();
			}
		} else h264SEImsg->parameter_sets_info_src_op_id[i] = readGolombUE();
		if(h264SEImsg->bitstream_restriction_info_present_flag[i]){
			h264SEImsg->motion_vector_over_pic_boundaries_flag[i] = readBits(1);
			h264SEImsg->max_bytes_per_pic_denom[i] = readGolombUE();
			if(h264SEImsg->max_bytes_per_pic_denom[i] > 16){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] max_bytes_per_pic_denom[" << i <<"] value (" << (int)h264SEImsg->max_bytes_per_pic_denom[i] << ") not in valid range (0..16)").str());
			}
			h264SEImsg->max_bits_per_mb_denom[i] = readGolombUE();
			if(h264SEImsg->max_bits_per_mb_denom[i] > 16){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] max_bits_per_mb_denom[" << i <<"] value (" << (int)h264SEImsg->max_bits_per_mb_denom[i] << ") not in valid range (0..16)").str());
			}
			h264SEImsg->log2_max_mv_length_horizontal[i] = readGolombUE();
			if(h264SEImsg->log2_max_mv_length_horizontal[i] > 16){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] log2_max_mv_length_horizontal[" << i <<"] value (" << (int)h264SEImsg->log2_max_mv_length_horizontal[i] << ") not in valid range (0..16)").str());
			}
			h264SEImsg->log2_max_mv_length_vertical[i] = readGolombUE();
			if(h264SEImsg->log2_max_mv_length_vertical[i] > 16){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] max_bits_per_mb_denom[" << i <<"] value (" << (int)h264SEImsg->log2_max_mv_length_vertical[i] << ") not in valid range (0..16)").str());
			}
			h264SEImsg->num_reorder_frames[i] = readGolombUE();
			if(h264SEImsg->num_reorder_frames[i] > 16){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] num_reorder_frames[" << i <<"] value (" << (int)h264SEImsg->num_reorder_frames[i] << ") not in valid range (0..16)").str());
			}
			h264SEImsg->max_dec_frame_buffering[i] = readGolombUE();
			if(h264SEImsg->num_reorder_frames[i] > activeSPS.MaxDpbFrames){
				h264SEI.errors.push_back((std::ostringstream() << "[H264 SEI Mcvd view scalability info] num_reorder_frames[" << i <<"] value (" << h264SEImsg->num_reorder_frames[i] << ") not in valid range (0.." << (int)activeSPS.MaxDpbFrames << ")").str());
			}
		}
	}
}

void H264BitstreamReader::readSEIMvcdOpViewInfo(H264SEIMvcdViewScalabilityInfo::movi& movi){
	movi.view_info_depth_view_present_flag = readBits(1);
	if(movi.view_info_depth_view_present_flag) movi.mvcd_depth_view_flag = readBits(1);
	movi.view_info_texture_view_present_flag = readBits(1);
	if(movi.view_info_texture_view_present_flag) movi.mvcd_texture_view_flag = readBits(1);
}