#ifndef TOOLKIT_CODEC_UTILS_H265NAL_H_
#define TOOLKIT_CODEC_UTILS_H265NAL_H_

#include <cstdint>
#include <string>
#include <vector>

class UnitFieldList;

struct H265NAL {
	enum UnitType {
		UnitType_TRAIL_N     = 0,
		UnitType_TRAIL_R     = 1,
		UnitType_TSA_N       = 2,
		UnitType_TSA_R       = 3,
		UnitType_STSA_N      = 4,
		UnitType_STSA_R      = 5,
		UnitType_RADL_N      = 6,
		UnitType_RADL_R      = 7,
		UnitType_RASL_N      = 8,
		UnitType_RASL_R      = 9,
		UnitType_BLA_W_LP    = 16,
		UnitType_BLA_W_RADL  = 17,
		UnitType_BLA_N_LP    = 18,
		UnitType_IDR_W_RADL  = 19,
		UnitType_IDR_N_LP    = 20,
		UnitType_CRA_NUT     = 21,
		UnitType_IRAP_VCL23  = 23,
		UnitType_VPS         = 32,
		UnitType_SPS         = 33,
		UnitType_PPS         = 34,
		UnitType_AUD         = 35,
		UnitType_EOS_NUT     = 36,
		UnitType_EOB_NUT     = 37,
		UnitType_FD_NUT      = 38,
		UnitType_SEI_PREFIX  = 39,
		UnitType_SEI_SUFFIX  = 40,
		UnitType_Unspecified = 0xFF
	};

	H265NAL();
	H265NAL(uint8_t forbiddenZeroBit, UnitType nalUnitType, uint8_t nuhLayerId, uint8_t nuhTemporalIdPlus1, uint32_t nalSize, uint8_t* nalData);
	virtual ~H265NAL();

	uint8_t forbidden_zero_bit;
	UnitType nal_unit_type;
	uint8_t nuh_layer_id;
	uint8_t nuh_temporal_id_plus1;

	uint8_t TemporalId;

	uint32_t nal_size;
	uint8_t* nal_data;

	bool completelyParsed;

	std::vector<std::string> minorErrors;
	std::vector<std::string> majorErrors;

	bool isSlice() const;
	static bool isSlice(UnitType nal_unit_type);
	bool isIRAP() const;
	bool isIDR() const;
	bool isTSA() const;
	bool isSTSA() const;
	virtual UnitFieldList dump_fields();
	virtual void validate();
};

#endif // TOOLKIT_CODEC_UTILS_H265NAL_H_
