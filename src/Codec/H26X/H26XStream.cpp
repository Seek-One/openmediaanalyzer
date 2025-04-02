#include <cstring>

#include "H26XStream.h"
#include "H26XUtils.h"

H26XStream::H26XStream()
{
	m_pPacketData = NULL;
	m_iPacketLength = 0;
}

std::vector<NALData> H26XStream::splitNAL(uint8_t* pPacketData, uint32_t iPacketLength)
{
	std::vector<NALData> listNAL;
	m_pPacketData = pPacketData;
	m_iPacketLength = iPacketLength;


	// Find next startcode or reach packet end
	bool bEndPacket = false;
	do {
		// Skip the startcode at the NAL begin
		if (memcmp(g_startCode3Bytes, m_pPacketData, 3) == 0) {
			m_pPacketData += 3;
			m_iPacketLength -= 3;
		} else if (memcmp(g_startCode4Bytes, m_pPacketData, 4) == 0) {
			m_pPacketData += 4;
			m_iPacketLength -= 4;
		}

		uint64_t iDataIndex = 0;
		bEndPacket = !seekStartCode(iDataIndex);

		// When no startcode was found, we consider that all remaining data are the NAL
		if (bEndPacket) {
			NALData nalData;
			nalData.pData = m_pPacketData;
			nalData.iLength = m_iPacketLength;
			listNAL.push_back(nalData);
		} else {
			NALData nalData;
			nalData.pData = m_pPacketData;
			nalData.iLength = iDataIndex;
			listNAL.push_back(nalData);

			m_pPacketData += iDataIndex;
			m_iPacketLength -= iDataIndex;
		}
	} while (!bEndPacket);

	m_pPacketData = NULL;
	m_iPacketLength = 0;

	return listNAL;
}

bool H26XStream::seekStartCode(uint64_t& iDataIndex)
{
	uint64_t iLocalIndex = iDataIndex;
	while (iLocalIndex + 3 < m_iPacketLength) {
		if (m_pPacketData[iLocalIndex] == 0x00 && m_pPacketData[iLocalIndex + 1] == 0x00 && m_pPacketData[iLocalIndex + 2] == 0x01) {
			iDataIndex = iLocalIndex;
			return true;
		}
		++iLocalIndex;
	}

	iDataIndex = iLocalIndex;
	return false;
}
