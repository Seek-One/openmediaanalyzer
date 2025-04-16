#include <iostream>
#include <cstring>

#include "H264Utils.h"

#include "H264NAL.h"

H264NAL::H264NAL():
	H264NAL(0, 0, 0, nullptr)
{}

H264NAL::H264NAL(uint8_t forbiddenZeroBit, uint8_t nalRefIdc, uint32_t nalSize, uint8_t* nalData):
	forbidden_zero_bit(forbiddenZeroBit), nal_ref_idc(nalRefIdc), nal_size(nalSize+3), nal_data(nullptr), nal_unit_type(UnitType_Unspecified)
{
	if(nalData == nullptr) return;
	nal_data = new uint8_t[nal_size];
	std::memcpy(nal_data, g_startCode3Bytes, 3);
	std::memcpy(nal_data+3, nalData, nalSize);
}

H264NAL::~H264NAL(){
	if(nal_data) delete[] nal_data;
}

std::vector<std::string> H264NAL::dump_fields(){
	return std::vector<std::string>();
}
