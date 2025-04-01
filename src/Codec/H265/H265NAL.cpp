#include "H265NAL.h"

H265NAL::H265NAL()
{
	forbidden_zero_bit = 0;
	nal_unit_type = UnitType_Unspecified;
	nuh_layer_id = 0;
	nuh_temporal_id_plus1 = 0;
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
