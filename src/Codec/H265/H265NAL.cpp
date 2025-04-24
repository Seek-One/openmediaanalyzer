#include <cstring>

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
		return true;

	default:
		break;
	}

	return false;
}

std::vector<std::string> H265NAL::dump_fields(){
	return std::vector<std::string>();
}
