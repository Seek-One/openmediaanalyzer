#include <sstream>
#include <algorithm>
#include <unordered_set>

#include "H265HrdParameters.h"

#include "H265VPS.h"

H265VPS::H265VPS():
	H265VPS(0, UnitType_Unspecified, 0, 0, 0, nullptr)
{}

H265VPS::H265VPS(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, uint8_t* nal_data):
	H265NAL(forbidden_zero_bit, nal_unit_type, nuh_layer_id, nuh_temporal_id_plus1, nal_size, nal_data)
{
	vps_video_parameter_set_id = 0;
	vps_base_layer_internal_flag = 0;
	vps_base_layer_available_flag = 0;
	vps_max_layers_minus1 = 0;
	vps_max_sub_layers_minus1 = 0;
	vps_temporal_id_nesting_flag = 0;
	vps_sub_layer_ordering_info_present_flag = 0;
	vps_max_layer_id = 0;
	vps_num_layer_sets_minus1 = 0;
	vps_timing_info_present_flag = 0;
	vps_num_units_in_tick = 0;
	vps_time_scale = 0;
	vps_poc_proportional_to_timing_flag = 0;
	vps_num_ticks_poc_diff_one_minus1 = 0;
	vps_num_hrd_parameters = 0;
	vps_extension_flag = 0;
}

std::vector<std::string> H265VPS::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "vps_video_parameter_set_id:" << (int)vps_video_parameter_set_id).str());
	fields.push_back((std::ostringstream() << "vps_base_layer_internal_flag:" << (int)vps_base_layer_internal_flag).str());
	fields.push_back((std::ostringstream() << "vps_base_layer_available_flag:" << (int)vps_base_layer_available_flag).str());
	fields.push_back((std::ostringstream() << "vps_max_layers_minus1:" << (int)vps_max_layers_minus1).str());
	fields.push_back((std::ostringstream() << "vps_max_sub_layers_minus1:" << (int)vps_max_sub_layers_minus1).str());
	fields.push_back((std::ostringstream() << "vps_temporal_id_nesting_flag:" << (int)vps_temporal_id_nesting_flag).str());
	std::vector<std::string> profileTierLevelFields = profile_tier_level.dump_fields();
	std::transform(profileTierLevelFields.begin(), profileTierLevelFields.end(), std::back_inserter(fields), [](const std::string& profileTierLevelField){
		return "  " + profileTierLevelField;
	});
	fields.push_back((std::ostringstream() << "vps_sub_layer_ordering_info_present_flag:" << (int)vps_sub_layer_ordering_info_present_flag).str());
	for(int i = vps_sub_layer_ordering_info_present_flag ? 0 : vps_max_sub_layers_minus1;i <= vps_max_sub_layers_minus1;++i){
		fields.push_back((std::ostringstream() << "  vps_max_dec_pic_buffering_minus1[" << i << "]:" << (int)vps_max_dec_pic_buffering_minus1[i]).str());
		fields.push_back((std::ostringstream() << "  vps_max_num_reorder_pics[" << i << "]:" << (int)vps_max_num_reorder_pics[i]).str());
		fields.push_back((std::ostringstream() << "  vps_max_latency_increase_plus1[" << i << "]:" << (int)vps_max_latency_increase_plus1[i]).str());
	}
	fields.push_back((std::ostringstream() << "vps_max_layer_id:" << (int)vps_max_layer_id).str());
	fields.push_back((std::ostringstream() << "vps_num_layer_sets_minus1:" << (int)vps_num_layer_sets_minus1).str());
	for(int i = 1;i <= vps_num_layer_sets_minus1;++i){
		for(int j = 0;j <= vps_max_layer_id;++j){
			fields.push_back((std::ostringstream() << "  layer_id_included_flag[" << i << "][" << j << "]:" << (int)layer_id_included_flag[i][j]).str());
		}
	}
	fields.push_back((std::ostringstream() << "vps_timing_info_present_flag:" << (int)vps_timing_info_present_flag).str());
	if(vps_timing_info_present_flag){
		fields.push_back((std::ostringstream() << "  vps_num_units_in_tick:" << vps_num_units_in_tick).str());
		fields.push_back((std::ostringstream() << "  vps_time_scale:" << vps_time_scale).str());
		fields.push_back((std::ostringstream() << "  vps_poc_proportional_to_timing_flag:" << (int)vps_poc_proportional_to_timing_flag).str());
		if(vps_poc_proportional_to_timing_flag) fields.push_back((std::ostringstream() << "    vps_num_ticks_poc_diff_one_minus1:" << vps_num_ticks_poc_diff_one_minus1).str());
		fields.push_back((std::ostringstream() << "  vps_num_hrd_parameters:" << vps_num_hrd_parameters).str());
		for(int i = 0;i < vps_num_hrd_parameters;++i){
			fields.push_back((std::ostringstream() << "  hrd_layer_set_idx[" << i << "]:" << hrd_layer_set_idx[i]).str());
			if(i > 0) fields.push_back((std::ostringstream() << "  cprms_present_flag[" << i << "]:" << (int)cprms_present_flag[i]).str());
			std::vector<std::string> hrdParametersFields = hrd_parameters[i].dump_fields(cprms_present_flag[i]);
			std::transform(hrdParametersFields.begin(), hrdParametersFields.end(), std::back_inserter(fields), [](const std::string& hrdParameterField){
				return "    " + hrdParameterField;
			});
		}
	}
	fields.push_back((std::ostringstream() << "vps_extension_flag:" << (int)vps_extension_flag).str());
	return fields;
}

