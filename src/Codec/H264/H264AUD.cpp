#include <cstring>
#include <fmt/core.h>

#include "H264AUD.h"

H264AUD::H264AUD():
	H264AUD(0, 0, 0, nullptr)
{}

H264AUD::H264AUD(uint8_t forbidden_zero_bit, uint8_t nal_ref_idc, uint32_t nal_size, uint8_t* nal_data):
	H264NAL(forbidden_zero_bit, nal_ref_idc, nal_size, nal_data)
{
	nal_unit_type = UnitType_AUD;
	primary_pic_type = 7; // All slices allowed
}

std::vector<std::string> H264AUD::dump_fields(){
	std::vector<std::string> fields;
	if(!completelyParsed) return fields;
	fields.push_back(fmt::format("primary_pic_type:{}", primary_pic_type));
	return fields;
}

void H264AUD::validate(){
	H264NAL::validate();
	if(!completelyParsed) return;
}
