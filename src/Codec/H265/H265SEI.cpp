#include <iostream>
#include <cstring>

#include "../../StringHelpers/UnitFieldList.h"

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

UnitFieldList H265SEIMessage::dump_fields(){
	return UnitFieldList("SEI Message");
}

UnitFieldList H265SEI::dump_fields(){
	UnitFieldList fields = UnitFieldList("Supplemental Enhancement Information", H265NAL::dump_fields());
	if(!completelyParsed) return fields;
	for(H265SEIMessage* message : messages){
		fields.addItem(message->dump_fields());
	}
	return fields;
}

void H265SEI::validate(){
	H265NAL::validate();
	if(!completelyParsed) return;
}