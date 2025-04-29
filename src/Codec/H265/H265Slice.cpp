#include <iostream>
#include <sstream>
#include <algorithm>

#include "H265PPS.h"
#include "H265SPS.h"
#include "H265VPS.h"

#include "H265Slice.h"

RefPicListsModification::RefPicListsModification()
{
	ref_pic_list_modification_flag_l0 = 0;
	ref_pic_list_modification_flag_l1 = 0;
}

std::vector<std::string> RefPicListsModification::dump_fields(const H265Slice& slice) const{
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "ref_pic_list_modification_flag_l0:" << ref_pic_list_modification_flag_l0).str());
	if(ref_pic_list_modification_flag_l0){
		for(int i = 0;i <= slice.num_ref_idx_l0_active_minus1;++i) fields.push_back((std::ostringstream() << "  list_entry_l0[" << i << "]:" << list_entry_l0[i]).str());
	}
	if(slice.slice_type == H265Slice::SliceType_B){
		fields.push_back((std::ostringstream() << "ref_pic_list_modification_flag_l1:" << ref_pic_list_modification_flag_l1).str());
		if(ref_pic_list_modification_flag_l0){
			for(int i = 0;i <= slice.num_ref_idx_l1_active_minus1;++i) fields.push_back((std::ostringstream() << "  list_entry_l1[" << i << "]:" << list_entry_l1[i]).str());
		}
	}
	return fields;
}

H265PredWeightTable::H265PredWeightTable(){
	delta_chroma_log2_weight_denom = 0;
}

H265Slice::H265Slice():
	H265Slice(0, UnitType_Unspecified, 0, 0, 0, nullptr)
{}

H265Slice::H265Slice(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, uint8_t* nal_data):
	H265NAL(forbidden_zero_bit, nal_unit_type, nuh_layer_id, nuh_temporal_id_plus1, nal_size, nal_data)
{
	first_slice_segment_in_pic_flag = 0;
	no_output_of_prior_pics_flag = 0;
	slice_pic_parameter_set_id = 0;
	dependent_slice_segment_flag = 0;
	slice_segment_address = 0;
	slice_type = SliceType_None;
	pic_output_flag = 1;
	colour_plane_id = 0;
	slice_pic_order_cnt_lsb = 0;
	short_term_ref_pic_set_sps_flag = 0;
	short_term_ref_pic_set_idx = 0;
	num_long_term_sps = 0;
	num_long_term_pics = 0;
	NumLongTerm = 0;
	slice_temporal_mvp_enabled_flag = 0;
	slice_sao_luma_flag = 0;
	slice_sao_chroma_flag = 0;
	num_ref_idx_active_override_flag = 0;
	num_ref_idx_l0_active_minus1 = 0;
	num_ref_idx_l1_active_minus1 = 0;
	mvd_l1_zero_flag = 0;
	cabac_init_flag = 0;
	collocated_from_l0_flag = 0;
	collocated_ref_idx = 0;
	five_minus_max_num_merge_cand = 0;
	slice_qp_delta = 0;
	slice_cb_qp_offset = 0;
	slice_cr_qp_offset = 0;
	slice_act_y_qp_offset = 0;
	slice_act_cb_qp_offset = 0;
	slice_act_cr_qp_offset = 0;
	deblocking_filter_override_flag = 0;
	slice_deblocking_filter_disabled_flag = 0;
	slice_beta_offset_div2 = 0;
	slice_tc_offset_div2 = 0;
	slice_loop_filter_across_slices_enabled_flag = 0;
	num_entry_point_offsets = 0;
	offset_len_minus1 = 0;
	slice_segment_header_extension_length = 0;
	IdrPicFlag = 0;
	NoRaslOutputFlag = 0;
	CurrRpsIdx = 0;
	NumPicTotalCurr = 0;
	NumShortTermPictureSliceHeaderBits = 0;
	NumLongTermPictureSliceHeaderBits = 0;
	NumRpsCurrTempList0 = 0;
	NumRpsCurrTempList1 = 0;
}

// void H265Slice::decodeRPS()
// {
// 	H265SPS* h265SPS = getSPS();
// 	if (nuh_layer_id != 0) {
// 		std::cerr << "[H265::Stream] Multi layer case not handled\n";
// 		return;
// 	}

// 	// 8.1.3 Decoding process for a coded picture with nuh_layer_id equal to 0
// 	if (nal_unit_type == H265NAL::UnitType_BLA_W_LP || nal_unit_type == H265NAL::UnitType_CRA_NUT) {
// 		std::cerr << "[H265::Stream] UseAltCpbParamsFlag not handled\n";
// 		return;
// 	}

