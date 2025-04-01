#include "H265ProfileTierLevel.h"

#include <cstring>

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
