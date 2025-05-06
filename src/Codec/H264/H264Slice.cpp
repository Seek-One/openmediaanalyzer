#include <iostream>
#include <cstring>
#include <sstream>

#include "H264PPS.h"
#include "H264SPS.h"

#include "H264Slice.h"

H264Slice::H264Slice():
	H264Slice(0, 0, UnitType::UnitType_Unspecified, 0, nullptr)
{}

H264Slice::H264Slice(uint8_t forbiddenZeroBit, uint8_t nalRefIdc, UnitType nalUnitType, uint32_t nalSize, uint8_t* nalData):
	H264NAL(forbiddenZeroBit, nalRefIdc, nalSize, nalData)
{
	nal_unit_type = nalUnitType;
	IdrPicFlag = 0;
	first_mb_in_slice = 0;
	slice_type = SliceType_Unspecified;
	pic_parameter_set_id = 0;
	colour_plane_id = 0;
	frame_num = 0;
	field_pic_flag = 0;
	bottom_field_flag = 0;
	idr_pic_id = 0;
	pic_order_cnt_lsb = 0;
	delta_pic_order_cnt_bottom = 0;
	delta_pic_order_cnt[0] = 0;
	delta_pic_order_cnt[1] = 0;
	redundant_pic_cnt = 0;
	direct_spatial_mv_pred_flag = 0;
	num_ref_idx_active_override_flag = 0;
	num_ref_idx_l0_active_minus1 = 0;
	num_ref_idx_l1_active_minus1 = 0;
	cabac_init_idc = 0;
	slice_qp_delta = 0;
	sp_for_switch_flag = 0;
	slice_qs_delta = 0;
	disable_deblocking_filter_idc = 0;
	slice_alpha_c0_offset_div2 = 0;
	slice_beta_offset_div2 = 0;
	slice_group_change_cycle = 0;

	pwt.luma_log2_weight_denom = 0;
	pwt.chroma_log2_weight_denom = 0;
	memset(pwt.luma_weight_l0_flag, 0, 64 * sizeof(uint8_t));
	memset(pwt.luma_weight_l0, 0, 64 * sizeof(int16_t));
	memset(pwt.luma_offset_l0, 0, 64 * sizeof(int8_t));
	memset(pwt.chroma_weight_l0_flag, 0, 64 * sizeof(uint8_t));
	for (int i = 0; i < 64; ++i) {
		pwt.chroma_weight_l0[i][0] = 0;
		pwt.chroma_weight_l0[i][1] = 0;
	}
	for (int i = 0; i < 64; ++i) {
		pwt.chroma_offset_l0[i][0] = 0;
		pwt.chroma_offset_l0[i][1] = 0;
	}

	memset(pwt.luma_weight_l1_flag, 0, 64 * sizeof(uint8_t));
	memset(pwt.luma_weight_l1, 0, 64 * sizeof(int16_t));
	memset(pwt.luma_offset_l1, 0, 64 * sizeof(int8_t));
	memset(pwt.chroma_weight_l1_flag, 0, 64 * sizeof(uint8_t));
	for (int i = 0; i < 64; ++i) {
		pwt.chroma_weight_l1[i][0] = 0;
		pwt.chroma_weight_l1[i][1] = 0;
	}
	for (int i = 0; i < 64; ++i) {
		pwt.chroma_offset_l1[i][0] = 0;
		pwt.chroma_offset_l1[i][1] = 0;
	}

	rplr.ref_pic_list_reordering_flag_l0 = 0;
	for (int i = 0; i < 64; ++i) {
		rplr.reorder_l0[i].reordering_of_pic_nums_idc = 0;
		rplr.reorder_l0[i].abs_diff_pic_num_minus1 = 0;
		rplr.reorder_l0[i].long_term_pic_num = 0;
	}

	rplr.ref_pic_list_reordering_flag_l1 = 0;
	for (int i = 0; i < 64; ++i) {
		rplr.reorder_l1[i].reordering_of_pic_nums_idc = 0;
		rplr.reorder_l1[i].abs_diff_pic_num_minus1 = 0;
		rplr.reorder_l1[i].long_term_pic_num = 0;
	}

	drpm.no_output_of_prior_pics_flag = 0;
	drpm.long_term_reference_flag = 0;
	drpm.adaptive_ref_pic_marking_mode_flag = 0;

	memset(drpm.memory_management_control_operation, 0, 64 * sizeof(uint8_t));
	memset(drpm.difference_of_pic_nums_minus1, 0, 64 * sizeof(uint32_t));
	memset(drpm.long_term_pic_num, 0, 64 * sizeof(uint32_t));
	memset(drpm.long_term_frame_idx, 0, 64 * sizeof(uint32_t));
	memset(drpm.max_long_term_frame_idx_plus1, 0, 64 * sizeof(uint32_t));
}

