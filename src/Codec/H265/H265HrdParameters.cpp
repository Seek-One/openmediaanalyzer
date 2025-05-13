#include <fmt/core.h>
#include <algorithm>

#include "H265HrdParameters.h"

H265SubLayerHrdParameters::H265SubLayerHrdParameters()
{

}

std::vector<std::string> H265SubLayerHrdParameters::dump_fields(uint8_t sub_pic_hrd_params_present_flag){
	std::vector<std::string> fields;
	for(int i = 0;i < bit_rate_du_value_minus1.size();++i){
		fields.push_back(fmt::format("bit_rate_value_minus1[{}]:{}", i, bit_rate_value_minus1[i]));
		fields.push_back(fmt::format("cpb_size_value_minus1[{}]:{}", i, cpb_size_value_minus1[i]));
		if(sub_pic_hrd_params_present_flag){
			fields.push_back(fmt::format("  cpb_size_du_value_minus1[{}]:{}", i, cpb_size_du_value_minus1[i]));
			fields.push_back(fmt::format("  bit_rate_du_value_minus1[{}]:{}", i, bit_rate_du_value_minus1[i]));
		}
		fields.push_back(fmt::format("cbr_flag[{}]:{}", i, cbr_flag[i]));
	}
	return fields;
}

H265HrdParameters::H265HrdParameters()
{
	nal_hrd_parameters_present_flag = 0;
	vcl_hrd_parameters_present_flag = 0;
	sub_pic_hrd_params_present_flag = 0;
	tick_divisor_minus2 = 0;
	du_cpb_removal_delay_increment_length_minus1 = 0;
	sub_pic_cpb_params_in_pic_timing_sei_flag = 0;
	dpb_output_delay_du_length_minus1 = 0;
	bit_rate_scale = 0;
	cpb_size_scale = 0;
	cpb_size_du_scale = 0;
	initial_cpb_removal_delay_length_minus1 = 0;
	au_cpb_removal_delay_length_minus1 = 0;
	dpb_output_delay_length_minus1 = 0;
}

std::vector<std::string> H265HrdParameters::dump_fields(uint8_t commonInfPresentFlag){
	std::vector<std::string> fields;
	if(commonInfPresentFlag){
		fields.push_back(fmt::format("nal_hrd_parameters_present_flag:{}", nal_hrd_parameters_present_flag));
		fields.push_back(fmt::format("vcl_hrd_parameters_present_flag:{}", vcl_hrd_parameters_present_flag));
		if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag){
			fields.push_back(fmt::format("  sub_pic_hrd_params_present_flag:{}", sub_pic_hrd_params_present_flag));
			if(sub_pic_hrd_params_present_flag){
				fields.push_back(fmt::format("    tick_divisor_minus2:{}", tick_divisor_minus2));
				fields.push_back(fmt::format("    du_cpb_removal_delay_increment_length_minus1:{}", du_cpb_removal_delay_increment_length_minus1));
				fields.push_back(fmt::format("    sub_pic_cpb_params_in_pic_timing_sei_flag:{}", sub_pic_cpb_params_in_pic_timing_sei_flag));
				fields.push_back(fmt::format("    dpb_output_delay_du_length_minus1:{}", dpb_output_delay_du_length_minus1));
			}
			fields.push_back(fmt::format("  bit_rate_scale:{}", bit_rate_scale));
			fields.push_back(fmt::format("  cpb_size_scale:{}", cpb_size_scale));
			if(sub_pic_hrd_params_present_flag) fields.push_back(fmt::format("    cpb_size_du_scale:{}", cpb_size_du_scale));
			fields.push_back(fmt::format("  initial_cpb_removal_delay_length_minus1:{}", initial_cpb_removal_delay_length_minus1));
			fields.push_back(fmt::format("  au_cpb_removal_delay_length_minus1:{}", au_cpb_removal_delay_length_minus1));
			fields.push_back(fmt::format("  dpb_output_delay_length_minus1:{}", dpb_output_delay_length_minus1));
		}
	}
	for(int i = 0;i < fixed_pic_rate_general_flag.size();++i){
		fields.push_back(fmt::format("fixed_pic_rate_general_flag[{}]:{}", i, fixed_pic_rate_general_flag[i]));
		if(!fixed_pic_rate_general_flag[i]) fields.push_back(fmt::format("  fixed_pic_rate_within_cvs_flag[{}]:{}", i, fixed_pic_rate_within_cvs_flag[i]));
		if(fixed_pic_rate_within_cvs_flag[i]) fields.push_back(fmt::format("  elemental_duration_in_tc_minus1[{}]:{}", i, elemental_duration_in_tc_minus1[i]));
		else fields.push_back(fmt::format("  low_delay_hrd_flag[{}]:{}", i, low_delay_hrd_flag[i]));
		if(!low_delay_hrd_flag[i]) fields.push_back(fmt::format("  cpb_cnt_minus1[{}]:{}", i, cpb_cnt_minus1[i]));
		if(nal_hrd_parameters_present_flag){
			std::vector<std::string> nalSubLayerHrdParametersFields = nal_sub_layer_hrd_parameters[i].dump_fields(sub_pic_hrd_params_present_flag);
			std::transform(nalSubLayerHrdParametersFields.begin(), nalSubLayerHrdParametersFields.end(), std::back_inserter(fields), [](const std::string& subField){
				return "    nal_" + subField;
			});
		}
		if(vcl_hrd_parameters_present_flag){
			std::vector<std::string> vclSubLayerHrdParametersFields = vcl_sub_layer_hrd_parameters[i].dump_fields(sub_pic_hrd_params_present_flag);
			std::transform(vclSubLayerHrdParametersFields.begin(), vclSubLayerHrdParametersFields.end(), std::back_inserter(fields), [](const std::string& subField){
				return "    vcl_" + subField;
			});
		}
	}
	return fields;
}

