#include <iostream>

#include "H265BitstreamReader.h"
#include "H265Stream.h"

H265Stream::H265Stream()
{
	TargetDecLayerIdList = 0;
	HighestTid = 0;
	SubPicHrdFlag = false;
	IRAPPicture = false;
	bIsFirstPicture = false;
	NoRaslOutputFlag = false;
	HandleCraAsBlaFlag = false;
	PicOrderCntVal = 0;
	prevPicOrderCntLsb = 0;
	prevPicOrderCntMsb = 0;
	MaxPicOrderCntLsb = 0;
}

Size H265Stream::getUncroppedPictureSize() const
{
	return Size(m_sps.pic_width_in_luma_samples, m_sps.pic_height_in_luma_samples);
}

Size H265Stream::getPictureSize() const
{
	Size size;
	size.width =  (m_sps.pic_width_in_luma_samples - ((m_sps.SubWidthC * m_sps.conf_win_right_offset) + 1)) - (m_sps.conf_win_left_offset * m_sps.SubWidthC) + 1;
	size.height = (m_sps.pic_height_in_luma_samples - ((m_sps.SubHeightC * m_sps.conf_win_bottom_offset) + 1)) - (m_sps.SubHeightC * m_sps.conf_win_top_offset) + 1;
	return size;
}

const H265NAL& H265Stream::getNAL() const
{
	return m_currentNAL;
}

const H265VPS& H265Stream::getVPS() const
{
	return m_vps;
}

const H265SPS& H265Stream::getSPS() const
{
	return m_sps;
}

const H265PPS& H265Stream::getPPS() const
{
	return m_pps;
}

const std::vector<H265Slice>& H265Stream::getSliceList() const
{
	return m_listSlices;
}

bool H265Stream::parsePacket(uint8_t* pPacketData, uint32_t iPacketLength)
{
	m_listSlices.clear();

	std::vector<NALData> listNAL = splitNAL(pPacketData, iPacketLength);

	bool bRes = true;
	for (int i = 0; i < listNAL.size() && bRes; ++i) {
		bRes = parseNAL(listNAL[i].pData, (uint32_t)listNAL[i].iLength);
	}

	return bRes;
}

