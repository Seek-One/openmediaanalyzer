//
// Created by ebeuque on 28/08/25.
//

#ifndef TOOLKIT_CODEC_UTILS_H265_H265NALHEADER_H
#define TOOLKIT_CODEC_UTILS_H265_H265NALHEADER_H

#include "../H26X/H26XTypes.h"
#include "../H26X/H26XNALHeader.h"

#include "H265NALUnitType.h"

struct H265NALHeader : public H26XNALHeader
{
	H265NALHeader();

	uint8_t forbidden_zero_bit;
	H265NALUnitType::Type nal_unit_type;
	uint8_t nuh_layer_id;
	uint8_t nuh_temporal_id_plus1;

	uint8_t TemporalId;

	void setup();

	bool isSlice() const;
	static bool isSlice(H265NALUnitType::Type nal_unit_type);
	bool isIRAP() const;
	bool isIDR() const;
	bool isTSA() const;
	bool isSTSA() const;

	void dump(H26XDumpObject& dumpObject) const override;
	void checkErrors(H26XErrors& errors) const override;
};

#endif //TOOLKIT_CODEC_UTILS_H265_H265NALHEADER_H
