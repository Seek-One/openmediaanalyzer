//
// Created by ebeuque on 27/11/2020.
//

#ifndef TOOLKIT_CODEC_H265UTILS_H
#define TOOLKIT_CODEC_H265UTILS_H

#include "../H26X/H26XUtils.h"
#include "../FrameBuffer.h"

#define H265_GetNalUnitType(byte) ((byte >> 1) & 0x3F)

class H265Utils
{
public:
	// Check if this frame buffer can be considered as key frame (not just I-Frame)
	static bool isKeyFrame(FrameBuffer* pFrameBuffer);
};


#endif //TOOLKIT_CODEC_H265UTILS_H
