//
// Created by ebeuque on 28/08/25.
//

#include "H264NALHeader.h"

H264NALHeader::H264NALHeader()
{
	forbidden_zero_bit = 0;
	nal_ref_idc = 0;
	nal_unit_type = H264NALUnitType::Unspecified;
}

void H264NALHeader::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("NAL Unit Header");
	dumpObject.addUnitField("forbidden_zero_bit", forbidden_zero_bit);
	dumpObject.addUnitField("nal_ref_idc", nal_ref_idc);
	dumpObject.addUnitField("nal_unit_type", nal_unit_type);
	dumpObject.endUnitFieldList();
}

void H264NALHeader::checkErrors(H26XErrors& errors) const
{
	if (forbidden_zero_bit != 0) {
		errors.add(H26XError::Minor, "[NAL Header] forbidden_zero_bit not equal to 0");
	}
	if(nal_ref_idc == 0){
		switch(nal_unit_type){
		case H264NALUnitType::SPS:
			errors.add(H26XError::Minor, "[NAL Header] Sequence parameter set marked as unimportant");
			break;
		case H264NALUnitType::PPS:
			errors.add(H26XError::Minor, "[NAL Header] Picture parameter set marked as unimportant");
			break;
		case H264NALUnitType::IDRFrame:
			errors.add(H26XError::Minor, "[NAL Header] IDR frame marked as unimportant");
			break;
		default:
			break;
		}
	}
}