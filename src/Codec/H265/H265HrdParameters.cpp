#include <sstream>
#include <algorithm>

#include "H265HrdParameters.h"

H265SubLayerHrdParameters::H265SubLayerHrdParameters()
{

}

std::vector<std::string> H265SubLayerHrdParameters::dump_fields(uint8_t sub_pic_hrd_params_present_flag){
	std::vector<std::string> fields;
	for(int i = 0;i < bit_rate_du_value_minus1.size();++i){
		fields.push_back((std::ostringstream() << "bit_rate_value_minus1[" << i << "]:" << bit_rate_value_minus1[i]).str());
		fields.push_back((std::ostringstream() << "cpb_size_value_minus1[" << i << "]:" << cpb_size_value_minus1[i]).str());
		if(sub_pic_hrd_params_present_flag){
			fields.push_back((std::ostringstream() << "  cpb_size_du_value_minus1[" << i << "]:" << cpb_size_du_value_minus1[i]).str());
			fields.push_back((std::ostringstream() << "  bit_rate_du_value_minus1[" << i << "]:" << bit_rate_du_value_minus1[i]).str());
		}
		fields.push_back((std::ostringstream() << "cbr_flag[" << i << "]:" << cbr_flag[i]).str());
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
		fields.push_back((std::ostringstream() << "nal_hrd_parameters_present_flag:" << (int)nal_hrd_parameters_present_flag).str());
		fields.push_back((std::ostringstream() << "vcl_hrd_parameters_present_flag:" << (int)vcl_hrd_parameters_present_flag).str());
		if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag){
			fields.push_back((std::ostringstream() << "  sub_pic_hrd_params_present_flag:" << (int)sub_pic_hrd_params_present_flag).str());
			if(sub_pic_hrd_params_present_flag){
				fields.push_back((std::ostringstream() << "    tick_divisor_minus2:" << (int)tick_divisor_minus2).str());
				fields.push_back((std::ostringstream() << "    du_cpb_removal_delay_increment_length_minus1:" << (int)du_cpb_removal_delay_increment_length_minus1).str());
				fields.push_back((std::ostringstream() << "    sub_pic_cpb_params_in_pic_timing_sei_flag:" << (int)sub_pic_cpb_params_in_pic_timing_sei_flag).str());
				fields.push_back((std::ostringstream() << "    dpb_output_delay_du_length_minus1:" << (int)dpb_output_delay_du_length_minus1).str());
			}
			fields.push_back((std::ostringstream() << "  bit_rate_scale:" << (int)bit_rate_scale).str());
			fields.push_back((std::ostringstream() << "  cpb_size_scale:" << (int)cpb_size_scale).str());
			if(sub_pic_hrd_params_present_flag) fields.push_back((std::ostringstream() << "    cpb_size_du_scale:" << (int)cpb_size_du_scale).str());
			fields.push_back((std::ostringstream() << "  initial_cpb_removal_delay_length_minus1:" << (int)initial_cpb_removal_delay_length_minus1).str());
			fields.push_back((std::ostringstream() << "  au_cpb_removal_delay_length_minus1:" << (int)au_cpb_removal_delay_length_minus1).str());
			fields.push_back((std::ostringstream() << "  dpb_output_delay_length_minus1:" << (int)dpb_output_delay_length_minus1).str());
		}
	}
	for(int i = 0;i < fixed_pic_rate_general_flag.size();++i){
		fields.push_back((std::ostringstream() << "fixed_pic_rate_general_flag[" << i << "]:" << (int)fixed_pic_rate_general_flag[i]).str());
		if(!fixed_pic_rate_general_flag[i]) fields.push_back((std::ostringstream() << "  fixed_pic_rate_within_cvs_flag[" << i << "]:" << (int)fixed_pic_rate_within_cvs_flag[i]).str());
		if(fixed_pic_rate_within_cvs_flag[i]) fields.push_back((std::ostringstream() << "  elemental_duration_in_tc_minus1[" << i << "]:" << elemental_duration_in_tc_minus1[i]).str());
		else fields.push_back((std::ostringstream() << "  low_delay_hrd_flag[" << i << "]:" << (int)low_delay_hrd_flag[i]).str());
		if(!low_delay_hrd_flag[i]) fields.push_back((std::ostringstream() << "  cpb_cnt_minus1[" << i << "]:" << cpb_cnt_minus1[i]).str());
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
				errors.push_back((std::ostringstream() << "[H265 HRD parameters] elemental_duration_in_tc_minus1[" << i <<"] value (" << (int)elemental_duration_in_tc_minus1[i] <<") not in valid range (0..2047)").str());
			}
		}
		if(cpb_cnt_minus1[i] > 31) errors.push_back((std::ostringstream() << "[H265 HRD parameters] cpb_cnt_minus1[" << i << "] value (" << cpb_cnt_minus1[i] << ") not in valid range (0..31)").str());
		if(nal_hrd_parameters_present_flag){
			for(int j = 0;j <= cpb_cnt_minus1[i];++j){
				if(nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] == UINT32_MAX){
					errors.push_back((std::ostringstream() << "[H265 HRD parameters] nal_sub_layer_hrd_parameters[" << i << "].bit_rate_value_minus1[" << j << "] value (" << nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] << ") not in valid range (0..4294967294)").str());
				}
				if(nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] == UINT32_MAX){
					errors.push_back((std::ostringstream() << "[H265 HRD parameters] nal_sub_layer_hrd_parameters[" << i << "].cpb_size_value_minus1[" << j << "] value (" << nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] << ") not in valid range (0..4294967294)").str());
				}
				if(j > 0){
					if(nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j-1]){
						errors.push_back((std::ostringstream() << "[H265 HRD parameters] nal_sub_layer_hrd_parameters[" << i << "].bit_rate_value_minus1[" << j << "] value (" << nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] << ") less or equal to previous bit rate value").str());
					}
					if(nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j-1]){
						errors.push_back((std::ostringstream() << "[H265 HRD parameters] nal_sub_layer_hrd_parameters[" << i << "].cpb_size_value_minus1[" << j << "] value (" << nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] << ") less or equal to previous cpb size value").str());
					}
				}
				if(sub_pic_hrd_params_present_flag){
					if(nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] == UINT32_MAX){
						errors.push_back((std::ostringstream() << "[H265 HRD parameters] nal_sub_layer_hrd_parameters[" << i << "].cpb_size_du_value_minus1[" << j << "] value (" << nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] << ") not in valid range (0..4294967294)").str());
					}
					if(nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] == UINT32_MAX){
						errors.push_back((std::ostringstream() << "[H265 HRD parameters] nal_sub_layer_hrd_parameters[" << i << "].bit_rate_du_value_minus1[" << j << "] value (" << nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] << ") not in valid range (0..4294967294)").str());
					}
					if(j > 0){
						if(nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j-1]){
							errors.push_back((std::ostringstream() << "[H265 HRD parameters] nal_sub_layer_hrd_parameters[" << i << "].cpb_size_du_value_minus1[" << j << "] value (" << nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] << ") less or equal to previous cpb value du value").str());
						}
						if(nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j-1]){
							errors.push_back((std::ostringstream() << "[H265 HRD parameters] nal_sub_layer_hrd_parameters[" << i << "].bit_rate_du_value_minus1[" << j << "] value (" << nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] << ") less or equal to previous bit rate du value").str());
						}
					}
				}
			}
		}
		if(vcl_hrd_parameters_present_flag){

			for(int j = 0;j <= cpb_cnt_minus1[i];++j){
				if(vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] == UINT32_MAX){
					errors.push_back((std::ostringstream() << "[H265 HRD parameters] vcl_sub_layer_hrd_parameters[" << i << "].bit_rate_value_minus1[" << j << "] value (" << vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] << ") not in valid range (0..4294967294)").str());
				}
				if(vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] == UINT32_MAX){
					errors.push_back((std::ostringstream() << "[H265 HRD parameters] vcl_sub_layer_hrd_parameters[" << i << "].cpb_size_value_minus1[" << j << "] value (" << vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] << ") not in valid range (0..4294967294)").str());
				}
				if(j > 0){
					if(vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j-1]){
						errors.push_back((std::ostringstream() << "[H265 HRD parameters] vcl_sub_layer_hrd_parameters[" << i << "].bit_rate_value_minus1[" << j << "] value (" << vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] << ") less or equal to previous bit rate value").str());
					}
					if(vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j-1]){
						errors.push_back((std::ostringstream() << "[H265 HRD parameters] vcl_sub_layer_hrd_parameters[" << i << "].cpb_size_value_minus1[" << j << "] value (" << vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] << ") less or equal to previous cpb size value").str());
					}
				}
				if(sub_pic_hrd_params_present_flag){
					if(vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] == UINT32_MAX){
						errors.push_back((std::ostringstream() << "[H265 HRD parameters] vcl_sub_layer_hrd_parameters[" << i << "].cpb_size_du_value_minus1[" << j << "] value (" << vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] << ") not in valid range (0..4294967294)").str());
					}
					if(vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] == UINT32_MAX){
						errors.push_back((std::ostringstream() << "[H265 HRD parameters] vcl_sub_layer_hrd_parameters[" << i << "].bit_rate_du_value_minus1[" << j << "] value (" << vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] << ") not in valid range (0..4294967294)").str());
					}
					if(j > 0){
						if(vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j-1]){
							errors.push_back((std::ostringstream() << "[H265 HRD parameters] vcl_sub_layer_hrd_parameters[" << i << "].cpb_size_du_value_minus1[" << j << "] value (" << vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] << ") less or equal to previous cpb value du value").str());
						}
						if(vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j-1]){
							errors.push_back((std::ostringstream() << "[H265 HRD parameters] vcl_sub_layer_hrd_parameters[" << i << "].bit_rate_du_value_minus1[" << j << "] value (" << vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] << ") less or equal to previous bit rate du value").str());
						}
					}
				}
			}
		}
	}
}