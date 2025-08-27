#include <cstring>
#include <string>

#include "../../StringHelpers/StringFormatter.h"

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

void H265ProfileTierLevel::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Profile Tier Level");
	H26X_BREAKABLE_SCOPE(H26XDumpScope)
	{
		const uint8_t extraProfiles[4] = {5, 9, 10, 11};
		const uint8_t inbldProfiles[7] = {1, 2, 3, 4, 5, 9, 11};
		
		dumpObject.addUnitField("general_profile_space", general_profile_space);
		dumpObject.addUnitField("general_tier_flag", general_tier_flag);
		dumpObject.addUnitField("general_profile_idc", general_profile_idc);
		for (int i = 0; i < 32; ++i) {
			dumpObject.addIdxUnitField("general_profile_compatibility_flag", i, general_profile_compatibility_flag[i]);
		}
		dumpObject.addUnitField("general_progressive_source_flag", general_progressive_source_flag);
		dumpObject.addUnitField("general_interlaced_source_flag", general_interlaced_source_flag);
		dumpObject.addUnitField("general_non_packed_constraint_flag", general_non_packed_constraint_flag);
		dumpObject.addUnitField("general_frame_only_constraint_flag", general_frame_only_constraint_flag);
		bool constraintFlags = false;
		for (int i = 4; i < 12 && !constraintFlags; ++i) {
			constraintFlags = constraintFlags || general_profile_idc == i || general_profile_compatibility_flag[i];
		}
		if (constraintFlags) {
			dumpObject.addUnitField("general_max_10bit_constraint_flag", general_max_10bit_constraint_flag);
			dumpObject.addUnitField("general_max_8bit_constraint_flag", general_max_8bit_constraint_flag);
			dumpObject.addUnitField("general_max_422chroma_constraint_flag", general_max_422chroma_constraint_flag);
			dumpObject.addUnitField("general_max_420chroma_constraint_flag", general_max_420chroma_constraint_flag);
			dumpObject.addUnitField("general_max_monochrome_constraint_flag", general_max_monochrome_constraint_flag);
			dumpObject.addUnitField("general_intra_constraint_flag", general_intra_constraint_flag);
			dumpObject.addUnitField("general_one_picture_only_constraint_flag", general_one_picture_only_constraint_flag);
			dumpObject.addUnitField("general_lower_bit_rate_constraint_flag", general_lower_bit_rate_constraint_flag);
			bool extraConstraintFlag = false;
			for (int i = 0; i < 4 && !extraConstraintFlag; ++i) {
				extraConstraintFlag = extraConstraintFlag || general_profile_idc == extraProfiles[i] || general_profile_compatibility_flag[extraProfiles[i]];
			}
			if (extraConstraintFlag) {
			}
		} else if (general_profile_idc == 2 || general_profile_compatibility_flag[2]) {
			dumpObject.addUnitField("general_one_picture_only_constraint_flag", general_one_picture_only_constraint_flag);
		}
		bool inbldFlagPresent = false;
		for (int i = 0; i < 7 && !inbldFlagPresent; i++) {
			inbldFlagPresent = inbldFlagPresent || general_profile_idc == inbldProfiles[i] || general_profile_compatibility_flag[inbldProfiles[i]];
		}
		if (inbldFlagPresent) {
			dumpObject.addUnitField("general_one_picture_only_constraint_flag", general_inbld_flag);
		}
		dumpObject.addUnitField("general_level_idc", general_level_idc);
		uint32_t maxNumSubLayersMinus1 = sub_layer_profile_present_flag.size();
		for (uint32_t i = 0; i < maxNumSubLayersMinus1; ++i) {
			dumpObject.startIdxValueUnitFieldList("sub_layer_profile_present_flag", i, sub_layer_profile_present_flag[i]);
			if (sub_layer_profile_present_flag[i]) {
				dumpObject.addIdxUnitField("sub_layer_profile_space", i, sub_layer_profile_space[i]);
				dumpObject.addIdxUnitField("sub_layer_tier_flag", i, sub_layer_tier_flag[i]);
				dumpObject.addIdxUnitField("sub_layer_profile_idc", i, sub_layer_profile_idc[i]);
				for (int j = 0; j < 32; ++j){
					dumpObject.addDblIdxUnitField("sub_layer_profile_idc", i, j, sub_layer_profile_compatibility_flag[i][j]);
				}
				dumpObject.addIdxUnitField("sub_layer_progressive_source_flag", i, sub_layer_progressive_source_flag[i]);
				dumpObject.addIdxUnitField("sub_layer_interlaced_source_flag", i, sub_layer_interlaced_source_flag[i]);
				dumpObject.addIdxUnitField("sub_layer_non_packed_constraint_flag", i, sub_layer_non_packed_constraint_flag[i]);
				dumpObject.addIdxUnitField("sub_layer_frame_only_constraint_flag", i, sub_layer_frame_only_constraint_flag[i]);
				bool subConstraintFlags = false;
				for (int k = 4; k < 12 && !subConstraintFlags; ++k) {
					subConstraintFlags = subConstraintFlags || sub_layer_profile_idc[i] == k || sub_layer_profile_compatibility_flag[i][k];
				}
				if (subConstraintFlags) {
					dumpObject.addIdxUnitField("sub_layer_max_10bit_constraint_flag", i, sub_layer_max_10bit_constraint_flag[i]);
					dumpObject.addIdxUnitField("sub_layer_max_8bit_constraint_flag", i, sub_layer_max_8bit_constraint_flag[i]);
					dumpObject.addIdxUnitField("sub_layer_max_422chroma_constraint_flag", i, sub_layer_max_422chroma_constraint_flag[i]);
					dumpObject.addIdxUnitField("sub_layer_max_420chroma_constraint_flag", i, sub_layer_max_420chroma_constraint_flag[i]);
					dumpObject.addIdxUnitField("sub_layer_max_monochrome_constraint_flag", i, sub_layer_max_monochrome_constraint_flag[i]);
					dumpObject.addIdxUnitField("sub_layer_intra_constraint_flag", i, sub_layer_intra_constraint_flag[i]);
					dumpObject.addIdxUnitField("sub_layer_one_picture_only_constraint_flag", i, sub_layer_one_picture_only_constraint_flag[i]);
					dumpObject.addIdxUnitField("sub_layer_lower_bit_rate_constraint_flag", i, sub_layer_lower_bit_rate_constraint_flag[i]);
					bool subExtraConstraintFlag = false;
					for (int k = 0; k < 4 && !subExtraConstraintFlag; ++k) {
						subExtraConstraintFlag = subExtraConstraintFlag || sub_layer_profile_idc[i] == extraProfiles[k] || sub_layer_profile_compatibility_flag[i][extraProfiles[k]];
					}
					if (subExtraConstraintFlag) {
						dumpObject.addIdxUnitField("sub_layer_max_14bit_constraint_flag", i, sub_layer_max_14bit_constraint_flag[i]);
					}
				} else if (sub_layer_profile_idc[i] == 2 || sub_layer_profile_compatibility_flag[i][2]) {
					dumpObject.addIdxUnitField("sub_layer_one_picture_only_constraint_flag", i, sub_layer_one_picture_only_constraint_flag[i]);
				}
				bool subInbldFlagPresent = false;
				for (int k = 0; k < 7 && !subInbldFlagPresent; ++k) {
					subInbldFlagPresent = subInbldFlagPresent || sub_layer_profile_idc[i] == inbldProfiles[k] || sub_layer_profile_compatibility_flag[i][inbldProfiles[k]];
				}
				if (subInbldFlagPresent) {
					dumpObject.addIdxUnitField("  sub_layer_inbld_flag", i, sub_layer_inbld_flag[i]);
				}
			}
			dumpObject.endIdxValueUnitFieldList();

			dumpObject.startIdxValueUnitFieldList("sub_layer_level_present_flag", i, sub_layer_level_present_flag[i]);
			if (sub_layer_level_present_flag[i]) {
				dumpObject.addIdxUnitField("sub_layer_level_idc", i, sub_layer_level_idc[i]);
			}
			dumpObject.endIdxValueUnitFieldList();
		}
	}
}

