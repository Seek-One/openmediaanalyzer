#ifndef TOOLKIT_CODEC_UTILS_H264SPS_H_
#define TOOLKIT_CODEC_UTILS_H264SPS_H_

#define EXTENDED_SAR 255

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "H264NAL.h"

struct H264SPS
{
	uint8_t profile_idc;

	uint8_t constrained_set0_flag;
	uint8_t constrained_set1_flag;
	uint8_t constrained_set2_flag;
	uint8_t constrained_set3_flag;
	uint8_t constrained_set4_flag;
	uint8_t reserved_zero;

	uint8_t level_idc;
	uint32_t seq_parameter_set_id;

	uint32_t chromat_format_idc;
	uint8_t separate_colour_plane_flag;
	uint32_t bit_depth_luma_minus8;
	uint32_t bit_depth_chroma_minus8;
	uint8_t qpprime_y_zero_tranform_bypass_flag;
	uint8_t seq_scaling_matrix_present_flag;
	uint8_t seq_scaling_list_present_flag[12];

	uint32_t log2_max_frame_num_minus4;
	uint32_t pic_order_cnt_type;
	uint32_t log2_max_pic_order_cnt_lsb_minus4;
	uint8_t delta_pic_order_always_zero_flag;
	int32_t offset_for_non_ref_pic;
	int32_t offset_for_top_to_bottom_field;
	uint32_t num_ref_frames_in_pic_order_cnt_cycle;
	uint32_t offset_for_ref_frame;

	uint32_t max_num_ref_frames;
	uint8_t gaps_in_frame_num_value_allowed_flag;
	uint32_t pic_width_in_mbs_minus1;
	uint32_t pic_height_in_map_units_minus1;

	uint8_t frame_mbs_only_flag;
	uint8_t mb_adaptive_frame_field_flag;

	uint8_t direct_8x8_inference_flag;

	uint8_t frame_cropping_flag;
	uint8_t frame_crop_left_offset;
	uint8_t frame_crop_right_offset;
	uint8_t frame_crop_top_offset;
	uint8_t frame_crop_bottom_offset;
};

struct H264SPS2 : public H264NAL {
	H264SPS2();
	~H264SPS2();

	uint8_t profile_idc;
	uint8_t constraint_set0_flag;
	uint8_t constraint_set1_flag;
	uint8_t constraint_set2_flag;
	uint8_t constraint_set3_flag;
	uint8_t constraint_set4_flag;
	uint8_t constraint_set5_flag;
	uint8_t reserved_zero_2bits;
	uint8_t level_idc;

	uint8_t seq_parameter_set_id;
	uint8_t chroma_format_idc;
	uint8_t separate_colour_plane_flag;
	uint8_t bit_depth_luma_minus8;
	uint8_t bit_depth_chroma_minus8;
	uint8_t qpprime_y_zero_transform_bypass_flag;

	uint8_t seq_scaling_matrix_present_flag;
	uint8_t seq_scaling_list_present_flag[12];
	uint8_t scaling_lists_4x4[6][16];
	uint8_t scaling_lists_8x8[6][64];

	uint8_t log2_max_frame_num_minus4;
	uint8_t pic_order_cnt_type;

	uint8_t log2_max_pic_order_cnt_lsb_minus4;

	uint8_t delta_pic_order_always_zero_flag;
	int32_t offset_for_non_ref_pic;
	int32_t offset_for_top_to_bottom_field;
	uint32_t num_ref_frames_in_pic_order_cnt_cycle;
	int32_t offset_for_ref_frame[256];

	uint32_t max_num_ref_frames;
	uint8_t gaps_in_frame_num_value_allowed_flag;
	uint32_t pic_width_in_mbs_minus1;
	uint32_t pic_height_in_map_units_minus1;
	uint8_t frame_mbs_only_flag;

	uint8_t mb_adaptive_frame_field_flag;

	uint8_t direct_8x8_inference_flag;

	uint8_t QpBdOffsetY;
	uint32_t PicHeightInMapUnits;
	uint32_t PicWidthInSamplesL;
	uint32_t FrameHeightInMbs;
	uint32_t PicWidthInMbs;
	uint32_t PicSizeInMapUnits;
	uint32_t MaxPicOrderCntLsb;
	uint8_t MaxDpbFrames;
	uint8_t ChromaArrayType;
	uint8_t CropUnitX;
	uint8_t CropUnitY;

	uint8_t frame_cropping_flag;
	uint32_t frame_crop_left_offset;
	uint32_t frame_crop_right_offset;
	uint32_t frame_crop_top_offset;
	uint32_t frame_crop_bottom_offset;

