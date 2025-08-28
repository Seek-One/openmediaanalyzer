//
// Created by ebeuque on 28/08/25.
//

#include "../H26X/H26XUtils.h"
#include "H265NALHeader.h"

H265NALHeader::H265NALHeader()
{
	this->forbidden_zero_bit = 0;
	this->nal_unit_type = H265NALUnitType::Unspecified;
	this->nuh_layer_id = 0;
	this->nuh_temporal_id_plus1 = 0;
}

void H265NALHeader::setup()
{
	this->TemporalId = nuh_temporal_id_plus1-1;
}

bool H265NALHeader::isSlice() const
{
	return H265NALHeader::isSlice(nal_unit_type);
}

bool H265NALHeader::isSlice(H265NALUnitType::Type nal_unit_type)
{
	switch (nal_unit_type) {
	case H265NALUnitType::TRAIL_N:
	case H265NALUnitType::TRAIL_R:
	case H265NALUnitType::TSA_N:
	case H265NALUnitType::TSA_R:
	case H265NALUnitType::STSA_N:
	case H265NALUnitType::STSA_R:
	case H265NALUnitType::RADL_N:
	case H265NALUnitType::RADL_R:
	case H265NALUnitType::RASL_N:
	case H265NALUnitType::RASL_R:
	case H265NALUnitType::BLA_W_LP:
	case H265NALUnitType::BLA_W_RADL:
	case H265NALUnitType::BLA_N_LP:
	case H265NALUnitType::IDR_W_RADL:
	case H265NALUnitType::IDR_N_LP:
	case H265NALUnitType::CRA_NUT:
		return true;
	default:
		break;
	}
	return false;
}

bool H265NALHeader::isIRAP() const
{
	switch (nal_unit_type) {
	case H265NALUnitType::BLA_W_LP:
	case H265NALUnitType::BLA_W_RADL:
	case H265NALUnitType::BLA_N_LP:
	case H265NALUnitType::IDR_W_RADL:
	case H265NALUnitType::IDR_N_LP:
	case H265NALUnitType::CRA_NUT:
		return true;
	default:
		break;
	}
	return false;
}

bool H265NALHeader::isIDR() const
{
	switch(nal_unit_type){
	case H265NALUnitType::IDR_N_LP:
	case H265NALUnitType::IDR_W_RADL:
		return true;
	default:
		break;
	}
	return false;
}

bool H265NALHeader::isTSA() const
{
	switch (nal_unit_type) {
	case H265NALUnitType::TSA_N:
	case H265NALUnitType::TSA_R:
		return true;
	default:
		break;
	}
	return false;
}

bool H265NALHeader::isSTSA() const
{
	switch (nal_unit_type) {
	case H265NALUnitType::STSA_N:
	case H265NALUnitType::STSA_R:
		return true;
	default:
		break;
	}
	return false;
}

void H265NALHeader::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("NAL Unit Header");
	dumpObject.addUnitField("forbidden_zero_bit", forbidden_zero_bit);
	dumpObject.addUnitField("nal_unit_type", nal_unit_type);
	dumpObject.addUnitField("nuh_layer_id", nuh_layer_id);
	dumpObject.addUnitField("TemporalId", TemporalId);
	dumpObject.endUnitFieldList();
}

void H265NALHeader::checkErrors(H26XErrors& errors) const
{
	if(forbidden_zero_bit != 0){
		errors.add(H26XError::Minor, "[NAL header] forbidden_zero_bit not equal to 0");
	}
	if(nuh_layer_id > 62){
		errors.add(H26XError::Minor, H26XUtils::formatString("nuh_layer_id value (%ld) not in valid range(0..62)", nuh_layer_id));
	}
	if(nuh_temporal_id_plus1 == 0){
		errors.add(H26XError::Minor, "[NAL header] nuh_temporal_id_plus1 equal to 0");
	}
	if(isIRAP() && TemporalId != 0){
		errors.add(H26XError::Minor, "[NAL header] TemporalId of IRAP picture not equal to 0");
	}else if(TemporalId == 0){
		if(isTSA()){
			errors.add(H26XError::Minor, "[NAL header] TemporalId of TSA picture equal to 0");
		}else if (isSTSA() && nuh_layer_id == 0){
			errors.add(H26XError::Minor, "[NAL header] TemporalId of base layer STSA picture equal to 0");
		}
	} else if(nal_unit_type == H265NALUnitType::VPS || nal_unit_type == H265NALUnitType::SPS){
		errors.add(H26XError::Minor, "[NAL header] TemporalId of VPS/SPS not equal to 0");
	}
}