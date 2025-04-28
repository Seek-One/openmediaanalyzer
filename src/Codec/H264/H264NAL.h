#ifndef TOOLKIT_CODEC_UTILS_H264NAL_H_
#define TOOLKIT_CODEC_UTILS_H264NAL_H_

#include <cstdint>
#include <string>
#include <vector>

struct H264NAL {
	enum UnitType {
		UnitType_Unspecified = 0x00,
		UnitType_NonIDRFrame = 0x01, // Non-IDR Frame
		UnitType_FramePartitionA = 0x02, // Frame with type A partition
		UnitType_FramePartitionB = 0x03, // Frame with type B partition
		UnitType_FramePartitionC = 0x04, // Frame with type C partition
		UnitType_IDRFrame = 0x05, // IDR Frame
		UnitType_SEI = 0x06, // Supplemental enhancement information
		UnitType_SPS = 0x07, // Sequence parameter set
		UnitType_PPS = 0x08, // Picture parameter set
		UnitType_AUD = 0x09, // Access unit delimiter
		UnitType_EOSeq = 0x0a, // End of Sequence
		UnitType_EOStr = 0x0b, // End of Stream
		UnitType_Filler = 0x0c, // Filler
		UnitType_SPSExt = 0x0d, // SPS extention
		UnitType_PrefixNAL = 0x0e, // Prefix NAL
		UnitType_SubsetSPS = 0x0f, // SPS subset
		UnitType_DPS = 0x10, // Depth parameter set
		UnitType_AUX = 0x13, // Auxiliary coded frame without partitioning
		UnitType_SVCExt = 0x14, // Scalable Video Coding Extention
		UnitType_3DSlice = 0x15, // Coded frame extension for depth or 3D-AVC
	};

	H264NAL();
	H264NAL(uint8_t forbiddenZeroBit, uint8_t nalRefIdc, uint32_t nalSize, uint8_t* nalData);
	virtual ~H264NAL();

	uint8_t forbidden_zero_bit;
	uint8_t nal_ref_idc;
	UnitType nal_unit_type;
	uint32_t nal_size;
	uint8_t* nal_data;

	std::vector<std::string> errors;

	virtual std::vector<std::string> dump_fields();
	virtual void validate();
};

#endif // TOOLKIT_CODEC_UTILS_H264NAL_H_
