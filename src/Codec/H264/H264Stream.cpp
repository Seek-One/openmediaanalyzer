#include <iostream>
#include <algorithm>
#include <numeric>

#include "H264BitstreamReader.h"
#include "H264GOP.h"
#include "H264AccessUnit.h"
#include "H264NAL.h"
#include "H264PPS.h"
#include "H264Slice.h"
#include "H264SPS.h"
#include "H264SEI.h"
#include "H264AUD.h"
#include "H264Stream.h"
#include "H264Utils.h"

PictureOrderCount::PictureOrderCount(uint32_t iTopFieldOrderValue, uint32_t iBottomFieldOrderValue)
{
	iTopFieldOrder = iTopFieldOrderValue;
	iBottomFieldOrder = iBottomFieldOrderValue;
}

H264Stream::H264Stream():
	m_sizeInMb(Size(-1, -1)), m_sizeUncropped(Size(-1, -1)), m_sizeCropped(Size(-1, -1)), m_pCurrentAccessUnit(nullptr)
{
	MbaffFrameFlag = 0;

	m_prevPicOrderCntMsb = 0;
	m_prevPicOrderCntLsb = 0;
	m_prevFrameNumOffset = 0;
	m_prevFrameNum = 0;
	m_iPrevMMCO = 0;
}

H264Stream::~H264Stream(){
	m_GOPs.clear();
	H264SPS2::SPSMap.clear();
	H264PPS::PPSMap.clear();
}

const H264NAL& H264Stream::getNAL() const
{
	return m_currentNAL;
}

std::deque<H264GOP*> H264Stream::getGOPs() const 
{
	std::deque<H264GOP*> GOPs;
	std::transform(m_GOPs.begin(), m_GOPs.end(), std::back_inserter(GOPs),
					[](const std::unique_ptr<H264GOP>& ptr) { return ptr.get(); });
	return GOPs;
}

uint32_t H264Stream::popFrontGOPs(uint32_t count){
	uint32_t removedAccessUnits = 0;
	for(int i = 0;i < count;++i) {
		removedAccessUnits += m_GOPs.front()->count();
		m_GOPs.pop_front();
	}
	return removedAccessUnits;
}

uint32_t H264Stream::accessUnitCount() const
{
	return std::accumulate(m_GOPs.begin(), m_GOPs.end(), 0, [](uint32_t accumulator, const std::unique_ptr<H264GOP>& ptr){
		return accumulator + ptr->count();
	});
}

std::list<H264AccessUnit*> H264Stream::getLastAccessUnits(uint32_t count) const
{
	std::list<H264AccessUnit*> pAccessUnits = std::list<H264AccessUnit*>();
	for(auto itGOP = m_GOPs.rbegin(); itGOP != m_GOPs.rend() && count > 0;++itGOP){
		H264GOP* pGOP = itGOP->get();
		for(auto itAccessUnit = pGOP->accessUnits.rbegin(); itAccessUnit != pGOP->accessUnits.rend() && count > 0;++itAccessUnit){
			pAccessUnits.push_front(itAccessUnit->get());
			count--;
		}
	}
	return pAccessUnits;
}


PictureOrderCount H264Stream::computePOC() {
	if (m_GOPs.back()->accessUnits.size() == 0) {
		std::cerr << "[H264::Stream] No slices found to compute POC\n";
		return PictureOrderCount(0, 0);
	}

	switch (m_sps.pic_order_cnt_type) {
	case 0:
		return computePOCType0();

	case 1:
		return computePOCType1();

	case 2:
		return computePOCType2();

	default:
		std::cerr << "[H264::Stream] Invalid POC type: " << m_sps.pic_order_cnt_type << "\n";
	}

	return PictureOrderCount(0, 0);
}

const Size& H264Stream::getMbsPictureSize() const
{
	return m_sizeInMb;
}

const Size& H264Stream::getUncroppedPictureSize() const
{
	return m_sizeUncropped;
}

