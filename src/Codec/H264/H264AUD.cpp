#include <cstring>
#include <sstream>

#include "H264AUD.h"

H264AUD::H264AUD()
{
	nal_unit_type = UnitType_AUD;
	primary_pic_type = 7; // All slices allowed
}

std::vector<std::string> H264AUD::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "primary_pic_type:" << (int)primary_pic_type).str());
	return fields;
}
