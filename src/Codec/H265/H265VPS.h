#ifndef TOOLKIT_CODEC_UTILS_H265VPS_H_
#define TOOLKIT_CODEC_UTILS_H265VPS_H_

#include <cstdint>
#include <unordered_map>

#include "H265ProfileTierLevel.h"
#include "H265NAL.h"

struct H265HrdParameters;

struct H265VPS : public H265NAL {
	H265VPS();
	H265VPS(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, uint8_t* nal_data);

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
	uint16_t vps_num_layer_sets_minus1;
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

	static inline std::unordered_map<uint8_t, H265VPS*> VPSMap;

	std::vector<std::string> dump_fields() override;
	void validate() override;
};


#endif // TOOLKIT_CODEC_UTILS_H265VPS_H_
