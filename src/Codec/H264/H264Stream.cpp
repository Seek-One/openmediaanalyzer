#include <iostream>
#include <algorithm>
#include <numeric>

#include "../H26X/H26XErrorsMsg.h"
#include "H264BitstreamReader.h"
#include "H264GOP.h"
#include "H264AccessUnit.h"
#include "H264SPS.h"
#include "H264PPS.h"
#include "H264Slice.h"
#include "H264SEI.h"
#include "H264AUD.h"

#include "H264Stream.h"

PictureOrderCount::PictureOrderCount(uint32_t iTopFieldOrderValue, uint32_t iBottomFieldOrderValue)
{
	iTopFieldOrder = iTopFieldOrderValue;
	iBottomFieldOrder = iBottomFieldOrderValue;
}

H264Stream::H264Stream():
	m_pActiveSPS(nullptr), m_pActivePPS(nullptr), m_pCurrentAccessUnit(nullptr)
{
	MbaffFrameFlag = 0;
}

H264Stream::~H264Stream(){
	m_GOPs.clear();
	H264SPS::SPSMap.clear();
	H264PPS::PPSMap.clear();
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
	for(uint32_t i = 0;i < count;++i) {
		removedAccessUnits += m_GOPs.front()->size();
		m_GOPs.pop_front();
	}
	return removedAccessUnits;
}

