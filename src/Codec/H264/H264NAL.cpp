#include <cstring>

#include "H264NAL.h"

H264NAL::H264NAL():
	H264NAL(0, 0, 0, nullptr)
{}

H264NAL::H264NAL(uint8_t forbiddenZeroBit, uint8_t nalRefIdc, uint32_t nalSize, uint8_t* nalData):
	forbidden_zero_bit(forbiddenZeroBit), nal_ref_idc(nalRefIdc), nal_size(nalSize), nal_data(new uint8_t[nalSize]), nal_unit_type(UnitType_Unspecified)
{
	std::memcpy(nal_data, nalData, nalSize);
}

std::vector<std::string> H264NAL::dump_fields(){
	return std::vector<std::string>();
}