	uint8_t vui_parameters_present_flag;
	uint8_t aspect_ratio_info_present_flag;
	uint8_t aspect_ratio_idc;
	uint16_t sar_width;
	uint16_t sar_height;
	uint8_t overscan_info_present_flag;
	uint8_t overscan_appropriate_flag;
	uint8_t video_signal_type_present_flag;
	uint8_t video_format;
	uint8_t video_full_range_flag;
	uint8_t colour_description_present_flag;
	uint8_t colour_primaries;
	uint8_t transfer_characteristics;
	uint8_t matrix_coefficients;
	uint8_t chroma_loc_info_present_flag;
	uint8_t chroma_sample_loc_type_top_field;
	uint8_t chroma_sample_loc_type_bottom_field;
	uint8_t timing_info_present_flag;
	uint32_t num_units_tick;
	uint32_t time_scale;
	uint8_t fixed_frame_rate_flag;
	uint8_t nal_hrd_parameters_present_flag;
	uint8_t nal_cpb_cnt_minus1;
	uint8_t nal_bit_rate_scale;
	uint8_t nal_cpb_size_scale;
	uint32_t nal_bit_rate_value_minus1[32];
	uint32_t nal_cpb_size_value_minus1[32];
	uint8_t nal_cbr_flag[32];
	uint8_t nal_initial_cpb_removal_delay_length_minus1;
	uint8_t nal_cpb_removal_delay_length_minus1;
	uint8_t nal_dpb_output_delay_length_minus1;
	uint8_t nal_time_offset_length;
	uint8_t vcl_hrd_parameters_present_flag;
	uint8_t vcl_cpb_cnt_minus1;
	uint8_t vcl_bit_rate_scale;
	uint8_t vcl_cpb_size_scale;
	uint32_t vcl_bit_rate_value_minus1[32];
	uint32_t vcl_cpb_size_value_minus1[32];
	uint8_t vcl_cbr_flag[32];
	uint8_t vcl_initial_cpb_removal_delay_length_minus1;
	uint8_t vcl_cpb_removal_delay_length_minus1;
	uint8_t vcl_dpb_output_delay_length_minus1;
	uint8_t vcl_time_offset_length;
	uint8_t low_delay_hrd_flag;
	uint8_t pic_struct_present_flag;
	uint8_t bitstream_restriction_flag;
	uint8_t motion_vectors_over_pic_boundaries_flag;
	uint8_t max_bytes_per_pic_denom;
	uint8_t max_bits_per_mb_denom;
	uint8_t log2_max_mv_length_horizontal;
	uint8_t log2_max_mv_length_vertical;
	uint8_t max_num_reorder_frames;
	uint8_t max_dec_frame_buffering;

	std::vector<std::string> errors;

	std::vector<std::string> dump_fields() override;

	static inline std::unordered_map<uint8_t, H264SPS2&> SPSMap;
	
	static inline uint32_t MaxMBPS[20] = {
		1485, 1485, 3000, 6000, 11880, 11880, 19800, 20250, 40500,
		108000, 216000, 245760, 245760, 522240, 589824, 983040, 
		2073600, 4177920, 8355840, 16711680
	};

	static inline uint32_t MaxFS[20] = {
		99, 99, 396, 396, 396, 396, 792, 1620, 1620, 3600, 5120,
		8192, 8192, 8704, 22080, 36864, 36864, 139264, 139264, 139264
	};

	static inline uint32_t MaxDpbMbs[20] = {
		396, 396, 900, 2376, 2376, 2376, 4752, 8100, 8100, 18000,
		20480, 32768, 32768, 34816, 110400, 184320, 184320, 696320,
		696320, 696320
	};

	static inline uint32_t MaxBR[20] = {
		64, 128, 192, 384, 768, 2000, 4000, 4000, 10000, 14000, 
		20000, 20000, 50000, 50000, 135000, 240000, 240000, 240000,
		480000, 800000
	};

	static inline uint32_t MaxCPB[20] = {
		175, 350, 500, 1000, 2000, 2000, 4000, 4000, 10000, 14000,
		20000, 25000, 62500, 62500, 135000, 240000, 240000, 240000,
		480000, 800000
	};

	static inline uint16_t MaxVmvR[20] = {
		64, 64, 128, 128, 128, 128, 256, 256, 256, 512, 512, 512, 512,
		512, 512, 512, 512, 8192, 8192, 8192
	};

	static inline uint8_t MinCR[20] = {
		2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2
	};

	static inline uint8_t MaxMvsPer2Mb[20] {
		0, 0, 0, 0, 0, 0, 0, 0, 32, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16
	};

	uint8_t level_limit_index();
};

#endif // TOOLKIT_CODEC_UTILS_H264SPS_H_