uint32_t H264Stream::accessUnitCount() const
{
	return std::accumulate(m_GOPs.begin(), m_GOPs.end(), 0, [](uint32_t accumulator, const std::unique_ptr<H264GOP>& ptr){
		return accumulator + ptr->size();
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

std::vector<H264AccessUnit*> H264Stream::getAccessUnits() const
{
	std::vector<H264AccessUnit*> pAccessUnits = std::vector<H264AccessUnit*>();
	for(auto itGOP = m_GOPs.begin(); itGOP != m_GOPs.end();++itGOP){
		H264GOP* pGOP = itGOP->get();
		for(auto itAccessUnit = pGOP->accessUnits.begin(); itAccessUnit != pGOP->accessUnits.end();++itAccessUnit){
			pAccessUnits.push_back(itAccessUnit->get());
		}
	}
	return pAccessUnits;
}

bool H264Stream::parsePacket(const uint8_t* pPacketData, uint32_t iPacketLength)
{
	std::vector<NALData> listNAL = splitNAL(pPacketData, iPacketLength);

	bool bRes = true;
	for (uint32_t i = 0; i < listNAL.size(); ++i) {
		if(!parseNAL(listNAL[i].pData, (uint32_t)listNAL[i].iLength)) bRes = false;
	}

	return bRes;
}

/* Called in the case of a finite stream,
   completes the processing of the very last GOP
   which is normally done upon detecting a new GOP 
 */
void H264Stream::lastPacketParsed()
{
	H264GOP* lastGOP = m_GOPs.back().get();
	lastGOP->accessUnits.back()->decodable = true;
	lastGOP->validate();

	errors.add(lastGOP->errors);
	lastGOP->errors.clear();
	errors.clear(ERR_MSG_LIMIT);
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
		   prev->slice_type == curr->slice_type) return true;
	}
	if(prev->getNALHeader()->nal_ref_idc != curr->getNALHeader()->nal_ref_idc && (prev->getNALHeader()->nal_ref_idc == 0 || curr->getNALHeader()->nal_ref_idc == 0)) return true;
	if(prev->IdrPicFlag != curr->IdrPicFlag) return true;
	if(prev->idr_pic_id != curr->idr_pic_id) return true;
	return false;
}

bool H264Stream::parseNAL(const uint8_t* pNALData, uint32_t iNALLength)
{
	H264BitstreamReader bitstreamReader(pNALData, iNALLength);
	bool previousUnitIsVLC = false;
	if(m_GOPs.empty()){
		m_GOPs.push_back(std::make_unique<H264GOP>());
	}
	if(!m_pCurrentAccessUnit) {
		m_pCurrentAccessUnit = new H264AccessUnit();
		m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
	}
	if(!m_pCurrentAccessUnit->empty()) {
		H264NAL* lastUnit = m_pCurrentAccessUnit->last();
		previousUnitIsVLC = lastUnit->getNalUnitType() == H264NALUnitType::NonIDRFrame || lastUnit->getNalUnitType() == H264NALUnitType::IDRFrame;
	} 
	try {
		bitstreamReader.readNALHeader(m_currentNALHeader);
	} catch(const std::runtime_error& e) { 
		errors.add(H26XError::Major, std::string("[NAL Header] ").append(e.what()));
		return false;
	}

	if ((m_currentNALHeader.nal_unit_type == H264NALUnitType::PrefixNAL) ||
		(m_currentNALHeader.nal_unit_type == H264NALUnitType::SVCExt) ||
		(m_currentNALHeader.nal_unit_type == H264NALUnitType::Slice3D)) {
		std::cerr << "[H264::Stream] NAL: SVC or 3D extension not handled\n";
	}
	switch (m_currentNALHeader.nal_unit_type) {
		case H264NALUnitType::SPS: {
			H264SPS* pSps = new H264SPS(&m_currentNALHeader, iNALLength, pNALData);
			try { 
				bitstreamReader.readSPS(*pSps);
				m_pActiveSPS = pSps;
			} catch(const std::runtime_error& e) { 
				pSps->errors.add(H26XError::Major, std::string("[SPS] ").append(e.what()));
				pSps->completelyParsed = false;
			}
			if(previousUnitIsVLC){
				newAccessUnit();
			}
			H264SPS::SPSMap.insert_or_assign(pSps->seq_parameter_set_id, pSps);
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264SPS>(pSps));
			break;
		}
		case H264NALUnitType::PPS: {
			H264PPS* pPps = new H264PPS(&m_currentNALHeader, iNALLength, pNALData);
			try { bitstreamReader.readPPS(*pPps);
			} catch(const std::runtime_error& e) { 
				pPps->errors.add(H26XError::Major, std::string("[PPS] ").append(e.what()));
				pPps->completelyParsed = false;
			}
			if(previousUnitIsVLC) newAccessUnit();	
			m_pActivePPS = pPps;
			H264PPS::PPSMap.insert_or_assign(pPps->pic_parameter_set_id, pPps);
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264PPS>(pPps));
			break;
		}
		case H264NALUnitType::NonIDRFrame: 
		case H264NALUnitType::IDRFrame: {
			H264Slice* pSlice = new H264Slice(&m_currentNALHeader, iNALLength, pNALData);
			try { bitstreamReader.readSlice(*pSlice);
			} catch(const std::runtime_error& e) { 
				pSlice->errors.add(H26XError::Major, std::string("[Slice] ").append(e.what()));
				pSlice->completelyParsed = false;
			}
			if(previousUnitIsVLC){
				H264Slice* previousSlice = m_pCurrentAccessUnit->slice();
				if(newCodedPicture(previousSlice, pSlice)) newAccessUnit();
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
					previousGOP->accessUnits.back()->decodable = true;
					previousGOP->validate();

					errors.add(previousGOP->errors);
					previousGOP->errors.clear();
					errors.clear(ERR_MSG_LIMIT);
				}
				if(pSlice->getNALHeader()->nal_unit_type == H264NALUnitType::IDRFrame) m_GOPs.back()->hasIDR = true;
			} 
			validateFrameNum(pSlice);
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264Slice>(pSlice));
			break;
		}
		case H264NALUnitType::SEI: {
			H264SEI* pSei = new H264SEI(&m_currentNALHeader, iNALLength, pNALData);
			try { 
				bitstreamReader.readSEI(*pSei, m_pActiveSPS);
			} catch(const std::runtime_error& e) { 
				pSei->errors.add(H26XError::Minor, std::string("[SEI] ").append(e.what()));
				pSei->completelyParsed = false;
			}
			if(previousUnitIsVLC) newAccessUnit();
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264SEI>(pSei));
			break;
		}
		case H264NALUnitType::AUD: {
			H264AUD* pAud = new H264AUD(&m_currentNALHeader, iNALLength, pNALData);
			try { bitstreamReader.readAUD(*pAud);
			} catch(const std::runtime_error& e) { 
				pAud->errors.add(H26XError::Minor, std::string("[AUD] ").append(e.what()));
				pAud->completelyParsed = false;
			}
			if(previousUnitIsVLC) newAccessUnit();	
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H264AUD>(pAud));
			break;
		}
		default:
			errors.add(H26XError::Major, "Unhandled NAL type detected");
			std::cerr << "[H264::Stream] NAL: Unhandled nal type " << m_currentNALHeader.nal_unit_type << "\n";
			break;
	}
	return true;
}

