#ifndef TOOLKIT_CODEC_UTILS_H264_H264NAL_H_
#define TOOLKIT_CODEC_UTILS_H264_H264NAL_H_

#include "../H26X/H26XNALUnit.h"
#include "H264NALHeader.h"

struct H264NALUnit : public H26XNALUnit
{
	H264NALUnit(H264NALHeader* pNALHeader, uint32_t nalSize, const uint8_t* nalData);
	virtual ~H264NALUnit();

	uint32_t nal_size;
	uint8_t* nal_data;

	inline H264NALHeader* getNALHeader() const
	{
		return (H264NALHeader*)m_pNALHeader;
	}

	inline H264NALUnitType::Type getNalUnitType() const
	{
		auto pH264NALHeader = getNALHeader();
		if(pH264NALHeader){
			return pH264NALHeader->nal_unit_type;
		}
		return H264NALUnitType::Unspecified;
	}

	virtual void dump(H26XDumpObject& dumpObject) const override;
	virtual void validate() override;
};

#endif // TOOLKIT_CODEC_UTILS_H264_H264NAL_H_