// 	if (IdrPicFlag) {
// 		prevPicOrderCntLsb = 0;
// 		prevPicOrderCntMsb = 0;
// 	}

// 	MaxPicOrderCntLsb = 1 << (h265SPS->log2_max_pic_order_cnt_lsb_minus4 + 4);
// 	uint32_t PicOrderCntMsb = 0;
// 	PicOrderCntVal = 0;
// 	if (!IRAPPicture && NoRaslOutputFlag) {
// 		std::cerr << "[H265::Stream] Specific prevPicOrderCntLsb and prevPicOrderCntMsb not handled\n";
// 	}
// 	else if ((slice_pic_order_cnt_lsb < prevPicOrderCntLsb) &&
// 			((prevPicOrderCntLsb - slice_pic_order_cnt_lsb) >= (MaxPicOrderCntLsb / 2 )))
// 	{
// 		PicOrderCntMsb = prevPicOrderCntMsb + MaxPicOrderCntLsb;
// 	}
// 	else if ((slice_pic_order_cnt_lsb > prevPicOrderCntLsb) &&
// 			((slice_pic_order_cnt_lsb - prevPicOrderCntLsb) > (MaxPicOrderCntLsb / 2 )))
// 	{
// 		PicOrderCntMsb = prevPicOrderCntMsb - MaxPicOrderCntLsb;
// 	}
// 	else {
// 		PicOrderCntMsb = prevPicOrderCntMsb;
// 	}

// 	PicOrderCntVal = PicOrderCntMsb + slice_pic_order_cnt_lsb;

// 	if ( (nuh_temporal_id_plus1 - 1) == 0) {
// 		prevPicOrderCntLsb = slice_pic_order_cnt_lsb;
// 		prevPicOrderCntMsb = PicOrderCntMsb;
// 	}

// 	// 8.3.2 Decoding process for reference picture set
// 	if (!IdrPicFlag) {
// 		const H265ShortTermRefPicSet& CurrRsp = h265SPS->short_term_ref_pic_set[CurrRpsIdx];

// 		for (uint32_t i = 0; i < CurrRsp.NumNegativePics; ++i) {
// 			if (CurrRsp.UsedByCurrPicS0[i]) {
// 				PocStCurrBefore.push_back(PicOrderCntVal + CurrRsp.DeltaPocS0[i]);
// 			} else {
// 				PocStFoll.push_back(PicOrderCntVal + CurrRsp.DeltaPocS0[i]);
// 			}
// 		}
// 		RefPicSetStCurrBefore.resize(PocStCurrBefore.size());

// 		for (uint32_t i = 0; i < CurrRsp.NumPositivePics; ++i) {
// 			if (CurrRsp.UsedByCurrPicS1[i]) {
// 				PocStCurrAfter.push_back(PicOrderCntVal + CurrRsp.DeltaPocS1[i]);
// 			} else {
// 				PocStFoll.push_back(PicOrderCntVal + CurrRsp.DeltaPocS1[i]);
// 			}
// 		}
// 		RefPicSetStCurrAfter.resize(PocStCurrAfter.size());
// 		RefPicSetStFoll.resize(PocStFoll.size());

// 		for (uint32_t i = 0; i < num_long_term_sps + num_long_term_pics; ++i) {
// 			uint32_t pocLt = PocLsbLt[i];

// 			if (delta_poc_msb_present_flag[i]) {
// 				pocLt += PicOrderCntVal - DeltaPocMsbCycleLt[i] * MaxPicOrderCntLsb - (PicOrderCntVal & (MaxPicOrderCntLsb - 1));
// 			}

// 			if (UsedByCurrPicLt[i]) {
// 				PocLtCurr.push_back(pocLt);
// 				CurrDeltaPocMsbPresentFlag.push_back(delta_poc_msb_present_flag[i]);
// 			} else {
// 				PocLtFoll.push_back(pocLt);
// 				FollDeltaPocMsbPresentFlag.push_back(delta_poc_msb_present_flag[i]);
// 			}
// 		}
// 		RefPicSetLtCurr.resize(CurrDeltaPocMsbPresentFlag.size());
// 		RefPicSetLtFoll.resize(FollDeltaPocMsbPresentFlag.size());
// 	}
// }