void H264Stream::validateFrameNum(H264Slice* pSlice)
{
	if(!pSlice->getSPS() || !pSlice->errors.empty()){
		return;
	}
	std::vector<H264AccessUnit*> pAccessUnits = getAccessUnits();
	pAccessUnits.pop_back(); // remove the current access unit
	// frame_num of an IDR picure equal to 0 : already covered in H264Slice::validate();
	if(pSlice->getNALHeader()->nal_unit_type == H264NALUnitType::IDRFrame) {
		pSlice->PrevRefFrameNum = 0;
		return;
	}else {
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

		// no gaps
		bool foundPrevFrame = false;
		for(auto it = pAccessUnits.rbegin();it != pAccessUnits.rend();++it){ // most recent previous access unit that has a reference pic
			if((*it)->hasReferencePicture() && (*it)->frameNumber().has_value()) {
				pSlice->PrevRefFrameNum = (*it)->frameNumber().value();
				foundPrevFrame = true;
				break;
			}
		}

		if(!foundPrevFrame){
			pSlice->errors.add(H26XError::Minor, "[Slice frame number] Couldn't derive PrevRefFrameNumber");
			return;
		}
	}	
	if(pSlice->frame_num == pSlice->PrevRefFrameNum){
		std::vector<H264AccessUnit*> pCurrentGOPAccessUnits = m_GOPs.back()->getAccessUnits();
		pCurrentGOPAccessUnits.pop_back();
		if(pCurrentGOPAccessUnits.empty()) return;
		H264AccessUnit* previousAccessUnit = pCurrentGOPAccessUnits.back();
		H264Slice* prevSlice = previousAccessUnit->slice();
		bool consecutive = previousAccessUnit->hasReferencePicture();
		bool oppositeParities = prevSlice->field_pic_flag && pSlice->field_pic_flag && prevSlice->bottom_field_flag != pSlice->bottom_field_flag;
		bool precedingIsIDR = previousAccessUnit->slice()->getNALHeader()->nal_unit_type == H264NALUnitType::IDRFrame;
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
		bool precedingPreviousReference = precedingPreviousAccessUnit && precedingPreviousAccessUnit->primary_coded_slice() && precedingPreviousAccessUnit->primary_coded_slice()->getNALHeader()->nal_ref_idc == 0;
		if(!consecutive || !oppositeParities || (!precedingIsIDR && !markingPictureOperation && !precedingPreviousPrimaryPic && !precedingPreviousReference)){
			pSlice->errors.add(H26XError::Minor, "[Slice frame number] frame_num shouldn't be equal to PrevRefFrameNum");
			return;
		}
	} else {
		std::vector<uint16_t> UnusedShortTermFrameNums;
		uint32_t MaxFrameNum = pSlice->getSPS()->MaxFrameNumber;
		for(uint32_t i = (pSlice->PrevRefFrameNum+1)%MaxFrameNum;i < pSlice->frame_num;++i){
			UnusedShortTermFrameNums.push_back(i);
		}
		std::vector<H264AccessUnit*> pCurrentGOPAccessUnits = m_GOPs.back()->getAccessUnits();
		pCurrentGOPAccessUnits.pop_back();
		for(H264AccessUnit* pAccessUnit : pCurrentGOPAccessUnits){
			H264Slice* previousSlice = pAccessUnit->slice();
			if(previousSlice && previousSlice->getNALHeader()->nal_ref_idc != 0 && !previousSlice->drpm.long_term_reference_flag){
				if(std::find(UnusedShortTermFrameNums.begin(), UnusedShortTermFrameNums.end(), previousSlice->frame_num) != UnusedShortTermFrameNums.end()){
					pSlice->errors.add(H26XError::Minor, "[Slice frame number] Previous frame/field has a frame_num marked as unused");
					return;
				}
			}
		}
		if(!pSlice->getSPS()->gaps_in_frame_num_value_allowed_flag){
			if(pSlice->frame_num != ((pSlice->PrevRefFrameNum+1)%MaxFrameNum)){
				pSlice->errors.add(H26XError::Minor, "[Slice frame number] frame_num isn't directly succeeding PrevRefFrameNum");
				return;
			}
		} else {
			//see above comment on gaps
		}
	}
}

void H264Stream::newAccessUnit(){
	m_GOPs.back()->hasSlice = true;
	if(!m_pCurrentAccessUnit->hasMajorErrors()) m_pCurrentAccessUnit->decodable = true;
	m_pCurrentAccessUnit->FrameNum = m_pCurrentAccessUnit->slice()->frame_num;
	// computeCurrentAccessUnitPOC();
	// computeCurrentAccessUnitRPL();
	// markDecodedReferencePictures();
	m_pCurrentAccessUnit = new H264AccessUnit();
	m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H264AccessUnit>(m_pCurrentAccessUnit));
}

