//
// Created by ebeuque on 28/08/25.
//

#ifndef TOOLKIT_CODEC_UTILS_H264_H264NALHEADER_H
#define TOOLKIT_CODEC_UTILS_H264_H264NALHEADER_H

#include "../H26X/H26XTypes.h"
#include "../H26X/H26XNALHeader.h"
#include "H264NALUnitType.h"

struct H264NALHeader : public H26XNALHeader
{
	H264NALHeader();

	uint8_t forbidden_zero_bit;
	uint8_t nal_ref_idc;
	H264NALUnitType::Type nal_unit_type;

	void dump(H26XDumpObject& dumpObject) const override;
	void checkErrors(H26XErrors& errors) const override;
};

#endif //TOOLKIT_CODEC_UTILS_H264_H264NALHEADER_H
