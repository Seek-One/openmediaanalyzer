//
// Created by ebeuque on 28/08/25.
//

#ifndef TOOLKIT_CODEC_UTILS_H264_H264NALUNITTYPE_H
#define TOOLKIT_CODEC_UTILS_H264_H264NALUNITTYPE_H

class H264NALUnitType
{
public:
	enum Type {
		Unspecified = 0x00,
		NonIDRFrame = 0x01, // Non-IDR Frame
		FramePartitionA = 0x02, // Frame with type A partition
		FramePartitionB = 0x03, // Frame with type B partition
		FramePartitionC = 0x04, // Frame with type C partition
		IDRFrame = 0x05, // IDR Frame
		SEI = 0x06, // Supplemental enhancement information
		SPS = 0x07, // Sequence parameter set
		PPS = 0x08, // Picture parameter set
		AUD = 0x09, // Access unit delimiter
		EOSeq = 0x0a, // End of Sequence
		EOStr = 0x0b, // End of Stream
		Filler = 0x0c, // Filler
		SPSExt = 0x0d, // SPS extention
		PrefixNAL = 0x0e, // Prefix NAL
		SubsetSPS = 0x0f, // SPS subset
		DPS = 0x10, // Depth parameter set
		AUX = 0x13, // Auxiliary coded frame without partitioning
		SVCExt = 0x14, // Scalable Video Coding Extention
		Slice3D = 0x15, // Coded frame extension for depth or 3D-AVC
	};
};

#endif //TOOLKIT_CODEC_UTILS_H264_H264NALUNITTYPE_H