void H264Stream::computeCurrentAccessUnitPOC(){
	H264Slice* pCurrentSlice = m_pCurrentAccessUnit->slice();
	H264SPS* pCurrentSPS = pCurrentSlice->getSPS();

	H264AccessUnit* pPrevAccessUnit = getLastAccessUnits(1).front();
	H264Slice* pPrevSlice = pPrevAccessUnit->slice();
	uint8_t i = 0;
	bool memOp5Found = false;

	while(pPrevSlice->drpm.memory_management_control_operation[i] != 0 && !memOp5Found){
		memOp5Found = pPrevSlice->drpm.memory_management_control_operation[i++] == 5;
	}
	uint16_t prevFrameNumOffset = memOp5Found ? 0 : pPrevAccessUnit->FrameNumOffset;
	if(pCurrentSlice->IdrPicFlag) m_pCurrentAccessUnit->FrameNumOffset = 0;
	else if(pPrevSlice->frame_num > pCurrentSlice->frame_num) m_pCurrentAccessUnit->FrameNumOffset = prevFrameNumOffset + pCurrentSPS->MaxFrameNumber;
	else m_pCurrentAccessUnit->FrameNumOffset = prevFrameNumOffset;
	
	switch(pCurrentSPS->pic_order_cnt_type){
		case 0:{
			uint16_t prevPicOrderCntMsb = 0;
			uint16_t prevPicOrderCntLsb = 0;
			if(pCurrentSlice->getNALHeader()->nal_unit_type != H264NALUnitType::IDRFrame){
				if(memOp5Found) {
					if(!pPrevSlice->bottom_field_flag) prevPicOrderCntLsb = pPrevAccessUnit->TopFieldOrderCnt;
				} else {
					prevPicOrderCntMsb = pPrevAccessUnit->PicOrderCntMsb;
					prevPicOrderCntLsb = pPrevSlice->pic_order_cnt_lsb;
				}
			}
			if((pCurrentSlice->pic_order_cnt_lsb < prevPicOrderCntLsb) && ((uint32_t)(prevPicOrderCntLsb - pCurrentSlice->pic_order_cnt_lsb) >= (pCurrentSPS->MaxPicOrderCntLsb/2))){
				m_pCurrentAccessUnit->PicOrderCntMsb = prevPicOrderCntMsb + pCurrentSPS->MaxPicOrderCntLsb;
			} else if ((pCurrentSlice->pic_order_cnt_lsb > prevPicOrderCntLsb) && ((uint32_t)(pCurrentSlice->pic_order_cnt_lsb - prevPicOrderCntLsb) > (pCurrentSPS->MaxPicOrderCntLsb/2))){
				m_pCurrentAccessUnit->PicOrderCntMsb = prevPicOrderCntMsb - pCurrentSPS->MaxPicOrderCntLsb;
			} else m_pCurrentAccessUnit->PicOrderCntMsb = prevPicOrderCntMsb;
			if(!pCurrentSlice->bottom_field_flag) m_pCurrentAccessUnit->TopFieldOrderCnt = m_pCurrentAccessUnit->PicOrderCntMsb + pCurrentSlice->pic_order_cnt_lsb;
			if(!pCurrentSlice->field_pic_flag) m_pCurrentAccessUnit->BottomFieldOrderCnt = m_pCurrentAccessUnit->TopFieldOrderCnt + pCurrentSlice->delta_pic_order_cnt_bottom;
			else m_pCurrentAccessUnit->BottomFieldOrderCnt = m_pCurrentAccessUnit->PicOrderCntMsb + pCurrentSlice->pic_order_cnt_lsb;
			break;
		}
		case 1:{
			uint16_t absFrameNum = pCurrentSPS->num_ref_frames_in_pic_order_cnt_cycle != 0 ? m_pCurrentAccessUnit->FrameNumOffset + pCurrentSlice->frame_num : 0;
			if(pCurrentSlice->getNALHeader()->nal_ref_idc == 0 && absFrameNum > 0){
				--absFrameNum;
			}
			uint16_t expectedPicOrderCnt = 0;
			if(absFrameNum > 0){
				uint16_t picOrderCntCycleCnt = (absFrameNum-1)/pCurrentSPS->num_ref_frames_in_pic_order_cnt_cycle;
				uint16_t frameNumInPicOrderCntCycle = (absFrameNum-1)%pCurrentSPS->num_ref_frames_in_pic_order_cnt_cycle;
				expectedPicOrderCnt = picOrderCntCycleCnt*pCurrentSPS->ExpectedDeltaPerPicOrderCntCycle;
				for(i = 0;i <= frameNumInPicOrderCntCycle;++i) expectedPicOrderCnt *= pCurrentSPS->offset_for_ref_frame[i];
			}
			if(pCurrentSlice->getNALHeader()->nal_ref_idc == 0){
				expectedPicOrderCnt += pCurrentSPS->offset_for_non_ref_pic;
			}
			if(!pCurrentSlice->field_pic_flag){
				m_pCurrentAccessUnit->TopFieldOrderCnt = expectedPicOrderCnt + pCurrentSlice->delta_pic_order_cnt[0];
				m_pCurrentAccessUnit->BottomFieldOrderCnt = m_pCurrentAccessUnit->TopFieldOrderCnt + pCurrentSPS->offset_for_top_to_bottom_field + pCurrentSlice->delta_pic_order_cnt[1];	
			} else if(!pCurrentSlice->bottom_field_flag){
				m_pCurrentAccessUnit->TopFieldOrderCnt = expectedPicOrderCnt + pCurrentSlice->delta_pic_order_cnt[0];
			}
			else{
				m_pCurrentAccessUnit->BottomFieldOrderCnt = expectedPicOrderCnt + pCurrentSPS->offset_for_top_to_bottom_field + pCurrentSlice->delta_pic_order_cnt[0];
			}
			break;
		}
		case 2:{
			uint16_t tempPicOrderCnt;
			if(pCurrentSlice->IdrPicFlag){
				tempPicOrderCnt = 0;
			}else if(pCurrentSlice->getNALHeader()->nal_ref_idc == 0){
				tempPicOrderCnt = 2 * (m_pCurrentAccessUnit->FrameNumOffset + pCurrentSlice->frame_num) - 1;
			}else{
				tempPicOrderCnt = 2 * (m_pCurrentAccessUnit->FrameNumOffset + pCurrentSlice->frame_num) - 1;
			}

			if(!pCurrentSlice->field_pic_flag){
				m_pCurrentAccessUnit->TopFieldOrderCnt = tempPicOrderCnt;
				m_pCurrentAccessUnit->BottomFieldOrderCnt = tempPicOrderCnt;
			}else if(pCurrentSlice->bottom_field_flag){
				m_pCurrentAccessUnit->BottomFieldOrderCnt = tempPicOrderCnt;
			}else{
				m_pCurrentAccessUnit->TopFieldOrderCnt = tempPicOrderCnt;
			}
			break;
		}
	}
	if(!pCurrentSlice->field_pic_flag) m_pCurrentAccessUnit->PicOrderCnt = std::min(m_pCurrentAccessUnit->TopFieldOrderCnt, m_pCurrentAccessUnit->BottomFieldOrderCnt);
	else if(!pCurrentSlice->bottom_field_flag) m_pCurrentAccessUnit->PicOrderCnt = m_pCurrentAccessUnit->TopFieldOrderCnt;
	else m_pCurrentAccessUnit->PicOrderCnt = m_pCurrentAccessUnit->BottomFieldOrderCnt;
}