bool H264Slice::isSlice(H264NAL* NALUnit){
	return NALUnit->nal_unit_type == H264NAL::UnitType_NonIDRFrame || NALUnit->nal_unit_type == H264NAL::UnitType_IDRFrame;
}

std::vector<std::string> H264Slice::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back((std::ostringstream() << "first_mb_in_slice:" << first_mb_in_slice).str());
    fields.push_back((std::ostringstream() << "slice_type:" << slice_type-1).str());
    fields.push_back((std::ostringstream() << "pic_parameter_set_id:" << pic_parameter_set_id).str());
	
	auto referencedPPS = H264PPS::PPSMap.find(pic_parameter_set_id);
	if(referencedPPS == H264PPS::PPSMap.end()) return fields;
	H264PPS* pPps = referencedPPS->second;
	auto referencedSPS = H264SPS::SPSMap.find(pPps->seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) return fields;
	H264SPS* pSps = referencedSPS->second;
	if(pSps->separate_colour_plane_flag == 1) fields.push_back((std::ostringstream() << "  colour_plane_id:" << (int)colour_plane_id).str());
    fields.push_back((std::ostringstream() << "frame_num:" << frame_num).str());
	if(!pSps->frame_mbs_only_flag){
		fields.push_back((std::ostringstream() << "  field_pic_flag:" << (int)field_pic_flag).str());
		if(field_pic_flag) fields.push_back((std::ostringstream() << "    bottom_field_flag:" << (int)bottom_field_flag).str());
	}
	if(IdrPicFlag) fields.push_back((std::ostringstream() << "  idr_pic_id:" << (int)idr_pic_id).str());
	if(pSps->pic_order_cnt_type == 0){
		fields.push_back((std::ostringstream() << "  pic_order_cnt_lsb:" << (int)pic_order_cnt_lsb).str());
		if(pPps->bottom_field_pic_order_in_frame_present_flag && !field_pic_flag) fields.push_back((std::ostringstream() << "    delta_pic_order_cnt_bottom:" << delta_pic_order_cnt_bottom).str());
	}
	if(pSps->pic_order_cnt_type == 1 && pSps->delta_pic_order_always_zero_flag){
		fields.push_back((std::ostringstream() << "  delta_pic_order_cnt0:" << delta_pic_order_cnt[0]).str());
		if(pPps->bottom_field_pic_order_in_frame_present_flag && !field_pic_flag) fields.push_back((std::ostringstream() << "    delta_pic_order_cnt1:" << delta_pic_order_cnt[1]).str());
	}
	if(pPps->redundant_pic_cnt_present_flag) fields.push_back((std::ostringstream() << "  redundant_pic_cnt:" << (int)redundant_pic_cnt).str());
	if(slice_type == SliceType_B) fields.push_back((std::ostringstream() << "  direct_spatial_mv_pred_flag:" << (int)direct_spatial_mv_pred_flag).str());
	if(slice_type == SliceType_P || slice_type == SliceType_SP || slice_type == SliceType_B){
		fields.push_back((std::ostringstream() << "  num_ref_idx_active_override_flag:" << (int)num_ref_idx_active_override_flag).str());
		if(num_ref_idx_active_override_flag){
			fields.push_back((std::ostringstream() << "    num_ref_idx_l0_active_minus1:" << (int)num_ref_idx_l0_active_minus1).str());
			if(slice_type == SliceType_B) fields.push_back((std::ostringstream() << "      num_ref_idx_l1_active_minus1:" << (int)num_ref_idx_l1_active_minus1).str());
		}
		
	}
	
	fields.push_back((std::ostringstream() << "pwt.luma_log2_weight_denom:" << (int)pwt.luma_log2_weight_denom).str());
	fields.push_back((std::ostringstream() << "pwt.chroma_log2_weight_denom:" << (int)pwt.chroma_log2_weight_denom).str());
	for(int i = 0;i < 64;++i){;
		if(pwt.luma_weight_l0_flag[i] != 0){
			fields.push_back((std::ostringstream() << "  pwt.luma_weight_l0_flag[" << i << "]:" << (int)pwt.luma_weight_l0_flag[i]).str());
			fields.push_back((std::ostringstream() << "  pwt.luma_weight_l0[" << i << "]:" << pwt.luma_weight_l0[i]).str());
			fields.push_back((std::ostringstream() << "  pwt.luma_offset_l0[" << i << "]:" << (int)pwt.luma_offset_l0[i]).str());
			fields.push_back((std::ostringstream() << "  pwt.chroma_weight_l0_flag[" << i << "]:" << (int)pwt.chroma_weight_l0_flag[i]).str());
			if(pwt.chroma_weight_l0_flag[i] != 0){
				for(int j = 0;j < 2;++j) {
					fields.push_back((std::ostringstream() << "    pwt.chroma_weight_l0[" << i << "][" << j << "]:" << pwt.chroma_weight_l0[i][j]).str());
					fields.push_back((std::ostringstream() << "    pwt.chroma_offset_l0[" << i << "][" << j << "]:" << (int)pwt.chroma_offset_l0[i][j]).str());
				}
			}
		}
	}
	for(int i = 0;i < 64;++i){;
		if(pwt.luma_weight_l1_flag[i] == 1){
			fields.push_back((std::ostringstream() << "  pwt.luma_weight_l1_flag[" << i << "]:" << (int)pwt.luma_weight_l1_flag[i]).str());
			fields.push_back((std::ostringstream() << "  pwt.luma_weight_l1[" << i << "]:" << pwt.luma_weight_l1[i]).str());
			fields.push_back((std::ostringstream() << "  pwt.luma_offset_l1[" << i << "]:" << (int)pwt.luma_offset_l1[i]).str());
			fields.push_back((std::ostringstream() << "  pwt.chroma_weight_l1_flag[" << i << "]:" << (int)pwt.chroma_weight_l1_flag[i]).str());
			if(pwt.chroma_weight_l1_flag[i] == 1){
				for(int j = 0;j < 2;++j) {
					fields.push_back((std::ostringstream() << "    pwt.chroma_weight_l1[" << i << "][" << j << "]:" << pwt.chroma_weight_l1[i][j]).str());
					fields.push_back((std::ostringstream() << "    pwt.chroma_offset_l1[" << i << "][" << j << "]:" << (int)pwt.chroma_offset_l1[i][j]).str());
				}
			}
		}
	}

	if(pPps->entropy_coding_mode_flag && slice_type != SliceType_I && slice_type != SliceType_SI) fields.push_back((std::ostringstream() << "  cabac_init_idc:" << (int)cabac_init_idc).str());
	fields.push_back((std::ostringstream() << "slice_qp_delta:" << (int)slice_qp_delta).str());
	if(slice_type == SliceType_SP || slice_type == SliceType_SI){
		if(slice_type == SliceType_SP) fields.push_back((std::ostringstream() << "    sp_for_switch_flag:" << (int)sp_for_switch_flag).str());
		fields.push_back((std::ostringstream() << "  slice_qs_delta:" << (int)slice_qs_delta).str());
	}
	if(pPps->deblocking_filter_control_present_flag){
		fields.push_back((std::ostringstream() << "  disable_deblocking_filter_idc:" << (int)disable_deblocking_filter_idc).str());
		if(disable_deblocking_filter_idc != 1){
			fields.push_back((std::ostringstream() << "    slice_alpha_c0_offset_div2:" << (int)slice_alpha_c0_offset_div2).str());
			fields.push_back((std::ostringstream() << "    slice_beta_offset_div2:" << (int)slice_beta_offset_div2).str());
		}
	}
	if(pPps->num_slice_groups_minus1 > 0 && pPps->slice_group_map_type >= 3 && pPps->slice_group_map_type <= 5){
		fields.push_back((std::ostringstream() << "  slice_group_change_cycle:" << slice_group_change_cycle).str());
	}

	fields.push_back((std::ostringstream() << "IdrPicFlag:" << (int)IdrPicFlag).str());
    fields.push_back((std::ostringstream() << "PrevRefFrameNum:" << (int)PrevRefFrameNum).str());

	return fields;
}

