#include <iostream>
#include <algorithm>
#include <numeric>

#include "H265BitstreamReader.h"
#include "H265AccessUnit.h"

#include "H265Stream.h"

H265Stream::H265Stream():
	firstPicture(true), endOfSequenceFlag(false), m_pCurrentAccessUnit(nullptr), m_pActiveVPS(nullptr), m_pActiveSPS(nullptr), m_pActivePPS(nullptr)
{}

H265Stream::~H265Stream(){
	m_GOPs.clear();
	H265PPS::PPSMap.clear();
	H265SPS::SPSMap.clear();
	H265VPS::VPSMap.clear();
}

std::deque<H265GOP*> H265Stream::getGOPs() const 
{
	std::deque<H265GOP*> GOPs;
	std::transform(m_GOPs.begin(), m_GOPs.end(), std::back_inserter(GOPs),
					[](const std::unique_ptr<H265GOP>& ptr) { return ptr.get(); });
	return GOPs;
}

uint32_t H265Stream::popFrontGOPs(uint32_t count){
	uint32_t removedAccessUnits = 0;
	for(int i = 0;i < count;++i) {
		removedAccessUnits += m_GOPs.front()->count();
		m_GOPs.pop_front();
	}
	return removedAccessUnits;
}

uint32_t H265Stream::accessUnitCount() const
{
	return std::accumulate(m_GOPs.begin(), m_GOPs.end(), 0, [](uint32_t accumulator, const std::unique_ptr<H265GOP>& ptr){
		return accumulator + ptr->count();
	});
}

std::list<H265AccessUnit*> H265Stream::getLastAccessUnits(uint32_t count) const
{
	std::list<H265AccessUnit*> pAccessUnits = std::list<H265AccessUnit*>();
	for(auto itGOP = m_GOPs.rbegin(); itGOP != m_GOPs.rend() && count > 0;++itGOP){
		H265GOP* pGOP = itGOP->get();
		for(auto itAccessUnit = pGOP->accessUnits.rbegin(); itAccessUnit != pGOP->accessUnits.rend() && count > 0;++itAccessUnit){
			pAccessUnits.push_front(itAccessUnit->get());
			count--;
		}
	}
	return pAccessUnits;
}


Size H265Stream::getUncroppedPictureSize() const
{
	return Size(m_pActiveSPS->pic_width_in_luma_samples, m_pActiveSPS->pic_height_in_luma_samples);
}

Size H265Stream::getPictureSize() const
{
	Size size;
	size.width =  (m_pActiveSPS->pic_width_in_luma_samples - ((m_pActiveSPS->SubWidthC * m_pActiveSPS->conf_win_right_offset) + 1)) - (m_pActiveSPS->conf_win_left_offset * m_pActiveSPS->SubWidthC) + 1;
	size.height = (m_pActiveSPS->pic_height_in_luma_samples - ((m_pActiveSPS->SubHeightC * m_pActiveSPS->conf_win_bottom_offset) + 1)) - (m_pActiveSPS->SubHeightC * m_pActiveSPS->conf_win_top_offset) + 1;
	return size;
}

bool H265Stream::parsePacket(uint8_t* pPacketData, uint32_t iPacketLength)
{
	std::vector<NALData> listNAL = splitNAL(pPacketData, iPacketLength);

	bool bRes = true;
	for (int i = 0; i < listNAL.size() && bRes; ++i) {
		bRes = parseNAL(listNAL[i].pData, (uint32_t)listNAL[i].iLength);
	}

	return bRes;
}

