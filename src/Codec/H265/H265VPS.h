#ifndef TOOLKIT_CODEC_UTILS_H265VPS_H_
#define TOOLKIT_CODEC_UTILS_H265VPS_H_

#include <vector>
#include <cstdint>

#include "H265ProfileTierLevel.h"

struct H265SubLayerHrdParameters {
	H265SubLayerHrdParameters();

	std::vector<uint32_t> bit_rate_value_minus1;
	std::vector<uint32_t> cpb_size_value_minus1;
	std::vector<uint32_t> cpb_size_du_value_minus1;
	std::vector<uint32_t> bit_rate_du_value_minus1;
	std::vector<uint8_t> cbr_flag;
};

struct H265HrdParameters {
	H265HrdParameters();

	uint8_t nal_hrd_parameters_present_flag;
	uint8_t vcl_hrd_parameters_present_flag;
	uint8_t sub_pic_hrd_params_present_flag;
	uint8_t tick_divisor_minus2;
	uint8_t du_cpb_removal_delay_increment_length_minus1;
	uint8_t sub_pic_cpb_params_in_pic_timing_sei_flag;
	uint8_t dpb_output_delay_du_length_minus1;
	uint8_t bit_rate_scale;
	uint8_t cpb_size_scale;
	uint8_t cpb_size_du_scale;
	uint8_t initial_cpb_removal_delay_length_minus1;
	uint8_t au_cpb_removal_delay_length_minus1;
	uint8_t dpb_output_delay_length_minus1;
	std::vector<uint8_t> fixed_pic_rate_general_flag;
	std::vector<uint8_t> fixed_pic_rate_within_cvs_flag;
	std::vector<uint32_t> elemental_duration_in_tc_minus1;
	std::vector<uint8_t> low_delay_hrd_flag;
	std::vector<uint32_t> cpb_cnt_minus1;
	std::vector<H265SubLayerHrdParameters> nal_sub_layer_hrd_parameters;
	std::vector<H265SubLayerHrdParameters> vcl_sub_layer_hrd_parameters;
};

struct H265VPS {
	H265VPS();

	uint8_t vps_video_parameter_set_id;
	uint8_t vps_base_layer_internal_flag;
	uint8_t vps_base_layer_available_flag;
	uint8_t vps_max_layers_minus1;
	uint8_t vps_max_sub_layers_minus1;
	uint8_t vps_temporal_id_nesting_flag;
	H265ProfileTierLevel profile_tier_level;
	uint8_t vps_sub_layer_ordering_info_present_flag;
	std::vector<uint32_t> vps_max_dec_pic_buffering_minus1;
	std::vector<uint32_t> vps_max_num_reorder_pics;
	std::vector<uint32_t> vps_max_latency_increase_plus1;
	uint8_t vps_max_layer_id;
	uint32_t vps_num_layer_sets_minus1;
	std::vector<UInt8Vector> layer_id_included_flag;
	uint8_t vps_timing_info_present_flag;
	uint32_t vps_num_units_in_tick;
	uint32_t vps_time_scale;
	uint8_t vps_poc_proportional_to_timing_flag;
	uint32_t vps_num_ticks_poc_diff_one_minus1;
	uint32_t vps_num_hrd_parameters;
	std::vector<uint32_t> hrd_layer_set_idx;
	std::vector<uint8_t> cprms_present_flag;
	std::vector<H265HrdParameters> hrd_parameters;
	uint8_t vps_extension_flag;
};


#endif // TOOLKIT_CODEC_UTILS_H265VPS_H_
