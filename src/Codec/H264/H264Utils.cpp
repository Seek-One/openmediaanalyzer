/*
 * H264Utils.cpp
 *
 *  Created on: 10 déc. 2014
 *      Author: ebeuque
 */

#include <cstring>
#include <algorithm>
#include <iostream>

#include "H264Utils.h"

const uint8_t g_startBytes[] = {0x00, 0x00, 0x00, 0x01};

#define BASELINE_PROFILE 66
#define MAIN_PROFILE 77


H264NalUnitMetaInfos::H264NalUnitMetaInfos()
{
	m_iFirstPacketNumber = 0;
}

H264NalUnitMetaInfos::~H264NalUnitMetaInfos()
{

}

void H264NalUnitMetaInfos::setPresentationTime(const Timeval& tvPresentationTime)
{
	m_tvPresentationTime = tvPresentationTime;
}

void H264NalUnitMetaInfos::setFrameTime(const Timeval& tvFrameTime)
{
	m_tvFrameTime = tvFrameTime;
}

void H264NalUnitMetaInfos::setFirstPacketNumber(unsigned long iPacketNumber)
{
	m_iFirstPacketNumber = iPacketNumber;
}

H264NalUnitMetaInfos& H264NalUnitMetaInfos::operator= (const H264NalUnitMetaInfos& other)
{
	m_tvPresentationTime = other.m_tvPresentationTime;
	m_tvFrameTime = other.m_tvFrameTime;
	m_iFirstPacketNumber = other.m_iFirstPacketNumber;
	return *this;
}

H264RepackFrame::H264RepackFrame()
{
	m_pBuffer = NULL;
	m_pBufferComplete = NULL;
	m_iInitialBufferSize = 0;
	m_sizeSPS = 0;
	m_sizePPS = 0;
	m_sizeSEI = 0;
	m_bNextIsKeyFrame = false;
	m_bHasFirstSliceFound = false;
	m_iOptions = 0;
}