const Size& H264Stream::getPictureSize() const
{
	return m_sizeCropped;
}

bool H264Stream::parsePacket(const uint8_t* pPacketData, uint32_t iPacketLength)
{
	std::vector<NALData> listNAL = splitNAL(pPacketData, iPacketLength);

	bool bRes;
	for (int i = 0; i < listNAL.size(); ++i) {
		bRes = bRes && parseNAL(listNAL[i].pData, (uint32_t)listNAL[i].iLength);
	}

	if (bRes) {
		computeSizes();
	}

	return bRes;
}

// returns true if curr marks the beginning of a new access unit
bool newCodedPicture(H264Slice prev, H264Slice curr){
	if(prev.frame_num != curr.frame_num) return true;
	if(prev.pic_parameter_set_id != curr.pic_parameter_set_id) return true;
	std::optional<H264SPS2> optPrevSPS = prev.getSPS();
	std::optional<H264SPS2> optCurrSPS = curr.getSPS();
	if(optPrevSPS.has_value() && optCurrSPS.has_value()){
		H264SPS2 prevSPS = optPrevSPS.value();
		H264SPS2 currSPS = optCurrSPS.value();
		if(!prevSPS.frame_mbs_only_flag && !currSPS.frame_mbs_only_flag &&
		    prev.field_pic_flag && curr.field_pic_flag){
				if(prev.bottom_field_flag != curr.bottom_field_flag) return true;
			}
			if(prevSPS.pic_order_cnt_type == 0 && currSPS.pic_order_cnt_type == 0){
			if(prev.pic_order_cnt_lsb != curr.pic_order_cnt_lsb) return true;
			if(prev.delta_pic_order_cnt_bottom != curr.delta_pic_order_cnt_bottom) return true;
		}
		if(prevSPS.pic_order_cnt_type == 1 && currSPS.pic_order_cnt_type == 1){
			if(prev.delta_pic_order_cnt[0] != curr.delta_pic_order_cnt[0]) return true;
			if(prev.delta_pic_order_cnt[1] != curr.delta_pic_order_cnt[1]) return true;
		}
	} else {
		if(prev.first_mb_in_slice == curr.first_mb_in_slice &&
		   prev.slice_type == curr.slice_type &&
		   prev.pic_parameter_set_id == curr.pic_parameter_set_id) return true;
	}
	if(prev.nal_ref_idc != curr.nal_ref_idc && (prev.nal_ref_idc == 0 || curr.nal_ref_idc == 0)) return true;
	if(prev.IdrPicFlag != curr.IdrPicFlag) return true;
	if(prev.idr_pic_id != curr.idr_pic_id) return true;
	return false;
}