void H265Stream::startDecodingProcess()
{
	TargetDecLayerIdList = 0; // In future, this variable must be modified by "external means" so we keep it
	HighestTid = m_sps.sps_max_sub_layers_minus1; // Same, this variable must be override by "external means"
	SubPicHrdFlag = false; // Set to false until we don't handle HRD syntax
	HandleCraAsBlaFlag = false; // Same, this variable must be override by "external means"

	if (m_currentNAL.nuh_layer_id != 0) {
		std::cerr << "[H265::Stream] Multi layer case not handled\n";
		return;
	}

	// 8.1.3 Decoding process for a coded picture with nuh_layer_id equal to 0
	if (m_currentNAL.nal_unit_type == H265NAL::UnitType_BLA_W_LP || m_currentNAL.nal_unit_type == H265NAL::UnitType_CRA_NUT) {
		std::cerr << "[H265::Stream] UseAltCpbParamsFlag not handled\n";
		return;
	}

	int8_t IDRPicFlag = (m_currentNAL.nal_unit_type == H265NAL::UnitType_IDR_W_RADL) || (m_currentNAL.nal_unit_type == H265NAL::UnitType_IDR_N_LP);
	IRAPPicture = (m_currentNAL.nal_unit_type >= H265NAL::UnitType_BLA_W_LP) && (m_currentNAL.nal_unit_type <= H265NAL::UnitType_IRAP_VCL23);

	if ((m_currentNAL.nal_unit_type == H265NAL::UnitType_IDR_W_RADL) ||
		(m_currentNAL.nal_unit_type == H265NAL::UnitType_IDR_N_LP) ||
		(m_currentNAL.nal_unit_type == H265NAL::UnitType_BLA_W_LP) ||
		(m_currentNAL.nal_unit_type == H265NAL::UnitType_BLA_W_RADL) ||
		(m_currentNAL.nal_unit_type == H265NAL::UnitType_BLA_N_LP) ||
		bIsFirstPicture)
	{
		NoRaslOutputFlag = 1;
	}
	else if (HandleCraAsBlaFlag) {
		NoRaslOutputFlag = HandleCraAsBlaFlag;
	}
	else {
		HandleCraAsBlaFlag = 0;
		NoRaslOutputFlag = 0;
	}

	// 8.3.1 Decoding process for picture order count
	if (m_listSlices.size() == 0) {
		std::cerr << "[H265::Stream] No available slices to decode picture order count\n";
		return;
	}

	if (IDRPicFlag) {
		prevPicOrderCntLsb = 0;
		prevPicOrderCntMsb = 0;
	}

	MaxPicOrderCntLsb = 1 << (m_sps.log2_max_pic_order_cnt_lsb_minus4 + 4);
	uint32_t PicOrderCntMsb = 0;
	PicOrderCntVal = 0;
	const H265Slice& slice = m_listSlices[0];
	if (!IRAPPicture && NoRaslOutputFlag) {
		std::cerr << "[H265::Stream] Specific prevPicOrderCntLsb and prevPicOrderCntMsb not handled\n";
	}
	else if ((slice.slice_pic_order_cnt_lsb < prevPicOrderCntLsb) &&
			((prevPicOrderCntLsb - slice.slice_pic_order_cnt_lsb) >= (MaxPicOrderCntLsb / 2 )))
	{
		PicOrderCntMsb = prevPicOrderCntMsb + MaxPicOrderCntLsb;
	}
	else if ((slice.slice_pic_order_cnt_lsb > prevPicOrderCntLsb) &&
			((slice.slice_pic_order_cnt_lsb - prevPicOrderCntLsb) > (MaxPicOrderCntLsb / 2 )))
	{
		PicOrderCntMsb = prevPicOrderCntMsb - MaxPicOrderCntLsb;
	}
	else {
		PicOrderCntMsb = prevPicOrderCntMsb;
	}

	PicOrderCntVal = PicOrderCntMsb + slice.slice_pic_order_cnt_lsb;

	if ( (m_currentNAL.nuh_temporal_id_plus1 - 1) == 0) {
		prevPicOrderCntLsb = slice.slice_pic_order_cnt_lsb;
		prevPicOrderCntMsb = PicOrderCntMsb;
	}

	// 8.3.2 Decoding process for reference picture set
	PocStCurrBefore.clear();
	PocStCurrAfter.clear();
	PocStFoll.clear();
	PocLtCurr.clear();
	PocLtFoll.clear();
	CurrDeltaPocMsbPresentFlag.clear();
	FollDeltaPocMsbPresentFlag.clear();

	if (!IDRPicFlag) {
		const H265ShortTermRefPicSet& CurrRsp = m_sps.short_term_ref_pic_set[slice.CurrRpsIdx];

		for (uint32_t i = 0; i < CurrRsp.NumNegativePics; ++i) {
			if (CurrRsp.UsedByCurrPicS0[i]) {
				PocStCurrBefore.push_back(PicOrderCntVal + CurrRsp.DeltaPocS0[i]);
			} else {
				PocStFoll.push_back(PicOrderCntVal + CurrRsp.DeltaPocS0[i]);
			}
		}

		for (uint32_t i = 0; i < CurrRsp.NumPositivePics; ++i) {
			if (CurrRsp.UsedByCurrPicS1[i]) {
				PocStCurrAfter.push_back(PicOrderCntVal + CurrRsp.DeltaPocS1[i]);
			} else {
				PocStFoll.push_back(PicOrderCntVal + CurrRsp.DeltaPocS1[i]);
			}
		}

		for (uint32_t i = 0; i < slice.num_long_term_sps + slice.num_long_term_pics; ++i) {
			uint32_t pocLt = slice.PocLsbLt[i];

			if (slice.delta_poc_msb_present_flag[i]) {
				pocLt += PicOrderCntVal - slice.DeltaPocMsbCycleLt[i] * MaxPicOrderCntLsb - (PicOrderCntVal & (MaxPicOrderCntLsb - 1));
			}

			if (slice.UsedByCurrPicLt[i]) {
				PocLtCurr.push_back(pocLt);
				CurrDeltaPocMsbPresentFlag.push_back(slice.delta_poc_msb_present_flag[i]);
			} else {
				PocLtFoll.push_back(pocLt);
				FollDeltaPocMsbPresentFlag.push_back(slice.delta_poc_msb_present_flag[i]);
			}
		}
	}
}

