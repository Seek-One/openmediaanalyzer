#include <iostream>
#include <cstring>

#include "H265SEI.h"

H265SEI::H265SEI():
	H265SEI(0, H265NALUnitType::Unspecified, 0, 0, 0, nullptr)
{}

H265SEI::H265SEI(uint8_t forbidden_zero_bit, H265NALUnitType::Type nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, const uint8_t* nal_data):
	H265NAL(forbidden_zero_bit, nal_unit_type, nuh_layer_id, nuh_temporal_id_plus1, nal_size, nal_data)
{}

H265SEI::~H265SEI(){
	for(H265SEIMessage* message : messages) delete message;
	messages.clear();
}

H265SEIMessage::H265SEIMessage(){
	payloadType = UINT8_MAX;
}

void H265SEIMessage::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("SEI Message");
	dumpObject.endUnitFieldList();
}

void H265SEI::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Supplemental Enhancement Information");
	H26X_BREAKABLE_SCOPE(H26XDumpScope) {
		H265NAL::dump(dumpObject);

		if (!completelyParsed) {
			break;
		}

		for (H265SEIMessage *message: messages) {
			message->dump(dumpObject);
		}
	}
	dumpObject.endUnitFieldList();
}

void H265SEI::validate(){
	H265NAL::validate();
	if(!completelyParsed) return;
}