// void H265Slice::computeRef()
// {
// 	H265PPS* h265PPS = getPPS();
// 	// 8.3.4 Decoding process for reference picture lists construction
// 	const RefPicListsModification& ref_pic_lists_modification = ref_pic_lists_modification;
// 	uint32_t rIdx = 0;
// 	if(slice_type == H265Slice::SliceType_P) {
// 		while (rIdx < NumRpsCurrTempList0) {
// 			for (int i = 0; i < PocStCurrBefore.size() && rIdx < NumRpsCurrTempList0; rIdx++, i++) {
// 				RefPicListTemp0.push_back(RefPicSetStCurrBefore[i]);
// 			}
// 			for (int i = 0; i < PocStCurrAfter.size() && rIdx < NumRpsCurrTempList0; rIdx++, i++) {
// 				RefPicListTemp0.push_back(RefPicSetStCurrAfter[i]);
// 			}
// 			for (int i = 0; i < PocLtCurr.size() && rIdx < NumRpsCurrTempList0; rIdx++, i++) {
// 				RefPicListTemp0.push_back(RefPicSetLtCurr[i]);
// 			}
// 			if (h265PPS->pps_extension_present_flag) {
// 				std::cerr << "[H265 Slice] pps_extension_present_flag not handled\n";
// 				//RefPicListTemp0[rIdx++] = currPic;
// 			}
// 		}
// 		for (rIdx = 0; rIdx <= num_ref_idx_l0_active_minus1; rIdx++) {
// 			int val = ref_pic_lists_modification.ref_pic_list_modification_flag_l0
// 					  ? RefPicListTemp0[ref_pic_lists_modification.list_entry_l0[rIdx]] : RefPicListTemp0[rIdx];
// 			RefPicList0.push_back(val);
// 		}
// 		/*
// 		if( m_pps.pps_curr_pic_ref_enabled_flag && !ref_pic_lists_modification.ref_pic_list_modification_flag_l0 &&
// 			NumRpsCurrTempList0 > ( num_ref_idx_l0_active_minus1 + 1 ) ) {
// 			RefPicList0[num_ref_idx_l0_active_minus1] = currPic
// 		}*/
// 	}

// 	if(slice_type == H265Slice::SliceType_B) {
// 		rIdx = 0;
// 		while (rIdx < NumRpsCurrTempList1) {
// 			for (int i = 0; i < PocStCurrAfter.size() && rIdx < NumRpsCurrTempList1; rIdx++, i++) {
// 				RefPicListTemp1.push_back(RefPicSetStCurrAfter[i]);
// 			}
// 			for (int i = 0; i < PocStCurrBefore.size() && rIdx < NumRpsCurrTempList1; rIdx++, i++) {
// 				RefPicListTemp1.push_back(RefPicSetStCurrBefore[i]);
// 			}
// 			for (int i = 0; i < PocLtCurr.size() && rIdx < NumRpsCurrTempList1; rIdx++, i++) {
// 				RefPicListTemp1.push_back(RefPicSetLtCurr[i]);
// 			}
// 			/*
// 			if (m_pps.pps_curr_pic_ref_enabled_flag) {
// 				RefPicListTemp1[rIdx++] = currPic;
// 			}*/
// 		}
// 		for (rIdx = 0; rIdx <= num_ref_idx_l1_active_minus1; rIdx++)
// 		{
// 			int val = ref_pic_lists_modification.ref_pic_list_modification_flag_l1 ? RefPicListTemp1[ref_pic_lists_modification.list_entry_l1[rIdx]] : RefPicListTemp1[rIdx];
// 			RefPicList1.push_back(val);
// 		}
// 	}
// }

