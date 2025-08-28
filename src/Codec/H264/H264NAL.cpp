#include <iostream>
#include <cstring>

#include "../H26X/H26XUtils.h"

#include "H264NAL.h"

H264NAL::H264NAL():
	H264NAL(0, 0, 0, nullptr)
{}

H264NAL::H264NAL(uint8_t forbiddenZeroBit, uint8_t nalRefIdc, uint32_t nalSize, const uint8_t* nalData):
	forbidden_zero_bit(forbiddenZeroBit), nal_ref_idc(nalRefIdc),   nal_unit_type(H264NALUnitType::Unspecified), nal_size(nalSize+3), nal_data(nullptr)
{
	if(nalData == nullptr){
		return;
	}
	nal_data = new uint8_t[nal_size];
	std::memcpy(nal_data, g_startCode3Bytes, 3);
	std::memcpy(nal_data+3, nalData, nalSize);
}

H264NAL::~H264NAL(){
	if(nal_data) delete[] nal_data;
}

void H264NAL::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("NAL Unit");
	dumpObject.addUnitField("forbidden_zero_bit", forbidden_zero_bit);
	dumpObject.addUnitField("nal_ref_idc", nal_ref_idc);
	dumpObject.addUnitField("nal_unit_type", nal_unit_type);
	dumpObject.endUnitFieldList();
}

void H264NAL::validate()
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
