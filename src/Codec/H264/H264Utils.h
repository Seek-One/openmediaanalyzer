/*
 * H264Utils.h
 *
 *  Created on: 10 déc. 2014
 *      Author: ebeuque
 */

#ifndef TOOLKIT_CODEC_UTILS_H264UTILS_H_
#define TOOLKIT_CODEC_UTILS_H264UTILS_H_


#include "../ByteBuffer.h"
#include "../Size.h"
#include "../Timeval.h"
#include "../FrameBuffer.h"

#include "H264NAL.h"
#include "H264PPS.h"
#include "H264Slice.h"
#include "H264SPS.h"
#include "../H26X/H26XUtils.h"

/* RFC 6184 RTP Payload Format for H.264 Video
 *
 * The first byte of the H264 payload represents the NAL Unit which has the following format:
 *
 *     +---------------+
 *     |0|1|2|3|4|5|6|7|
 *     +-+-+-+-+-+-+-+-+
 *     |F|NRI|  Type   |
 *     +---------------+
 *
 *  F:       1 bit
 *           forbidden_zero_bit.  The H.264 specification declares a
 *           value of 1 as a syntax violation.
 *
 *  NRI:     2 bits
 *           nal_ref_idc.  A value of 00 indicates that the content of
 *           the NAL unit is not used to reconstruct reference pictures
 *           for inter picture prediction.  Such NAL units can be
 *           discarded without risking the integrity of the reference
 *           pictures.  Values greater than 00 indicate that the decoding
 *           of the NAL unit is required to maintain the integrity of the
 *           reference pictures.
 *
 *   Type:   5 bits
 *           nal_unit_type.  This component specifies the NAL unit
 *           payload type
 *
 **/


#define H264_GetNalUnitType(byte) (byte & 0x1f)

#define H264_PROPSMAXSIZE 256

struct H264FrameInfos
{
	// byte 1
	bool f;
	int nri;
	int nal_unit_type;

	// byte 2
	int start_bit;
	int end_bit;
	int reserved_bit;
	int nal_unit_bits;
};

class H264NalUnitMetaInfos
{
public:
	Timeval m_tvPresentationTime;
	Timeval m_tvFrameTime;
	unsigned long m_iFirstPacketNumber;

public:
	H264NalUnitMetaInfos();
	virtual ~H264NalUnitMetaInfos();

	void setPresentationTime(const Timeval& tvPresentationTime);
	void setFrameTime(const Timeval& tvFrameTime);
	void setFirstPacketNumber(unsigned long iPacketNumber);

	H264NalUnitMetaInfos& operator= (const H264NalUnitMetaInfos& other);
};

class H264RepackFrame
{
public:
	enum Options {
		NonIDRFramePossibleKeyFrame = 0x01,  // Non-IDR frame preceded by SPS or PPS can be considered has key frame
	};

public:
	H264RepackFrame();
	virtual ~H264RepackFrame();

	void setOptions(int iOptions);

	void setPPS();

	void addNALUnit(const unsigned char* data, size_t len, const H264NalUnitMetaInfos& infos, bool bHasStartCode = false);

	bool isComplete() const;
	ByteBuffer* detachBuffer();
	const Timeval& getPresentationTime() const; // Must be called before detach
	const Timeval& getFrameTime() const; // Must be called before detach
	unsigned long getFirstPacketNumber() const;  // Must be called before detach
	const Size& getImageSize() const;

private:
	void checkBuffer(const H264NalUnitMetaInfos& infos);
	void setBufferComplete(bool bSet);
	void writeNALUnit(const unsigned char* data, size_t len, bool bWithPS, const H264NalUnitMetaInfos& infos);

private:
	int m_iOptions;

	unsigned char m_bufSPS[H264_PROPSMAXSIZE];
	int m_sizeSPS;
	unsigned char m_bufPPS[H264_PROPSMAXSIZE];
	int m_sizePPS;
	unsigned char m_bufSEI[H264_PROPSMAXSIZE];
	int m_sizeSEI;

	bool m_bNextIsKeyFrame;
	bool m_bHasFirstSliceFound;

	size_t m_iInitialBufferSize;
	H264NalUnitMetaInfos m_infosFrame;
	ByteBuffer* m_pBuffer;
	H264NalUnitMetaInfos m_infosFrameComplete;
	ByteBuffer* m_pBufferComplete;
	Size m_imageSizeSPS;
};

class H264Utils
{
public:
	// Check if this frame buffer can be considered as key frame (not just I-Frame)
	static bool isKeyFrame(FrameBuffer* pFrameBuffer);
	static bool isKeyFrame(const unsigned char* buf, size_t iSize);

	static void getFrameInfos(const unsigned char* data, H264FrameInfos& infos);

	static void parseSPS(const unsigned char* data, H264SPS& h264SPS);
	static void parseSlice(const unsigned char* data, int len, bool& bHasFirstMacroblocks);

	static H264Slice::SliceType getSliceType(int value);
};

#endif /* TOOLKIT_CODEC_UTILS_H264UTILS_H_ */