void H264Stream::computeCurrentAccessUnitRPL(){
	computeRPLPictureNumbers();
	computeRPLInit();	
}

void H264Stream::computeRPLPictureNumbers(){
	H264Slice* pCurrentSlice = m_pCurrentAccessUnit->slice();
	H264SPS* pCurrentSPS = pCurrentSlice->getSPS();

	for(H264AccessUnit* pAccessUnit : getAccessUnits()){
		if(pAccessUnit->rpm == RPM_Unused) continue;
		if(pAccessUnit->rpm == RPM_ShortTermReference){
			if(pAccessUnit->FrameNum > pCurrentSlice->frame_num) pAccessUnit->FrameNumWrap = pAccessUnit->FrameNum - pCurrentSPS->MaxFrameNumber;
			else pAccessUnit->FrameNumWrap = pAccessUnit->FrameNum;
			if(pCurrentSlice->field_pic_flag) pAccessUnit->PicNum = 2 * pAccessUnit->FrameNumWrap + ((pCurrentSlice->bottom_field_flag == pAccessUnit->slice()->bottom_field_flag) ? 1 : 0);
			else pAccessUnit->PicNum = pAccessUnit->FrameNumWrap;
		} else { // Long-term reference
			if(pCurrentSlice->field_pic_flag) pAccessUnit->LongTermPicNum = 2 * pAccessUnit->LongTermFrameIdx + ((pCurrentSlice->bottom_field_flag == pAccessUnit->slice()->bottom_field_flag) ? 1 : 0);
			else pAccessUnit->LongTermPicNum = pAccessUnit->LongTermFrameIdx;
		}
	}
}

