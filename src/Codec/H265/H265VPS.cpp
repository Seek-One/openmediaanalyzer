#include <algorithm>
#include <unordered_set>

#include "../../StringHelpers/StringFormatter.h"
#include "../../StringHelpers/UnitFieldList.h"

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

UnitFieldList H265VPS::dump_fields(){
	UnitFieldList fields = UnitFieldList("Video Parameter Set", H265NAL::dump_fields());
	if(!completelyParsed) return fields;
	fields.addItem(UnitField("vps_video_parameter_set_id", vps_video_parameter_set_id));
	fields.addItem(UnitField("vps_base_layer_internal_flag", vps_base_layer_internal_flag));
	fields.addItem(UnitField("vps_base_layer_available_flag", vps_base_layer_available_flag));
	fields.addItem(UnitField("vps_max_layers_minus1", vps_max_layers_minus1));
	fields.addItem(UnitField("vps_max_sub_layers_minus1", vps_max_sub_layers_minus1));
	fields.addItem(UnitField("vps_temporal_id_nesting_flag", vps_temporal_id_nesting_flag));
	fields.addItem(profile_tier_level.dump_fields());
	fields.addItem(UnitField("vps_sub_layer_ordering_info_present_flag", vps_sub_layer_ordering_info_present_flag));
	for(uint8_t i = vps_sub_layer_ordering_info_present_flag ? 0 : vps_max_sub_layers_minus1;i <= vps_max_sub_layers_minus1;++i){
		fields.addItem(IdxUnitField("vps_max_dec_pic_buffering_minus1", vps_max_dec_pic_buffering_minus1[i], i));
		fields.addItem(IdxUnitField("vps_max_num_reorder_pics", vps_max_num_reorder_pics[i], i));
		fields.addItem(IdxUnitField("vps_max_latency_increase_plus1", vps_max_latency_increase_plus1[i], i));
	}
	fields.addItem(UnitField("vps_max_layer_id", vps_max_layer_id));
	ValueUnitFieldList vps_num_layer_sets_minus1Field = ValueUnitFieldList("vps_num_layer_sets_minus1", vps_num_layer_sets_minus1);
	for(uint16_t i = 1;i <= vps_num_layer_sets_minus1;++i){
		for(uint8_t j = 0;j <= vps_max_layer_id;++j){
			vps_num_layer_sets_minus1Field.addItem(DblIdxUnitField("layer_id_included_flag", layer_id_included_flag[i][j], i, j));
		}
	}
	fields.addItem(std::move(vps_num_layer_sets_minus1Field));
	ValueUnitFieldList vps_timing_info_present_flagField = ValueUnitFieldList("vps_timing_info_present_flag", vps_timing_info_present_flag);
	if(vps_timing_info_present_flag){
		vps_timing_info_present_flagField.addItem(UnitField("vps_num_units_in_tick", vps_num_units_in_tick));
		vps_timing_info_present_flagField.addItem(UnitField("vps_time_scale", vps_time_scale));
		ValueUnitFieldList vps_poc_proportional_to_timing_flagField = ValueUnitFieldList("vps_poc_proportional_to_timing_flag", vps_poc_proportional_to_timing_flag);
		vps_timing_info_present_flagField.addItem(std::move(vps_poc_proportional_to_timing_flagField));
		if(vps_poc_proportional_to_timing_flag) vps_poc_proportional_to_timing_flagField.addItem(UnitField("vps_num_ticks_poc_diff_one_minus1", vps_num_ticks_poc_diff_one_minus1));
		ValueUnitFieldList vps_num_hrd_parametersField = ValueUnitFieldList("vps_num_hrd_parameters", vps_num_hrd_parameters);
		vps_timing_info_present_flagField.addItem(std::move(vps_num_hrd_parametersField));
		for(uint32_t i = 0;i < vps_num_hrd_parameters;++i){
			vps_num_hrd_parametersField.addItem(IdxUnitField("hrd_layer_set_idx", hrd_layer_set_idx[i], i));
			if(i > 0) vps_num_hrd_parametersField.addItem(IdxUnitField("cprms_present_flag", cprms_present_flag[i], i));
			vps_num_hrd_parametersField.addItem(hrd_parameters[i].dump_fields(cprms_present_flag[i]));
		}
	}
	fields.addItem(std::move(vps_timing_info_present_flagField));
	fields.addItem(UnitField("vps_extension_flag", vps_extension_flag));
	return fields;
}

