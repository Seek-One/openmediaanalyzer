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
	H264SPS::SPSMap.clear();
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

	switch (m_pActiveSPS->pic_order_cnt_type) {
	case 0:
		return computePOCType0();

	case 1:
		return computePOCType1();

	case 2:
		return computePOCType2();

	default:
		std::cerr << "[H264::Stream] Invalid POC type: " << m_pActiveSPS->pic_order_cnt_type << "\n";
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

bool H264Stream::parsePacket(uint8_t* pPacketData, uint32_t iPacketLength)
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
bool newCodedPicture(H264Slice* prev, H264Slice* curr){
	if(prev->frame_num != curr->frame_num) return true;
	if(prev->pic_parameter_set_id != curr->pic_parameter_set_id) return true;
	H264SPS* pPrevSPS = prev->getSPS();
	H264SPS* pCurrSPS = curr->getSPS();
	if(pPrevSPS && pCurrSPS){

		if(!pPrevSPS->frame_mbs_only_flag && !pCurrSPS->frame_mbs_only_flag &&
		    prev->field_pic_flag && curr->field_pic_flag){
				if(prev->bottom_field_flag != curr->bottom_field_flag) return true;
			}
			if(pPrevSPS->pic_order_cnt_type == 0 && pCurrSPS->pic_order_cnt_type == 0){
			if(prev->pic_order_cnt_lsb != curr->pic_order_cnt_lsb) return true;
			if(prev->delta_pic_order_cnt_bottom != curr->delta_pic_order_cnt_bottom) return true;
		}
		if(pPrevSPS->pic_order_cnt_type == 1 && pCurrSPS->pic_order_cnt_type == 1){
			if(prev->delta_pic_order_cnt[0] != curr->delta_pic_order_cnt[0]) return true;
			if(prev->delta_pic_order_cnt[1] != curr->delta_pic_order_cnt[1]) return true;
		}
	} else {
		if(prev->first_mb_in_slice == curr->first_mb_in_slice &&
		   prev->slice_type == curr->slice_type &&
		   prev->pic_parameter_set_id == curr->pic_parameter_set_id) return true;
	}
	if(prev->nal_ref_idc != curr->nal_ref_idc && (prev->nal_ref_idc == 0 || curr->nal_ref_idc == 0)) return true;
	if(prev->IdrPicFlag != curr->IdrPicFlag) return true;
	if(prev->idr_pic_id != curr->idr_pic_id) return true;
	return false;
}

bool H264Stream::parseNAL(uint8_t* pNALData, uint32_t iNALLength)
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


	if ((m_currentNAL.nal_unit_type == H264NAL::UnitType_PrefixNAL) ||
		(m_currentNAL.nal_unit_type == H264NAL::UnitType_SVCExt) ||
		(m_currentNAL.nal_unit_type == H264NAL::UnitType_3DSlice)) {
		std::cerr << "[H264::Stream] NAL: SVC or 3D extension not handled\n";
	}
	switch (m_currentNAL.nal_unit_type) {
		case H264NAL::UnitType_SPS: {
			H264SPS* pSps = new H264SPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_ref_idc, iNALLength, pNALData);
			try { bitstreamReader.readSPS(*pSps);
			} catch(const std::runtime_error& e) { pSps->majorErrors.push_back(std::string("[SPS] ").append(e.what()));}
			if(previousUnitIsVLC) {
				m_pCurrentAccessUnit = new H264AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
			}
			m_pActiveSPS = pSps;
			H264SPS::SPSMap.insert_or_assign(pSps->seq_parameter_set_id, pSps);
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264SPS>(pSps));
			break;
		}
		case H264NAL::UnitType_PPS: {
			H264PPS* pPps = new H264PPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_ref_idc, iNALLength, pNALData);
			try { bitstreamReader.readPPS(*pPps);
			} catch(const std::runtime_error& e) { pPps->majorErrors.push_back(std::string("[PPS] ").append(e.what()));}
			if(previousUnitIsVLC) {
				m_pCurrentAccessUnit = new H264AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
			}			
			m_pActivePPS = pPps;
			H264PPS::PPSMap.insert_or_assign(pPps->pic_parameter_set_id, pPps);
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264PPS>(pPps));
			break;
		}
		case H264NAL::UnitType_IDRFrame:
		case H264NAL::UnitType_NonIDRFrame: {
			H264Slice* pSlice = new H264Slice(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_ref_idc, m_currentNAL.nal_unit_type, iNALLength, pNALData);
			try { bitstreamReader.readSlice(*pSlice);
			} catch(const std::runtime_error& e) { pSlice->majorErrors.push_back(std::string("[Slice] ").append(e.what()));}
			if(previousUnitIsVLC){
				H264Slice* previousSlice = m_pCurrentAccessUnit->slice();
				if(newCodedPicture(previousSlice, pSlice)) {
					m_pCurrentAccessUnit = new H264AccessUnit();
					m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
				}
			} 
			if(pSlice->slice_type == H264Slice::SliceType_I){ // I-frame marks new GOP
				// move access unit inserted in the previous GOP to a new one,
				// unless it's the very first access unit of the GOP (access units can start with
				// non-slice NAL units)
				if(m_GOPs.back()->hasSlice){ 
					H264GOP* previousGOP = m_GOPs.back().get();
					m_GOPs.push_back(std::make_unique<H264GOP>());
					m_GOPs.back()->accessUnits.push_back(std::move(previousGOP->accessUnits.back()));
					previousGOP->accessUnits.pop_back();
					previousGOP->validate();

					minorErrors.insert(minorErrors.end(), previousGOP->minorErrors.begin(), previousGOP->minorErrors.end());
					previousGOP->minorErrors.clear();
					for(int i = 0;minorErrors.size() > ERR_MSG_LIMIT && i < minorErrors.size() - ERR_MSG_LIMIT;++i) minorErrors.pop_front();

					majorErrors.insert(majorErrors.end(), previousGOP->majorErrors.begin(), previousGOP->majorErrors.end());
					previousGOP->majorErrors.clear();
					for(int i = 0;majorErrors.size() > ERR_MSG_LIMIT && i < majorErrors.size() - ERR_MSG_LIMIT;++i) majorErrors.pop_front();
				}
				if(pSlice->nal_unit_type == H264NAL::UnitType_IDRFrame) m_GOPs.back()->hasIDR = true;
			} 
			validateFrameNum(pSlice);
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264Slice>(pSlice));
			m_GOPs.back()->hasSlice = true;
			break;
		}
		case H264NAL::UnitType_SEI: {
			H264SEI* pSei = new H264SEI(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_ref_idc, iNALLength, pNALData);
			try { bitstreamReader.readSEI(*pSei, *m_pActiveSPS);
			} catch(const std::runtime_error& e) { pSei->minorErrors.push_back(std::string("[SEI] ").append(e.what()));}
			if(previousUnitIsVLC) {
				m_pCurrentAccessUnit = new H264AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
			}	
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264SEI>(pSei));
			break;
		}
		case H264NAL::UnitType_AUD: {
			H264AUD* pAud = new H264AUD(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_ref_idc, iNALLength, pNALData);
			try { bitstreamReader.readAUD(*pAud);
			} catch(const std::runtime_error& e) { pAud->minorErrors.push_back(std::string("[AUD] ").append(e.what()));}
			if(previousUnitIsVLC) {
				m_pCurrentAccessUnit = new H264AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
			}			
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264AUD>(pAud));
			break;
		}
		default:
			majorErrors.push_back("Unhandled NAL type detected");
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
	H264PPS* pPps = referencedPPS->second;
	auto referencedSPS = H264SPS::SPSMap.find(pPps->seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) return;
	H264SPS* pSps = referencedSPS->second;
	// All of equation reference are taken form Rec. ITU-T H.264 (06/2019)
	// Compute luma width
	// uint32_t PicWidthInMbs = m_sps.pic_width_in_mbs_minus1 + 1; // (7-13)

	// Compute luma height
	// uint32_t PicHeightInMapUnits = m_sps.pic_height_in_map_units_minus1 + 1; // (7-16)
	// uint32_t FrameHeightInMbs = (2 - m_sps.frame_mbs_only_flag) * PicHeightInMapUnits; // (7-18)
	uint32_t PicHeightInMbs = pSps->FrameHeightInMbs / (1 + pSlice->field_pic_flag); // (7-26)

	m_sizeInMb = Size(pSps->PicWidthInMbs, PicHeightInMbs);

	uint32_t PicWidthInSamplesL = pSps->PicWidthInMbs * 16; // (7-14)
	uint32_t PicHeightInSamplesL = PicHeightInMbs * 16; // (7-27)

	// Uncropped size
	m_sizeUncropped = Size((int)PicWidthInSamplesL, (int)PicHeightInSamplesL);

	// If the image was cropped
	if (m_pActiveSPS->frame_cropping_flag) {
		int iWidth = (PicWidthInSamplesL - (m_pActiveSPS->CropUnitX * m_pActiveSPS->frame_crop_right_offset + 1)) - (m_pActiveSPS->CropUnitX * m_pActiveSPS->frame_crop_left_offset) + 1;
		int iHeight = ((16 * PicHeightInMbs) - (m_pActiveSPS->CropUnitY * m_pActiveSPS->frame_crop_bottom_offset + 1)) - (m_pActiveSPS->CropUnitY * m_pActiveSPS->frame_crop_top_offset) + 1;

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
	int iMaxPicOrderCntLsb = 1 << (m_pActiveSPS->log2_max_pic_order_cnt_lsb_minus4 + 4);
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
		int iMaxFrameNum = 1 << (m_pActiveSPS->log2_max_frame_num_minus4 + 4);
		iFrameNumOffset = prevFrameNumOffset;
		if (m_prevFrameNum > pLastSlice->frame_num) {
			iFrameNumOffset += iMaxFrameNum;
		}
	}

	int absFrameNum = 0;
	if (!m_pActiveSPS->num_ref_frames_in_pic_order_cnt_cycle) {
		absFrameNum = iFrameNumOffset + pLastSlice->frame_num;
	}

	if (!m_currentNAL.nal_ref_idc && absFrameNum > 0) {
		--absFrameNum;
	}

	int expectedPicOrderCnt = 0;
	if (absFrameNum > 0) {
		int iExpectedDeltaPerPicOrderCntCycle = 0;
		for (uint32_t i = 0; i < m_pActiveSPS->num_ref_frames_in_pic_order_cnt_cycle; ++i) {
			iExpectedDeltaPerPicOrderCntCycle += m_pActiveSPS->offset_for_ref_frame[i];
		}

		int picOrderCntCycleCnt = (absFrameNum - 1) / m_pActiveSPS->num_ref_frames_in_pic_order_cnt_cycle;
		int frameNumInPicOrderCntCycle = (absFrameNum - 1) % m_pActiveSPS->num_ref_frames_in_pic_order_cnt_cycle;

		expectedPicOrderCnt = picOrderCntCycleCnt * iExpectedDeltaPerPicOrderCntCycle;
		for (int i = 0; i <= frameNumInPicOrderCntCycle; ++i) {
			expectedPicOrderCnt += m_pActiveSPS->offset_for_ref_frame[i];
		}

		if (!m_currentNAL.nal_ref_idc) {
			expectedPicOrderCnt += m_pActiveSPS->offset_for_non_ref_pic;
		}
	}

	int iTopFieldOrderCnt = expectedPicOrderCnt + pLastSlice->delta_pic_order_cnt[0];
	int iBottomFieldOrderCnt = iTopFieldOrderCnt + m_pActiveSPS->offset_for_top_to_bottom_field;
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
		int iMaxFrameNum = (1 << (m_pActiveSPS->log2_max_frame_num_minus4 + 4));
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