void H264Stream::computeRPLInit(){
	std::vector<H264AccessUnit*> pAccessUnits = getAccessUnits();
	H264Slice* pCurrentSlice = m_pCurrentAccessUnit->slice();
	std::vector<H264AccessUnit*> tempRefPicList0, tempLongTermRefs;

	if(pCurrentSlice->slice_type == H264Slice::SliceType_P || pCurrentSlice->slice_type == H264Slice::SliceType_SP){
		if(pCurrentSlice->field_pic_flag){
			std::vector<H264AccessUnit*> refFrameList0ShortTerm, refFrameList0LongTerm;
			for(H264AccessUnit* pAccessUnit : pAccessUnits) if(pAccessUnit->rpm == RPM_ShortTermReference) refFrameList0ShortTerm.push_back(pAccessUnit);
			std::sort(refFrameList0ShortTerm.begin(), refFrameList0ShortTerm.end(), [](H264AccessUnit* lhs, H264AccessUnit* rhs){
				return lhs->FrameNumWrap > rhs->FrameNumWrap;
			});
			for(H264AccessUnit* pAccessUnit : pAccessUnits) if(pAccessUnit->rpm == RPM_ShortTermReference) refFrameList0LongTerm.push_back(pAccessUnit);
			std::sort(refFrameList0LongTerm.begin(),refFrameList0LongTerm.end(), [](H264AccessUnit* lhs, H264AccessUnit* rhs){
				return lhs->LongTermFrameIdx < rhs->LongTermFrameIdx;
			});
			pCurrentSlice->RefPicList0 = computeRPLFieldInit(refFrameList0ShortTerm, refFrameList0LongTerm);
		} else {
			for(H264AccessUnit* pAccessUnit : pAccessUnits) {
				if(pAccessUnit->rpm == RPM_ShortTermReference) tempRefPicList0.push_back(pAccessUnit);
				else if(pAccessUnit->rpm == RPM_LongTermReference) tempLongTermRefs.push_back(pAccessUnit);
			}
			std::sort(tempRefPicList0.begin(), tempRefPicList0.end(), [](H264AccessUnit* lhs, H264AccessUnit* rhs){
				return lhs->PicNum > rhs->PicNum;
			});
			std::sort(tempLongTermRefs.begin(), tempLongTermRefs.end(), [](H264AccessUnit* lhs, H264AccessUnit* rhs){
				return lhs->LongTermPicNum < rhs->LongTermPicNum;
			});
			tempRefPicList0.insert(tempRefPicList0.end(), tempLongTermRefs.begin(), tempLongTermRefs.end());
			std::transform(tempRefPicList0.begin(), tempRefPicList0.end(), std::back_inserter(pCurrentSlice->RefPicList0), [](H264AccessUnit* pAccessUnit){
				return pAccessUnit->PicOrderCnt;
			});
		}
		pCurrentSlice->RefPicList0.resize(pCurrentSlice->num_ref_idx_l0_active_minus1+1);
	} else if(pCurrentSlice->slice_type == H264Slice::SliceType_B){
		if(pCurrentSlice->field_pic_flag){
			std::vector<H264AccessUnit*> refFrameList0ShortTerm, refFrameList1ShortTerm, refFrameListLongTerm;
			std::vector<H264AccessUnit*> GT_POCs, LTE_POCs;

			for(H264AccessUnit* pAccessUnit : pAccessUnits){
				if(pAccessUnit->rpm == RPM_ShortTermReference){
					if(pAccessUnit->PicOrderCnt <= m_pCurrentAccessUnit->PicOrderCnt) refFrameList0ShortTerm.push_back(pAccessUnit);
					else GT_POCs.push_back(pAccessUnit);
				} else if(pAccessUnit->rpm == RPM_LongTermReference) refFrameListLongTerm.push_back(pAccessUnit);
			}
			std::sort(refFrameList0ShortTerm.begin(), refFrameList0ShortTerm.end(), [](H264AccessUnit* lhs, H264AccessUnit* rhs){
				return lhs->PicOrderCnt > rhs->PicOrderCnt;
			});
			std::sort(GT_POCs.begin(), GT_POCs.end(), [](H264AccessUnit* lhs, H264AccessUnit* rhs){
				return lhs->PicOrderCnt < rhs->PicOrderCnt;
			});
			refFrameList0ShortTerm.insert(refFrameList0ShortTerm.end(), GT_POCs.begin(), GT_POCs.end());

			for(H264AccessUnit* pAccessUnit : pAccessUnits){
				if(pAccessUnit->rpm == RPM_ShortTermReference){
					if(pAccessUnit->PicOrderCnt > m_pCurrentAccessUnit->PicOrderCnt) refFrameList1ShortTerm.push_back(pAccessUnit);
					else LTE_POCs.push_back(pAccessUnit);
				}
			}
			std::sort(refFrameList1ShortTerm.begin(), refFrameList1ShortTerm.end(), [](H264AccessUnit* lhs, H264AccessUnit* rhs){
				return lhs->PicOrderCnt < rhs->PicOrderCnt;
			});
			std::sort(LTE_POCs.begin(), LTE_POCs.end(), [](H264AccessUnit* lhs, H264AccessUnit* rhs){
				return lhs->PicOrderCnt > rhs->PicOrderCnt;
			});
			refFrameList1ShortTerm.insert(refFrameList1ShortTerm.end(), LTE_POCs.begin(), LTE_POCs.end());
			std::sort(refFrameListLongTerm.begin(), refFrameListLongTerm.end(), [](H264AccessUnit* lhs, H264AccessUnit* rhs){
				return lhs->LongTermFrameIdx < rhs->LongTermFrameIdx;
			});

			pCurrentSlice->RefPicList0 = computeRPLFieldInit(refFrameList0ShortTerm, refFrameListLongTerm);
			pCurrentSlice->RefPicList1 = computeRPLFieldInit(refFrameList1ShortTerm, refFrameListLongTerm);
		} else {
			std::vector<uint16_t> GTE_POCs, LTE_POCs;
			std::vector<H264AccessUnit*> longTermRefs;
			for(H264AccessUnit* pAccessUnit : pAccessUnits){
				if(pAccessUnit->rpm == RPM_ShortTermReference){
					if(pAccessUnit->PicOrderCnt < m_pCurrentAccessUnit->PicOrderCnt) pCurrentSlice->RefPicList0.push_back(pAccessUnit->PicOrderCnt);
					else GTE_POCs.push_back(pAccessUnit->PicOrderCnt);
				} else if(pAccessUnit->rpm == RPM_LongTermReference) longTermRefs.push_back(pAccessUnit);
			}
			std::sort(pCurrentSlice->RefPicList0.begin(), pCurrentSlice->RefPicList0.end(), [](uint16_t lhs, uint16_t rhs){
				return lhs > rhs;
			});
			std::sort(GTE_POCs.begin(), GTE_POCs.end());
			std::sort(longTermRefs.begin(), longTermRefs.end(), [](H264AccessUnit* lhs, H264AccessUnit* rhs){
				return lhs->LongTermPicNum < rhs->LongTermPicNum;
			});
			pCurrentSlice->RefPicList0.insert(pCurrentSlice->RefPicList0.end(), GTE_POCs.begin(), GTE_POCs.end());
			std::transform(longTermRefs.begin(), longTermRefs.end(), std::back_inserter(pCurrentSlice->RefPicList0), [](H264AccessUnit* pAccessUnit){
				return pAccessUnit->PicOrderCnt;
			});

			for(H264AccessUnit* pAccessUnit : pAccessUnits){
				if(pAccessUnit->rpm == RPM_ShortTermReference){
					if(pAccessUnit->PicOrderCnt > m_pCurrentAccessUnit->PicOrderCnt) pCurrentSlice->RefPicList0.push_back(pAccessUnit->PicOrderCnt);
					else LTE_POCs.push_back(pAccessUnit->PicOrderCnt);
				}
			}
			std::sort(pCurrentSlice->RefPicList1.begin(), pCurrentSlice->RefPicList1.end());
			std::sort(LTE_POCs.begin(), LTE_POCs.end(), [](uint16_t lhs, uint16_t rhs){
				return lhs > rhs;
			});
			pCurrentSlice->RefPicList1.insert(pCurrentSlice->RefPicList1.end(), LTE_POCs.begin(), LTE_POCs.end());
			std::transform(longTermRefs.begin(), longTermRefs.end(), std::back_inserter(pCurrentSlice->RefPicList1), [](H264AccessUnit* pAccessUnit){
				return pAccessUnit->PicOrderCnt;
			});			
			if(pCurrentSlice->RefPicList1.size() > 1 && pCurrentSlice->RefPicList0 == pCurrentSlice->RefPicList1) std::swap(pCurrentSlice->RefPicList1[0], pCurrentSlice->RefPicList1[1]);
		}
		pCurrentSlice->RefPicList0.resize(pCurrentSlice->num_ref_idx_l0_active_minus1+1);
		pCurrentSlice->RefPicList1.resize(pCurrentSlice->num_ref_idx_l1_active_minus1+1);
	}
}
	
