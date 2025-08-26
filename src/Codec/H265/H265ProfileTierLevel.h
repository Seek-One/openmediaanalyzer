#ifndef TOOLKIT_CODEC_UTILS_H265_PROFILE_TIER_LEVEL_H_
#define TOOLKIT_CODEC_UTILS_H265_PROFILE_TIER_LEVEL_H_

#include <string>

#include "H265Types.h"

#include "../H26X/H26XDumpObject.h"

struct H265ProfileTierLevel {
	H265ProfileTierLevel();

	uint8_t general_profile_space;
	uint8_t general_tier_flag;
	uint8_t general_profile_idc;
	uint8_t general_profile_compatibility_flag[32];
	uint8_t general_progressive_source_flag;
	uint8_t general_interlaced_source_flag;
	uint8_t general_non_packed_constraint_flag;
	uint8_t general_frame_only_constraint_flag;
	uint8_t general_max_12bit_constraint_flag;
	uint8_t general_max_10bit_constraint_flag;
	uint8_t general_max_8bit_constraint_flag;
	uint8_t general_max_422chroma_constraint_flag;
	uint8_t general_max_420chroma_constraint_flag;
	uint8_t general_max_monochrome_constraint_flag;
	uint8_t general_intra_constraint_flag;
	uint8_t general_one_picture_only_constraint_flag;
	uint8_t general_lower_bit_rate_constraint_flag;
	uint8_t general_max_14bit_constraint_flag;
	uint8_t general_inbld_flag;
	uint8_t general_level_idc;
	std::vector<uint8_t> sub_layer_profile_present_flag;
	std::vector<uint8_t> sub_layer_level_present_flag;
	std::vector<uint8_t> sub_layer_profile_space;
	std::vector<uint8_t> sub_layer_tier_flag;
	std::vector<uint8_t> sub_layer_profile_idc;
	std::vector<UInt8Vector> sub_layer_profile_compatibility_flag;
	std::vector<uint8_t> sub_layer_progressive_source_flag;
	std::vector<uint8_t> sub_layer_interlaced_source_flag;
	std::vector<uint8_t> sub_layer_non_packed_constraint_flag;
	std::vector<uint8_t> sub_layer_frame_only_constraint_flag;
	std::vector<uint8_t> sub_layer_max_12bit_constraint_flag;
	std::vector<uint8_t> sub_layer_max_10bit_constraint_flag;
	std::vector<uint8_t> sub_layer_max_8bit_constraint_flag;
	std::vector<uint8_t> sub_layer_max_422chroma_constraint_flag;
	std::vector<uint8_t> sub_layer_max_420chroma_constraint_flag;
	std::vector<uint8_t> sub_layer_max_monochrome_constraint_flag;
	std::vector<uint8_t> sub_layer_intra_constraint_flag;
	std::vector<uint8_t> sub_layer_one_picture_only_constraint_flag;
	std::vector<uint8_t> sub_layer_lower_bit_rate_constraint_flag;
	std::vector<uint8_t> sub_layer_max_14bit_constraint_flag;
	std::vector<uint8_t> sub_layer_inbld_flag;
	std::vector<uint8_t> sub_layer_level_idc;

	std::vector<std::string> errors;

	void dump(H26XDumpObject& dumpObject) const;
	void validate(uint8_t iProfilePresentFlag);
};

#endif // TOOLKIT_CODEC_UTILS_H265_PROFILE_TIER_LEVEL_H_
