#ifndef TOOLKIT_CODEC_UTILS_H264_BITSTREAM_READER_H_
#define TOOLKIT_CODEC_UTILS_H264_BITSTREAM_READER_H_

#include <cstdint>

#include "../H26X/H26XBitstreamReader.h"

struct H264NALHeader;
struct H264NAL;
struct H264PPS;
struct H264Slice;
struct H264SPS;
struct H264AUD;
struct H264SEI;


class H264BitstreamReader: public H26XBitstreamReader {
public:
	H264BitstreamReader(const uint8_t* pNALData, uint32_t iNALLength);

	void readNALHeader(H264NALHeader& h264NALHeader);
	void readSPS(H264SPS& h264SPS);
	void readPPS(H264PPS& h264PPS);
	void readSlice(H264Slice& h264Slice);
	void readAUD(H264AUD& h264AUD);
	void readSEI(H264SEI& h264SEI, const H264SPS* activeSPS);

private:
	void readScalingList(uint8_t* pScalingList, uint8_t iScalingListSize, bool bUseDefaultScalingList);
	void readRefPicListReordering(H264Slice& h264Slice);
	void readPredWeightTable(const H264SPS& h264SPS, const H264PPS& h264PPS, H264Slice& h264Slice);
	void readDecRefPicMarking(H264Slice& h264Slice);
	void readSEIBufferingPeriod(H264SEI& h264SEI);
	void readSEIPicTiming(H264SEI& h264SEI, const H264SPS& activeSPS);
	void readSEIFillerPayload(H264SEI& h264SEI, uint64_t payloadSize);
	void readSEIUserDataUnregistered(H264SEI& h264SEI, uint64_t payloadSize);
    void readSEIRecoveryPoint(H264SEI& h264SEI, const H264SPS &activeSPS);
	void readSEIFullFrameFreeze(H264SEI& h264SEI);
};

#endif // TOOLKIT_CODEC_UTILS_H264_BITSTREAM_READER_H_