std::vector<uint16_t> H264Stream::computeRPLFieldInit(std::vector<H264AccessUnit*> refFrameListXShortTerm, std::vector<H264AccessUnit*> refFrameListLongTerm){
	std::vector<uint16_t> RefPicListX;
	H264Slice* pCurrentSlice = m_pCurrentAccessUnit->slice();
	for(std::vector<H264AccessUnit*> refFrameList : {refFrameListXShortTerm, refFrameListLongTerm}){
		std::deque<H264AccessUnit*> bottomField, topField;
		for(H264AccessUnit* pAccessUnit : refFrameList){
			if(pAccessUnit->slice()->bottom_field_flag) bottomField.push_back(pAccessUnit);
			else topField.push_back(pAccessUnit);
		}
		bool bottomParity = pCurrentSlice->bottom_field_flag;
		while(!bottomField.empty() && !topField.empty()){
			if(bottomParity) {
				RefPicListX.push_back(bottomField.front()->PicOrderCnt);
				bottomField.pop_front();
			} else {
				RefPicListX.push_back(topField.front()->PicOrderCnt);
				topField.pop_front();
			}
			bottomParity = !bottomParity;
		}
		if(!bottomField.empty()) std::transform(bottomField.begin(), bottomField.end(), std::back_inserter(RefPicListX), [](H264AccessUnit* pAccessUnit){
			return pAccessUnit->PicOrderCnt;
		});
		else if(!topField.empty()) std::transform(topField.begin(), topField.end(), std::back_inserter(RefPicListX), [](H264AccessUnit* pAccessUnit){
			return pAccessUnit->PicOrderCnt;
		});
	}

	return RefPicListX;
}

