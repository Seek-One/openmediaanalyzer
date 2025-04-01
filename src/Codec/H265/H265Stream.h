#ifndef TOOLKIT_CODEC_UTILS_H265_STREAM_H_
#define TOOLKIT_CODEC_UTILS_H265_STREAM_H_

#include <vector>
#include <list>

#include "../H26X/H26XStream.h"
#include "../Size.h"

#include "H265NAL.h"
#include "H265PPS.h"
#include "H265Slice.h"
#include "H265SPS.h"
#include "H265VPS.h"

class H265Stream: public H26XStream {
public:
	H265Stream();

	// Size without cropping
	Size getUncroppedPictureSize() const;
	// Real picture size
	Size getPictureSize() const;

	const H265NAL& getNAL() const;
	const H265VPS& getVPS() const;
	const H265SPS& getSPS() const;
	const H265PPS& getPPS() const;
	const std::vector<H265Slice>& getSliceList() const;

	virtual bool parsePacket(const uint8_t* pPacketData, uint32_t iPacketLength);

	// Extract all necessary informations for the decoding process
	void startDecodingProcess();

	void computeRef();

public:
	// Variable to handle the decoding process 8
	uint8_t TargetDecLayerIdList; // 8.1.2
	uint8_t HighestTid; // 8.1.2
	bool SubPicHrdFlag; // 8.1.2
	bool IRAPPicture; // 8.1.3
	bool bIsFirstPicture; // 8.1.3
	bool NoRaslOutputFlag; // 8.1.3
	bool HandleCraAsBlaFlag; // 8.1.3
	uint32_t PicOrderCntVal; // 8.3.1
	uint32_t prevPicOrderCntLsb; // 8.3.1
	uint32_t prevPicOrderCntMsb; // 8.3.1
	uint32_t MaxPicOrderCntLsb; // 8.3.2
	Int32Vector PocStCurrBefore; // 8.3.2
	Int32Vector PocStCurrAfter; // 8.3.2
	Int32Vector PocStFoll; // 8.3.2
	Int32Vector PocLtCurr; // 8.3.2
	Int32Vector PocLtFoll; // 8.3.2
	UInt8Vector CurrDeltaPocMsbPresentFlag; // 8.3.2
	UInt8Vector FollDeltaPocMsbPresentFlag; // 8.3.2

	// Names taken from HEVC ref to be more easier to adapt reference process
	// This list store the frame index from m_listFrames

	// Short-term picture lists
	std::vector<int> RefPicSetStCurrBefore;
	std::vector<int> RefPicSetStCurrAfter;
	std::list<int> RefPicSetStFoll;

	// Long-term picture lists
	std::vector<int> RefPicSetLtCurr;
	std::list<int> RefPicSetLtFoll;

	// Reference picture lists
	std::vector<int> RefPicList0; // 8.3.4 (For P-slice)
	std::vector<int> RefPicList1; // 8.3.4 (For B-slice)
	std::vector<int> RefPicListTemp0;
	std::vector<int> RefPicListTemp1;

private:
	virtual bool parseNAL(const uint8_t* pNALData, uint32_t iNALLength);

private:
	H265NAL m_currentNAL;
	H265VPS m_vps;
	H265SPS m_sps;
	H265PPS m_pps;
	std::vector<H265Slice> m_listSlices;
};

#endif // TOOLKIT_CODEC_UTILS_H265_STREAM_H_
