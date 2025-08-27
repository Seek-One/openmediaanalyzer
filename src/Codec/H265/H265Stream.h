#ifndef TOOLKIT_CODEC_UTILS_H265_STREAM_H_
#define TOOLKIT_CODEC_UTILS_H265_STREAM_H_

#include <vector>
#include <list>
#include <deque>
#include <memory>

#include "../H26X/H26XStream.h"

#include "H265NAL.h"

struct H265PPS;
struct H265Slice;
struct H265SPS;
struct H265VPS;
struct H265GOP;
struct H265AccessUnit;

class H265Stream: public H26XStream {
public:
	H265Stream();
	~H265Stream();

	std::deque<H265GOP*> getGOPs() const;
	uint32_t popFrontGOPs(uint32_t count);
	uint32_t accessUnitCount() const;
	std::list<H265AccessUnit*> getLastAccessUnits(uint32_t count) const;
	std::vector<H265AccessUnit*> getAccessUnits() const;

	bool parsePacket(const uint8_t* pPacketData, uint32_t iPacketLength) override;
	void lastPacketParsed();

	// Extract all necessary information for the decoding process
	void startDecodingProcess();

public:
	bool firstPicture;
	bool endOfSequenceFlag;

	H26XErrors errors;

private:
	bool parseNAL(const uint8_t* pNALData, uint32_t iNALLength) override;

private:
	H265NAL m_currentNAL;
	H265VPS* m_pActiveVPS;
	H265SPS* m_pActiveSPS;
	H265PPS* m_pActivePPS;

	std::deque<std::unique_ptr<H265GOP>> m_GOPs;
	H265AccessUnit* m_pCurrentAccessUnit;
	H265AccessUnit* m_pNextAccessUnit;

	void checkPrevRefPicList(H265AccessUnit* pAccessUnit, H265Slice* pSlice);
	void newAccessUnit();
};

#endif // TOOLKIT_CODEC_UTILS_H265_STREAM_H_