bool H264Stream::parseNAL(const uint8_t* pNALData, uint32_t iNALLength)
{
	H264BitstreamReader bitstreamReader(pNALData, iNALLength);
	bool previousUnitIsVLC = false;
	if(m_GOPs.empty()) m_GOPs.push_back(std::make_unique<H264GOP>());
	if(!m_pCurrentAccessUnit) {
		m_pCurrentAccessUnit = new H264AccessUnit();
		m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
	}
	if(!m_pCurrentAccessUnit->empty()) {
		H264NAL* lastUnit = m_pCurrentAccessUnit->last();
		previousUnitIsVLC = lastUnit->nal_unit_type == H264NAL::UnitType_NonIDRFrame || lastUnit->nal_unit_type == H264NAL::UnitType_IDRFrame;
	}
	bitstreamReader.readNALHeader(m_currentNAL);
	std::vector<std::string> header_errors;

	if (m_currentNAL.forbidden_zero_bit != 0) {
		header_errors.push_back("[H264 NAL Header] forbidden_zero_bit not equal to 0");
	}

	if ((m_currentNAL.nal_unit_type == H264NAL::UnitType_PrefixNAL) ||
		(m_currentNAL.nal_unit_type == H264NAL::UnitType_SVCExt) ||
		(m_currentNAL.nal_unit_type == H264NAL::UnitType_3DSlice)) {
		std::cerr << "[H264::Stream] NAL: SVC or 3D extention not handled\n";
	}
	switch (m_currentNAL.nal_unit_type) {
		case H264NAL::UnitType_SPS:
			if(m_currentNAL.nal_ref_idc == 0) header_errors.push_back("[H264 NAL Header] Sequence parameter set marked as unimportant");
			m_sps.forbidden_zero_bit = m_currentNAL.forbidden_zero_bit;
			m_sps.nal_ref_idc = m_currentNAL.nal_ref_idc;
			m_sps.nal_size = iNALLength;
			bitstreamReader.readSPS(m_sps);
			if(previousUnitIsVLC) {
				m_pCurrentAccessUnit = new H264AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
			}
			m_pCurrentAccessUnit->addNALUnit(std::make_unique<H264SPS2>(m_sps));
			for(std::string err : header_errors) m_sps.errors.push_back(err);
			break;
		case H264NAL::UnitType_PPS:
			if(m_currentNAL.nal_ref_idc == 0) header_errors.push_back("[H264 NAL Header] Picture parameter set marked as unimportant");
			m_pps.forbidden_zero_bit = m_currentNAL.forbidden_zero_bit;
			m_pps.nal_ref_idc = m_currentNAL.nal_ref_idc;
			m_pps.nal_size = iNALLength;
			bitstreamReader.readPPS(m_pps);
			if(previousUnitIsVLC) {
				m_pCurrentAccessUnit = new H264AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
			}			
			m_pCurrentAccessUnit->addNALUnit(std::make_unique<H264PPS>(m_pps));
			for(std::string err : header_errors) m_pps.errors.push_back(err);
			break;
		case H264NAL::UnitType_IDRFrame:
			if(m_currentNAL.nal_ref_idc == 0) header_errors.push_back("[H264 NAL Header] IDR frame marked as unimportant");
		case H264NAL::UnitType_NonIDRFrame: {
			H264Slice slice = H264Slice();
			slice.forbidden_zero_bit = m_currentNAL.forbidden_zero_bit;
			slice.nal_ref_idc = m_currentNAL.nal_ref_idc;
			slice.nal_unit_type = m_currentNAL.nal_unit_type;
			bitstreamReader.readSlice(slice);
			slice.nal_size = iNALLength;
			for(std::string err : header_errors) slice.errors.push_back(err);
			MbaffFrameFlag = m_sps.mb_adaptive_frame_field_flag && !slice.field_pic_flag;
			if(previousUnitIsVLC){
				H264Slice* previousSlice = m_pCurrentAccessUnit->slice();
				if(newCodedPicture(*previousSlice, slice)) {
					m_pCurrentAccessUnit = new H264AccessUnit();
					m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
				}
			}
			if(slice.slice_type == H264Slice::SliceType_I){ // I-frame marks new GOP
				// move access unit inserted in the previous GOP to a new one,
				// unless it's the very first access unit of the GOP (access units can start with
				// non-slice NAL units)
				if(m_GOPs.back()->hasSlice){ 
					H264GOP* previousGOP = m_GOPs.back().get();
					m_GOPs.push_back(std::make_unique<H264GOP>());
					m_GOPs.back()->accessUnits.push_back(std::move(previousGOP->accessUnits.back()));
					previousGOP->accessUnits.pop_back();
				}
				if(slice.nal_unit_type == H264NAL::UnitType_IDRFrame) m_GOPs.back()->hasIDR = true;
			} 
			m_pCurrentAccessUnit->addNALUnit(std::make_unique<H264Slice>(slice));
			m_GOPs.back()->hasSlice = true;
			for(std::string err : header_errors) slice.errors.push_back(err);
			break;
		}
		case H264NAL::UnitType_SEI: {
			H264SEI* h264SEI = new H264SEI();
			h264SEI->forbidden_zero_bit = m_currentNAL.forbidden_zero_bit;
			h264SEI->nal_ref_idc = m_currentNAL.nal_ref_idc;
			h264SEI->nal_size = iNALLength;
			bitstreamReader.readSEI(*h264SEI, m_sps);
			if(previousUnitIsVLC) {
				m_pCurrentAccessUnit = new H264AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
			}	
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264SEI>(h264SEI));
			for(std::string err : header_errors) h264SEI->errors.push_back(err);
			break;
		}
		case H264NAL::UnitType_AUD: {
			H264AUD h264AUD;
			h264AUD.forbidden_zero_bit = m_currentNAL.forbidden_zero_bit;
			h264AUD.nal_ref_idc = m_currentNAL.nal_ref_idc;
			h264AUD.nal_size = iNALLength;
			bitstreamReader.readAUD(h264AUD);
			if(previousUnitIsVLC) {
				m_pCurrentAccessUnit = new H264AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
			}			
			m_pCurrentAccessUnit->addNALUnit(std::make_unique<H264AUD>(h264AUD));
			for(std::string err : header_errors) h264AUD.errors.push_back(err);
			break;
		}
		default:
			std::cerr << "[H264::Stream] NAL: Unhandled nal type " << m_currentNAL.nal_unit_type << "\n";
			break;
	}
	return true;
}

