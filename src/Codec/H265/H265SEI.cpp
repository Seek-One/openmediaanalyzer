#include <iostream>
#include <cstring>
#include <sstream>

#include "H265SEI.h"

#include "H265SEI.h"

H265SEI::H265SEI():
	H265SEI(0, UnitType_Unspecified, 0, 0, 0, nullptr)
{}

H265SEI::H265SEI(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, uint8_t* nal_data):
	H265NAL(forbidden_zero_bit, nal_unit_type, nuh_layer_id, nuh_temporal_id_plus1, nal_size, nal_data)
{}

H265SEI::~H265SEI(){
	for(H265SEIMessage* message : messages) delete message;
	messages.clear();
}

H265SEIMessage::H265SEIMessage(){
	payloadType = UINT8_MAX;
}

std::vector<std::string> H265SEIMessage::dump_fields(){
	return std::vector<std::string>();
}

std::vector<std::string> H265SEI::dump_fields(){
	std::vector<std::string> fields;
	for(H265SEIMessage* message : messages){
		std::vector<std::string> msgFields = message->dump_fields();
		fields.insert(fields.end(), msgFields.begin(), msgFields.end());
	}
	return fields;
}