void H265Stream::computeRef()
{
	const H265Slice& slice = m_listSlices[0];

	// 8.3.4 Decoding process for reference picture lists construction
	const RefPicListsModification& ref_pic_lists_modification = slice.ref_pic_lists_modification;
	uint32_t rIdx = 0;
	if(slice.slice_type == H265Slice::SliceType_P) {
		while (rIdx < slice.NumRpsCurrTempList0) {
			for (int i = 0; i < PocStCurrBefore.size() && rIdx < slice.NumRpsCurrTempList0; rIdx++, i++) {
				RefPicListTemp0.push_back(RefPicSetStCurrBefore[i]);
			}
			for (int i = 0; i < PocStCurrAfter.size() && rIdx < slice.NumRpsCurrTempList0; rIdx++, i++) {
				RefPicListTemp0.push_back(RefPicSetStCurrAfter[i]);
			}
			for (int i = 0; i < PocLtCurr.size() && rIdx < slice.NumRpsCurrTempList0; rIdx++, i++) {
				RefPicListTemp0.push_back(RefPicSetLtCurr[i]);
			}
			if (m_pps.pps_extension_present_flag) {
				std::cerr << "[H265] pps_extension_present_flag not handled\n";
				//RefPicListTemp0[rIdx++] = currPic;
			}
		}
		for (rIdx = 0; rIdx <= slice.num_ref_idx_l0_active_minus1; rIdx++) {
			int val = ref_pic_lists_modification.ref_pic_list_modification_flag_l0
					  ? RefPicListTemp0[ref_pic_lists_modification.list_entry_l0[rIdx]] : RefPicListTemp0[rIdx];
			RefPicList0.push_back(val);
		}
		/*
		if( m_pps.pps_curr_pic_ref_enabled_flag && !ref_pic_lists_modification.ref_pic_list_modification_flag_l0 &&
			NumRpsCurrTempList0 > ( num_ref_idx_l0_active_minus1 + 1 ) ) {
			RefPicList0[num_ref_idx_l0_active_minus1] = currPic
		}*/
	}

	if(slice.slice_type == H265Slice::SliceType_B) {
		rIdx = 0;
		while (rIdx < slice.NumRpsCurrTempList1) {
			for (int i = 0; i < PocStCurrAfter.size() && rIdx < slice.NumRpsCurrTempList1; rIdx++, i++) {
				RefPicListTemp1.push_back(RefPicSetStCurrAfter[i]);
			}
			for (int i = 0; i < PocStCurrBefore.size() && rIdx < slice.NumRpsCurrTempList1; rIdx++, i++) {
				RefPicListTemp1.push_back(RefPicSetStCurrBefore[i]);
			}
			for (int i = 0; i < PocLtCurr.size() && rIdx < slice.NumRpsCurrTempList1; rIdx++, i++) {
				RefPicListTemp1.push_back(RefPicSetLtCurr[i]);
			}
			/*
			if (m_pps.pps_curr_pic_ref_enabled_flag) {
				RefPicListTemp1[rIdx++] = currPic;
			}*/
		}
		for (rIdx = 0; rIdx <= slice.num_ref_idx_l1_active_minus1; rIdx++)
		{
			int val = ref_pic_lists_modification.ref_pic_list_modification_flag_l1 ? RefPicListTemp1[ref_pic_lists_modification.list_entry_l1[rIdx]] : RefPicListTemp1[rIdx];
			RefPicList1.push_back(val);
		}
	}
}

bool H265Stream::parseNAL(uint8_t* pNALData, uint32_t iNALLength)
{
	H265BitstreamReader bitstreamReader(pNALData, iNALLength);
	bitstreamReader.readNALHeader(m_currentNAL);

	if (m_currentNAL.forbidden_zero_bit != 0) {
		std::cerr << "[H265::Stream] Invalid NAL: forbidden_zero_bit no equal to 0\n";
		return false;
	}

	switch (m_currentNAL.nal_unit_type) {
	case H265NAL::UnitType_VPS:
		bitstreamReader.readVPS(m_vps);
		/*
            {
                H265Debug dbg;
                dbg.write("vps", m_vps);
                dbg.print();
            }*/
		break;

	case H265NAL::UnitType_SPS:
		bitstreamReader.readSPS(m_sps);
		/*
            {
                H265Debug dbg;
                dbg.write("sps", m_sps);
                dbg.print();
            }*/
		break;

	case H265NAL::UnitType_PPS:
		bitstreamReader.readPPS(m_pps);
		/*
			{
				H265Debug dbg;
				dbg.write("pps", m_pps);
				dbg.print();
			}
		 */
		break;

	default:
		if (m_currentNAL.isSlice()) {
			bIsFirstPicture = false;
			H265Slice slice;
			bitstreamReader.readSlice(slice, m_currentNAL, m_sps, m_pps);
			m_listSlices.push_back(slice);
		}
		break;
	}

	return true;
}