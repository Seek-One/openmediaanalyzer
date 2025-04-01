#ifndef TOOLKIT_CODEC_UTILS_H264_BITSTREAM_READER_H_
#define TOOLKIT_CODEC_UTILS_H264_BITSTREAM_READER_H_

#include <cstdint>

#include "H264SEI.h"

#include "../H26X/H26XBitstreamReader.h"

struct H264NAL;
struct H264PPS;
struct H264Slice;
struct H264SPS2;
struct H264AUD;


class H264BitstreamReader: public H26XBitstreamReader {
public:
	H264BitstreamReader(const uint8_t* pNALData, uint32_t iNALLength);

	void readNALHeader(H264NAL& h264NAL);
	void readSPS(H264SPS2& h264SPS);
	void readPPS(H264PPS& h264PPS);
	void readSlice(H264Slice& h264Slice);
	void readAUD(H264AUD& h264AUD);
	void readSEI(H264SEI& h264SEI, const H264SPS2& activeSPS);

private:
	void readScalingList(uint8_t* pScalingList, uint8_t iScalingListSize, bool bUseDefaultScalingList);
	void readRefPicListReordering(H264Slice& h264Slice);
	void readPredWeightTable(const H264SPS2& h264SPS, const H264PPS& h264PPS, H264Slice& h264Slice);
	void readDecRefPicMarking(H264Slice& h264Slice);
	void readSEIBufferingPeriod(H264SEI& h264SEI);
	void readSEIPicTiming(H264SEI& h264SEI, const H264SPS2& activeSPS);
	void readSEIFullFrameFreeze(H264SEI& h264SEI);
	void readSEIMvcdViewScalabilityInfo(H264SEI& h264SEI, const H264SPS2& activeSPS);
	void readSEIMvcdOpViewInfo(H264SEIMvcdViewScalabilityInfo::movi& movi);
	void readSEIFillerPayload(H264SEI& h264SEI, uint payloadSize);
    void readSEIRecoveryPoint(H264SEI& h264SEI, const H264SPS2 &activeSPS);
};

#endif // TOOLKIT_CODEC_UTILS_H264_BITSTREAM_READER_H_
