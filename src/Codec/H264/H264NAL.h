#ifndef TOOLKIT_CODEC_UTILS_H264NAL_H_
#define TOOLKIT_CODEC_UTILS_H264NAL_H_

#include "../H26X/H26XNAL.h"
#include "H264NALUnitType.h"

struct H264NAL : public H26XNAL
{
	H264NAL();
	H264NAL(uint8_t forbiddenZeroBit, uint8_t nalRefIdc, uint32_t nalSize, const uint8_t* nalData);
	virtual ~H264NAL();

	uint8_t forbidden_zero_bit;
	uint8_t nal_ref_idc;
	H264NALUnitType::Type nal_unit_type;
	uint32_t nal_size;
	uint8_t* nal_data;

	virtual void dump(H26XDumpObject& dumpObject) const override;
	virtual void validate() override;
};

#endif // TOOLKIT_CODEC_UTILS_H264NAL_H_