void H265ProfileTierLevel::validate(uint8_t iProfilePresentFlag){
	if (iProfilePresentFlag) {
		if(general_profile_space != 0){
			errors.add(H26XError::Minor, "[Profile tier level] general_profile_space not equal to 0");
		}
		if(!general_profile_compatibility_flag[general_profile_idc]) {
			errors.add(H26XError::Minor, "[Profile tier level] general_profile_compatibility_flag[general_profile_idc] not set");
		}
	}
	for (uint8_t i = 0; i < sub_layer_profile_present_flag.size(); ++i) {
		if(!iProfilePresentFlag && sub_layer_profile_present_flag[i]){
			errors.add(H26XError::Minor, StringFormatter::formatString("[Profile tier level] sub_layer_profile_present_flag[%d] set (profilePresentFlag not set)", i));
		}
	}

	for (uint8_t i = 0; i < sub_layer_profile_present_flag.size(); ++i) {
		if (sub_layer_profile_present_flag[i]) {
			if(sub_layer_profile_space[i] != 0){
				errors.add(H26XError::Minor, StringFormatter::formatString("[Profile tier level] sub_layer_profile_space[%d] not equal to 0", i));
			}
			if(!sub_layer_profile_compatibility_flag[i][sub_layer_profile_idc[i]]){
				errors.add(H26XError::Minor, StringFormatter::formatString("[Profile tier level] sub_layer_profile_compatibility_flag[%d][sub_layer_profile_idc[%d]] not set", i, i));
			}
		}
	}
}