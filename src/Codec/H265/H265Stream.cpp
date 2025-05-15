#include <iostream>
#include <algorithm>
#include <numeric>

#include "H265BitstreamReader.h"
#include "H265AccessUnit.h"

#include "H265Stream.h"

H265Stream::H265Stream():
	firstPicture(true), endOfSequenceFlag(false), m_pCurrentAccessUnit(nullptr), m_pNextAccessUnit(nullptr), 
	m_pActiveVPS(nullptr), m_pActiveSPS(nullptr), m_pActivePPS(nullptr)
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
	for(int i = 0;i < count;++i) {
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
		m_pNextAccessUnit = new H265AccessUnit();
		m_GOPs.push_back(std::make_unique<H265GOP>());
		m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
	}
	H265Slice* currentAccessUnitSlice = m_pCurrentAccessUnit->slice();
	switch (m_currentNAL.nal_unit_type) {
		case H265NAL::UnitType_VPS:{
			m_pActiveVPS = new H265VPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			try{ bitstreamReader.readVPS(*m_pActiveVPS);
			} catch(const std::runtime_error& e){ m_pActiveVPS->majorErrors.push_back(std::string("[VPS] ").append(e.what())); }
			if(m_pActiveVPS->nuh_layer_id == 0 && currentAccessUnitSlice){
				m_GOPs.back()->setAccessUnitDecodability();
				m_pCurrentAccessUnit = m_pNextAccessUnit;
				m_pNextAccessUnit = new H265AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265VPS>(m_pActiveVPS));
			H265VPS::VPSMap.insert_or_assign(m_pActiveVPS->vps_video_parameter_set_id, m_pActiveVPS);
			break;
		}
		case H265NAL::UnitType_SPS:{
			m_pActiveSPS = new H265SPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			try { bitstreamReader.readSPS(*m_pActiveSPS);
			} catch(const std::runtime_error& e){ m_pActiveSPS->majorErrors.push_back(std::string("[SPS] ").append(e.what())); }
			if(m_pActiveSPS->nuh_layer_id == 0 && currentAccessUnitSlice){
				m_GOPs.back()->setAccessUnitDecodability();
				m_pCurrentAccessUnit = m_pNextAccessUnit;
				m_pNextAccessUnit = new H265AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265SPS>(m_pActiveSPS));
			H265SPS::SPSMap.insert_or_assign(m_pActiveSPS->sps_seq_parameter_set_id, m_pActiveSPS);
			break;
		}
		case H265NAL::UnitType_PPS:{
			m_pActivePPS = new H265PPS(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			try { bitstreamReader.readPPS(*m_pActivePPS);
			} catch(const std::runtime_error& e){ m_pActiveVPS->majorErrors.push_back(std::string("[PPS] ").append(e.what())); }
			if(m_pActivePPS->nuh_layer_id == 0 && currentAccessUnitSlice){
				m_GOPs.back()->setAccessUnitDecodability();
				m_pCurrentAccessUnit = m_pNextAccessUnit;
				m_pNextAccessUnit = new H265AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
			}
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265PPS>(m_pActivePPS));
			H265PPS::PPSMap.insert_or_assign(m_pActivePPS->pps_pic_parameter_set_id, m_pActivePPS);
			break;
		}
		case H265NAL::UnitType_SEI_PREFIX:
		case H265NAL::UnitType_SEI_SUFFIX: {
			H265SEI* pSEI = new H265SEI(m_currentNAL.forbidden_zero_bit, m_currentNAL.nal_unit_type, m_currentNAL.nuh_layer_id, m_currentNAL.nuh_temporal_id_plus1, iNALLength, pNALData);
			try { bitstreamReader.readSEI(*pSEI);
			} catch(const std::runtime_error& e){ pSEI->minorErrors.push_back(std::string("[SEI] ").append(e.what())); }
			if(pSEI->nal_unit_type == H265NAL::UnitType_SEI_PREFIX && pSEI->nuh_layer_id == 0 && currentAccessUnitSlice){
				m_GOPs.back()->setAccessUnitDecodability();
				m_pCurrentAccessUnit = m_pNextAccessUnit;
				m_pNextAccessUnit = new H265AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
			}
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
			} catch(const std::runtime_error& e){ pSlice->majorErrors.push_back(std::string("[Slice] ").append(e.what())); }
			if(pSlice->nuh_layer_id == 0 && pSlice->first_slice_segment_in_pic_flag && m_pCurrentAccessUnit->slice()){
				m_GOPs.back()->setAccessUnitDecodability();
				m_pCurrentAccessUnit = m_pNextAccessUnit;
				m_pNextAccessUnit = new H265AccessUnit();
				m_GOPs.back()->accessUnits.push_back(std::unique_ptr<H265AccessUnit>(m_pCurrentAccessUnit));
			}
			if(pSlice->isIDR() && pSlice->first_slice_segment_in_pic_flag){ // IDR-frame marks new GOP
				// move access unit inserted in the previous GOP to a new one,
				// unless it's the very first access unit of the GOP (access units can start with
				// non-slice NAL units)
				if(m_GOPs.back()->hasSlice){ 
					H265GOP* previousGOP = m_GOPs.back().get();
					m_GOPs.push_back(std::make_unique<H265GOP>());
					m_GOPs.back()->accessUnits.push_back(std::move(previousGOP->accessUnits.back()));
					previousGOP->accessUnits.pop_back();
					previousGOP->accessUnits.back()->decodable = true;
					previousGOP->validate();

					minorErrors.insert(minorErrors.end(), previousGOP->minorErrors.begin(), previousGOP->minorErrors.end());
					previousGOP->minorErrors.clear();
					for(int i = 0;minorErrors.size() > ERR_MSG_LIMIT && i < minorErrors.size() - ERR_MSG_LIMIT;++i) minorErrors.pop_front();

					majorErrors.insert(majorErrors.end(), previousGOP->majorErrors.begin(), previousGOP->majorErrors.end());
					previousGOP->majorErrors.clear();
					for(int i = 0;majorErrors.size() > ERR_MSG_LIMIT && i < majorErrors.size() - ERR_MSG_LIMIT;++i) majorErrors.pop_front();
				}
				if(pSlice->nal_unit_type == H265NAL::UnitType_IDR_N_LP || pSlice->nal_unit_type == H265NAL::UnitType_IDR_W_RADL) m_GOPs.back()->hasIDR = true;
			} 
			m_pCurrentAccessUnit->addNALUnit(std::unique_ptr<H265Slice>(pSlice));
			if(pSlice->first_slice_segment_in_pic_flag){
				m_pCurrentAccessUnit->PicOrderCntVal = pSlice->PicOrderCntVal;
				m_pCurrentAccessUnit->PicOrderCntMsb = pSlice->PicOrderCntMsb;
			}
			firstPicture = false; endOfSequenceFlag = false;
			m_GOPs.back()->hasSlice = true;
			break;
		}
	}
	return true;
}