void H264Stream::validateFrameNum(H264Slice* pSlice){
	if(!pSlice->getSPS()) return;
	std::list<H264AccessUnit*> pAccessUnits = getLastAccessUnits(accessUnitCount());
	pAccessUnits.pop_back(); // remove the current access unit
	if(pSlice->nal_unit_type == H264NAL::UnitType_IDRFrame) pSlice->PrevRefFrameNum = 0;
	else {
		// no gaps
		bool foundNoGaps = false;
		for(auto it = pAccessUnits.rbegin();it != pAccessUnits.rend();++it){ // most recent previous access unit that has a reference pic
			if((*it)->hasReferencePicture()) {
				pSlice->PrevRefFrameNum = (*it)->frameNumber().value();
				foundNoGaps = true;
				break;
			}
		}
		// gaps : rarely allowed and entails more checks/processing, do later if time allows it
		// bool foundGaps = false;
		// for(auto it = pAccessUnits.rbegin();it != pAccessUnits.rend()-1;++it){
		// 	H264AccessUnit* prevAccessUnit = *(it+1);
		// 	if(prevAccessUnit->hasReferencePicture() && (*it)->hasNonReferencePicture() && (*it)->hasFrameGaps){
		// 		pSlice->PrevRefFrameNum = (*it)->frameNumber().value();
		// 		foundGaps = true;
		// 		break;
		// 	}
		// }

		if(!foundNoGaps){ // if(!foundNoGaps && !foundGaps){
			pSlice->minorErrors.push_back("[Slice Frame number] Couldn't derive PrevRefFrameNumber");
			return;
		}
	}	
	if(pSlice->nal_unit_type == H264NAL::UnitType_IDRFrame && pSlice->frame_num != 0){
		pSlice->minorErrors.push_back("[Slice Frame number] frame_num of an IDR picture should be 0");
		return;
	} 
	if(pSlice->frame_num == pSlice->PrevRefFrameNum){
		std::vector<H264AccessUnit*> pCurrentGOPAccessUnits = m_GOPs.back()->getAccessUnits();
		pCurrentGOPAccessUnits.pop_back();
		if(pCurrentGOPAccessUnits.empty()) return;
		H264AccessUnit* previousAccessUnit = pCurrentGOPAccessUnits.back();
		H264Slice* prevSlice = previousAccessUnit->slice();
		bool consecutive = previousAccessUnit->hasReferencePicture();
		bool oppositeParities = prevSlice->field_pic_flag && pSlice->field_pic_flag && prevSlice->bottom_field_flag != pSlice->bottom_field_flag;
		bool precedingIsIDR = previousAccessUnit->slice()->nal_unit_type == H264NAL::UnitType_IDRFrame;
		bool markingPictureOperation = false;
		int i = 0;
		while(prevSlice->drpm.memory_management_control_operation[i] != 0){
			if(prevSlice->drpm.memory_management_control_operation[i++] == 5){
				markingPictureOperation = true;
				break;
			}
		} 
		bool precedingPreviousPrimaryPic = false;
		pAccessUnits.pop_back();
		H264AccessUnit* precedingPreviousAccessUnit = pAccessUnits.back();
		if(precedingPreviousAccessUnit && precedingPreviousAccessUnit->primary_coded_slice()){
			precedingPreviousPrimaryPic = precedingPreviousAccessUnit->primary_coded_slice()->frame_num != pSlice->PrevRefFrameNum;
		}
		bool precedingPreviousReference = precedingPreviousAccessUnit && precedingPreviousAccessUnit->primary_coded_slice() && precedingPreviousAccessUnit->primary_coded_slice()->nal_ref_idc == 0;
		if(!consecutive || !oppositeParities || (!precedingIsIDR && !markingPictureOperation && !precedingPreviousPrimaryPic && !precedingPreviousReference)){
			pSlice->minorErrors.push_back("[Slice Frame number] frame_num shouldn't be equal to PrevRefFrameNum");
			return;
		}
	} else {
		std::vector<uint16_t> UnusedShortTermFrameNums;
		uint16_t MaxFrameNum = pSlice->getSPS()->computeMaxFrameNumber();
		for(uint16_t i = (pSlice->PrevRefFrameNum+1)%MaxFrameNum;i != pSlice->frame_num;++i){
			UnusedShortTermFrameNums.push_back(i);
		}
		std::vector<H264AccessUnit*> pCurrentGOPAccessUnits = m_GOPs.back()->getAccessUnits();
		pCurrentGOPAccessUnits.pop_back();
		for(H264AccessUnit* pAccessUnit : pCurrentGOPAccessUnits){
			H264Slice* previousSlice = pAccessUnit->slice();
			if(previousSlice && previousSlice->nal_ref_idc != 0 && !previousSlice->drpm.long_term_reference_flag){
				if(std::find(UnusedShortTermFrameNums.begin(), UnusedShortTermFrameNums.end(), previousSlice->frame_num) != UnusedShortTermFrameNums.end()){
					pSlice->minorErrors.push_back("[Slice Frame number] Previous frame/field has a frame_num marked as unused");
					return;
				}
			}
		}
		if(!pSlice->getSPS()->gaps_in_frame_num_value_allowed_flag){
			if(pSlice->frame_num != ((pSlice->PrevRefFrameNum+1)%MaxFrameNum)){
				pSlice->minorErrors.push_back("[Slice Frame number] frame_num isn't directly succeeding PrevRefFrameNum");
				return;
			}
		} else {
			//see above comment on gaps
		}
	}
}
