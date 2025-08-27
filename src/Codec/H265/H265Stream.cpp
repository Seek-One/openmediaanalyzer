#include <iostream>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <sstream>

#include "../H26X/H26XErrorsMsg.h"

#include "H265BitstreamReader.h"
#include "H265AccessUnit.h"
#include "H265GOP.h"
#include "H265SEI.h"

#include "H265Stream.h"

H265Stream::H265Stream():
	firstPicture(true), endOfSequenceFlag(false), 
	m_pActiveVPS(nullptr), m_pActiveSPS(nullptr), m_pActivePPS(nullptr),
	m_pCurrentAccessUnit(nullptr), m_pNextAccessUnit(nullptr)
{}

H265Stream::~H265Stream(){
	m_GOPs.clear();
	H265PPS::PPSMap.clear();
	H265SPS::SPSMap.clear();
	H265VPS::VPSMap.clear();
	if(m_pNextAccessUnit) delete m_pNextAccessUnit;
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
	for(uint32_t i = 0;i < count;++i) {
		removedAccessUnits += m_GOPs.front()->size();
		m_GOPs.pop_front();
	}
	return removedAccessUnits;
}

uint32_t H265Stream::accessUnitCount() const
{
	return std::accumulate(m_GOPs.begin(), m_GOPs.end(), 0, [](uint32_t accumulator, const std::unique_ptr<H265GOP>& ptr){
		return accumulator + ptr->size();
	});
}

std::list<H265AccessUnit*> H265Stream::getLastAccessUnits(uint32_t count) const
{
	std::list<H265AccessUnit*> pAccessUnits;
	for(auto itGOP = m_GOPs.rbegin(); itGOP != m_GOPs.rend() && count > 0;++itGOP){
		H265GOP* pGOP = itGOP->get();
		for(auto itAccessUnit = pGOP->accessUnits.rbegin(); itAccessUnit != pGOP->accessUnits.rend() && count > 0;++itAccessUnit){
			pAccessUnits.push_front(itAccessUnit->get());
			count--;
		}
	}
	return pAccessUnits;
}

std::vector<H265AccessUnit*> H265Stream::getAccessUnits() const
{
	std::vector<H265AccessUnit*> pAccessUnits;
	for(auto itGOP = m_GOPs.begin(); itGOP != m_GOPs.end();++itGOP){
		H265GOP* pGOP = itGOP->get();
		for(auto itAccessUnit = pGOP->accessUnits.begin(); itAccessUnit != pGOP->accessUnits.end();++itAccessUnit){
			pAccessUnits.push_back(itAccessUnit->get());
		}
	}
	return pAccessUnits;
}

bool H265Stream::parsePacket(const uint8_t* pPacketData, uint32_t iPacketLength)
{
	std::vector<NALData> listNAL = splitNAL(pPacketData, iPacketLength);

	bool bRes = true;
	for (uint32_t i = 0; i < listNAL.size() && bRes; ++i) {
		bRes = parseNAL(listNAL[i].pData, (uint32_t)listNAL[i].iLength);
	}

	return bRes;
}

/* Called in the case of a finite stream,
   completes the processing of the very last GOP
   which is normally done upon detecting a new GOP 
 */
void H265Stream::lastPacketParsed()
{
	H265GOP* lastGOP = m_GOPs.back().get();
	lastGOP->setAccessUnitDecodability();
	lastGOP->validate();

	errors.add(lastGOP->errors);
	lastGOP->errors.clear();
	errors.clear(ERR_MSG_LIMIT);
}

