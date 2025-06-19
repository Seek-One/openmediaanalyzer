#include <algorithm>

#include "../../StringHelpers/StringFormatter.h"

#include "H265HrdParameters.h"

H265SubLayerHrdParameters::H265SubLayerHrdParameters()
{

}

UnitFieldList H265SubLayerHrdParameters::dump_fields(uint8_t sub_pic_hrd_params_present_flag){
	UnitFieldList fields = UnitFieldList("Sub-HRD Parameters");
	for(uint32_t i = 0;i < bit_rate_du_value_minus1.size();++i){
		fields.addItem(IdxUnitField("bit_rate_value_minus1", bit_rate_value_minus1[i], i));
		fields.addItem(IdxUnitField("cpb_size_value_minus1", cpb_size_value_minus1[i], i));
		if(sub_pic_hrd_params_present_flag){
			fields.addItem(IdxUnitField("cpb_size_du_value_minus1", cpb_size_du_value_minus1[i], i));
			fields.addItem(IdxUnitField("bit_rate_du_value_minus1", bit_rate_du_value_minus1[i], i));
		}
		fields.addItem(IdxUnitField("cbr_flag", cbr_flag[i], i));
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

UnitFieldList H265HrdParameters::dump_fields(uint8_t commonInfPresentFlag){
	UnitFieldList fields = UnitFieldList("HRD Parameters");
	if(commonInfPresentFlag){
		fields.addItem(UnitField("nal_hrd_parameters_present_flag", nal_hrd_parameters_present_flag));
		fields.addItem(UnitField("vcl_hrd_parameters_present_flag", vcl_hrd_parameters_present_flag));
		if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag){
			ValueUnitFieldList sub_pic_hrd_params_present_flagField = ValueUnitFieldList("sub_pic_hrd_params_present_flag", sub_pic_hrd_params_present_flag);
			if(sub_pic_hrd_params_present_flag){
				sub_pic_hrd_params_present_flagField.addItem(UnitField("tick_divisor_minus2", tick_divisor_minus2));
				sub_pic_hrd_params_present_flagField.addItem(UnitField("du_cpb_removal_delay_increment_length_minus1", du_cpb_removal_delay_increment_length_minus1));
				sub_pic_hrd_params_present_flagField.addItem(UnitField("sub_pic_cpb_params_in_pic_timing_sei_flag", sub_pic_cpb_params_in_pic_timing_sei_flag));
				sub_pic_hrd_params_present_flagField.addItem(UnitField("dpb_output_delay_du_length_minus1", dpb_output_delay_du_length_minus1));
			}
			fields.addItem(std::move(sub_pic_hrd_params_present_flagField));
			fields.addItem(UnitField("bit_rate_scale", bit_rate_scale));
			fields.addItem(UnitField("cpb_size_scale", cpb_size_scale));
			if(sub_pic_hrd_params_present_flag) fields.addItem(UnitField("cpb_size_du_scale", cpb_size_du_scale));
			fields.addItem(UnitField("initial_cpb_removal_delay_length_minus1", initial_cpb_removal_delay_length_minus1));
			fields.addItem(UnitField("au_cpb_removal_delay_length_minus1", au_cpb_removal_delay_length_minus1));
			fields.addItem(UnitField("dpb_output_delay_length_minus1", dpb_output_delay_length_minus1));
		}
	}
	for(uint32_t i = 0;i < fixed_pic_rate_general_flag.size();++i){
		IdxValueUnitFieldList fixed_pic_rate_general_flagField = IdxValueUnitFieldList("fixed_pic_rate_general_flag", fixed_pic_rate_general_flag[i], i);
		IdxValueUnitFieldList fixed_pic_rate_within_cvs_flagField = IdxValueUnitFieldList("fixed_pic_rate_within_cvs_flag", fixed_pic_rate_within_cvs_flag[i], i);
		if(!fixed_pic_rate_general_flag[i]) {
			fixed_pic_rate_general_flagField.addItem(std::move(fixed_pic_rate_within_cvs_flagField));
		}
		fields.addItem(std::move(fixed_pic_rate_general_flagField));
		if(fixed_pic_rate_within_cvs_flag[i]) fixed_pic_rate_within_cvs_flagField.addItem(IdxUnitField("elemental_duration_in_tc_minus1", elemental_duration_in_tc_minus1[i], i));
		else fixed_pic_rate_within_cvs_flagField.addItem(IdxUnitField("low_delay_hrd_flag", low_delay_hrd_flag[i], i));
		if(!low_delay_hrd_flag[i]) fields.addItem(IdxUnitField("cpb_cnt_minus1", cpb_cnt_minus1[i], i));
		if(nal_hrd_parameters_present_flag) fields.addItem(std::move(nal_sub_layer_hrd_parameters[i].dump_fields(sub_pic_hrd_params_present_flag)));
		if(vcl_hrd_parameters_present_flag) fields.addItem(std::move(vcl_sub_layer_hrd_parameters[i].dump_fields(sub_pic_hrd_params_present_flag)));
	}
	return fields;
}

void H265HrdParameters::validate(){
	for(uint32_t i = 0;i < fixed_pic_rate_general_flag.size();++i){
		if(fixed_pic_rate_within_cvs_flag[i]) {
			if(elemental_duration_in_tc_minus1[i] > 2047) {
				errors.push_back(StringFormatter::formatString("[HRD parameters] elemental_duration_in_tc_minus1[%d] value (%ld) not in valid range (0..2047)", i, elemental_duration_in_tc_minus1[i]));
			}
		}
		if(cpb_cnt_minus1[i] > 31) errors.push_back(StringFormatter::formatString("[HRD parameters] cpb_cnt_minus1[%d] value (%ld) not in valid range (0..31)", i, cpb_cnt_minus1[i]));
		if(nal_hrd_parameters_present_flag){
			for(int j = 0;j <= cpb_cnt_minus1[i];++j){
				if(nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] == UINT32_MAX){
					errors.push_back(StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].bit_rate_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
				}
				if(nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] == UINT32_MAX){
					errors.push_back(StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].cpb_size_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
				}
				if(j > 0){
					if(nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j-1]){
						errors.push_back(StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].bit_rate_value_minus1[%d] value (%ld) less or equal to previous bit rate value", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
					}
					if(nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j-1]){
						errors.push_back(StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].cpb_size_value_minus1[%d] value (%ld) less or equal to previous cpb size value", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
					}
				}
				if(sub_pic_hrd_params_present_flag){
					if(nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] == UINT32_MAX){
						errors.push_back(StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].cpb_size_du_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
					}
					if(nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] == UINT32_MAX){
						errors.push_back(StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].bit_rate_du_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
					}
					if(j > 0){
						if(nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j-1]){
							errors.push_back(StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].cpb_size_du_value_minus1[%d] value (%ld) less or equal to previous cpb value du value", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
						}
						if(nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j-1]){
							errors.push_back(StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].bit_rate_du_value_minus1[%d] value (%ld) less or equal to previous bit rate du value", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
						}
					}
				}
			}
		}
		if(vcl_hrd_parameters_present_flag){

			for(int j = 0;j <= cpb_cnt_minus1[i];++j){
				if(vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] == UINT32_MAX){
					errors.push_back(StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].bit_rate_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
				}
				if(vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] == UINT32_MAX){
					errors.push_back(StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].cpb_size_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
				}
				if(j > 0){
					if(vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j-1]){
						errors.push_back(StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].bit_rate_value_minus1[%d] value (%ld) less or equal to previous bit rate value", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
					}
					if(vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j-1]){
						errors.push_back(StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].cpb_size_value_minus1[%d] value (%ld) less or equal to previous cpb size value", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
					}
				}
				if(sub_pic_hrd_params_present_flag){
					if(vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] == UINT32_MAX){
						errors.push_back(StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].cpb_size_du_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
					}
					if(vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] == UINT32_MAX){
						errors.push_back(StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].bit_rate_du_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
					}
					if(j > 0){
						if(vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j-1]){
							errors.push_back(StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].cpb_size_du_value_minus1[%d] value (%ld) less or equal to previous cpb value du value", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
						}
						if(vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j-1]){
							errors.push_back(StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].bit_rate_du_value_minus1[%d] value (%ld) less or equal to previous bit rate du value", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
						}
					}
				}
			}
		}
	}
}