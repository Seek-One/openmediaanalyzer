#include <iostream>
#include <cstring>
#include <fmt/core.h>

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

	CurrPicNum = frame_num;
}

bool H264Slice::isSlice(H264NAL* NALUnit){
	return NALUnit->nal_unit_type == H264NAL::UnitType_NonIDRFrame || NALUnit->nal_unit_type == H264NAL::UnitType_IDRFrame;
}

UnitFieldList H264Slice::dump_fields(){
	UnitFieldList fields = UnitFieldList("Slice", H264NAL::dump_fields());
	if(!completelyParsed) return fields;
	fields.addItem(UnitField("first_mb_in_slice", first_mb_in_slice));
    fields.addItem(UnitField("slice_type", slice_type-1));
    fields.addItem(UnitField("pic_parameter_set_id", pic_parameter_set_id));
	
	auto referencedPPS = H264PPS::PPSMap.find(pic_parameter_set_id);
	if(referencedPPS == H264PPS::PPSMap.end()) return fields;
	H264PPS* pPps = referencedPPS->second;
	auto referencedSPS = H264SPS::SPSMap.find(pPps->seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) return fields;
	H264SPS* pSps = referencedSPS->second;
	if(pSps->separate_colour_plane_flag == 1) fields.addItem(UnitField("colour_plane_id", colour_plane_id));
    fields.addItem(UnitField("frame_num", frame_num));
	if(!pSps->frame_mbs_only_flag){
		ValueUnitFieldList field_pic_flagField = ValueUnitFieldList("field_pic_flag", field_pic_flag);
		if(field_pic_flag) field_pic_flagField.addItem(UnitField("bottom_field_flag", bottom_field_flag));
		else if(pPps->bottom_field_pic_order_in_frame_present_flag){
			if(pSps->pic_order_cnt_type == 0) field_pic_flagField.addItem(UnitField("delta_pic_order_cnt_bottom", delta_pic_order_cnt_bottom));
			else if(pSps->pic_order_cnt_type == 1) fields.addItem(UnitField("delta_pic_order_cnt1", delta_pic_order_cnt[1]));
		}
		fields.addItem(std::move(field_pic_flagField));
	}
	if(IdrPicFlag) fields.addItem(UnitField("idr_pic_id", idr_pic_id));
	if(pSps->pic_order_cnt_type == 0){
		fields.addItem(UnitField("pic_order_cnt_lsb", pic_order_cnt_lsb));
	}
	if(pSps->pic_order_cnt_type == 1 && pSps->delta_pic_order_always_zero_flag){
		fields.addItem(UnitField("delta_pic_order_cnt0", delta_pic_order_cnt[0]));
	}
	if(pPps->redundant_pic_cnt_present_flag) fields.addItem(UnitField("redundant_pic_cnt", redundant_pic_cnt));
	if(slice_type == SliceType_B) fields.addItem(UnitField("direct_spatial_mv_pred_flag", direct_spatial_mv_pred_flag));
	if(slice_type == SliceType_P || slice_type == SliceType_SP || slice_type == SliceType_B){
		ValueUnitFieldList num_ref_idx_active_override_flagField = ValueUnitFieldList("num_ref_idx_active_override_flag", num_ref_idx_active_override_flag);
		if(num_ref_idx_active_override_flag){
			num_ref_idx_active_override_flagField.addItem(UnitField("num_ref_idx_l0_active_minus1", num_ref_idx_l0_active_minus1));
			if(slice_type == SliceType_B) num_ref_idx_active_override_flagField.addItem(UnitField("num_ref_idx_l1_active_minus1", num_ref_idx_l1_active_minus1));
		}
		fields.addItem(std::move(num_ref_idx_active_override_flagField));
	}
	UnitFieldList pwtField = UnitFieldList("pwt");
	pwtField.addItem(UnitField("luma_log2_weight_denom", pwt.luma_log2_weight_denom));
	pwtField.addItem(UnitField("chroma_log2_weight_denom", pwt.chroma_log2_weight_denom));
	for(int i = 0;i < 64;++i){
		IdxValueUnitFieldList luma_weight_l0_flagField = IdxValueUnitFieldList("luma_weight_l0_flag", pwt.luma_weight_l0_flag[i], i);
		if(pwt.luma_weight_l0_flag[i] != 0){
			luma_weight_l0_flagField.addItem(IdxUnitField("luma_weight_l0", pwt.luma_weight_l0[i], i));
			luma_weight_l0_flagField.addItem(IdxUnitField("luma_offset_l0", pwt.luma_offset_l0[i], i));
			IdxValueUnitFieldList chroma_weight_l0_flagField = IdxValueUnitFieldList("chroma_weight_l0_flag", pwt.chroma_weight_l0_flag[i], i);
			luma_weight_l0_flagField.addItem(std::move(chroma_weight_l0_flagField));
			if(pwt.chroma_weight_l0_flag[i] != 0){
				for(int j = 0;j < 2;++j) {
					chroma_weight_l0_flagField.addItem(DblIdxUnitField("chroma_weight_l0", pwt.chroma_weight_l0[i][j], i, j));
					chroma_weight_l0_flagField.addItem(DblIdxUnitField("chroma_offset_l0", pwt.chroma_offset_l0[i][j], i, j));
				}
			}
		}
		pwtField.addItem(std::move(luma_weight_l0_flagField));
	}
	for(int i = 0;i < 64;++i){;
		IdxValueUnitFieldList luma_weight_l1_flagField = IdxValueUnitFieldList("luma_weight_l1_flag", pwt.luma_weight_l1_flag[i], i);
		if(pwt.luma_weight_l1_flag[i] == 1){
			luma_weight_l1_flagField.addItem(IdxUnitField("luma_weight_l1", pwt.luma_weight_l1[i], i));
			luma_weight_l1_flagField.addItem(IdxUnitField("luma_offset_l1", pwt.luma_offset_l1[i], i));
			IdxValueUnitFieldList chroma_weight_l1_flagField = IdxValueUnitFieldList("chroma_weight_l1_flag", pwt.chroma_weight_l1_flag[i], i);
			luma_weight_l1_flagField.addItem(std::move(chroma_weight_l1_flagField));
			if(pwt.chroma_weight_l1_flag[i] == 1){
				for(int j = 0;j < 2;++j) {
					chroma_weight_l1_flagField.addItem(DblIdxUnitField("chroma_weight_l1", pwt.chroma_weight_l1[i][j], i, j));
					chroma_weight_l1_flagField.addItem(DblIdxUnitField("chroma_offset_l1", pwt.chroma_offset_l1[i][j], i, j));
				}
			}
		}
		pwtField.addItem(std::move(luma_weight_l1_flagField));
	}
	fields.addItem(std::move(pwtField));
	if(pPps->entropy_coding_mode_flag && slice_type != SliceType_I && slice_type != SliceType_SI) fields.addItem(UnitField("cabac_init_idc", cabac_init_idc));
	fields.addItem(UnitField("slice_qp_delta", slice_qp_delta));
	if(slice_type == SliceType_SP || slice_type == SliceType_SI){
		if(slice_type == SliceType_SP) fields.addItem(UnitField("sp_for_switch_flag", sp_for_switch_flag));
		fields.addItem(UnitField("slice_qs_delta", slice_qs_delta));
	}
	if(pPps->deblocking_filter_control_present_flag){
		ValueUnitFieldList disable_deblocking_filter_idcField = ValueUnitFieldList("disable_deblocking_filter_idc", disable_deblocking_filter_idc);
		if(disable_deblocking_filter_idc != 1){
			disable_deblocking_filter_idcField.addItem(UnitField("slice_alpha_c0_offset_div2", slice_alpha_c0_offset_div2));
			disable_deblocking_filter_idcField.addItem(UnitField("slice_beta_offset_div2", slice_beta_offset_div2));
		}
		fields.addItem(std::move(disable_deblocking_filter_idcField));
	}
	if(pPps->num_slice_groups_minus1 > 0 && pPps->slice_group_map_type >= 3 && pPps->slice_group_map_type <= 5){
		fields.addItem(UnitField("slice_group_change_cycle", slice_group_change_cycle));
	}

	fields.addItem(UnitField("IdrPicFlag", IdrPicFlag));
    fields.addItem(UnitField("PrevRefFrameNum", PrevRefFrameNum));

	return fields;
}