bool H265Stream::parseNAL(const uint8_t* pNALData, uint32_t iNALLength)
{
	H265BitstreamReader bitstreamReader(pNALData, iNALLength);

	try {
		bitstreamReader.readNALHeader(m_currentNAL);
	} catch(const std::runtime_error& e) {
		errors.add(H26XError::Major, std::string("[NAL Header] ").append(e.what()));
	}

	if(!m_pCurrentAccessUnit) {
		m_pCurrentAccessUnit = new H265AccessUnit();
		m_pNextAccessUnit = new H265AccessUnit();
		m_GOPs.push_back(std::make_unique<H265GOP>());
		m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
	}
	H265Slice* currentAccessUnitSlice = m_pCurrentAccessUnit->slice();
	switch (m_currentNAL.nal_unit_type) {
		case H265NAL::UnitType_VPS:{
			m_pActiveVPS = new H265VPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			try{
				bitstreamReader.readVPS(*m_pActiveVPS);
			} catch(const std::runtime_error& e){ 
				m_pActiveVPS->errors.add(H26XError::Major, std::string("[VPS] ").append(e.what()));
				m_pActiveVPS->completelyParsed = false;
			}
			if(m_pActiveVPS->nuh_layer_id == 0 && currentAccessUnitSlice){
				newAccessUnit();
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265VPS>(m_pActiveVPS));
			H265VPS::VPSMap.insert_or_assign(m_pActiveVPS->vps_video_parameter_set_id, m_pActiveVPS);
			break;
		}
		case H265NAL::UnitType_SPS:{
			m_pActiveSPS = new H265SPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			try {
				bitstreamReader.readSPS(*m_pActiveSPS);
			} catch(const std::runtime_error& e){ 
				m_pActiveSPS->errors.add(H26XError::Major, std::string("[SPS] ").append(e.what()));
				m_pActiveSPS->completelyParsed = false;
			}
			if(m_pActiveSPS->nuh_layer_id == 0 && currentAccessUnitSlice){
				newAccessUnit();
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265SPS>(m_pActiveSPS));
			H265SPS::SPSMap.insert_or_assign(m_pActiveSPS->sps_seq_parameter_set_id, m_pActiveSPS);
			break;
		}
		case H265NAL::UnitType_PPS:{
			m_pActivePPS = new H265PPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			try {
				bitstreamReader.readPPS(*m_pActivePPS);
			} catch(const std::runtime_error& e){ 
				m_pActivePPS->errors.add(H26XError::Major, std::string("[PPS] ").append(e.what()));
				m_pActivePPS->completelyParsed = false;
			}
			if(m_pActivePPS->nuh_layer_id == 0 && currentAccessUnitSlice){
				newAccessUnit();
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265PPS>(m_pActivePPS));
			H265PPS::PPSMap.insert_or_assign(m_pActivePPS->pps_pic_parameter_set_id, m_pActivePPS);
			break;
		}
		case H265NAL::UnitType_SEI_PREFIX:
		case H265NAL::UnitType_SEI_SUFFIX: {
			H265SEI* pSEI = new H265SEI(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			try { bitstreamReader.readSEI(*pSEI);
			} catch(const std::runtime_error& e){ 
				pSEI->errors.add(H26XError::Minor, std::string("[SEI] ").append(e.what()));
				pSEI->completelyParsed = false;
			}
			if(pSEI->nal_unit_type == H265NAL::UnitType_SEI_PREFIX && pSEI->nuh_layer_id == 0 && currentAccessUnitSlice) newAccessUnit();
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265SEI>(pSEI));
			break;
		}
		default:{
			if (!m_currentNAL.isSlice()) {
				std::cerr << "[Stream] Unsupported NAL unit type : " << (int)m_currentNAL.nal_unit_type << "\n";
				break;
			}
			H265Slice* pSlice = new H265Slice(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			if(firstPicture || endOfSequenceFlag) pSlice->NoRaslOutputFlag = 1;
			try { bitstreamReader.readSlice(*pSlice, getAccessUnits(), m_pNextAccessUnit);
			} catch(const std::runtime_error& e){ 
				pSlice->errors.add(H26XError::Major, std::string("[Slice] ").append(e.what()));
				pSlice->completelyParsed = false;
			}
			if(pSlice->nuh_layer_id == 0 && pSlice->first_slice_segment_in_pic_flag && m_pCurrentAccessUnit->slice()){
				newAccessUnit();
			}
			if(pSlice->slice_type == H265Slice::SliceType_I && pSlice->first_slice_segment_in_pic_flag){ // I-frame marks new GOP
				// move access unit inserted in the previous GOP to a new one,
				// unless it's the very first access unit of the GOP (access units can start with
				// non-slice NAL units)
				if(m_GOPs.back()->hasSlice){ 
					H265GOP* previousGOP = m_GOPs.back().get();
					m_GOPs.push_back(std::make_unique<H265GOP>());
					m_GOPs.back()->accessUnits.push_back(std::move(previousGOP->accessUnits.back()));
					previousGOP->accessUnits.pop_back();
					previousGOP->setAccessUnitDecodability();
					previousGOP->validate();

					errors.add(previousGOP->errors);
					previousGOP->errors.clear();
					errors.clear(ERR_MSG_LIMIT);
				}
				if(pSlice->isIDR()) m_GOPs.back()->hasIDR = true;
			} 
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265Slice>(pSlice));
			if(pSlice->first_slice_segment_in_pic_flag){
				m_pCurrentAccessUnit->PicOrderCntVal = pSlice->PicOrderCntVal;
				m_pCurrentAccessUnit->PicOrderCntMsb = pSlice->PicOrderCntMsb;
			}
			checkPrevRefPicList(m_pCurrentAccessUnit, pSlice);
			firstPicture = false; endOfSequenceFlag = false;
			m_GOPs.back()->hasSlice = true;
			break;
		}
	}
	return true;
}

void H265Stream::checkPrevRefPicList(H265AccessUnit* pCurrentAccessUnit, H265Slice* pSlice)
{
	std::unordered_set<int32_t> seenPOC, missingPOC;
	for(H265AccessUnit* pAccessUnit : m_GOPs.back()->getAccessUnits()){
		seenPOC.insert(pAccessUnit->PicOrderCntVal);
	}
	for(int32_t previousShortTermRefFramePOC : pSlice->PocStCurrBefore){
		if(seenPOC.find(previousShortTermRefFramePOC) == seenPOC.end()){
			missingPOC.insert(previousShortTermRefFramePOC);
		}
	}
	if(!missingPOC.empty()){
        std::ostringstream missingPOCStr;
		missingPOCStr << "[Slice] Missing reference frames : [" << (*missingPOC.begin());
        auto missingPOCIt = missingPOC.begin();
        missingPOCIt++;
        for(;missingPOCIt != missingPOC.end();missingPOCIt++){
			missingPOCStr << (*missingPOCIt) << ", ";
		}
        missingPOCStr << "]";
        pSlice->errors.add(H26XError::Major, missingPOCStr.str());
    } 
}

void H265Stream::newAccessUnit(){
	m_GOPs.back()->setAccessUnitDecodability();
	m_pCurrentAccessUnit = m_pNextAccessUnit;
	m_pNextAccessUnit = new H265AccessUnit();
	m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
}