#ifndef TOOLKIT_CODEC_UTILS_H26X_UTILS_H_
#define TOOLKIT_CODEC_UTILS_H26X_UTILS_H_

#include <string>

#include "H26XTypes.h"

extern const uint8_t g_startCode3Bytes[3];
extern const uint8_t g_startCode4Bytes[4];

extern const uint8_t g_rasterScan4x4[16+1];
extern const uint8_t g_rasterScan8x8[64];

class H26XUtils {
public:
	static std::string formatString(const char* fmt, ...);
};

#endif // TOOLKIT_CODEC_UTILS_H26X_UTILS_H_