void H264Stream::computeSizes() {
	if(m_GOPs.back()->accessUnits.empty()) return;
	H264Slice* pSlice = m_GOPs.back()->accessUnits[0]->slice();
	if (!pSlice) {
		// std::cerr << "[H264::Stream] No slices found to compute picture size in macro blocks\n";
		return;
	}

	auto referencedPPS = H264PPS::PPSMap.find(pSlice->pic_parameter_set_id);
	if(referencedPPS == H264PPS::PPSMap.end()) return;
	const H264PPS pps = referencedPPS->second;
	auto referencedSPS = H264SPS2::SPSMap.find(pps.seq_parameter_set_id);
	if(referencedSPS == H264SPS2::SPSMap.end()) return;
	const H264SPS2 sps = referencedSPS->second;
	// All of equation reference are taken form Rec. ITU-T H.264 (06/2019)
	// Compute luma width
	// uint32_t PicWidthInMbs = m_sps.pic_width_in_mbs_minus1 + 1; // (7-13)

	// Compute luma height
	// uint32_t PicHeightInMapUnits = m_sps.pic_height_in_map_units_minus1 + 1; // (7-16)
	// uint32_t FrameHeightInMbs = (2 - m_sps.frame_mbs_only_flag) * PicHeightInMapUnits; // (7-18)
	uint32_t PicHeightInMbs = sps.FrameHeightInMbs / (1 + pSlice->field_pic_flag); // (7-26)

	m_sizeInMb = Size(sps.PicWidthInMbs, PicHeightInMbs);

	uint32_t PicWidthInSamplesL = sps.PicWidthInMbs * 16; // (7-14)
	uint32_t PicHeightInSamplesL = PicHeightInMbs * 16; // (7-27)

	// Uncropped size
	m_sizeUncropped = Size((int)PicWidthInSamplesL, (int)PicHeightInSamplesL);

	// If the image was cropped
	if (m_sps.frame_cropping_flag) {
		int iWidth = (PicWidthInSamplesL - (m_sps.CropUnitX * m_sps.frame_crop_right_offset + 1)) - (m_sps.CropUnitX * m_sps.frame_crop_left_offset) + 1;
		int iHeight = ((16 * PicHeightInMbs) - (m_sps.CropUnitY * m_sps.frame_crop_bottom_offset + 1)) - (m_sps.CropUnitY * m_sps.frame_crop_top_offset) + 1;

		m_sizeCropped = Size(iWidth, iHeight);
	} else {
		m_sizeCropped = m_sizeUncropped;
	}

	return;
}