void H264Slice::validate(){
	if(slice_type == H264Slice::SliceType_Unspecified){
		majorErrors.push_back("[Slice] Invalid slice type");
	}
	if(pic_parameter_set_id > 255){
		minorErrors.push_back((std::ostringstream() << "[Slice] pic_parameter_set_id value (" << (int)pic_parameter_set_id << ") not in valid range (0.255)").str());
	}
	H264PPS* pH264PPS;
	auto referencedPPS = H264PPS::PPSMap.find(pic_parameter_set_id);
	if(referencedPPS == H264PPS::PPSMap.end()){
		majorErrors.push_back((std::ostringstream() << "[Slice] reference to unknown PPS (" << (int)pic_parameter_set_id << ")").str());
		return;
	}
	pH264PPS = referencedPPS->second;
	H264SPS* pH264SPS;
	auto referencedSPS = H264SPS::SPSMap.find(pH264PPS->seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()){
		majorErrors.push_back((std::ostringstream() << "[Slice] This unit's PPS is referencing an unknown SPS (" << (int)pH264PPS->seq_parameter_set_id << ")").str());
		return;
	}
	pH264SPS = referencedSPS->second;
	if(nal_unit_type == H264NAL::UnitType_IDRFrame || pH264SPS->max_num_ref_frames == 0){
		switch(slice_type){
			case H264Slice::SliceType_I: case H264Slice::SliceType_SI: break;
			default:
				majorErrors.push_back((std::ostringstream() << "[Slice] slice_type value (" << (int)slice_type-1 << ") of IDR should be in {2, 4, 7, 9}").str());
				break;
		}
	}
	if (pH264SPS->separate_colour_plane_flag){
		if(colour_plane_id > 2){
			minorErrors.push_back((std::ostringstream() << "[Slice] colour_plane_id value (" << (int)colour_plane_id << ") not in valid range (0..2)").str());
		}
	}

	if(slice_type == H264Slice::SliceType_I && frame_num != 0){
		
		minorErrors.push_back((std::ostringstream() << "[Slice] frame_num of an IDR picture (" << (int)frame_num << ") should be 0").str());
	}

	if (pH264SPS->pic_order_cnt_type == 0){
		if(pic_order_cnt_lsb > pH264SPS->MaxPicOrderCntLsb-1){
			minorErrors.push_back((std::ostringstream() << "[Slice] pic_order_cnt_lsb value (" << (int)pic_order_cnt_lsb << ") not in valid range (0.." << pH264SPS->MaxPicOrderCntLsb-1 << ")").str());
		}
	}
	if (pH264PPS->redundant_pic_cnt_present_flag){
		if(redundant_pic_cnt > 127){
			minorErrors.push_back((std::ostringstream() << "[Slice] redundant_pic_cnt (" << (int)redundant_pic_cnt << ") not in valid range (0..127)").str());
		}
	}
}

H264PPS* H264Slice::getPPS() const{
	auto referencedPPS = H264PPS::PPSMap.find(pic_parameter_set_id);
	if(referencedPPS == H264PPS::PPSMap.end()) return nullptr;
	return referencedPPS->second;
}

H264SPS* H264Slice::getSPS() const{
	H264PPS* pPps = getPPS();
	if(!pPps) return nullptr;
	auto referencedSPS = H264SPS::SPSMap.find(pPps->seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) return {};
	return referencedSPS->second;
}
