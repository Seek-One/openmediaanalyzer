#include <cstring>
#include <string>

#include "../../StringHelpers/StringFormatter.h"
#include "../../StringHelpers/UnitFieldList.h"

#include "H265ProfileTierLevel.h"

H265ProfileTierLevel::H265ProfileTierLevel()
{
	general_profile_space = 0;
	general_tier_flag = 0;
	general_profile_idc = 0;
	memset(general_profile_compatibility_flag, 0, 32);
	general_progressive_source_flag = 0;
	general_interlaced_source_flag = 0;
	general_non_packed_constraint_flag = 0;
	general_frame_only_constraint_flag = 0;
	general_max_12bit_constraint_flag = 0;
	general_max_10bit_constraint_flag = 0;
	general_max_8bit_constraint_flag = 0;
	general_max_422chroma_constraint_flag = 0;
	general_max_420chroma_constraint_flag = 0;
	general_max_monochrome_constraint_flag = 0;
	general_intra_constraint_flag = 0;
	general_one_picture_only_constraint_flag = 0;
	general_lower_bit_rate_constraint_flag = 0;
	general_max_14bit_constraint_flag = 0;
	general_one_picture_only_constraint_flag = 0;
	general_inbld_flag = 0;
	general_level_idc = 0;
}

UnitFieldList H265ProfileTierLevel::dump_fields(){
	UnitFieldList fields = UnitFieldList("Profile Tier Level");
	const uint8_t extraProfiles[4] = {5, 9, 10, 11};
	const uint8_t inbldProfiles[7] = {1, 2, 3, 4, 5, 9, 11};
	fields.addItem(UnitField("general_profile_space", general_profile_space));
	fields.addItem(UnitField("general_tier_flag", general_tier_flag));
	fields.addItem(UnitField("general_profile_idc", general_profile_idc));
	for(int i = 0;i < 32;++i) fields.addItem(IdxUnitField("general_profile_compatibility_flag", general_profile_compatibility_flag[i], i));
	fields.addItem(UnitField("general_progressive_source_flag", general_progressive_source_flag));
	fields.addItem(UnitField("general_interlaced_source_flag", general_interlaced_source_flag));
	fields.addItem(UnitField("general_non_packed_constraint_flag", general_non_packed_constraint_flag));
	fields.addItem(UnitField("general_frame_only_constraint_flag", general_frame_only_constraint_flag));
	bool constraintFlags = false;
	for(int i = 4;i < 12 && !constraintFlags;++i) constraintFlags = constraintFlags || general_profile_idc == i || general_profile_compatibility_flag[i];
	if(constraintFlags){
		fields.addItem(UnitField("general_max_10bit_constraint_flag", general_max_10bit_constraint_flag));
		fields.addItem(UnitField("general_max_8bit_constraint_flag", general_max_8bit_constraint_flag));
		fields.addItem(UnitField("general_max_422chroma_constraint_flag", general_max_422chroma_constraint_flag));
		fields.addItem(UnitField("general_max_420chroma_constraint_flag", general_max_420chroma_constraint_flag));
		fields.addItem(UnitField("general_max_monochrome_constraint_flag", general_max_monochrome_constraint_flag));
		fields.addItem(UnitField("general_intra_constraint_flag", general_intra_constraint_flag));
		fields.addItem(UnitField("general_one_picture_only_constraint_flag", general_one_picture_only_constraint_flag));
		fields.addItem(UnitField("general_lower_bit_rate_constraint_flag", general_lower_bit_rate_constraint_flag));
		bool extraConstraintFlag = false; 
		for(int i = 0; i < 4 && !extraConstraintFlag;++i) extraConstraintFlag = extraConstraintFlag || general_profile_idc == extraProfiles[i] || general_profile_compatibility_flag[extraProfiles[i]];
		if(extraConstraintFlag){
		}
	} else if(general_profile_idc == 2 || general_profile_compatibility_flag[2]){
		fields.addItem(UnitField("general_one_picture_only_constraint_flag", general_one_picture_only_constraint_flag));
	}
	bool inbldFlagPresent = false; 
	for(int i = 0; i < 7 && !inbldFlagPresent;++i) inbldFlagPresent = inbldFlagPresent || general_profile_idc == inbldProfiles[i] || general_profile_compatibility_flag[inbldProfiles[i]];
	if(inbldFlagPresent){
		fields.addItem(UnitField("general_one_picture_only_constraint_flag", general_inbld_flag));
	}
	fields.addItem(UnitField("general_level_idc", general_level_idc));
	uint32_t maxNumSubLayersMinus1 = sub_layer_profile_present_flag.size();
	for(uint32_t i = 0;i < maxNumSubLayersMinus1;++i){
		IdxValueUnitFieldList sub_layer_profile_present_flagField = IdxValueUnitFieldList("sub_layer_profile_present_flag", sub_layer_profile_present_flag[i], i);
		if(sub_layer_profile_present_flag[i]){
			sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_profile_space", sub_layer_profile_space[i], i));
			sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_tier_flag", sub_layer_tier_flag[i], i));
			sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_profile_idc", sub_layer_profile_idc[i], i));
			for(int j =0;j < 32;++j) sub_layer_profile_present_flagField.addItem(DblIdxUnitField("sub_layer_profile_idc", sub_layer_profile_compatibility_flag[i][j], i, j));
			sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_progressive_source_flag", sub_layer_progressive_source_flag[i], i));
			sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_interlaced_source_flag", sub_layer_interlaced_source_flag[i], i));
			sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_non_packed_constraint_flag", sub_layer_non_packed_constraint_flag[i], i));
			sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_frame_only_constraint_flag", sub_layer_frame_only_constraint_flag[i], i));
			bool subConstraintFlags = false;
			for(int k = 4;k < 12 && !subConstraintFlags;++k) subConstraintFlags = subConstraintFlags || sub_layer_profile_idc[i] == k || sub_layer_profile_compatibility_flag[i][k];
			if(subConstraintFlags){
				sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_max_10bit_constraint_flag", sub_layer_max_10bit_constraint_flag[i], i));
				sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_max_8bit_constraint_flag", sub_layer_max_8bit_constraint_flag[i], i));
				sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_max_422chroma_constraint_flag", sub_layer_max_422chroma_constraint_flag[i], i));
				sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_max_420chroma_constraint_flag", sub_layer_max_420chroma_constraint_flag[i], i));
				sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_max_monochrome_constraint_flag", sub_layer_max_monochrome_constraint_flag[i], i));
				sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_intra_constraint_flag", sub_layer_intra_constraint_flag[i], i));
				sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_one_picture_only_constraint_flag", sub_layer_one_picture_only_constraint_flag[i], i));
				sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_lower_bit_rate_constraint_flag", sub_layer_lower_bit_rate_constraint_flag[i], i));
				bool subExtraConstraintFlag = false; 
				for(int k = 0;k < 4 && !subExtraConstraintFlag;++k) subExtraConstraintFlag = subExtraConstraintFlag || sub_layer_profile_idc[i] == extraProfiles[k] || sub_layer_profile_compatibility_flag[i][extraProfiles[k]];
				if(subExtraConstraintFlag) {
					sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_max_14bit_constraint_flag", sub_layer_max_14bit_constraint_flag[i], i));
				}
			} else if(sub_layer_profile_idc[i] == 2 || sub_layer_profile_compatibility_flag[i][2]){
				sub_layer_profile_present_flagField.addItem(IdxUnitField("sub_layer_one_picture_only_constraint_flag", sub_layer_one_picture_only_constraint_flag[i], i));
			}
			bool subInbldFlagPresent = false;
			for(int k = 0;k < 7 && !subInbldFlagPresent;++k) subInbldFlagPresent = subInbldFlagPresent || sub_layer_profile_idc[i] == inbldProfiles[k] || sub_layer_profile_compatibility_flag[i][inbldProfiles[k]];
			if(subInbldFlagPresent){
				sub_layer_profile_present_flagField.addItem(IdxUnitField("  sub_layer_inbld_flag", sub_layer_inbld_flag[i], i));
			}
		}
		fields.addItem(std::move(sub_layer_profile_present_flagField));

		IdxValueUnitFieldList sub_layer_level_present_flagField = IdxValueUnitFieldList("sub_layer_level_present_flag", sub_layer_level_present_flag[i], i);
		if(sub_layer_level_present_flag[i]){
			sub_layer_level_present_flagField.addItem(IdxUnitField("sub_layer_level_idc", sub_layer_level_idc[i], i));
		}
		fields.addItem(std::move(sub_layer_level_present_flagField));
	}
	return fields;
}