PictureOrderCount H264Stream::computePOCType0() {
	// Compute POC in according section 8.2.1.1 of H264 reference

	// IDR case
	int prevPicOrderCntMsb = 0;
	int prevPicOrderCntLsb = 0;

	// No IDR case
	if (m_currentNAL.nal_unit_type != H264NAL::UnitType_IDRFrame) {
		if (m_iPrevMMCO == 5) {
			std::cerr << "[H264::Stream] Memory management control operation for POC computation not handled\n";
		} else {
			prevPicOrderCntMsb = m_prevPicOrderCntMsb;
			prevPicOrderCntLsb = m_prevPicOrderCntLsb;
		}
	}

	// h264 reference equation 8-3
	H264Slice* pLastSlice = m_GOPs.back()->accessUnits.back()->slice();
	int iPicOrderCntMsb = 0;
	int iMaxPicOrderCntLsb = 1 << (m_sps.log2_max_pic_order_cnt_lsb_minus4 + 4);
	if ((pLastSlice->pic_order_cnt_lsb < prevPicOrderCntLsb) &&
		((prevPicOrderCntLsb - pLastSlice->pic_order_cnt_lsb) >= (iMaxPicOrderCntLsb / 2)))
	{
		iPicOrderCntMsb = prevPicOrderCntMsb + iMaxPicOrderCntLsb;
	} else if ((pLastSlice->pic_order_cnt_lsb > prevPicOrderCntLsb) &&
		((pLastSlice->pic_order_cnt_lsb - prevPicOrderCntLsb) > (iMaxPicOrderCntLsb / 2))) {
		iPicOrderCntMsb = prevPicOrderCntMsb - iMaxPicOrderCntLsb;
	} else {
		iPicOrderCntMsb = prevPicOrderCntMsb;
	}

	int iTopFieldOrderCnt = iPicOrderCntMsb + pLastSlice->pic_order_cnt_lsb; // Equation 8-4
	int iBottomFieldOrderCnt = iTopFieldOrderCnt;

	// If it's a frame
	if (!pLastSlice->field_pic_flag) {
		iBottomFieldOrderCnt += iTopFieldOrderCnt + pLastSlice->delta_pic_order_cnt_bottom;
	}

	// Update previous reference infos for next computation
	if (m_currentNAL.nal_ref_idc) {
		m_prevPicOrderCntMsb = iPicOrderCntMsb;
		m_prevPicOrderCntLsb = pLastSlice->pic_order_cnt_lsb;
		m_iPrevMMCO = pLastSlice->drpm.memory_management_control_operation[0];
	}

	return PictureOrderCount(iTopFieldOrderCnt, iBottomFieldOrderCnt);
}