void H265VPS::validate(){
	H265NAL::validate();
	if(!completelyParsed) return;
	if(!vps_base_layer_internal_flag && vps_max_layers_minus1 == 0) minorErrors.push_back("[VPS] vps_max_layers_minus1 not greater than 0");
	if(vps_max_layers_minus1 > 62) minorErrors.push_back(StringFormatter::formatString("[VPS] vps_max_layers_minus1 value (%ld) not in valid range (0..62)", vps_max_layers_minus1));
	if(vps_max_sub_layers_minus1 > 6) minorErrors.push_back(StringFormatter::formatString("[VPS] vps_max_sub_layers_minus1 value (%ld) not in valid range (0..6)", vps_max_sub_layers_minus1));
	if(vps_max_sub_layers_minus1 == 0 && !vps_temporal_id_nesting_flag) minorErrors.push_back("[VPS] vps_temporal_id_nesting_flag not set (singular temporal sub-layer)");
	profile_tier_level.validate(1);
	minorErrors.insert(minorErrors.end(), profile_tier_level.errors.begin(), profile_tier_level.errors.end());
	profile_tier_level.errors.clear();
	if(!vps_base_layer_internal_flag && vps_sub_layer_ordering_info_present_flag) minorErrors.push_back("[VPS] vps_sub_layer_ordering_info_present_flag set (no base layer)");
	for (uint8_t i = (vps_sub_layer_ordering_info_present_flag ? 0 : vps_max_sub_layers_minus1); i <= vps_max_sub_layers_minus1; ++i) {
		if(!vps_base_layer_internal_flag){
			if(vps_max_dec_pic_buffering_minus1[i] != 0) minorErrors.push_back(StringFormatter::formatString("[VPS] vps_max_dec_pic_buffering_minus1[%d] value (%ld) not 0 (no base layer)", i, vps_max_dec_pic_buffering_minus1[i]));
			if(vps_max_num_reorder_pics[i] != 0) minorErrors.push_back(StringFormatter::formatString("[VPS] vps_max_num_reorder_pics[%d] value (%ld) not 0 (no base layer)", i, vps_max_num_reorder_pics[i]));
			if(vps_max_latency_increase_plus1[i] != 0) minorErrors.push_back(StringFormatter::formatString("[VPS] vps_max_latency_increase_plus1[%d] value (%ld) not 0 (no base layer)", i, vps_max_latency_increase_plus1[i]));
		}
	}
	if(vps_max_layer_id > 62) minorErrors.push_back(StringFormatter::formatString("[VPS] vps_max_layer_id value (%ld) not in valid range (0..62)", vps_max_layer_id));
	if(vps_max_layer_id > 1023) minorErrors.push_back(StringFormatter::formatString("[VPS] vps_num_layer_sets_minus1 value (%ld) not in valid range (0..1023)", vps_num_layer_sets_minus1));
	if (vps_timing_info_present_flag) {
		if(vps_num_units_in_tick == 0) minorErrors.push_back("[VPS] vps_num_units_in_tick not greater than 0");
		if(vps_time_scale == 0) minorErrors.push_back("[VPS] vps_time_scale not greater than 0");
		if (vps_poc_proportional_to_timing_flag) {
			if(vps_num_ticks_poc_diff_one_minus1 == UINT32_MAX) minorErrors.push_back(StringFormatter::formatString("[VPS] vps_num_ticks_poc_diff_one_minus1 value (%ld) not in valid range (0..4294967294)", vps_num_ticks_poc_diff_one_minus1));
		}
		if(vps_num_hrd_parameters > vps_num_layer_sets_minus1+1u) minorErrors.push_back(StringFormatter::formatString("[VPS] vps_num_hrd_parameters value (%ld) not in valid range (0..{})", vps_num_hrd_parameters, vps_num_layer_sets_minus1+1));
		for (uint32_t i = 0; i < vps_num_hrd_parameters; ++i) {
			if((!vps_base_layer_internal_flag && hrd_layer_set_idx[i] == 0) || hrd_layer_set_idx[i] > vps_num_layer_sets_minus1){
				minorErrors.push_back(StringFormatter::formatString("[VPS] hrd_layer_set_idx[%d] value (%ld) not in valid range ({}..{})", i, hrd_layer_set_idx[i], (vps_base_layer_internal_flag ? 0 : 1), vps_num_layer_sets_minus1));
			}
			hrd_parameters[i].validate();
			minorErrors.insert(minorErrors.end(), hrd_parameters[i].errors.begin(), hrd_parameters[i].errors.end());
			hrd_parameters[i].errors.clear();
		}
		std::unordered_set<uint32_t> seen_hrd_layer_set_idx;
		for(uint32_t hrd_layer_set_index : hrd_layer_set_idx) {
			if(!seen_hrd_layer_set_idx.insert(hrd_layer_set_index).second){
				minorErrors.push_back("[VPS] Duplicate values of hrd_layer_set_idx detected");
				break;
			}
		}
	}
}