bool H265Stream::parseNAL(uint8_t* pNALData, uint32_t iNALLength)
{
	H265BitstreamReader bitstreamReader(pNALData, iNALLength);
	bitstreamReader.readNALHeader(m_currentNAL);

	if(!m_pCurrentAccessUnit) {
		m_pCurrentAccessUnit = new H265AccessUnit();
		m_GOPs.push_back(std::make_unique<H265GOP>());
		m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
	}
	H265Slice* currentAccessUnitSlice = m_pCurrentAccessUnit->slice();
	switch (m_currentNAL.nal_unit_type) {
		case H265NAL::UnitType_VPS:{
			m_pActiveVPS = new H265VPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			bitstreamReader.readVPS(*m_pActiveVPS);
			if(m_pActiveVPS->nuh_layer_id == 0 && currentAccessUnitSlice){
				m_pCurrentAccessUnit = new H265AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265VPS>(m_pActiveVPS));
			H265VPS::VPSMap.insert_or_assign(m_pActiveVPS->vps_video_parameter_set_id, m_pActiveVPS);
			break;
		}
		case H265NAL::UnitType_SPS:{
			m_pActiveSPS = new H265SPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			bitstreamReader.readSPS(*m_pActiveSPS);
			if(m_pActiveSPS->nuh_layer_id == 0 && currentAccessUnitSlice){
				m_pCurrentAccessUnit = new H265AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265SPS>(m_pActiveSPS));
			H265SPS::SPSMap.insert_or_assign(m_pActiveSPS->sps_seq_parameter_set_id, m_pActiveSPS);
			break;
		}
		case H265NAL::UnitType_PPS:{
			m_pActivePPS = new H265PPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			bitstreamReader.readPPS(*m_pActivePPS);
			if(m_pActivePPS->nuh_layer_id == 0 && currentAccessUnitSlice){
				m_pCurrentAccessUnit = new H265AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265PPS>(m_pActivePPS));
			H265PPS::PPSMap.insert_or_assign(m_pActivePPS->pps_pic_parameter_set_id, m_pActivePPS);
			break;
		}
		case H265NAL::UnitType_SEI_PREFIX:
		case H265NAL::UnitType_SEI_SUFFIX: {
			H265SEI* pSEI = new H265SEI(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			bitstreamReader.readSEI(*pSEI);
			if(pSEI->nal_unit_type == H265NAL::UnitType_SEI_PREFIX && pSEI->nuh_layer_id == 0 && currentAccessUnitSlice){
				m_pCurrentAccessUnit = new H265AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265SEI>(pSEI));
			break;
		}
		default:{
			if (!m_currentNAL.isSlice()) std::cerr << "[H265 Stream] Unsupported NAL unit type : " << (int)m_currentNAL.nal_unit_type << "\n";
			H265Slice* pSlice = new H265Slice(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			if(firstPicture || endOfSequenceFlag) pSlice->NoRaslOutputFlag = 1;
			bitstreamReader.readSlice(*pSlice);
			if(pSlice->nuh_layer_id == 0 && currentAccessUnitSlice && currentAccessUnitSlice->slice_pic_order_cnt_lsb != pSlice->slice_pic_order_cnt_lsb){
				m_pCurrentAccessUnit = new H265AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265Slice>(pSlice));
			firstPicture = false; endOfSequenceFlag = false;
			break;
		}
	}
	return true;
}

void H265Stream::computePOC(){
	if(m_pCurrentAccessUnit->POCDecoded) return;
	H265Slice* pCurrentSlice = m_pCurrentAccessUnit->slice();
	H265SPS* pCurrentSPS = pCurrentSlice->getSPS();
	if(m_pCurrentAccessUnit->isIRAP() && pCurrentSlice->NoRaslOutputFlag){
		m_pCurrentAccessUnit->PicOrderCntMsb = 0;
	} else {
		H265AccessUnit* prevTid0Pic = nullptr;
		std::list<H265AccessUnit*> accessUnits = getLastAccessUnits(accessUnitCount());
		accessUnits.pop_back(); // remove current access unit
		for(auto accessUnitIt = accessUnits.rbegin();accessUnitIt != accessUnits.rend() && !prevTid0Pic;++accessUnitIt){
			H265AccessUnit* pAccessUnit = *accessUnitIt;
			H265Slice* pSlice = pAccessUnit->slice();
			if(pSlice && pSlice->TemporalId == 0 && !pAccessUnit->isRASL() && !pAccessUnit->isRADL() && !pAccessUnit->isSLNR()) prevTid0Pic = pAccessUnit;
		}
		if(!prevTid0Pic){
			std::cerr << "[H265 Stream] Couldn't compute POC for non-initial IRAP picture : missing prevTid0Pic\n";
			return;
		}
		uint32_t prevPicOrderCntLsb = prevTid0Pic->slice()->slice_pic_order_cnt_lsb;
		int32_t prevPicOrderCntMsb = prevTid0Pic->PicOrderCntMsb;
		if((pCurrentSlice->slice_pic_order_cnt_lsb < prevPicOrderCntLsb) &&
		   ((prevPicOrderCntLsb-pCurrentSlice->slice_pic_order_cnt_lsb) >= (pCurrentSPS->MaxPicOrderCntLsb/2))){
			m_pCurrentAccessUnit->PicOrderCntMsb = prevPicOrderCntMsb + pCurrentSPS->MaxPicOrderCntLsb;
		} else if((pCurrentSlice->slice_pic_order_cnt_lsb > prevPicOrderCntLsb) &&
		          ((pCurrentSlice->slice_pic_order_cnt_lsb - prevPicOrderCntLsb) > (pCurrentSPS->MaxPicOrderCntLsb/2))){
			m_pCurrentAccessUnit->PicOrderCntMsb = prevPicOrderCntMsb - pCurrentSPS->MaxPicOrderCntLsb;
		} else m_pCurrentAccessUnit->PicOrderCntMsb = prevPicOrderCntMsb;
	}
	m_pCurrentAccessUnit->PicOrderCntVal = m_pCurrentAccessUnit->PicOrderCntMsb + m_pCurrentAccessUnit->slice()->slice_pic_order_cnt_lsb;
	m_pCurrentAccessUnit->POCDecoded = true;
}

void H265Stream::computeRPS(){
	H265Slice* pCurrentSlice = m_pCurrentAccessUnit->slice();
	H265SPS* pCurrentSPS = pCurrentSlice->getSPS();
	std::list<H265AccessUnit*> accessUnits = getLastAccessUnits(accessUnitCount());
	accessUnits.pop_back(); // remove current access unit
	if(m_pCurrentAccessUnit->isIRAP() && pCurrentSlice->NoRaslOutputFlag){
		for(H265AccessUnit* pAccessUnit : accessUnits) {
			if(pAccessUnit->slice()->nuh_layer_id == pCurrentSlice->nuh_layer_id) pAccessUnit->status = H265AccessUnit::ReferenceStatus_Unused;
		}
	}
	if(m_pCurrentAccessUnit->isIDR()){
		m_pCurrentAccessUnit->PocStCurrBefore.resize(0);
		m_pCurrentAccessUnit->PocStCurrAfter.resize(0);
		m_pCurrentAccessUnit->PocStFoll.resize(0);
		m_pCurrentAccessUnit->PocLtCurr.resize(0);
		m_pCurrentAccessUnit->PocLtFoll.resize(0);
	} else {
		if(!pCurrentSPS) return;
		H265ShortTermRefPicSet currRPS = pCurrentSPS->short_term_ref_pic_set[pCurrentSlice->CurrRpsIdx];
		for(int i = 0;i < currRPS.NumNegativePics;++i){
			if(currRPS.UsedByCurrPicS0[i]) m_pCurrentAccessUnit->PocStCurrBefore.push_back(m_pCurrentAccessUnit->PicOrderCntVal + currRPS.DeltaPocS0[i]);
			else m_pCurrentAccessUnit->PocStFoll.push_back(m_pCurrentAccessUnit->PicOrderCntVal + currRPS.DeltaPocS0[i]);
		}
		for(int i = 0;i < currRPS.NumPositivePics;++i){
			if(currRPS.UsedByCurrPicS1[i]) m_pCurrentAccessUnit->PocStCurrAfter.push_back(m_pCurrentAccessUnit->PicOrderCntVal + currRPS.DeltaPocS1[i]);
			else m_pCurrentAccessUnit->PocStFoll.push_back(m_pCurrentAccessUnit->PicOrderCntVal + currRPS.DeltaPocS1[i]);
		}
		for(int i = 0;i < pCurrentSlice->num_long_term_sps + pCurrentSlice->num_long_term_pics;++i){
			uint32_t pocLt = pCurrentSlice->PocLsbLt[i];
			if(pCurrentSlice->delta_poc_msb_present_flag[i]){
				pocLt += m_pCurrentAccessUnit->PicOrderCntVal - pCurrentSlice->DeltaPocMsbCycleLt[i]*pCurrentSPS->MaxPicOrderCntLsb - 
						(m_pCurrentAccessUnit->PicOrderCntVal & (pCurrentSPS->MaxPicOrderCntLsb-1));
			}
			if(pCurrentSlice->UsedByCurrPicLt[i]){
				m_pCurrentAccessUnit->PocLtCurr.push_back(pocLt);
				m_pCurrentAccessUnit->CurrDeltaPocMsbPresentFlag.push_back(pCurrentSlice->delta_poc_msb_present_flag[i]);
			} else {
				m_pCurrentAccessUnit->PocLtFoll.push_back(pocLt);
				m_pCurrentAccessUnit->FollDeltaPocMsbPresentFlag.push_back(pCurrentSlice->delta_poc_msb_present_flag[i]);
			}
		}
	}

	for(H265AccessUnit* pAccessUnit : accessUnits) {
		if(pAccessUnit->slice()->nuh_layer_id == pCurrentSlice->nuh_layer_id) pAccessUnit->status = H265AccessUnit::ReferenceStatus_Unused;
	}
	// TODO: double check if all pictures in the DPB are eligible to be used as reference pictures
	m_pCurrentAccessUnit->RefPicSetLtCurr.resize(m_pCurrentAccessUnit->PocLtCurr.size());
	for(int i = 0;i < m_pCurrentAccessUnit->PocLtCurr.size();++i){
		m_pCurrentAccessUnit->RefPicSetLtCurr[i] = nullptr;
		if(!m_pCurrentAccessUnit->CurrDeltaPocMsbPresentFlag[i]){
			for(auto accessUnitIt = accessUnits.rbegin();accessUnitIt != accessUnits.rend() && !m_pCurrentAccessUnit->RefPicSetLtCurr[i];++accessUnitIt){
				if(((*accessUnitIt)->PicOrderCntVal & (pCurrentSPS->MaxPicOrderCntLsb-1)) == m_pCurrentAccessUnit->PocLtCurr[i] &&
					(*accessUnitIt)->slice()->nuh_layer_id == pCurrentSlice->nuh_layer_id){
					m_pCurrentAccessUnit->RefPicSetLtCurr[i] = *accessUnitIt;
					m_pCurrentAccessUnit->RefPicSetLtCurr[i]->status = H265AccessUnit::ReferenceStatus_LongTerm;
				}
			}
		} else {
			for(auto accessUnitIt = accessUnits.rbegin();accessUnitIt != accessUnits.rend() && !m_pCurrentAccessUnit->RefPicSetLtCurr[i];++accessUnitIt){
				if((*accessUnitIt)->PicOrderCntVal == m_pCurrentAccessUnit->PocLtCurr[i] &&
					(*accessUnitIt)->slice()->nuh_layer_id == pCurrentSlice->nuh_layer_id){
					m_pCurrentAccessUnit->RefPicSetLtCurr[i] = *accessUnitIt;
					m_pCurrentAccessUnit->RefPicSetLtCurr[i]->status = H265AccessUnit::ReferenceStatus_LongTerm;
				}
			}
		}
	}
	m_pCurrentAccessUnit->RefPicSetLtFoll.resize(m_pCurrentAccessUnit->PocLtFoll.size());
	for(int i = 0;i < m_pCurrentAccessUnit->PocLtFoll.size();++i){
		m_pCurrentAccessUnit->RefPicSetLtFoll[i] = nullptr;
		if(!m_pCurrentAccessUnit->FollDeltaPocMsbPresentFlag[i]){
			for(auto accessUnitIt = accessUnits.rbegin();accessUnitIt != accessUnits.rend() && !m_pCurrentAccessUnit->RefPicSetLtCurr[i];++accessUnitIt){
				if(((*accessUnitIt)->PicOrderCntVal & (pCurrentSPS->MaxPicOrderCntLsb-1)) == m_pCurrentAccessUnit->PocLtFoll[i] &&
					(*accessUnitIt)->slice()->nuh_layer_id == pCurrentSlice->nuh_layer_id){
					m_pCurrentAccessUnit->RefPicSetLtFoll[i] = *accessUnitIt;
					m_pCurrentAccessUnit->RefPicSetLtFoll[i]->status = H265AccessUnit::ReferenceStatus_LongTerm;
				}
			}
		} else {
			for(auto accessUnitIt = accessUnits.rbegin();accessUnitIt != accessUnits.rend() && !m_pCurrentAccessUnit->RefPicSetLtCurr[i];++accessUnitIt){
				if((*accessUnitIt)->PicOrderCntVal == m_pCurrentAccessUnit->PocLtFoll[i] &&
					(*accessUnitIt)->slice()->nuh_layer_id == pCurrentSlice->nuh_layer_id){
					m_pCurrentAccessUnit->RefPicSetLtFoll[i] = *accessUnitIt;
					m_pCurrentAccessUnit->RefPicSetLtFoll[i]->status = H265AccessUnit::ReferenceStatus_LongTerm;
				}
			}
		}
	}

	m_pCurrentAccessUnit->RefPicSetStCurrBefore.resize(m_pCurrentAccessUnit->PocStCurrBefore.size());
	for(int i = 0;i < m_pCurrentAccessUnit->PocStCurrBefore.size();++i){
		m_pCurrentAccessUnit->RefPicSetStCurrBefore[i] = nullptr;
		for(auto accessUnitIt = accessUnits.rbegin();accessUnitIt != accessUnits.rend() && !m_pCurrentAccessUnit->RefPicSetStCurrBefore[i];++accessUnitIt){
			if((*accessUnitIt)->PicOrderCntVal == m_pCurrentAccessUnit->PocStCurrBefore[i] && (*accessUnitIt)->slice()->nuh_layer_id == pCurrentSlice->nuh_layer_id){
				m_pCurrentAccessUnit->RefPicSetStCurrBefore[i] = *accessUnitIt;
				m_pCurrentAccessUnit->RefPicSetStCurrBefore[i]->status = H265AccessUnit::ReferenceStatus_ShortTerm;
			}
		}
	}
	m_pCurrentAccessUnit->RefPicSetStCurrAfter.resize(m_pCurrentAccessUnit->PocStCurrAfter.size());
	for(int i = 0;i < m_pCurrentAccessUnit->PocStCurrAfter.size();++i){
		m_pCurrentAccessUnit->RefPicSetStCurrAfter[i] = nullptr;
		for(auto accessUnitIt = accessUnits.rbegin();accessUnitIt != accessUnits.rend() && !m_pCurrentAccessUnit->RefPicSetStCurrAfter[i];++accessUnitIt){
			if((*accessUnitIt)->PicOrderCntVal == m_pCurrentAccessUnit->PocStCurrAfter[i] && (*accessUnitIt)->slice()->nuh_layer_id == pCurrentSlice->nuh_layer_id){
				m_pCurrentAccessUnit->RefPicSetStCurrAfter[i] = *accessUnitIt;
				m_pCurrentAccessUnit->RefPicSetStCurrAfter[i]->status = H265AccessUnit::ReferenceStatus_ShortTerm;
			}
		}
	}
	m_pCurrentAccessUnit->RefPicSetStFoll.resize(m_pCurrentAccessUnit->PocStFoll.size());
	for(int i = 0;i < m_pCurrentAccessUnit->PocStFoll.size();++i){
		m_pCurrentAccessUnit->RefPicSetStFoll[i] = nullptr;
		for(auto accessUnitIt = accessUnits.rbegin();accessUnitIt != accessUnits.rend() && !m_pCurrentAccessUnit->RefPicSetStFoll[i];++accessUnitIt){
			if((*accessUnitIt)->PicOrderCntVal == m_pCurrentAccessUnit->PocStFoll[i] && (*accessUnitIt)->slice()->nuh_layer_id == pCurrentSlice->nuh_layer_id){
				m_pCurrentAccessUnit->RefPicSetStFoll[i] = *accessUnitIt;
				m_pCurrentAccessUnit->RefPicSetStFoll[i]->status = H265AccessUnit::ReferenceStatus_ShortTerm;
			}
		}
	}
}

void H265Stream::computeRPL(){
	H265Slice* pCurrentSlice = m_pCurrentAccessUnit->slices().back();
	H265PPS* h265PPS = pCurrentSlice->getPPS();
	// 8.3.4 Decoding process for reference picture lists construction
	uint32_t NumRpsCurrTempList0 = std::max(pCurrentSlice->num_ref_idx_l0_active_minus1+1, pCurrentSlice->NumPicTotalCurr);
	uint32_t rIdx = 0;
	std::vector<H265AccessUnit*> RefPicListTemp0;
	if(pCurrentSlice->slice_type == H265Slice::SliceType_P) {
		while (rIdx < NumRpsCurrTempList0) {
			for (int i = 0; i < m_pCurrentAccessUnit->PocStCurrBefore.size() && rIdx < NumRpsCurrTempList0; rIdx++, i++) {
				RefPicListTemp0.push_back(m_pCurrentAccessUnit->RefPicSetStCurrBefore[i]);
			}
			for (int i = 0; i < m_pCurrentAccessUnit->PocStCurrAfter.size() && rIdx < NumRpsCurrTempList0; rIdx++, i++) {
				RefPicListTemp0.push_back(m_pCurrentAccessUnit->RefPicSetStCurrAfter[i]);
			}
			for (int i = 0; i < m_pCurrentAccessUnit->PocLtCurr.size() && rIdx < NumRpsCurrTempList0; rIdx++, i++) {
				RefPicListTemp0.push_back(m_pCurrentAccessUnit->RefPicSetLtCurr[i]);
			}
			if (h265PPS->pps_extension_present_flag) {
				std::cerr << "[H265 Slice] pps_extension_present_flag not handled\n";
				//RefPicListTemp0[rIdx++] = currPic;
			}
		}
		for (rIdx = 0; rIdx <= pCurrentSlice->num_ref_idx_l0_active_minus1; rIdx++) {
			H265AccessUnit* val = pCurrentSlice->ref_pic_lists_modification.ref_pic_list_modification_flag_l0
					  ? RefPicListTemp0[pCurrentSlice->ref_pic_lists_modification.list_entry_l0[rIdx]] : RefPicListTemp0[rIdx];
			pCurrentSlice->RefPicList0.push_back(val);
		}
		/*
		if( m_pps.pps_curr_pic_ref_enabled_flag && !ref_pic_lists_modification.ref_pic_list_modification_flag_l0 &&
			NumRpsCurrTempList0 > ( num_ref_idx_l0_active_minus1 + 1 ) ) {
			RefPicList0[num_ref_idx_l0_active_minus1] = currPic
		}*/
	}

	std::vector<H265AccessUnit*> RefPicListTemp1;
	uint32_t NumRpsCurrTempList1 = std::max(pCurrentSlice->num_ref_idx_l1_active_minus1+1, pCurrentSlice->NumPicTotalCurr);
	if(pCurrentSlice->slice_type == H265Slice::SliceType_B) {
		rIdx = 0;
		while (rIdx < NumRpsCurrTempList1) {
			for (int i = 0; i < m_pCurrentAccessUnit->PocStCurrAfter.size() && rIdx < NumRpsCurrTempList1; rIdx++, i++) {
				RefPicListTemp1.push_back(m_pCurrentAccessUnit->RefPicSetStCurrAfter[i]);
			}
			for (int i = 0; i < m_pCurrentAccessUnit->PocStCurrBefore.size() && rIdx < NumRpsCurrTempList1; rIdx++, i++) {
				RefPicListTemp1.push_back(m_pCurrentAccessUnit->RefPicSetStCurrBefore[i]);
			}
			for (int i = 0; i < m_pCurrentAccessUnit->PocLtCurr.size() && rIdx < NumRpsCurrTempList1; rIdx++, i++) {
				RefPicListTemp1.push_back(m_pCurrentAccessUnit->RefPicSetLtCurr[i]);
			}
			/*
			if (m_pps.pps_curr_pic_ref_enabled_flag) {
				RefPicListTemp1[rIdx++] = currPic;
			}*/
		}
		for (rIdx = 0; rIdx <= pCurrentSlice->num_ref_idx_l1_active_minus1; rIdx++)
		{
			H265AccessUnit* val = pCurrentSlice->ref_pic_lists_modification.ref_pic_list_modification_flag_l1 ? RefPicListTemp1[pCurrentSlice->ref_pic_lists_modification.list_entry_l1[rIdx]] : RefPicListTemp1[rIdx];
			pCurrentSlice->RefPicList1.push_back(val);
		}
	}
}