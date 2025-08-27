#include <algorithm>

#include "../../StringHelpers/StringFormatter.h"

#include "H265HrdParameters.h"

H265SubLayerHrdParameters::H265SubLayerHrdParameters()
{

}

void H265SubLayerHrdParameters::dump(H26XDumpObject& dumpObject, uint8_t sub_pic_hrd_params_present_flag) const
{
	dumpObject.startUnitFieldList("Sub-HRD Parameters");
	{
		for(uint32_t i = 0;i < bit_rate_du_value_minus1.size();++i) {
			dumpObject.addIdxUnitField("bit_rate_value_minus1", i, bit_rate_value_minus1[i]);
			dumpObject.addIdxUnitField("cpb_size_value_minus1", i, cpb_size_value_minus1[i]);
			if (sub_pic_hrd_params_present_flag) {
				dumpObject.addIdxUnitField("cpb_size_du_value_minus1", i, cpb_size_du_value_minus1[i]);
				dumpObject.addIdxUnitField("bit_rate_du_value_minus1", i, bit_rate_du_value_minus1[i]);
			}
			dumpObject.addIdxUnitField("cbr_flag", i, cbr_flag[i]);

		}
	}
	dumpObject.endUnitFieldList();
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

void H265HrdParameters::dump(H26XDumpObject& dumpObject, uint8_t commonInfPresentFlag) const
{
	dumpObject.startUnitFieldList("HRD Parameters");
	if(commonInfPresentFlag){
		dumpObject.addUnitField("nal_hrd_parameters_present_flag", nal_hrd_parameters_present_flag);
		dumpObject.addUnitField("vcl_hrd_parameters_present_flag", vcl_hrd_parameters_present_flag);
		if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
		{
			dumpObject.startValueUnitFieldList("sub_pic_hrd_params_present_flag", sub_pic_hrd_params_present_flag);
			if(sub_pic_hrd_params_present_flag){
				dumpObject.addUnitField("tick_divisor_minus2", tick_divisor_minus2);
				dumpObject.addUnitField("du_cpb_removal_delay_increment_length_minus1", du_cpb_removal_delay_increment_length_minus1);
				dumpObject.addUnitField("sub_pic_cpb_params_in_pic_timing_sei_flag", sub_pic_cpb_params_in_pic_timing_sei_flag);
				dumpObject.addUnitField("dpb_output_delay_du_length_minus1", dpb_output_delay_du_length_minus1);
			}
			dumpObject.endValueUnitFieldList();
			dumpObject.addUnitField("bit_rate_scale", bit_rate_scale);
			dumpObject.addUnitField("cpb_size_scale", cpb_size_scale);
			if(sub_pic_hrd_params_present_flag){
				dumpObject.addUnitField("cpb_size_du_scale", cpb_size_du_scale);
			}
			dumpObject.addUnitField("initial_cpb_removal_delay_length_minus1", initial_cpb_removal_delay_length_minus1);
			dumpObject.addUnitField("au_cpb_removal_delay_length_minus1", au_cpb_removal_delay_length_minus1);
			dumpObject.addUnitField("dpb_output_delay_length_minus1", dpb_output_delay_length_minus1);
		}
	}
	for(uint32_t i = 0;i < fixed_pic_rate_general_flag.size();++i)
	{
		dumpObject.startIdxValueUnitFieldList("fixed_pic_rate_general_flag", i, fixed_pic_rate_general_flag[i]);
		if(!fixed_pic_rate_general_flag[i]) {
			dumpObject.startIdxValueUnitFieldList("fixed_pic_rate_within_cvs_flag", fixed_pic_rate_within_cvs_flag[i], i);
			if(fixed_pic_rate_within_cvs_flag[i]){
				dumpObject.addIdxUnitField("elemental_duration_in_tc_minus1", i, elemental_duration_in_tc_minus1[i]);
			}else{
				dumpObject.addIdxUnitField("low_delay_hrd_flag", i, low_delay_hrd_flag[i]);
			}
			dumpObject.endIdxValueUnitFieldList();
		}
		dumpObject.endIdxValueUnitFieldList();


		if(!low_delay_hrd_flag[i]){
			dumpObject.addIdxUnitField("cpb_cnt_minus1", i, cpb_cnt_minus1[i]);
		}
		if(nal_hrd_parameters_present_flag){
			nal_sub_layer_hrd_parameters[i].dump(dumpObject, sub_pic_hrd_params_present_flag);
		}
		if(vcl_hrd_parameters_present_flag){
			vcl_sub_layer_hrd_parameters[i].dump(dumpObject, sub_pic_hrd_params_present_flag);
		}
	}
	dumpObject.endUnitFieldList();
}

void H265HrdParameters::validate(){
	for(uint32_t i = 0;i < fixed_pic_rate_general_flag.size();++i){
		if(fixed_pic_rate_within_cvs_flag[i]) {
			if(elemental_duration_in_tc_minus1[i] > 2047) {
				errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] elemental_duration_in_tc_minus1[%d] value (%ld) not in valid range (0..2047)", i, elemental_duration_in_tc_minus1[i]));
			}
		}
		if(cpb_cnt_minus1[i] > 31) errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] cpb_cnt_minus1[%d] value (%ld) not in valid range (0..31)", i, cpb_cnt_minus1[i]));
		if(nal_hrd_parameters_present_flag){
			for(int j = 0;j <= cpb_cnt_minus1[i];++j){
				if(nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] == UINT32_MAX){
					errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].bit_rate_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
				}
				if(nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] == UINT32_MAX){
					errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].cpb_size_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
				}
				if(j > 0){
					if(nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j-1]){
						errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].bit_rate_value_minus1[%d] value (%ld) less or equal to previous bit rate value", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
					}
					if(nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j-1]){
						errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].cpb_size_value_minus1[%d] value (%ld) less or equal to previous cpb size value", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
					}
				}
				if(sub_pic_hrd_params_present_flag){
					if(nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] == UINT32_MAX){
						errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].cpb_size_du_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
					}
					if(nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] == UINT32_MAX){
						errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].bit_rate_du_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
					}
					if(j > 0){
						if(nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j-1]){
							errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].cpb_size_du_value_minus1[%d] value (%ld) less or equal to previous cpb value du value", i, j, nal_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
						}
						if(nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] <= nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j-1]){
							errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] nal_sub_layer_hrd_parameters[%d].bit_rate_du_value_minus1[%d] value (%ld) less or equal to previous bit rate du value", i, j, nal_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
						}
					}
				}
			}
		}
		if(vcl_hrd_parameters_present_flag){

			for(int j = 0;j <= cpb_cnt_minus1[i];++j){
				if(vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] == UINT32_MAX){
					errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].bit_rate_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
				}
				if(vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] == UINT32_MAX){
					errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].cpb_size_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
				}
				if(j > 0){
					if(vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j-1]){
						errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].bit_rate_value_minus1[%d] value (%ld) less or equal to previous bit rate value", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_value_minus1[j]));
					}
					if(vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j-1]){
						errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].cpb_size_value_minus1[%d] value (%ld) less or equal to previous cpb size value", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_value_minus1[j]));
					}
				}
				if(sub_pic_hrd_params_present_flag){
					if(vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] == UINT32_MAX){
						errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].cpb_size_du_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
					}
					if(vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] == UINT32_MAX){
						errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].bit_rate_du_value_minus1[%d] value (%ld) not in valid range (0..4294967294)", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
					}
					if(j > 0){
						if(vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j-1]){
							errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].cpb_size_du_value_minus1[%d] value (%ld) less or equal to previous cpb value du value", i, j, vcl_sub_layer_hrd_parameters[i].cpb_size_du_value_minus1[j]));
						}
						if(vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j] <= vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j-1]){
							errors.add(H26XError::Minor, StringFormatter::formatString("[HRD parameters] vcl_sub_layer_hrd_parameters[%d].bit_rate_du_value_minus1[%d] value (%ld) less or equal to previous bit rate du value", i, j, vcl_sub_layer_hrd_parameters[i].bit_rate_du_value_minus1[j]));
						}
					}
				}
			}
		}
	}
}