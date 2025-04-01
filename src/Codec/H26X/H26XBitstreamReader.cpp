#include "H26XBitstreamReader.h"

H26XBitstreamReader::H26XBitstreamReader(const uint8_t* pNALData, uint32_t iNALLength)
{
	m_pNALData = pNALData;
	m_iBitsOffset = 0;

	// Remove empty trailing octets
	while (pNALData[iNALLength - 1] == 0) {
		--iNALLength;
	};
	m_iRemainingBits = iNALLength * 8;

	// In last byte, remove final bit to 1 and trailing zero
	int iMask = 0x01;
	int iShfitCount = 1;
	uint8_t iLastByte = pNALData[iNALLength - 1];
	while ((iLastByte & iMask) == 0) {
		iMask = iMask << 1;
		++iShfitCount;
	}
	m_iRemainingBits -= iShfitCount;
}

uint32_t H26XBitstreamReader::readBits(uint8_t iNumberBits)
{
	uint32_t iValue = 0;
	for (int i = 0; i < iNumberBits; i++) {
		iValue = (iValue << 1) | (readBit() ? 1 : 0);
		++m_iBitsOffset;

		// Skip the emulation_prevention_three_byte
		// Could be optimized
		if (m_iBitsOffset >= 24 && (m_iBitsOffset % 8) == 0) {
			int iBytePosition = (m_iBitsOffset / 8);
			if (m_pNALData[iBytePosition - 2] == 0x00 && m_pNALData[iBytePosition - 1] == 0x00 && m_pNALData[iBytePosition] == 0x03) {
				m_iBitsOffset += 8;
			}
		}
	}

	m_iRemainingBits -= iNumberBits;
	return iValue;
}

void H26XBitstreamReader::skipBits(uint8_t iNumberBits)
{
	m_iBitsOffset += iNumberBits;
	m_iRemainingBits -= iNumberBits;
}

uint8_t H26XBitstreamReader::readBit()
{
	return ((*(m_pNALData + (m_iBitsOffset >> 0x3))) >> (0x7 - (m_iBitsOffset & 0x7))) & 0x1;
}

uint32_t H26XBitstreamReader::readGolombUE()
{
	uint32_t zeros = 0;
	// calculate zero bits. Will be optimized.
	while (0 == readBits(1)) zeros++;

	// insert first 1 bit
	uint32_t info = 1 << zeros;

	for (int32_t i = zeros - 1; i >= 0; i--) {
		info |= readBits(1) << i;
	}

	return (info - 1);
}

int32_t H26XBitstreamReader::readGolombSE()
{
	const uint32_t x = readGolombUE();
	if (!x) {
		return 0;
	}
	else if (x & 1) {
		return static_cast<int32_t> ((x >> 1) + 1);
	}
	else {
		return -static_cast<int32_t> (x >> 1);
	}
}

bool H26XBitstreamReader::hasMoreRBSPData()
{
	if (m_iRemainingBits <= 0) {
		return false;
	}

	if (readBit() == 0) {
		return true;
	}

	uint32_t iOldBitsOffset = m_iBitsOffset;
	int64_t iOldRemainingBits = m_iRemainingBits;

	// Skip the first bit
	readBits(1);

	bool bRes = false;
	while (m_iRemainingBits > 0) {
		if (readBits(1) == 1) {
			bRes = true;
			break;
		}
	}

	m_iBitsOffset = iOldBitsOffset;
	m_iRemainingBits = iOldRemainingBits;

	return bRes;
}
