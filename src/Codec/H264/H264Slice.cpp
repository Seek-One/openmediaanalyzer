#include <cstring>
#include <sstream>

#include "H264PPS.h"
#include "H264SPS.h"

#include "H264Slice.h"

H264Slice::H264Slice()
{
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
	H264PPS pps = referencedPPS->second;
	auto referencedSPS = H264SPS2::SPSMap.find(pps.seq_parameter_set_id);
	if(referencedSPS == H264SPS2::SPSMap.end()) return fields;
	H264SPS2 sps = referencedSPS->second;
	if(sps.separate_colour_plane_flag == 1) fields.push_back((std::ostringstream() << "  colour_plane_id:" << (int)colour_plane_id).str());
    fields.push_back((std::ostringstream() << "frame_num:" << frame_num).str());
	if(!sps.frame_mbs_only_flag){
		fields.push_back((std::ostringstream() << "  field_pic_flag:" << (int)field_pic_flag).str());
		if(field_pic_flag) fields.push_back((std::ostringstream() << "    bottom_field_flag:" << (int)bottom_field_flag).str());
	}
	if(IdrPicFlag) fields.push_back((std::ostringstream() << "  idr_pic_id:" << (int)idr_pic_id).str());
	if(sps.pic_order_cnt_type == 0){
		fields.push_back((std::ostringstream() << "  pic_order_cnt_lsb:" << (int)pic_order_cnt_lsb).str());
		if(pps.bottom_field_pic_order_in_frame_present_flag && !field_pic_flag) fields.push_back((std::ostringstream() << "    delta_pic_order_cnt_bottom:" << delta_pic_order_cnt_bottom).str());
	}
	if(sps.pic_order_cnt_type == 1 && sps.delta_pic_order_always_zero_flag){
		fields.push_back((std::ostringstream() << "  delta_pic_order_cnt0:" << delta_pic_order_cnt[0]).str());
		if(pps.bottom_field_pic_order_in_frame_present_flag && !field_pic_flag) fields.push_back((std::ostringstream() << "    delta_pic_order_cnt1:" << delta_pic_order_cnt[1]).str());
	}
	if(pps.redundant_pic_cnt_present_flag) fields.push_back((std::ostringstream() << "  redundant_pic_cnt:" << (int)redundant_pic_cnt).str());
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

	if(pps.entropy_coding_mode_flag && slice_type != SliceType_I && slice_type != SliceType_SI) fields.push_back((std::ostringstream() << "  cabac_init_idc:" << (int)cabac_init_idc).str());
	fields.push_back((std::ostringstream() << "slice_qp_delta:" << (int)slice_qp_delta).str());
	if(slice_type == SliceType_SP || slice_type == SliceType_SI){
		if(slice_type == SliceType_SP) fields.push_back((std::ostringstream() << "    sp_for_switch_flag:" << (int)sp_for_switch_flag).str());
		fields.push_back((std::ostringstream() << "  slice_qs_delta:" << (int)slice_qs_delta).str());
	}
	if(pps.deblocking_filter_control_present_flag){
		fields.push_back((std::ostringstream() << "  disable_deblocking_filter_idc:" << (int)disable_deblocking_filter_idc).str());
		if(disable_deblocking_filter_idc != 1){
			fields.push_back((std::ostringstream() << "    slice_alpha_c0_offset_div2:" << (int)slice_alpha_c0_offset_div2).str());
			fields.push_back((std::ostringstream() << "    slice_beta_offset_div2:" << (int)slice_beta_offset_div2).str());
		}
	}
	if(pps.num_slice_groups_minus1 > 0 && pps.slice_group_map_type >= 3 && pps.slice_group_map_type <= 5){
		fields.push_back((std::ostringstream() << "  slice_group_change_cycle:" << slice_group_change_cycle).str());
	}

	fields.push_back((std::ostringstream() << "IdrPicFlag:" << (int)IdrPicFlag).str());
    fields.push_back((std::ostringstream() << "PrevRefFrameNum:" << (int)PrevRefFrameNum).str());

	return fields;
}

std::optional<H264PPS> H264Slice::getPPS() const{
	auto referencedPPS = H264PPS::PPSMap.find(pic_parameter_set_id);
	if(referencedPPS == H264PPS::PPSMap.end()) return {};
	return referencedPPS->second;
}

std::optional<H264SPS2> H264Slice::getSPS() const{
	std::optional<H264PPS> pps = getPPS();
	if(!pps.has_value()) return {};
	auto referencedSPS = H264SPS2::SPSMap.find(pps.value().seq_parameter_set_id);
	if(referencedSPS == H264SPS2::SPSMap.end()) return {};
	return referencedSPS->second;
}
