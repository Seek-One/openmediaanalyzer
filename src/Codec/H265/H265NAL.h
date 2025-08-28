#ifndef TOOLKIT_CODEC_UTILS_H265NAL_H_
#define TOOLKIT_CODEC_UTILS_H265NAL_H_

#include "../H26X/H26XNAL.h"
#include "H265NALUnitType.h"

struct H265NAL : public H26XNAL
{
	H265NAL();
	H265NAL(uint8_t forbiddenZeroBit, H265NALUnitType::Type nalUnitType, uint8_t nuhLayerId, uint8_t nuhTemporalIdPlus1, uint32_t nalSize, const uint8_t* nalData);
	virtual ~H265NAL();

	uint8_t forbidden_zero_bit;
	H265NALUnitType::Type nal_unit_type;
	uint8_t nuh_layer_id;
	uint8_t nuh_temporal_id_plus1;

	uint8_t TemporalId;

	uint32_t nal_size;
	uint8_t* nal_data;

	bool isSlice() const;
	static bool isSlice(H265NALUnitType::Type nal_unit_type);
	bool isIRAP() const;
	bool isIDR() const;
	bool isTSA() const;
	bool isSTSA() const;

	void dump(H26XDumpObject& dumpObject) const override;
	void validate() override;
};

#endif // TOOLKIT_CODEC_UTILS_H265NAL_H_
