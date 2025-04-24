#include <cstring>
#include <string>
#include <sstream>

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

std::vector<std::string> H265ProfileTierLevel::dump_fields(){
	std::vector<std::string> fields;
	const uint8_t extraProfiles[4] = {5, 9, 10, 11};
	const uint8_t inbldProfiles[7] = {1, 2, 3, 4, 5, 9, 11};
	fields.push_back((std::ostringstream() << "general_profile_space:" << (int)general_profile_space).str());
	fields.push_back((std::ostringstream() << "general_tier_flag:" << (int)general_tier_flag).str());
	fields.push_back((std::ostringstream() << "general_profile_idc:" << (int)general_profile_idc).str());
	for(int i = 0;i < 32;++i) fields.push_back((std::ostringstream() << "  general_profile_compatibility_flag[" << i << "]:" << (int)general_profile_compatibility_flag[i]).str());
	fields.push_back((std::ostringstream() << "general_progressive_source_flag:" << (int)general_progressive_source_flag).str());
	fields.push_back((std::ostringstream() << "general_interlaced_source_flag:" << (int)general_interlaced_source_flag).str());
	fields.push_back((std::ostringstream() << "general_non_packed_constraint_flag:" << (int)general_non_packed_constraint_flag).str());
	fields.push_back((std::ostringstream() << "general_frame_only_constraint_flag:" << (int)general_frame_only_constraint_flag).str());
	bool constraintFlags = false;
	for(int i = 4;i < 12 && !constraintFlags;++i) constraintFlags = constraintFlags || general_profile_idc == i || general_profile_compatibility_flag[i];
	if(constraintFlags){
		fields.push_back((std::ostringstream() << "general_max_10bit_constraint_flag:" << (int)general_max_10bit_constraint_flag).str());
		fields.push_back((std::ostringstream() << "general_max_8bit_constraint_flag:" << (int)general_max_8bit_constraint_flag).str());
		fields.push_back((std::ostringstream() << "general_max_422chroma_constraint_flag:" << (int)general_max_422chroma_constraint_flag).str());
		fields.push_back((std::ostringstream() << "general_max_420chroma_constraint_flag:" << (int)general_max_420chroma_constraint_flag).str());
		fields.push_back((std::ostringstream() << "general_max_monochrome_constraint_flag:" << (int)general_max_monochrome_constraint_flag).str());
		fields.push_back((std::ostringstream() << "general_intra_constraint_flag:" << (int)general_intra_constraint_flag).str());
		fields.push_back((std::ostringstream() << "general_one_picture_only_constraint_flag:" << (int)general_one_picture_only_constraint_flag).str());
		fields.push_back((std::ostringstream() << "general_lower_bit_rate_constraint_flag:" << (int)general_lower_bit_rate_constraint_flag).str());
		bool extraConstraintFlag = false; 
		for(int i = 0; i < 4 && !extraConstraintFlag;++i) extraConstraintFlag = extraConstraintFlag || general_profile_idc == extraProfiles[i] || general_profile_compatibility_flag[extraProfiles[i]];
		if(extraConstraintFlag){
		}
	} else if(general_profile_idc == 2 || general_profile_compatibility_flag[2]){
		fields.push_back((std::ostringstream() << "general_one_picture_only_constraint_flag:" << (int)general_one_picture_only_constraint_flag).str());
	}
	bool inbldFlagPresent = false; 
	for(int i = 0; i < 7 && !inbldFlagPresent;++i) inbldFlagPresent = inbldFlagPresent || general_profile_idc == inbldProfiles[i] || general_profile_compatibility_flag[inbldProfiles[i]];
	if(inbldFlagPresent){
		fields.push_back((std::ostringstream() << "general_one_picture_only_constraint_flag:" << (int)general_inbld_flag).str());
	}
	fields.push_back((std::ostringstream() << "general_level_idc:" << (int)general_level_idc).str());
	uint32_t maxNumSubLayersMinus1 = sub_layer_profile_present_flag.size();
	for(int i = 0;i < maxNumSubLayersMinus1;++i){
		fields.push_back((std::ostringstream() << "  sub_layer_profile_present_flag[" << i << "]:" << (int)sub_layer_profile_present_flag[i]).str());
		fields.push_back((std::ostringstream() << "  sub_layer_level_present_flag[" << i << "]:" << (int)sub_layer_level_present_flag[i]).str());
	}
	for(int i = 0;i < maxNumSubLayersMinus1;++i){
		if(sub_layer_profile_present_flag[i]){
			fields.push_back((std::ostringstream() << "  sub_layer_profile_space[" << i << "]:" << (int)sub_layer_profile_space[i]).str());
			fields.push_back((std::ostringstream() << "  sub_layer_tier_flag[" << i << "]:" << (int)sub_layer_tier_flag[i]).str());
			fields.push_back((std::ostringstream() << "  sub_layer_profile_idc[" << i << "]:" << (int)sub_layer_profile_idc[i]).str());
			for(int j =0;j < 32;++j) fields.push_back((std::ostringstream() << "    sub_layer_profile_idc[" << i << "][" << j << "]:" << (int)sub_layer_profile_compatibility_flag[i][j]).str());
			fields.push_back((std::ostringstream() << "  sub_layer_progressive_source_flag[" << i << "]:" << (int)sub_layer_progressive_source_flag[i]).str());
			fields.push_back((std::ostringstream() << "  sub_layer_interlaced_source_flag[" << i << "]:" << (int)sub_layer_interlaced_source_flag[i]).str());
			fields.push_back((std::ostringstream() << "  sub_layer_non_packed_constraint_flag[" << i << "]:" << (int)sub_layer_non_packed_constraint_flag[i]).str());
			fields.push_back((std::ostringstream() << "  sub_layer_frame_only_constraint_flag[" << i << "]:" << (int)sub_layer_frame_only_constraint_flag[i]).str());
			bool subConstraintFlags = false;
			for(int k = 4;k < 12 && !subConstraintFlags;++k) subConstraintFlags = subConstraintFlags || sub_layer_profile_idc[i] == k || sub_layer_profile_compatibility_flag[i][k];
			if(subConstraintFlags){
				fields.push_back((std::ostringstream() << "  sub_layer_max_10bit_constraint_flag[" << i <<"]:" << (int)sub_layer_max_10bit_constraint_flag[i]).str());
				fields.push_back((std::ostringstream() << "  sub_layer_max_8bit_constraint_flag[" << i <<"]:" << (int)sub_layer_max_8bit_constraint_flag[i]).str());
				fields.push_back((std::ostringstream() << "  sub_layer_max_422chroma_constraint_flag[" << i <<"]:" << (int)sub_layer_max_422chroma_constraint_flag[i]).str());
				fields.push_back((std::ostringstream() << "  sub_layer_max_420chroma_constraint_flag[" << i <<"]:" << (int)sub_layer_max_420chroma_constraint_flag[i]).str());
				fields.push_back((std::ostringstream() << "  sub_layer_max_monochrome_constraint_flag[" << i <<"]:" << (int)sub_layer_max_monochrome_constraint_flag[i]).str());
				fields.push_back((std::ostringstream() << "  sub_layer_intra_constraint_flag[" << i <<"]:" << (int)sub_layer_intra_constraint_flag[i]).str());
				fields.push_back((std::ostringstream() << "  sub_layer_one_picture_only_constraint_flag[" << i <<"]:" << (int)sub_layer_one_picture_only_constraint_flag[i]).str());
				fields.push_back((std::ostringstream() << "  sub_layer_lower_bit_rate_constraint_flag[" << i <<"]:" << (int)sub_layer_lower_bit_rate_constraint_flag[i]).str());
				bool subExtraConstraintFlag = false; 
				for(int k = 0;k < 4 && !subExtraConstraintFlag;++i) subExtraConstraintFlag = subExtraConstraintFlag || sub_layer_profile_idc[i] == extraProfiles[k] || sub_layer_profile_compatibility_flag[i][extraProfiles[k]];
				if(subExtraConstraintFlag) {
					fields.push_back((std::ostringstream() << "  sub_layer_max_14bit_constraint_flag[" << i <<"]:" << (int)sub_layer_max_14bit_constraint_flag[i]).str());
				}
			} else if(sub_layer_profile_idc[i] == 2 || sub_layer_profile_compatibility_flag[i][2]){
				fields.push_back((std::ostringstream() << "  sub_layer_one_picture_only_constraint_flag[" << i <<"]:" << (int)sub_layer_one_picture_only_constraint_flag[i]).str());
			}
			bool subInbldFlagPresent = false;
			for(int k = 0;k < 7 && !subInbldFlagPresent;++k) subInbldFlagPresent = subInbldFlagPresent || sub_layer_profile_idc[i] == inbldProfiles[k] || sub_layer_profile_compatibility_flag[i][inbldProfiles[k]];
			if(subInbldFlagPresent){
				fields.push_back((std::ostringstream() << "  sub_layer_inbld_flag[" << i <<"]:" << (int)sub_layer_inbld_flag[i]).str());
			}
		}
		if(sub_layer_level_present_flag[i]){
			fields.push_back((std::ostringstream() << "  sub_layer_level_idc[" << i <<"]:" << (int)sub_layer_level_idc[i]).str());
		}
	}
	return fields;
}