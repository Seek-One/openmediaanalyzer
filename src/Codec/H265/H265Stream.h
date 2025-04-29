#ifndef TOOLKIT_CODEC_UTILS_H265_STREAM_H_
#define TOOLKIT_CODEC_UTILS_H265_STREAM_H_

#include <vector>
#include <list>
#include <deque>
#include <memory>

#include "../H26X/H26XStream.h"
#include "../Size.h"

#include "H265NAL.h"
#include "H265PPS.h"
#include "H265Slice.h"
#include "H265SPS.h"
#include "H265VPS.h"
#include "H265GOP.h"

class H265Stream: public H26XStream {
public:
	H265Stream();
	~H265Stream();

	std::deque<H265GOP*> getGOPs() const;
	uint32_t popFrontGOPs(uint32_t count);
	uint32_t accessUnitCount() const;
	std::list<H265AccessUnit*> getLastAccessUnits(uint32_t count) const;

	// Size without cropping
	Size getUncroppedPictureSize() const;
	// Real picture size
	Size getPictureSize() const;

	virtual bool parsePacket(uint8_t* pPacketData, uint32_t iPacketLength);

	// Extract all necessary informations for the decoding process
	void startDecodingProcess();

	void computeRef();
	void computePOC();
	void computeRPS();

public:
	bool firstPicture;
	bool endOfSequenceFlag;

private:
	virtual bool parseNAL(uint8_t* pNALData, uint32_t iNALLength);

private:
	H265NAL m_currentNAL;
	H265VPS* m_pActiveVPS;
	H265SPS* m_pActiveSPS;
	H265PPS* m_pActivePPS;

	std::deque<std::unique_ptr<H265GOP>> m_GOPs;
	H265AccessUnit* m_pCurrentAccessUnit;
};

#endif // TOOLKIT_CODEC_UTILS_H265_STREAM_H_
