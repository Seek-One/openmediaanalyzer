//
// Created by ebeuque on 27/11/2020.
//

#ifndef TOOLKIT_CODEC_H265UTILS_H
#define TOOLKIT_CODEC_H265UTILS_H

#include "../H26X/H26XUtils.h"

#define H265_GetNalUnitType(byte) ((byte >> 1) & 0x3F)

#endif //TOOLKIT_CODEC_H265UTILS_H
