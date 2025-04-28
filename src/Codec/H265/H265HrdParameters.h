#ifndef TOOLKIT_CODEC_UTILS_H265HRD_PARAMETERS_H_
#define TOOLKIT_CODEC_UTILS_H265HRD_PARAMETERS_H_

#include <vector>
#include <cstdint>
#include <string>

struct H265SubLayerHrdParameters {
	H265SubLayerHrdParameters();

	std::vector<uint32_t> bit_rate_value_minus1;
	std::vector<uint32_t> cpb_size_value_minus1;
	std::vector<uint32_t> cpb_size_du_value_minus1;
	std::vector<uint32_t> bit_rate_du_value_minus1;
	std::vector<uint8_t> cbr_flag;

	std::vector<std::string> dump_fields(uint8_t sub_pic_hrd_params_present_flag);
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
	std::vector<uint16_t> elemental_duration_in_tc_minus1;
	std::vector<uint8_t> low_delay_hrd_flag;
	std::vector<uint8_t> cpb_cnt_minus1;
	std::vector<H265SubLayerHrdParameters> nal_sub_layer_hrd_parameters;
	std::vector<H265SubLayerHrdParameters> vcl_sub_layer_hrd_parameters;

	std::vector<std::string> errors;

	std::vector<std::string> dump_fields(uint8_t commonInfPresentFlag);
};

#endif // TOOLKIT_CODEC_UTILS_H265HRD_PARAMETERS_H_