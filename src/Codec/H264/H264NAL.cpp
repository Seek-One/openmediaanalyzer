#include <iostream>
#include <cstring>

#include "../H26X/H26XUtils.h"

#include "H264NAL.h"

H264NAL::H264NAL(H264NALHeader* pNALHeader, uint32_t nalSize, const uint8_t* nalData)
	: H26XNAL(pNALHeader), nal_size(nalSize+3), nal_data(nullptr)
{
	if(nalData == nullptr){
		return;
	}
	nal_data = new uint8_t[nal_size];
	std::memcpy(nal_data, g_startCode3Bytes, 3);
	std::memcpy(nal_data+3, nalData, nalSize);
}

H264NAL::~H264NAL(){
	if(nal_data){
		delete[] nal_data;
	}
}

void H264NAL::dump(H26XDumpObject& dumpObject) const
{
	H26XNAL::dump(dumpObject);
	if(m_pNALHeader){
		m_pNALHeader->dump(dumpObject);
	}
}

void H264NAL::validate()
{
	H26XNAL::validate();
	if(m_pNALHeader){
		m_pNALHeader->checkErrors(errors);
	}
}