std::vector<std::string> H265Slice::dump_fields(){
	std::vector<std::string> fields = H265NAL::dump_fields();
	fields.push_back((std::ostringstream() << "first_slice_segment_in_pic_flag:" << (int)first_slice_segment_in_pic_flag).str());
	if(nal_unit_type >= UnitType_BLA_W_LP && nal_unit_type <= UnitType_IRAP_VCL23) fields.push_back((std::ostringstream() << "  no_output_of_prior_pics_flag:" << (int)no_output_of_prior_pics_flag).str());
	fields.push_back((std::ostringstream() << "slice_pic_parameter_set_id:" << slice_pic_parameter_set_id).str());
	H265PPS* h265PPS = getPPS();
	H265SPS* h265SPS = getSPS();
	if(!h265PPS || !h265SPS) return fields;
	if(!first_slice_segment_in_pic_flag){
		if(h265PPS->dependent_slice_segments_enabled_flag){
			fields.push_back((std::ostringstream() << "    dependent_slice_segment_flag:" << (int)dependent_slice_segment_flag).str());
		}
		fields.push_back((std::ostringstream() << "  slice_segment_address:" << slice_segment_address).str());
	}
	if(!dependent_slice_segment_flag){
		fields.push_back((std::ostringstream() << "  slice_type:" << (int)slice_type).str());
		if(h265PPS->output_flag_present_flag) fields.push_back((std::ostringstream() << "    pic_output_flag:" << (int)pic_output_flag).str());
		if(h265SPS->separate_colour_plane_flag == 1) fields.push_back((std::ostringstream() << "    colour_plane_id:" << (int)colour_plane_id).str());
		if(!IdrPicFlag){
			fields.push_back((std::ostringstream() << "    slice_pic_order_cnt_lsb:" << slice_pic_order_cnt_lsb).str());
			fields.push_back((std::ostringstream() << "    short_term_ref_pic_set_sps_flag:" << (int)short_term_ref_pic_set_sps_flag).str());
			if(!short_term_ref_pic_set_sps_flag){
				std::vector<std::string> shortTermRefPicSetFields = h265SPS->short_term_ref_pic_set[h265SPS->num_short_term_ref_pic_sets].dump_fields(h265SPS->num_short_term_ref_pic_sets, h265SPS->num_short_term_ref_pic_sets);
				std::transform(shortTermRefPicSetFields.begin(), shortTermRefPicSetFields.end(), std::back_inserter(fields), [](const std::string& subField){
					return "    " + subField;
				});
			} else if(h265SPS->num_short_term_ref_pic_sets > 1) fields.push_back((std::ostringstream() << "      short_term_ref_pic_set_idx:" << (int)short_term_ref_pic_set_idx).str());
			if(h265SPS->long_term_ref_pics_present_flag){
				if(h265SPS->num_long_term_ref_pics_sps > 0) fields.push_back((std::ostringstream() << "      num_long_term_sps:" << num_long_term_sps).str());
				fields.push_back((std::ostringstream() << "    num_long_term_pics:" << num_long_term_pics).str());
				for(int i = 0;i < num_long_term_sps + num_long_term_pics;++i){
					if(i < num_long_term_sps){
						if(num_long_term_sps) fields.push_back((std::ostringstream() << "      lt_idx_sps[" << i << "]:" << lt_idx_sps[i]).str());
					} else {
						fields.push_back((std::ostringstream() << "      poc_lsb_lt[" << i << "]:" << poc_lsb_lt[i]).str());
						fields.push_back((std::ostringstream() << "      used_by_curr_pic_lt_flag[" << i << "]:" << (int)used_by_curr_pic_lt_flag[i]).str());
					}
					fields.push_back((std::ostringstream() << "    delta_poc_msb_present_flag[" << i << "]:" << (int)delta_poc_msb_present_flag[i]).str());
					if(delta_poc_msb_present_flag[i]) fields.push_back((std::ostringstream() << "      delta_poc_msb_cycle_lt[" << i << "]:" << delta_poc_msb_cycle_lt[i]).str());
				}
			}
			if(h265SPS->sps_temporal_mvp_enabled_flag) fields.push_back((std::ostringstream() << "  slice_temporal_mvp_enabled_flag:" << (int)slice_temporal_mvp_enabled_flag).str());
		}
		if(h265SPS->sample_adaptive_offset_enabled_flag){
			fields.push_back((std::ostringstream() << "  slice_sao_luma_flag:" << (int)slice_sao_luma_flag).str());
			if(h265SPS->ChromaArrayType !=0) fields.push_back((std::ostringstream() << "    slice_sao_chroma_flag:" << (int)slice_sao_chroma_flag).str());
		}
		if(slice_type == SliceType_P || slice_type == SliceType_B){
			fields.push_back((std::ostringstream() << "  num_ref_idx_active_override_flag:" << (int)num_ref_idx_active_override_flag).str());
			if(num_ref_idx_active_override_flag){
				fields.push_back((std::ostringstream() << "    num_ref_idx_l0_active_minus1:" << num_ref_idx_l0_active_minus1).str());
				if(slice_type == SliceType_B) fields.push_back((std::ostringstream() << "      num_ref_idx_l1_active_minus1:" << num_ref_idx_l1_active_minus1).str());
			}
			if(h265PPS->lists_modification_present_flag && NumPicTotalCurr > 1){
				std::vector<std::string> refPicListsModificationFields =  ref_pic_lists_modification.dump_fields(*this);
				std::transform(refPicListsModificationFields.begin(), refPicListsModificationFields.end(), std::back_inserter(fields), [](const std::string& subField){
					return "    " + subField;
				});
			}
			if(slice_type == SliceType_B) fields.push_back((std::ostringstream() << "    mvd_l1_zero_flag:" << (int)mvd_l1_zero_flag).str());
			if(cabac_init_flag) fields.push_back((std::ostringstream() << "    cabac_init_flag:" << (int)cabac_init_flag).str());
			if(slice_temporal_mvp_enabled_flag){
				if(slice_type == SliceType_B) fields.push_back((std::ostringstream() << "      collocated_from_l0_flag:" << (int)collocated_from_l0_flag).str());
				if((collocated_from_l0_flag && num_ref_idx_l0_active_minus1 > 0) || (!collocated_from_l0_flag && num_ref_idx_l1_active_minus1 > 0)){
					fields.push_back((std::ostringstream() << "      collocated_ref_idx:" << collocated_ref_idx).str());
				}
			}
		}
	}
	return fields;
}

