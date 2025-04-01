#include "H264NAL.h"

H264NAL::H264NAL()
{
	forbidden_zero_bit = 0;
	nal_ref_idc = 0;
	nal_unit_type = UnitType_Unspecified;
	nal_size = 0;
}

std::vector<std::string> H264NAL::dump_fields(){
	return std::vector<std::string>();
}
