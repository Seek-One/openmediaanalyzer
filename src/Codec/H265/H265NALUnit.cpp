#include "../H26X/H26XUtils.h"

#include "H265NALUnit.h"

H265NALUnit::H265NALUnit(H265NALHeader* pNALHeader, uint32_t nalSize, const uint8_t* nalData)
	: H26XNALUnit(pNALHeader), nal_size(nalSize + 3), nal_data(nullptr)
{
	if(nalData == nullptr){
		return;
	}
	nal_data = new uint8_t[nal_size];
	std::memcpy(nal_data, g_startCode3Bytes, 3);
	std::memcpy(nal_data+3, nalData, nalSize);
}

H265NALUnit::~H265NALUnit()
{
	if(nal_data){
		delete[] nal_data;
	}
}

void H265NALUnit::dump(H26XDumpObject& dumpObject) const
{
	H26XNALUnit::dump(dumpObject);
	if(m_pNALHeader){
		m_pNALHeader->dump(dumpObject);
	}
}

void H265NALUnit::validate()
{
	H26XNALUnit::validate();
	if(m_pNALHeader){
		m_pNALHeader->checkErrors(errors);
	}
}
