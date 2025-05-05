#ifndef TOOLKIT_CODEC_UTILS_H264_STREAM_H_
#define TOOLKIT_CODEC_UTILS_H264_STREAM_H_

#include <vector>
#include <deque>
#include <cstdint>
#include <memory>
#include <list>

#include "../Size.h"
#include "../H26X/H26XStream.h"

#include "H264AccessUnit.h"
#include "H264NAL.h"
#include "H264PPS.h"
#include "H264Slice.h"
#include "H264SPS.h"

#define ERR_MSG_LIMIT 15

struct H264GOP;

struct PictureOrderCount {
	PictureOrderCount(uint32_t iTopFieldOrderValue, uint32_t iBottomFieldOrderValue);

	uint32_t iTopFieldOrder;
	uint32_t iBottomFieldOrder;
};

class H264Stream: public H26XStream {
public:
	H264Stream();
	~H264Stream();

	const H264NAL& getNAL() const;
	std::deque<H264GOP*> getGOPs() const;
	uint32_t popFrontGOPs(uint32_t count);
	uint32_t accessUnitCount() const;
	std::list<H264AccessUnit*> getLastAccessUnits(uint32_t count) const;

	PictureOrderCount computePOC();

	// Size in macroblocks
	const Size& getMbsPictureSize() const;
	// Size without cropping
	const Size& getUncroppedPictureSize() const;
	// Real picture size
	const Size& getPictureSize() const;

	virtual bool parsePacket(uint8_t* pPacketData, uint32_t iPacketLength) override;


public:
	// Variables derived from bitstream
	uint8_t MbaffFrameFlag;

	std::deque<std::string> errors;

private:
	virtual bool parseNAL(uint8_t* pNALData, uint32_t iNALLength) override;

	inline int computeSubWidthC() const;
	inline int computeSubHeightC() const;

	void computeSizes();

	inline PictureOrderCount computePOCType0();
	inline PictureOrderCount computePOCType1();
	inline PictureOrderCount computePOCType2();

	void validateFrameNum(H264Slice* pSlice);

private:
	H264NAL m_currentNAL;
	H264SPS* m_pActiveSPS;
	H264PPS* m_pActivePPS;
	
	std::deque<std::unique_ptr<H264GOP>> m_GOPs;
	H264AccessUnit* m_pCurrentAccessUnit;

	// Useful size
	Size m_sizeInMb; // Size in macro blocks
	Size m_sizeUncropped; // Uncropped size
	Size m_sizeCropped; // Real size (with crop)

	// Picture Order Count
	uint32_t m_prevPicOrderCntMsb;
	uint32_t m_prevPicOrderCntLsb;
	uint32_t m_prevFrameNumOffset;
	uint32_t m_prevFrameNum;
	uint32_t m_iPrevMMCO;
};

#endif // TOOLKIT_CODEC_UTILS_H264_STREAM_H_
