#ifndef TOOLKIT_CODEC_UTILS_H26X_STREAM_H_
#define TOOLKIT_CODEC_UTILS_H26X_STREAM_H_

#include "H26XTypes.h"

struct NALData {
	const uint8_t* pData;
	size_t iLength;
};

class H26XStream {
public:
	H26XStream();
	virtual ~H26XStream() = default;

	// Extract all information from NAL units (maybe slow)
	virtual bool parsePacket(const uint8_t* pPacketData, uint32_t iPacketLength) = 0;

	// From the packet data, this function find all NAL in the current packet.
	// The returned list contains all NAL without startcode
	H26XVector<NALData> splitNAL(const uint8_t* pPacketData, uint32_t iPacketLength);

protected:
	virtual bool parseNAL(const uint8_t* pNALData, uint32_t iNALLength) = 0;

private:
	bool seekStartCode(uint64_t& iDataIndex);

private:
	const uint8_t* m_pPacketData;
	uint64_t m_iPacketLength;
};

#endif // TOOLKIT_CODEC_UTILS_H26X_STREAM_H_
