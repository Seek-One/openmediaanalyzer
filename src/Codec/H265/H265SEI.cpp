#include <iostream>
#include <cstring>

#include "H265SEI.h"

H265SEI::H265SEI(H265NALHeader* pNALHeader, uint32_t nal_size, const uint8_t* nal_data):
		H265NALUnit(pNALHeader, nal_size, nal_data)
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
		H265NALUnit::dump(dumpObject);

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
	H265NALUnit::validate();
	if(!completelyParsed) return;
}