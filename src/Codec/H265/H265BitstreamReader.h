#ifndef TOOLKIT_CODEC_UTILS_H265_BITSTREAM_READER_H_
#define TOOLKIT_CODEC_UTILS_H265_BITSTREAM_READER_H_

#include <cstdint>

#include "../H26X/H26XBitstreamReader.h"

#include "H265NAL.h"
#include "H265PPS.h"
#include "H265ScalingList.h"
#include "H265Slice.h"
#include "H265SPS.h"
#include "H265VPS.h"

class H265BitstreamReader: public H26XBitstreamReader {
public:
	H265BitstreamReader(uint8_t* pNALData, uint32_t iNALLength);

	void readNALHeader(H265NAL& h265NAL);
	void readVPS(H265VPS& h265VPS);
	void readSPS(H265SPS& h265SPS);
	void readPPS(H265PPS& h265PPS);
	void readSlice(H265Slice& h265Slice, const H265NAL& h265NAL, H265SPS& h265SPS, const H265PPS& h265PPS);

private:
	H265ProfileTierLevel readProfileTierLevel(uint8_t iProfilePresentFlag, uint8_t iMaxNumSubLayersMinus1);
	H265ScalingList readScalingList();
	H265ShortTermRefPicSet readShortTermRefPicSet(uint32_t iShortTermSetIndex, const H265SPS& h265SPS);
	void computeNumPicTotalCurr(H265Slice& h265Slice, const H265SPS& h265SPS);

	void readRefPicListsModification(H265Slice& h265Slice);
};

#endif // TOOLKIT_CODEC_UTILS_H265_BITSTREAM_READER_H_