void H265HrdParameters::validate(){
	for(int i = 0;i < fixed_pic_rate_general_flag.size();++i){
		if(fixed_pic_rate_within_cvs_flag[i]) {
			if(elemental_duration_in_tc_minus1[i] > 2047) {
				errors.push_back(fmt::format("[HRD parameters] elemental_duration_in_tc_minus1[{}] value ({}) not in valid range (0..2047)", i, elemental_duration_in_tc_minus1[i]));
			}
		}
		if(cpb_cnt_minus1[i] > 31) errors.push_back(fmt::format("[HRD parameters] cpb_cnt_minus1[{}] value ({}) not in valid range (0..31)", i, cpb_cnt_minus1[i]));
		if(nal_hrd_parameters_present_flag){
			for(int j = 0;j <= cpb_cnt_minus1[i];++j){
				if(nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] == UINT32_MAX){
					errors.push_back(fmt::format("[HRD parameters] nal_sub_layer_hrd_parameters[{}].bit_rate_value_minus1[{}] value ({}) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
				}
				if(nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] == UINT32_MAX){
					errors.push_back(fmt::format("[HRD parameters] nal_sub_layer_hrd_parameters[{}].cpb_size_value_minus1[{}] value ({}) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
				}
				if(j > 0){
					if(nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j-1]){
						errors.push_back(fmt::format("[HRD parameters] nal_sub_layer_hrd_parameters[{}].bit_rate_value_minus1[{}] value ({}) less or equal to previous bit rate value", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
					}
					if(nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j-1]){
						errors.push_back(fmt::format("[HRD parameters] nal_sub_layer_hrd_parameters[{}].cpb_size_value_minus1[{}] value ({}) less or equal to previous cpb size value", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
					}
				}
				if(sub_pic_hrd_params_present_flag){
					if(nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] == UINT32_MAX){
						errors.push_back(fmt::format("[HRD parameters] nal_sub_layer_hrd_parameters[{}].cpb_size_du_value_minus1[{}] value ({}) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
					}
					if(nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] == UINT32_MAX){
						errors.push_back(fmt::format("[HRD parameters] nal_sub_layer_hrd_parameters[{}].bit_rate_du_value_minus1[{}] value ({}) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
					}
					if(j > 0){
						if(nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j-1]){
							errors.push_back(fmt::format("[HRD parameters] nal_sub_layer_hrd_parameters[{}].cpb_size_du_value_minus1[{}] value ({}) less or equal to previous cpb value du value", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
						}
						if(nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j-1]){
							errors.push_back(fmt::format("[HRD parameters] nal_sub_layer_hrd_parameters[{}].bit_rate_du_value_minus1[{}] value ({}) less or equal to previous bit rate du value", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
						}
					}
				}
			}
		}
		if(vcl_hrd_parameters_present_flag){

			for(int j = 0;j <= cpb_cnt_minus1[i];++j){
				if(vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] == UINT32_MAX){
					errors.push_back(fmt::format("[HRD parameters] vcl_sub_layer_hrd_parameters[{}].bit_rate_value_minus1[{}] value ({}) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
				}
				if(vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] == UINT32_MAX){
					errors.push_back(fmt::format("[HRD parameters] vcl_sub_layer_hrd_parameters[{}].cpb_size_value_minus1[{}] value ({}) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
				}
				if(j > 0){
					if(vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j-1]){
						errors.push_back(fmt::format("[HRD parameters] vcl_sub_layer_hrd_parameters[{}].bit_rate_value_minus1[{}] value ({}) less or equal to previous bit rate value", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
					}
					if(vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j-1]){
						errors.push_back(fmt::format("[HRD parameters] vcl_sub_layer_hrd_parameters[{}].cpb_size_value_minus1[{}] value ({}) less or equal to previous cpb size value", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
					}
				}
				if(sub_pic_hrd_params_present_flag){
					if(vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] == UINT32_MAX){
						errors.push_back(fmt::format("[HRD parameters] vcl_sub_layer_hrd_parameters[{}].cpb_size_du_value_minus1[{}] value ({}) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
					}
					if(vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] == UINT32_MAX){
						errors.push_back(fmt::format("[HRD parameters] vcl_sub_layer_hrd_parameters[{}].bit_rate_du_value_minus1[{}] value ({}) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
					}
					if(j > 0){
						if(vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j-1]){
							errors.push_back(fmt::format("[HRD parameters] vcl_sub_layer_hrd_parameters[{}].cpb_size_du_value_minus1[{}] value ({}) less or equal to previous cpb value du value", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
						}
						if(vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j-1]){
							errors.push_back(fmt::format("[HRD parameters] vcl_sub_layer_hrd_parameters[{}].bit_rate_du_value_minus1[{}] value ({}) less or equal to previous bit rate du value", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
						}
					}
				}
			}
		}
	}
}