void H264Slice::validate(){
	H264NAL::validate();
	if(!completelyParsed) return;
	if(slice_type == H264Slice::SliceType_Unspecified){
		majorErrors.push_back("[Slice] Invalid slice type");
	}
	if(pic_parameter_set_id > 255){
		minorErrors.push_back(StringFormatter::formatString("[Slice] pic_parameter_set_id value (%ld) not in valid range (0.255)", pic_parameter_set_id));
	}
	H264PPS* pH264PPS;
	auto referencedPPS = H264PPS::PPSMap.find(pic_parameter_set_id);
	if(referencedPPS == H264PPS::PPSMap.end()){
		majorErrors.push_back(StringFormatter::formatString("[Slice] reference to unknown PPS (%ld)", pic_parameter_set_id));
		return;
	}
	pH264PPS = referencedPPS->second;
	H264SPS* pH264SPS;
	auto referencedSPS = H264SPS::SPSMap.find(pH264PPS->seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()){
		majorErrors.push_back(StringFormatter::formatString("[Slice] reference to unknown SPS (%ld)", pH264PPS->seq_parameter_set_id));
		return;
	}
	pH264SPS = referencedSPS->second;
	if(nal_unit_type == H264NAL::UnitType_IDRFrame || pH264SPS->max_num_ref_frames == 0){
		switch(slice_type){
			case H264Slice::SliceType_I: case H264Slice::SliceType_SI: break;
			default:
				majorErrors.push_back(StringFormatter::formatString("[Slice] slice_type value (%ld) of IDR should be in [2, 4, 7, 9]", slice_type-1));
				break;
		}
	}
	if (pH264SPS->separate_colour_plane_flag){
		if(colour_plane_id > 2){
			minorErrors.push_back(StringFormatter::formatString("[Slice] colour_plane_id value (%ld) not in valid range (0..2)", colour_plane_id));
		}
	}

	if(nal_unit_type == H264NAL::UnitType_IDRFrame && frame_num != 0){
		minorErrors.push_back(StringFormatter::formatString("[Slice] frame_num of an IDR picture (%ld) should be 0", frame_num));
	}

	if (pH264SPS->pic_order_cnt_type == 0){
		if(pic_order_cnt_lsb > pH264SPS->MaxPicOrderCntLsb-1){
			minorErrors.push_back(StringFormatter::formatString("[Slice] pic_order_cnt_lsb value (%ld) not in valid range (0..{})", pic_order_cnt_lsb, pH264SPS->MaxPicOrderCntLsb-1));
		}
	}
	if (pH264PPS->redundant_pic_cnt_present_flag){
		if(redundant_pic_cnt > 127){
			minorErrors.push_back(StringFormatter::formatString("[Slice] redundant_pic_cnt (%ld) not in valid range (0..127)", redundant_pic_cnt));
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