H264RepackFrame::~H264RepackFrame()
{
	if(m_pBuffer){
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
	if(m_pBufferComplete){
		delete m_pBufferComplete;
		m_pBufferComplete = NULL;
	}
}

void H264RepackFrame::setOptions(int iOptions)
{
	m_iOptions = iOptions;
}

void H264RepackFrame::checkBuffer(const H264NalUnitMetaInfos& infos)
{
	if(!m_pBuffer){
		m_pBuffer = new ByteBuffer(m_iInitialBufferSize);
		m_infosFrame = infos;
	}
}

void H264RepackFrame::addNALUnit(const unsigned char* data, size_t len, const H264NalUnitMetaInfos& infos, bool bHasStartCode)
{
	H264FrameInfos frameInfos;
	frameInfos.nal_unit_type = 0;
	H264Utils::getFrameInfos(data, frameInfos);

	bool bHasFirstMacroblocks = false;

	//Logger::debug("%p addNAL: %u bytes, %d", this, len, frameInfos.nal_unit_type);

	switch(frameInfos.nal_unit_type){
	case H264NAL::UnitType_IDRFrame:
	case H264NAL::UnitType_NonIDRFrame:
		break;
	case H264NAL::UnitType_AUD:
		// This is a delimiter, this mark the buffer as complete
		setBufferComplete(true);
		return;
	default:
		if(m_bHasFirstSliceFound){
			setBufferComplete(true);
			m_bHasFirstSliceFound = false;
		}
		break;
	}

	switch(frameInfos.nal_unit_type){
	case H264NAL::UnitType_SPS:
		m_bNextIsKeyFrame = true;
		if(len < H264_PROPSMAXSIZE){
			m_sizeSPS = (int)len;
			memcpy(m_bufSPS, data, len);

			H264SPS spsInfos;
			H264Utils::parseSPS(m_bufSPS, spsInfos);

			m_imageSizeSPS.width = ((spsInfos.pic_width_in_mbs_minus1 +1)*16) - spsInfos.frame_crop_right_offset*2 - spsInfos.frame_crop_left_offset*2;
			m_imageSizeSPS.height = ((2 - spsInfos.frame_mbs_only_flag)* (spsInfos.pic_height_in_map_units_minus1 +1) * 16) - (spsInfos.frame_crop_top_offset * 2) - (spsInfos.frame_crop_bottom_offset * 2);
		}else{
			std::cerr << "[H264::Utils] Size too big for SPS: " << len << "\n";
		}
		break;
	case H264NAL::UnitType_PPS:
		m_bNextIsKeyFrame = true;
		if(len < H264_PROPSMAXSIZE){
			m_sizePPS = (int)len;
			memcpy(m_bufPPS, data, len);
		}else{
			std::cerr << "[H264::Utils] Size too big for PPS: " << len << "\n";
		}
		break;
	case H264NAL::UnitType_SEI:
		if(len < H264_PROPSMAXSIZE){
			m_sizeSEI = (int)len;
			memcpy(m_bufSEI, data, len);
		}else{
			std::cerr << "[H264::Utils] Size too big for SEI: " << len << "\n";
		}
		break;
	case H264NAL::UnitType_IDRFrame:
		m_iInitialBufferSize = std::max(m_iInitialBufferSize, (m_pBuffer ? m_pBuffer->size() : len));
		H264Utils::parseSlice(data, (int)len, bHasFirstMacroblocks);
		if(bHasFirstMacroblocks && m_bHasFirstSliceFound){
			setBufferComplete(true);
			m_bHasFirstSliceFound = false;
		}
		writeNALUnit(data, len, true, infos);
		if(bHasFirstMacroblocks){
			m_bHasFirstSliceFound = true;
		}
		m_bNextIsKeyFrame = false;
		break;
	case H264NAL::UnitType_NonIDRFrame:
		H264Utils::parseSlice(data, (int)len, bHasFirstMacroblocks);
		if(bHasFirstMacroblocks && m_bHasFirstSliceFound){
			setBufferComplete(true);
			m_bHasFirstSliceFound = false;
		}
		if(m_iOptions & NonIDRFramePossibleKeyFrame){
			writeNALUnit(data, len, m_bNextIsKeyFrame, infos);
		}else{
			writeNALUnit(data, len, false, infos);
		}
		if(bHasFirstMacroblocks){
			m_bHasFirstSliceFound = true;
		}
		m_bNextIsKeyFrame = false;
		break;
	default:
		writeNALUnit(data, len, false, infos);
		break;
	}
}

void H264RepackFrame::writeNALUnit(const unsigned char* data, size_t len, bool bWithPS, const H264NalUnitMetaInfos& infos)
{
	checkBuffer(infos);
	if(m_pBuffer->size() == 0){
		if(bWithPS){
			m_pBuffer->append(g_startBytes, 4);
			m_pBuffer->append(m_bufSPS, m_sizeSPS);
			m_pBuffer->append(g_startBytes, 4);
			m_pBuffer->append(m_bufPPS, m_sizePPS);
		}
		if(m_sizeSEI > 0){
			m_pBuffer->append(g_startBytes, 4);
			m_pBuffer->append(m_bufSEI, m_sizeSEI);
		}
	}
	m_pBuffer->append(g_startBytes, 4);
	m_pBuffer->append(data, len);
}

void H264RepackFrame::setBufferComplete(bool bSet)
{
	if(bSet){
		if(m_pBufferComplete){
			delete m_pBufferComplete;
			m_pBufferComplete = NULL;
		}
		m_pBufferComplete = m_pBuffer;
		m_infosFrameComplete = m_infosFrame;
		m_pBuffer = NULL;
	}
}

bool H264RepackFrame::isComplete() const
{
	return (m_pBufferComplete != NULL);
}

ByteBuffer* H264RepackFrame::detachBuffer()
{
	ByteBuffer* pBuffer = NULL;
	if(m_pBufferComplete){
		pBuffer = m_pBufferComplete;
		m_pBufferComplete = NULL;
	}else if(m_pBuffer){
		pBuffer = m_pBuffer;
		m_pBuffer = NULL;
	}
	return pBuffer;
}

const Timeval& H264RepackFrame::getPresentationTime() const
{
	if(m_pBufferComplete){
		return m_infosFrameComplete.m_tvPresentationTime;
	}
	return m_infosFrame.m_tvPresentationTime;
}

const Timeval& H264RepackFrame::getFrameTime() const
{
	if(m_pBufferComplete){
		return m_infosFrameComplete.m_tvFrameTime;
	}
	return m_infosFrame.m_tvFrameTime;
}

unsigned long H264RepackFrame::getFirstPacketNumber() const
{
	if(m_pBufferComplete){
		return m_infosFrameComplete.m_iFirstPacketNumber;
	}
	return m_infosFrame.m_iFirstPacketNumber;
}

const Size& H264RepackFrame::getImageSize() const
{
	return m_imageSizeSPS;
}

bool H264Utils::isKeyFrame(FrameBuffer* pFrameBuffer)
{
	return isKeyFrame(pFrameBuffer->getData(), pFrameBuffer->getDataSize());
}

bool H264Utils::isKeyFrame(const unsigned char* buf, size_t iSize)
{
	if(buf){
		if( !memcmp(buf, g_startBytes, 4) )
		{
			// To be considered as key frame, the frame must have SPS, PPS and I-Frame data
			// but check only if it start with the SPS
			int nal_unit_type = H264_GetNalUnitType(*(buf+4));
			if(nal_unit_type == H264NAL::UnitType_SPS){
				return true;
			}
		}
	}
	return false;
}

void H264Utils::getFrameInfos(const unsigned char* data, H264FrameInfos& infos)
{
	if(data){
		infos.f = ((data[0] & 0x80) >> 6 ? true : false);
		infos.nri = (data[0] & 0x60) >> 5;
		infos.nal_unit_type = H264_GetNalUnitType(data[0]);

		infos.start_bit = (data[1] & 0x80);
		infos.end_bit = (data[1] & 0x40);
		infos.reserved_bit = (data[1] & 0x20);
		infos.nal_unit_bits = data[1] & 0x1F;
	}
}

inline uint32_t get_bit(const uint8_t * const base, uint32_t offset)
{
	return ((*(base + (offset >> 0x3))) >> (0x7 - (offset & 0x7))) & 0x1;
}

inline uint32_t get_bits(const uint8_t * const base, uint32_t * const offset, uint8_t bits)
{
	uint32_t value = 0;
	for (int i = 0; i < bits; i++)
	{
	  value = (value << 1) | (get_bit(base, (*offset)++) ? 1 : 0);
	}
	return value;
}

// This function implement decoding of exp-Golomb codes of zero range (used in H.264).
uint32_t golomb_get_ue(const uint8_t * const base, uint32_t * const offset)
{
	uint32_t zeros = 0;

	// calculate zero bits. Will be optimized.
	while (0 == get_bit(base, (*offset)++)) zeros++;

	// insert first 1 bit
	uint32_t info = 1 << zeros;

	for (int32_t i = zeros - 1; i >= 0; i--)
	{
		info |= get_bit(base, (*offset)++) << i;
	}

	return (info - 1);
}

// This function implement decoding of exp-Golomb codes of zero range (used in H.264).
int32_t golomb_get_se(const uint8_t * const base, uint32_t * const offset)
{
	const uint32_t x = golomb_get_ue(base, offset);
	if (!x)
	  return 0;
	else if (x & 1)
	  return static_cast < int32_t > ((x >> 1) + 1);
	else
	  return -static_cast < int32_t > (x >> 1);
}

// Decode unsigned integer
uint32_t golomb_get_u(uint32_t n, const uint8_t * const base, uint32_t * const offset)
{
	return get_bits(base, offset, n);
}

static void get_scaling_list (uint32_t i, uint32_t sizeOfScalingList, const uint8_t * const base, uint32_t * const offset)
{
	uint32_t lastScale = 8, nextScale = 8;
	uint32_t jx;
	int deltaScale;

	for (jx = 0; jx < sizeOfScalingList; jx++) {
		if (nextScale != 0) {
			deltaScale = (uint32_t)golomb_get_se(base, offset);
			nextScale = (lastScale + deltaScale + 256) % 256;
		}
		if (nextScale != 0) {
			lastScale = nextScale;
		}
	}
}

void H264Utils::parseSPS(const unsigned char* data, H264SPS& h264SPS)
{
	uint32_t offset = 8;

	h264SPS.profile_idc = get_bits(data, &offset, 8);

	h264SPS.constrained_set0_flag = get_bits(data, &offset, 1);
	h264SPS.constrained_set1_flag = get_bits(data, &offset, 1);
	h264SPS.constrained_set2_flag = get_bits(data, &offset, 1);
	h264SPS.constrained_set3_flag = get_bits(data, &offset, 1);
	h264SPS.constrained_set4_flag = get_bits(data, &offset, 1);
	h264SPS.reserved_zero = get_bits(data, &offset, 3);

	h264SPS.level_idc = get_bits(data, &offset, 8);
	h264SPS.seq_parameter_set_id = golomb_get_ue(data, &offset);

	switch(h264SPS.profile_idc)
	{
	case 100:
	case 110:
	case 122:
	case 244:
	case 44:
	case 83:
	case 86:
	case 118:
		h264SPS.chromat_format_idc = golomb_get_ue(data, &offset);
		if(h264SPS.chromat_format_idc == 3){
			h264SPS.separate_colour_plane_flag = get_bits(data, &offset, 1);
		}
		h264SPS.bit_depth_luma_minus8 = golomb_get_ue(data, &offset);
		h264SPS.bit_depth_chroma_minus8 = golomb_get_ue(data, &offset);
		h264SPS.qpprime_y_zero_tranform_bypass_flag = get_bits(data, &offset, 1);

		h264SPS.seq_scaling_matrix_present_flag = get_bits(data, &offset, 1);
		if(h264SPS.seq_scaling_matrix_present_flag){
			for(uint32_t i = 0; i < 8; i++){
				h264SPS.seq_scaling_list_present_flag[i] = get_bits(data, &offset, 1);
				if(h264SPS.seq_scaling_list_present_flag[i]){
					  get_scaling_list(i, i < 6 ? 16 : 64, data, &offset);
				}
			}
		}

		break;
	default:
		break;
	}

	h264SPS.log2_max_frame_num_minus4 = golomb_get_ue(data, &offset);
	h264SPS.pic_order_cnt_type = golomb_get_ue(data, &offset);
	if(h264SPS.pic_order_cnt_type == 0){
		h264SPS.log2_max_pic_order_cnt_lsb_minus4 = golomb_get_ue(data, &offset);
	}else if(h264SPS.pic_order_cnt_type == 1){
		h264SPS.delta_pic_order_always_zero_flag = get_bits(data, &offset, 1);
		h264SPS.offset_for_non_ref_pic = golomb_get_se(data, &offset);
		h264SPS.offset_for_top_to_bottom_field = golomb_get_se(data, &offset);
		h264SPS.num_ref_frames_in_pic_order_cnt_cycle = golomb_get_ue(data, &offset);
		for(uint32_t i=0; i<h264SPS.num_ref_frames_in_pic_order_cnt_cycle; i++){
			h264SPS.offset_for_ref_frame = golomb_get_se(data, &offset);
		}
	}
	h264SPS.max_num_ref_frames = golomb_get_ue(data, &offset);
	h264SPS.gaps_in_frame_num_value_allowed_flag = get_bits(data, &offset, 1);
	h264SPS.pic_width_in_mbs_minus1 = golomb_get_ue(data, &offset);
	h264SPS.pic_height_in_map_units_minus1 = golomb_get_ue(data, &offset);

	h264SPS.frame_mbs_only_flag = get_bits(data, &offset, 1);
	if(!h264SPS.frame_mbs_only_flag){
		h264SPS.mb_adaptive_frame_field_flag = get_bits(data, &offset, 1);
	}else{
		h264SPS.mb_adaptive_frame_field_flag = 0;
	}

	h264SPS.direct_8x8_inference_flag = get_bits(data, &offset, 1);

	h264SPS.frame_cropping_flag = get_bits(data, &offset, 1);
	if(h264SPS.frame_cropping_flag){
		h264SPS.frame_crop_left_offset = golomb_get_ue(data, &offset);
		h264SPS.frame_crop_right_offset = golomb_get_ue(data, &offset);
		h264SPS.frame_crop_top_offset = golomb_get_ue(data, &offset);
		h264SPS.frame_crop_bottom_offset = golomb_get_ue(data, &offset);
	}else{
		h264SPS.frame_crop_left_offset = 0;
		h264SPS.frame_crop_right_offset = 0;
		h264SPS.frame_crop_top_offset = 0;
		h264SPS.frame_crop_bottom_offset = 0;
	}


	/*
	Logger::debug("profile_idc: %d", h264SPS.profile_idc);
	Logger::debug("constrained_set0_flag: %d", h264SPS.constrained_set0_flag);
	Logger::debug("constrained_set1_flag: %d", h264SPS.constrained_set1_flag);
	Logger::debug("constrained_set2_flag: %d", h264SPS.constrained_setframe_mbs_only_flags2_flag);
	Logger::debug("constrained_set3_flag: %d", h264SPS.constrained_set3_flag);
	Logger::debug("constrained_set4_flag: %d", h264SPS.constrained_set4_flag);
	Logger::debug("reserved_zero: %d", h264SPS.reserved_zero);
	Logger::debug("level_idc: %d", h264SPS.level_idc);
	Logger::debug("seq_parameter_set_id: %d", h264SPS.seq_parameter_set_id);

	Logger::debug("log2_max_frame_num_minus4: %d", h264SPS.log2_max_frame_num_minus4);
	Logger::debug("pic_order_cnt_type: %d", h264SPS.pic_order_cnt_type);
	Logger::debug("log2_max_pic_order_cnt_lsb_minus4: %d", h264SPS.log2_max_pic_order_cnt_lsb_minus4);

	Logger::debug("max_num_ref_frames: %d", h264SPS.max_num_ref_frames);
	Logger::debug("gaps_in_frame_num_value_allowed_flag: %d", h264SPS.gaps_in_frame_num_value_allowed_flag);

	Logger::debug("pic_width_in_mbs_minus1: %d", h264SPS.pic_width_in_mbs_minus1);
	Logger::debug("pic_height_in_map_units_minus1: %d", h264SPS.pic_height_in_map_units_minus1);
	*/
}

void H264Utils::parseSlice(const unsigned char* data, int len, bool& bHasFirstMacroblocks)
{
	//BytesTool::printHexString(data, 40);
	uint32_t offset = 8;
	uint32_t adress;
	//uint32_t type;
	//uint32_t pic_parameter_set_id;
	//uint32_t frame_num;

	adress = golomb_get_ue(data, &offset);
	//type = golomb_get_ue(data, &offset);
	//pic_parameter_set_id = golomb_get_ue(data, &offset);

	//frame_num = golomb_get_u(0 + 4, data, &offset);

	//Logger::debug("addr: %d", adress);
	//Logger::debug("type: %d", DecodeUGolomb(data, &offset));

	bHasFirstMacroblocks = (adress == 0);
}

H264Slice::SliceType H264Utils::getSliceType(int value) {
	switch(value) {
	case 0:
	case 5:
		return H264Slice::SliceType_P;

	case 1:
	case 6:
		return H264Slice::SliceType_B;

	case 2:
	case 7:
		return H264Slice::SliceType_I;

	case 3:
	case 8:
		return H264Slice::SliceType_SP;

	case 4:
	case 9:
		return H264Slice::SliceType_SI;

	default:
		return H264Slice::SliceType_Unspecified;
	}
}
