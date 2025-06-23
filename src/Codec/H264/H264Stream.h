#ifndef TOOLKIT_CODEC_UTILS_H264_STREAM_H_
#define TOOLKIT_CODEC_UTILS_H264_STREAM_H_

#include <vector>
#include <deque>
#include <cstdint>
#include <memory>
#include <list>

#include "../H26X/H26XStream.h"

#include "H264AccessUnit.h"
#include "H264NAL.h"
#include "H264PPS.h"
#include "H264Slice.h"
#include "H264SPS.h"

#define ERR_MSG_LIMIT 8

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
	std::vector<H264AccessUnit*> getAccessUnits() const;

	virtual bool parsePacket(uint8_t* pPacketData, uint32_t iPacketLength) override;
	void lastPacketParsed();


public:
	// Variables derived from bitstream
	uint8_t MbaffFrameFlag;

	std::deque<std::string> minorErrors;
	std::deque<std::string> majorErrors;

private:
	virtual bool parseNAL(uint8_t* pNALData, uint32_t iNALLength) override;

	inline int computeSubWidthC() const;
	inline int computeSubHeightC() const;

	void validateFrameNum(H264Slice* pSlice);
	void newAccessUnit();
	void computeCurrentAccessUnitPOC();
	void computeCurrentAccessUnitRPL();
	void computeRPLPictureNumbers();
	void computeRPLInit();
	std::vector<uint16_t> computeRPLFieldInit(std::vector<H264AccessUnit*> refFrameListXShortTerm, std::vector<H264AccessUnit*> refFrameListLongTerm);
	void markDecodedReferencePictures();

private:
	H264NAL m_currentNAL;
	H264SPS* m_pActiveSPS;
	H264PPS* m_pActivePPS;
	
	std::deque<std::unique_ptr<H264GOP>> m_GOPs;
	H264AccessUnit* m_pCurrentAccessUnit;

};

#endif // TOOLKIT_CODEC_UTILS_H264_STREAM_H_
