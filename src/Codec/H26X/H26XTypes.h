//
// Created by ebeuque on 25/08/25.
//

#ifndef TOOLKIT_CODEC_UTILS_H26XTYPES_H
#define TOOLKIT_CODEC_UTILS_H26XTYPES_H

#include <cstdint>
#include <cstring>

#include "H26XVector.h"

#ifdef __cplusplus
extern "C" {
#endif

#define p_memcpy memcpy
#define p_memcmp memcmp
#define p_memset memset
int p_memcmpsi(const void* buffer, short val);

#ifdef __cplusplus
}
#endif

#endif //TOOLKIT_CODEC_UTILS_H26XTYPES_H
