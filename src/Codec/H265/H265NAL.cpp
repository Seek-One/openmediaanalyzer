#include <cstring>
#include <sstream>

#include "H265Utils.h"

#include "H265NAL.h"

H265NAL::H265NAL():
	H265NAL(0, UnitType_Unspecified, 0, 0, 0, nullptr)
{}

H265NAL::H265NAL(uint8_t forbiddenZeroBit, UnitType nalUnitType, uint8_t nuhLayerId, uint8_t nuhTemporalIdPlus1, uint32_t nalSize, uint8_t* nalData):
	forbidden_zero_bit(forbiddenZeroBit), nal_unit_type(nalUnitType), nuh_layer_id(nuhLayerId), nuh_temporal_id_plus1(nuhTemporalIdPlus1), 
	TemporalId(nuh_temporal_id_plus1-1), nal_size(nalSize+3), nal_data(nullptr)
{
	if(nalData == nullptr) return;
	nal_data = new uint8_t[nal_size];
	std::memcpy(nal_data, g_startCode3Bytes, 3);
	std::memcpy(nal_data+3, nalData, nalSize);
}

H265NAL::~H265NAL(){
	if(nal_data) delete[] nal_data;
}

bool H265NAL::isSlice() const
{
	return H265NAL::isSlice(nal_unit_type);
}

bool H265NAL::isSlice(UnitType nal_unit_type)
{
	switch (nal_unit_type) {
		case H265NAL::UnitType_TRAIL_N:
		case H265NAL::UnitType_TRAIL_R:
		case H265NAL::UnitType_TSA_N:
		case H265NAL::UnitType_TSA_R:
		case H265NAL::UnitType_STSA_N:
		case H265NAL::UnitType_STSA_R:
		case H265NAL::UnitType_RADL_N:
		case H265NAL::UnitType_RADL_R:
		case H265NAL::UnitType_RASL_N:
		case H265NAL::UnitType_RASL_R:
		case H265NAL::UnitType_BLA_W_LP:
		case H265NAL::UnitType_BLA_W_RADL:
		case H265NAL::UnitType_BLA_N_LP:
		case H265NAL::UnitType_IDR_W_RADL:
		case H265NAL::UnitType_IDR_N_LP:
		case H265NAL::UnitType_CRA_NUT:
			return true;
		default:
			break;
	}
	return false;
}

bool H265NAL::isIRAP() const{
	switch (nal_unit_type) {
		case H265NAL::UnitType_BLA_W_LP:
		case H265NAL::UnitType_BLA_W_RADL:
		case H265NAL::UnitType_BLA_N_LP:
		case H265NAL::UnitType_IDR_W_RADL:
		case H265NAL::UnitType_IDR_N_LP:
		case H265NAL::UnitType_CRA_NUT:
			return true;
	
		default:
			break;
	}
	return false;
}

bool H265NAL::isTSA() const{
	switch (nal_unit_type) {
		case H265NAL::UnitType_TSA_N:
		case H265NAL::UnitType_TSA_R:
			return true;
		default:
			break;
	}
	return false;
}

bool H265NAL::isSTSA() const{
	switch (nal_unit_type) {
		case H265NAL::UnitType_STSA_N:
		case H265NAL::UnitType_STSA_R:
			return true;
		default:
			break;
	}
	return false;
}

std::vector<std::string> H265NAL::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "nuh_layer_id:" << (int)nuh_layer_id).str());
	fields.push_back((std::ostringstream() << "TemporalId:" << (int)TemporalId).str());
	return fields;
}

void H265NAL::validate(){
	if(forbidden_zero_bit != 0) errors.push_back("[NAL header] forbidden_zero_bit not equal to 0");
	if(nuh_layer_id > 62) errors.push_back((std::ostringstream() << "nuh_layer_id value (" << (int)nuh_layer_id <<") not in valid range(0..62)").str());
	if(nuh_temporal_id_plus1 == 0) errors.push_back("[NAL header] nuh_temporal_id_plus1 equal to 0");
	if(isIRAP() && TemporalId != 0) errors.push_back("[NAL header] TemporalId of IRAP picture not equal to 0");
	else if(TemporalId == 0){
		if(isTSA()) errors.push_back("[NAL header] TemporalId of TSA picture equal to 0");
		else if (isSTSA() && nuh_layer_id == 0) errors.push_back("[NAL header] TemporalId of base layer STSA picture equal to 0");
	} else if(nal_unit_type == UnitType::UnitType_VPS || nal_unit_type == UnitType_SPS) errors.push_back("[NAL header] TemporalId of VPS/SPS not equal to 0");
}