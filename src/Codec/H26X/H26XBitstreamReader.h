#ifndef TOOLKIT_CODEC_UTILS_H26X_BITSTREAM_READER_H_
#define TOOLKIT_CODEC_UTILS_H26X_BITSTREAM_READER_H_

#include <cstdint>

#define END_OF_STREAM_ERR_MSG "Prematurely reached end of bitstream during parsing"

class H26XBitstreamReader {
public:
	H26XBitstreamReader(uint8_t* pNALData, uint32_t iNALLength);

protected:
	uint32_t readBits(uint8_t iNumberBits);
    void skipBits(uint8_t iNumberBits);
    uint8_t readBit();
    uint32_t readGolombUE();
	int32_t readGolombSE();
	bool hasMoreRBSPData();

protected:
	const uint8_t* m_pNALData;
	int64_t m_iRemainingBits;
	uint32_t m_iBitsOffset;
};

#endif // TOOLKIT_CODEC_UTILS_H26X_BITSTREAM_READER_H_