void H264Stream::markDecodedReferencePictures(){
	std::vector<H264AccessUnit*> pAccessUnits = getAccessUnits();
	H264Slice* pCurrentSlice = m_pCurrentAccessUnit->slice();
	if(pCurrentSlice->getNALHeader()->nal_ref_idc == 0){
		return;
	}
	H264SPS* pCurrentSPS = pCurrentSlice->getSPS();

	if(pCurrentSlice->getNALHeader()->nal_unit_type == H264NALUnitType::IDRFrame){
		for(H264AccessUnit* pAccessUnit : pAccessUnits) pAccessUnit->rpm = RPM_Unused;
		if(pCurrentSlice->drpm.long_term_reference_flag){
			m_pCurrentAccessUnit->rpm = RPM_LongTermReference;
			m_pCurrentAccessUnit->LongTermFrameIdx = 0;
		} else m_pCurrentAccessUnit->rpm = RPM_ShortTermReference;
	} else {
		bool memOp6 = false;
		if(pCurrentSlice->drpm.adaptive_ref_pic_marking_mode_flag){
			uint8_t i = 0;
			while(pCurrentSlice->drpm.memory_management_control_operation[i] != 0){
				switch(pCurrentSlice->drpm.memory_management_control_operation[i]){
					case 1:{
						uint16_t picNumX = pCurrentSlice->CurrPicNum - (pCurrentSlice->drpm.difference_of_pic_nums_minus1[i]+1);
						for(auto pAccessUnitIt = pAccessUnits.rbegin();pAccessUnitIt != pAccessUnits.rend();++pAccessUnitIt){
							if((*pAccessUnitIt)->slice() && (*pAccessUnitIt)->slice()->CurrPicNum == picNumX) {
								(*pAccessUnitIt)->rpm = RPM_Unused;
								break;
							}
						}
						break;
					}
					case 2:{
						for(auto pAccessUnitIt = pAccessUnits.rbegin();pAccessUnitIt != pAccessUnits.rend();++pAccessUnitIt){
							if((*pAccessUnitIt)->slice() && (*pAccessUnitIt)->LongTermPicNum == pCurrentSlice->drpm.long_term_pic_num[i]){
								(*pAccessUnitIt)->rpm = RPM_Unused;
								break;
							} 
						}
						break;
					}
					case 3:{
						uint16_t picNumX = pCurrentSlice->CurrPicNum - (pCurrentSlice->drpm.difference_of_pic_nums_minus1[i]+1);
						for(auto pAccessUnitIt = pAccessUnits.rbegin();pAccessUnitIt != pAccessUnits.rend();++pAccessUnitIt){
							if((*pAccessUnitIt)->slice() && (*pAccessUnitIt)->slice()->CurrPicNum == picNumX) {
								(*pAccessUnitIt)->rpm = RPM_LongTermReference;
								(*pAccessUnitIt)->LongTermFrameIdx = pCurrentSlice->drpm.long_term_frame_idx[i];
								break;
							}
						}
						break;
					}
					case 4:{
						for(auto pAccessUnitIt = pAccessUnits.rbegin();pAccessUnitIt != pAccessUnits.rend();++pAccessUnitIt){
							if((*pAccessUnitIt)->LongTermFrameIdx > pCurrentSlice->drpm.max_long_term_frame_idx_plus1[i]-1 && (*pAccessUnitIt)->rpm == RPM_LongTermReference) {
								(*pAccessUnitIt)->rpm = RPM_Unused;
							}
						}
						break;
					}
					case 5:{
						for(auto pAccessUnitIt = pAccessUnits.rbegin();pAccessUnitIt != pAccessUnits.rend();++pAccessUnitIt){
							(*pAccessUnitIt)->rpm = RPM_Unused;
						}
						break;
					}
					case 6:{
						memOp6 = true;
						for(auto pAccessUnitIt = pAccessUnits.rbegin();pAccessUnitIt != pAccessUnits.rend();++pAccessUnitIt){
							if((*pAccessUnitIt)->LongTermFrameIdx == pCurrentSlice->drpm.long_term_frame_idx[i] && (*pAccessUnitIt)->rpm == RPM_LongTermReference) {
								(*pAccessUnitIt)->rpm = RPM_Unused;
								break;
							}
						}
						m_pCurrentAccessUnit->rpm = RPM_LongTermReference;
						m_pCurrentAccessUnit->LongTermFrameIdx = pCurrentSlice->drpm.long_term_frame_idx[i];
						break;
					}
				}
				++i;
			}
		} else {
			if(pCurrentSlice->field_pic_flag && pCurrentSlice->bottom_field_flag && pAccessUnits.back()->rpm == RPM_ShortTermReference) m_pCurrentAccessUnit->rpm = RPM_ShortTermReference;
			else {
				uint16_t numShortTerm = 0;
				uint16_t numLongTerm = 0;
				std::vector<H264AccessUnit*> shortTermRefPictures;
				for(H264AccessUnit* pAccessUnit : pAccessUnits){
					if(pAccessUnit->rpm == RPM_ShortTermReference){
						++numShortTerm;
						shortTermRefPictures.push_back(pAccessUnit);
					} else if(pAccessUnit->rpm == RPM_LongTermReference) ++numLongTerm;
				}
				if(m_pCurrentAccessUnit->rpm == RPM_ShortTermReference){
					++numShortTerm;
					shortTermRefPictures.push_back(m_pCurrentAccessUnit);
				} else if(m_pCurrentAccessUnit->rpm == RPM_LongTermReference) ++numLongTerm;
				if(numShortTerm + numLongTerm == std::max(pCurrentSPS->max_num_ref_frames, 1u) && !shortTermRefPictures.empty()){
					H264AccessUnit* smallestFrameNumWrapAccessUnit = shortTermRefPictures.front();
					for(H264AccessUnit* shortTermRefPicture : shortTermRefPictures){
						if(shortTermRefPicture->FrameNumWrap < smallestFrameNumWrapAccessUnit->FrameNumWrap) smallestFrameNumWrapAccessUnit = shortTermRefPicture;
					}
					smallestFrameNumWrapAccessUnit->rpm = RPM_Unused;
				}
			}
		}
		if(m_pCurrentAccessUnit->rpm == RPM_LongTermReference && memOp6) m_pCurrentAccessUnit->rpm = RPM_ShortTermReference;
	}
}
