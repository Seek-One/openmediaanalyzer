#ifndef TOOLKIT_CODEC_UTILS_H265_H265NAL_H_
#define TOOLKIT_CODEC_UTILS_H265_H265NAL_H_

#include "../H26X/H26XNALUnit.h"
#include "H265NALHeader.h"

struct H265NALUnit : public H26XNALUnit
{
	H265NALUnit(H265NALHeader* pNALHeader, uint32_t nalSize, const uint8_t* nalData);
	virtual ~H265NALUnit();

	uint32_t nal_size;
	uint8_t* nal_data;

	inline H265NALHeader* getNALHeader() const
	{
		return (H265NALHeader*)m_pNALHeader;
	}

	inline H265NALUnitType::Type getNalUnitType() const
	{
		auto pH265NALHeader = getNALHeader();
		if(pH265NALHeader){
			return pH265NALHeader->nal_unit_type;
		}
		return H265NALUnitType::Unspecified;
	}

	void dump(H26XDumpObject& dumpObject) const override;
	void validate() override;
};

#endif // TOOLKIT_CODEC_UTILS_H265_H265NAL_H_