void H265VPS::validate(){
	if(!vps_base_layer_internal_flag && vps_max_layers_minus1 == 0) errors.push_back("[H265 VPS] vps_max_layers_minus1 not greater than 0");
	if(vps_max_layers_minus1 > 62) errors.push_back((std::ostringstream() << "[H265 VPS] vps_max_layers_minus1 value (" << (int)vps_max_layers_minus1 << ") not in valid range (0..62)").str());
	if(vps_max_sub_layers_minus1 > 6) errors.push_back((std::ostringstream() << "[H265 VPS] vps_max_sub_layers_minus1 value (" << (int)vps_max_sub_layers_minus1 << ") not in valid range (0..6)").str());
	if(vps_max_sub_layers_minus1 == 0 && !vps_temporal_id_nesting_flag) errors.push_back("[H265 VPS] vps_temporal_id_nesting_flag not set (singular temporal sub-layer)");
	profile_tier_level.validate(1);
	errors.insert(errors.end(), profile_tier_level.errors.begin(), profile_tier_level.errors.end());
	profile_tier_level.errors.clear();
	if(!vps_base_layer_internal_flag && vps_sub_layer_ordering_info_present_flag) errors.push_back("[H265 VPS] vps_sub_layer_ordering_info_present_flag set (no base layer)");
	for (int i = (vps_sub_layer_ordering_info_present_flag ? 0 : vps_max_sub_layers_minus1); i <= vps_max_sub_layers_minus1; ++i) {
		if(!vps_base_layer_internal_flag){
			if(vps_max_dec_pic_buffering_minus1[i] != 0) errors.push_back((std::ostringstream() << "[H265 VPS] vps_max_dec_pic_buffering_minus1[" << i << "] value (" << vps_max_dec_pic_buffering_minus1[i] << ") not 0 (no base layer)").str());
			if(vps_max_num_reorder_pics[i] != 0) errors.push_back((std::ostringstream() << "[H265 VPS] vps_max_num_reorder_pics[" << i << "] value (" << vps_max_num_reorder_pics[i] << ") not 0 (no base layer)").str());
			if(vps_max_latency_increase_plus1[i] != 0) errors.push_back((std::ostringstream() << "[H265 VPS] vps_max_latency_increase_plus1[" << i << "] value (" << vps_max_latency_increase_plus1[i] << ") not 0 (no base layer)").str());
		}
	}
	if(vps_max_layer_id > 62) errors.push_back((std::ostringstream() << "[H265 VPS] vps_max_layer_id value (" << (int)vps_max_layer_id << ") not in valid range (0..62)").str());
	if(vps_max_layer_id > 1023) errors.push_back((std::ostringstream() << "[H265 VPS] vps_num_layer_sets_minus1 value (" << (int)vps_num_layer_sets_minus1 << ") not in valid range (0..1023)").str());
	if (vps_timing_info_present_flag) {
		if(vps_num_units_in_tick == 0) errors.push_back("[H265 VPS] vps_num_units_in_tick not greater than 0");
		if(vps_time_scale == 0) errors.push_back("[H265 VPS] vps_time_scale not greater than 0");
		if (vps_poc_proportional_to_timing_flag) {
			if(vps_num_ticks_poc_diff_one_minus1 == UINT32_MAX) errors.push_back((std::ostringstream() << "[H265 VPS] vps_num_ticks_poc_diff_one_minus1 value (" << vps_num_ticks_poc_diff_one_minus1 << ") not in valid range (0..4294967294)").str());
		}
		if(vps_num_hrd_parameters > vps_num_layer_sets_minus1+1) errors.push_back((std::ostringstream() << "[H265 VPS] vps_num_hrd_parameters value (" << vps_num_hrd_parameters << ") not in valid range (0.." << vps_num_layer_sets_minus1+1 << ")").str());
		for (uint32_t i = 0; i < vps_num_hrd_parameters; ++i) {
			if((!vps_base_layer_internal_flag && hrd_layer_set_idx[i] == 0) || hrd_layer_set_idx[i] > vps_num_layer_sets_minus1){
				errors.push_back((std::ostringstream() << "[H265 VPS] hrd_layer_set_idx[" << i << "] value (" << hrd_layer_set_idx[i] << ") not in valid range (" << (vps_base_layer_internal_flag ? 0 : 1) << ".." << vps_num_layer_sets_minus1 << ")").str());
			}
			hrd_parameters[i].validate();
			errors.insert(errors.end(), hrd_parameters[i].errors.begin(), hrd_parameters[i].errors.end());
			hrd_parameters[i].errors.clear();
		}
		std::unordered_set<uint32_t> seen_hrd_layer_set_idx;
		for(uint32_t hrd_layer_set_index : hrd_layer_set_idx) {
			if(!seen_hrd_layer_set_idx.insert(hrd_layer_set_index).second){
				errors.push_back("[H265 VPS] Duplicate values of hrd_layer_set_idx detected");
				break;
			}
		}
	}
}