PictureOrderCount H264Stream::computePOCType1() {
	// Compute POC in according section 8.2.1.2 of H264 reference

	// IDR case
	
	int iFrameNumOffset = 0;
	H264Slice* pLastSlice = m_GOPs.back()->accessUnits.back()->slice();

	// No IDR case
	if (m_currentNAL.nal_unit_type != H264NAL::UnitType_IDRFrame) {
		int prevFrameNumOffset = 0;
		// Set prevFrameNumOffset value
		if (m_iPrevMMCO != 5) {
			prevFrameNumOffset = m_prevFrameNumOffset;
		}

		// Set FrameNumOffset value
		int iMaxFrameNum = 1 << (m_sps.log2_max_frame_num_minus4 + 4);
		iFrameNumOffset = prevFrameNumOffset;
		if (m_prevFrameNum > pLastSlice->frame_num) {
			iFrameNumOffset += iMaxFrameNum;
		}
	}

	int absFrameNum = 0;
	if (!m_sps.num_ref_frames_in_pic_order_cnt_cycle) {
		absFrameNum = iFrameNumOffset + pLastSlice->frame_num;
	}

	if (!m_currentNAL.nal_ref_idc && absFrameNum > 0) {
		--absFrameNum;
	}

	int expectedPicOrderCnt = 0;
	if (absFrameNum > 0) {
		int iExpectedDeltaPerPicOrderCntCycle = 0;
		for (uint32_t i = 0; i < m_sps.num_ref_frames_in_pic_order_cnt_cycle; ++i) {
			iExpectedDeltaPerPicOrderCntCycle += m_sps.offset_for_ref_frame[i];
		}

		int picOrderCntCycleCnt = (absFrameNum - 1) / m_sps.num_ref_frames_in_pic_order_cnt_cycle;
		int frameNumInPicOrderCntCycle = (absFrameNum - 1) % m_sps.num_ref_frames_in_pic_order_cnt_cycle;

		expectedPicOrderCnt = picOrderCntCycleCnt * iExpectedDeltaPerPicOrderCntCycle;
		for (int i = 0; i <= frameNumInPicOrderCntCycle; ++i) {
			expectedPicOrderCnt += m_sps.offset_for_ref_frame[i];
		}

		if (!m_currentNAL.nal_ref_idc) {
			expectedPicOrderCnt += m_sps.offset_for_non_ref_pic;
		}
	}

	int iTopFieldOrderCnt = expectedPicOrderCnt + pLastSlice->delta_pic_order_cnt[0];
	int iBottomFieldOrderCnt = iTopFieldOrderCnt + m_sps.offset_for_top_to_bottom_field;
	if (pLastSlice->bottom_field_flag) {
		iBottomFieldOrderCnt += pLastSlice->delta_pic_order_cnt[0];
	} else {
		iBottomFieldOrderCnt += pLastSlice->delta_pic_order_cnt[1];
	}

	// Update previous reference infos for next computation
	if (m_currentNAL.nal_ref_idc) {
		m_prevFrameNumOffset = iFrameNumOffset;
		m_prevFrameNum = pLastSlice->frame_num;
		m_iPrevMMCO = pLastSlice->drpm.memory_management_control_operation[0];
	}

	return PictureOrderCount(iTopFieldOrderCnt, iBottomFieldOrderCnt);
}

PictureOrderCount H264Stream::computePOCType2() {
	// Compute POC in according section 8.2.1.3 of H264 reference

	// IDR case
	int iFrameNumOffset = 0;
	int tempPicOrderCnt = 0;
	H264Slice* pLastSlice = m_GOPs.back()->accessUnits.back()->slice();

	// No IDR case
	if (m_currentNAL.nal_unit_type != H264NAL::UnitType_IDRFrame) {
		int prevFrameNumOffset = 0;
		// Set prevFrameNumOffset value
		if (m_iPrevMMCO != 5) {
			prevFrameNumOffset = m_prevFrameNumOffset;
		}

		// Set FrameNumOffset value
		int iMaxFrameNum = (1 << (m_sps.log2_max_frame_num_minus4 + 4));
		iFrameNumOffset = prevFrameNumOffset;
		if (m_prevFrameNum > pLastSlice->frame_num) {
			iFrameNumOffset += iMaxFrameNum;
		}

		// Set tempPicOrderCnt value
		tempPicOrderCnt = 2 * (iFrameNumOffset + pLastSlice->frame_num);
		if (!m_currentNAL.nal_ref_idc) {
			--tempPicOrderCnt;
		}
	}

	int iTopFieldOrderCnt = tempPicOrderCnt;
	int iBottomFieldOrderCnt = tempPicOrderCnt;

	// Update previous reference infos for next computation
	if (m_currentNAL.nal_ref_idc) {
		m_prevFrameNumOffset = iFrameNumOffset;
		m_iPrevMMCO = pLastSlice->drpm.memory_management_control_operation[0];
	}

	return PictureOrderCount(iTopFieldOrderCnt, iBottomFieldOrderCnt);
}