void H265ProfileTierLevel::validate(uint8_t iProfilePresentFlag){
	if (iProfilePresentFlag) {
		if(general_profile_space != 0) errors.push_back("[Profile tier level] general_profile_space not equal to 0");
		if(!general_profile_compatibility_flag[general_profile_idc]) errors.push_back("[Profile tier level] general_profile_compatibility_flag[general_profile_idc] not set");
	}
	for (uint8_t i = 0; i < sub_layer_profile_present_flag.size(); ++i) {
		if(!iProfilePresentFlag && sub_layer_profile_present_flag[i]) errors.push_back(StringFormatter::formatString("[Profile tier level] sub_layer_profile_present_flag[%d] set (profilePresentFlag not set)", i));
	}

	for (uint8_t i = 0; i < sub_layer_profile_present_flag.size(); ++i) {
		if (sub_layer_profile_present_flag[i]) {
			if(sub_layer_profile_space[i] != 0) errors.push_back(StringFormatter::formatString("[Profile tier level] sub_layer_profile_space[%d] not equal to 0", i));
			if(!sub_layer_profile_compatibility_flag[i][sub_layer_profile_idc[i]]) errors.push_back(StringFormatter::formatString("[Profile tier level] sub_layer_profile_compatibility_flag[%d][sub_layer_profile_idc[%d]] not set", i, i));
		}
	}
}