//
// Created by ebeuque on 27/11/2020.
//

#include <cstring>

#include "../H26X/H26XStream.h"
#include "H265NAL.h"

#include "H265Utils.h"

bool H265Utils::isKeyFrame(FrameBuffer* pFrameBuffer)
{
	const unsigned char* buf = pFrameBuffer->getData();
	if(buf){
		int nal_unit_type;
		if( !memcmp(buf, g_startCode4Bytes, 4) )
		{
			// To be considered as key frame, the frame must have SPS, PPS and I-Frame data
			// but check only if it start with the SPS
			nal_unit_type = H265_GetNalUnitType(*(buf+4));
			// TkCore::Logger::debug("h265: %d %ld", nal_unit_type, pFrameBuffer->getDataSize());
			switch (nal_unit_type) {
			case H265NAL::UnitType_VPS:
			case H265NAL::UnitType_SPS:
			case H265NAL::UnitType_PPS:
			//case H265NAL::UnitType_SEI_PREFIX:
			//case H265NAL::UnitType_SEI_SUFFIX:
			case H265NAL::UnitType_IDR_W_RADL:
			case H265NAL::UnitType_IDR_N_LP:
				return true;
			}
		}
	}
	return false;
}