H265PPS* H265Slice::getPPS() const{
	auto referencedPPS = H265PPS::PPSMap.find(slice_pic_parameter_set_id);
	if(referencedPPS == H265PPS::PPSMap.end()) return nullptr;
	return referencedPPS->second;
}

H265SPS* H265Slice::getSPS() const{
	H265PPS* pPps = getPPS();
	if(!pPps) return nullptr;
	auto referencedSPS = H265SPS::SPSMap.find(pPps->pps_seq_parameter_set_id);
	if(referencedSPS == H265SPS::SPSMap.end()) return nullptr;
	return referencedSPS->second;
}

H265VPS* H265Slice::getVPS() const{
	H265SPS* pSps = getSPS();
	if(!pSps) return nullptr;
	auto referencedVPS = H265VPS::VPSMap.find(pSps->sps_video_parameter_set_id);
	if(referencedVPS == H265VPS::VPSMap.end()) return nullptr;
	return referencedVPS->second;
}

void H265Slice::validate(){
	H265NAL::validate();
	if(slice_pic_parameter_set_id > 63) errors.push_back((std::ostringstream() << "[H265 Slice] slice_pic_parameter_set_id value (" << (int)slice_pic_parameter_set_id << ") not in valid range (0..63)").str());
	H265PPS* pPps = getPPS();
	if(!pPps){
		errors.push_back((std::ostringstream() << "[H265 Slice] reference to unknown PPS (" << (int)slice_pic_parameter_set_id << ")").str());
		return;
	}
	H265SPS* pSps = getSPS();
	if(!pSps){
		errors.push_back((std::ostringstream() << "[H265 Slice] reference to unknown SPS (" << (int)pPps->pps_seq_parameter_set_id << ")").str());
		return;
	}
	if(pPps->TemporalId > TemporalId) errors.push_back((std::ostringstream() << "[H265 Slice] referenced PPS has a greater TemporalId value").str());
	if(pPps->nuh_layer_id > nuh_layer_id) errors.push_back((std::ostringstream() << "[H265 Slice] referenced PPS has a greater nuh_layer_id value").str());
	if(pSps->nuh_layer_id > nuh_layer_id) errors.push_back((std::ostringstream() << "[H265 Slice] referenced SPS has a greater nuh_layer_id value").str());
	if(slice_segment_address > pSps->PicSizeInCtbsY-1) errors.push_back((std::ostringstream() << "[H265 Slice] slice_segment_address value (" << slice_segment_address << ") not in valid range (0.." << pSps->PicSizeInCtbsY-1 << ")").str());
	if(!pPps->dependent_slice_segments_enabled_flag){
		if(slice_type > 2) errors.push_back((std::ostringstream() << "[H265 Slice] slice_type value (" << (int)slice_type << ") not in valid range (0..2)").str());
		if(nal_unit_type >= UnitType_BLA_W_LP && nal_unit_type <= UnitType_IRAP_VCL23 &&
			nuh_layer_id == 0 && !pPps->pps_scc_extension.pps_curr_pic_ref_enabled_flag &&
			slice_type != 2) errors.push_back((std::ostringstream() << "[H265 Slice] slice_type value of an IRAP picture not equal to 2").str());
		if(pSps->sps_max_dec_pic_buffering_minus1[TemporalId] == 0 && nuh_layer_id == 0 &&
			!pPps->pps_scc_extension.pps_curr_pic_ref_enabled_flag && slice_type != 2) {
			errors.push_back((std::ostringstream() << "[H265 Slice] slice_type value not equal to 2").str());
		}
		if(colour_plane_id > 2) errors.push_back((std::ostringstream() << "[H265 Slice] colour_plane_id value (" << (int)colour_plane_id << ") not in valid range (0..2)").str());
	}
	if(slice_pic_order_cnt_lsb > pSps->MaxPicOrderCntLsb-1) errors.push_back((std::ostringstream() << "[H265 Slice] slice_pic_order_cnt_lsb value (" << slice_pic_order_cnt_lsb << ") not in valid range (0.." << pSps->MaxPicOrderCntLsb-1 << ")").str());
	if(short_term_ref_pic_set_idx > pSps->num_short_term_ref_pic_sets-1) errors.push_back((std::ostringstream() << "[H265 Slice] short_term_ref_pic_set_idx value (" << (int)short_term_ref_pic_set_idx << ") not in valid range (0.." << pSps->num_short_term_ref_pic_sets-1 << ")").str());
	if(num_long_term_sps > pSps->num_long_term_ref_pics_sps) errors.push_back((std::ostringstream() << "[H265 Slice] num_long_term_sps value (" << num_long_term_sps << ") not in valid range (0.." << pSps->num_long_term_ref_pics_sps << ")").str());
	uint32_t num_long_term_pics_limit = pSps->sps_max_dec_pic_buffering_minus1[TemporalId] - 
										pSps->short_term_ref_pic_set[CurrRpsIdx].NumNegativePics -
										pSps->short_term_ref_pic_set[CurrRpsIdx].NumPositivePics -
										num_long_term_sps - pPps->TwoVersionsOfCurrDecPicFlag;
	if(nuh_layer_id == 0 && num_long_term_pics > num_long_term_pics_limit){
		errors.push_back((std::ostringstream() << "[H265 Slice] num_long_term_pics value (" << num_long_term_pics << ") not in valid range (0.." << num_long_term_pics_limit << ")").str());
	}
	for(int i = 0;i < lt_idx_sps.size();++i){
		if(lt_idx_sps[i] > pSps->num_long_term_ref_pics_sps-1) errors.push_back((std::ostringstream() << "[H265 Slice] lt_idx_sps[" << i << "] value (" << lt_idx_sps[i] << ") not in valid range (0.." << pSps->num_long_term_ref_pics_sps-1 << ")").str());
	}
	// delta_poc_msb_present_flag[i]
	uint32_t delta_poc_msb_cycle_lt_limit = 1 << (32 - pSps->log2_max_pic_order_cnt_lsb_minus4-4);
	for(int i = 0;i < delta_poc_msb_cycle_lt.size();++i){
		if(delta_poc_msb_cycle_lt[i] > delta_poc_msb_cycle_lt_limit) errors.push_back((std::ostringstream() << "[H265 Slice] delta_poc_msb_cycle_lt[" << i << "] value (" << delta_poc_msb_cycle_lt[i] << ") not in valid range (0.." << delta_poc_msb_cycle_lt_limit << ")").str());
	}
	if(num_ref_idx_active_override_flag){
		if(num_ref_idx_l0_active_minus1 > 14) errors.push_back((std::ostringstream() << "[H265 Slice] num_ref_idx_l0_active_minus1 value (" << num_ref_idx_l0_active_minus1 << ") not in valid range (0..14)").str());
		if(num_ref_idx_l1_active_minus1 > 14) errors.push_back((std::ostringstream() << "[H265 Slice] num_ref_idx_l1_active_minus1 value (" << num_ref_idx_l1_active_minus1 << ") not in valid range (0..14)").str());
	}
	if(collocated_from_l0_flag && (slice_type == SliceType_P || slice_type == SliceType_B)  && collocated_ref_idx > num_ref_idx_l0_active_minus1){
		errors.push_back((std::ostringstream() << "[H265 Slice] collocated_ref_idx value (" << collocated_ref_idx << ") not in valid range (0.." << num_ref_idx_l0_active_minus1 << ")").str());
	}
	// if(slice_temporal_mvp_enabled_flag) ...
	// pred_weight_table
	// if(five_minus_max_num_merge_cand > 4) errors.push_back((std::ostringstream() << "[H265 Slice] five_minus_max_num_merge_cand value (" << five_minus_max_num_merge_cand << ") not in valid range (0..4)").str());

}
