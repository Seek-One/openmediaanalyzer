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
#include "H265SEI.h"

class H265BitstreamReader: public H26XBitstreamReader {
public:
	H265BitstreamReader(uint8_t* pNALData, uint32_t iNALLength);

	void readNALHeader(H265NAL& h265NAL);
	void readVPS(H265VPS& h265VPS);
	void readSPS(H265SPS& h265SPS);
	void readPPS(H265PPS& h265PPS);
	void readSlice(H265Slice& h265Slice);
	void readSEI(H265SEI& h265SEI);

private:
	H265ProfileTierLevel readProfileTierLevel(uint8_t iProfilePresentFlag, uint8_t iMaxNumSubLayersMinus1);
	H265ScalingList readScalingList();
	H265ShortTermRefPicSet readShortTermRefPicSet(uint32_t iShortTermSetIndex, const H265SPS& h265SPS);
	H265HrdParameters readHrdParameters(uint8_t commonInfPresentFlag, uint8_t maxNumSubLayersMinus1);
	H265VuiParameters readVuiParameters(uint8_t sps_max_sub_layers_minus1);
	H265SPSRangeExtension readSPSRangeExtension();
	H265SPSMultilayerExtension readSPSMultilayerExtension();
	H265SPS3DExtension readSPS3DExtension(const H265SPS& h265SPS);
	H265SPSSCCExtension readSPSSCCExtension(const H265SPS& h265SPS);
	H265PredWeightTable readSlicePredWeightTable(const H265Slice& h265Slice);
	void computeNumPicTotalCurr(H265Slice& h265Slice, const H265SPS& h265SPS);

	void readRefPicListsModification(H265Slice& h265Slice);
};

#endif // TOOLKIT_CODEC_UTILS_H265_BITSTREAM_READER_H_
