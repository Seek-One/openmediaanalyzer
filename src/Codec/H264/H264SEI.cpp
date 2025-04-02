#include <iostream>
#include <cstring>
#include <sstream>

#include "H264SPS.h"

#include "H264SEI.h"

H264SEI::H264SEI():
	H264SEI(0, 0, 0, nullptr)
{}

H264SEI::H264SEI(uint8_t forbidden_zero_bit, uint8_t nal_ref_idc, uint32_t nal_size, uint8_t* nal_data):
	H264NAL(forbidden_zero_bit, nal_ref_idc, nal_size, nal_data)
{
	nal_unit_type = UnitType_SEI;
}

// H264SEI::H264SEI(H264SEI* pH264SEI):
// 	messages(pH264SEI->messages)
// {
// 	forbidden_zero_bit = pH264SEI->forbidden_zero_bit;
// 	nal_ref_idc = pH264SEI->nal_ref_idc;
// 	nal_unit_type = UnitType_SEI;
// 	nal_size = pH264SEI->nal_size;
// }


H264SEI::~H264SEI(){
	if(nal_data) delete[] nal_data;
	for(H264SEIMessage* message : messages) delete message;
	messages.clear();
}

H264SEIMessage::H264SEIMessage(){
	payloadType = -1;
}

std::vector<std::string> H264SEI::dump_fields(){
	std::vector<std::string> fields;
	for(H264SEIMessage* message : messages){
		std::vector<std::string> msgFields;
		switch(message->payloadType){
			case SEI_BUFFERING_PERIOD:
				msgFields = reinterpret_cast<H264SEIBufferingPeriod*>(message)->dump_fields();
				break;
			case SEI_PIC_TIMING:
				msgFields = reinterpret_cast<H264SEIPicTiming*>(message)->dump_fields();
				break;
			case SEI_FILLER_PAYLOAD:
				msgFields = reinterpret_cast<H264SEIFillerPayload*>(message)->dump_fields();
				break;
			case SEI_RECOVERY_POINT:
				msgFields = reinterpret_cast<H264SEIRecoveryPoint*>(message)->dump_fields();
				break;
			case SEI_FULL_FRAME_FREEZE:
				msgFields = reinterpret_cast<H264SEIFullFrameFreeze*>(message)->dump_fields();
				break;
			case SEI_MVCD_VIEW_SCALABILITY_INFO:
				msgFields = reinterpret_cast<H264SEIMvcdViewScalabilityInfo*>(message)->dump_fields();
				break;
		}
		fields.insert(fields.end(), msgFields.begin(), msgFields.end());
	}
	return fields;
}

std::vector<std::string> H264SEIBufferingPeriod::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Buffering period");
	fields.push_back((std::ostringstream() << "  seq_parameter_set_id:" << (int)seq_parameter_set_id).str());
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) return fields;
	H264SPS* pSps = referencedSPS->second;
 	if(pSps->nal_hrd_parameters_present_flag){
		for(int i = 0;i <= pSps->nal_cpb_cnt_minus1;++i){
			fields.push_back((std::ostringstream() << "    nal_initial_cpb_removal_delay[" << i << "]:" << nal_initial_cpb_removal_delay[i]).str());
			fields.push_back((std::ostringstream() << "    nal_initial_cpb_removal_delay_offset[" << i << "]:" << nal_initial_cpb_removal_delay_offset[i]).str());
		}
	}
	if(pSps->vcl_hrd_parameters_present_flag){
		for(int i = 0;i <= pSps->vcl_cpb_cnt_minus1;++i){
			fields.push_back((std::ostringstream() << "    vcl_initial_cpb_removal_delay[" << i << "]:" << vcl_initial_cpb_removal_delay[i]).str());
			fields.push_back((std::ostringstream() << "    vcl_initial_cpb_removal_delay_offset[" << i << "]:" << vcl_initial_cpb_removal_delay_offset[i]).str());
		}
	}
	return fields;
}

std::vector<std::string> H264SEIPicTiming::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Pic timing");
	H264SPS* pSps = H264SPS::SPSMap.find(seq_parameter_set_id)->second;
	if(pSps->nal_hrd_parameters_present_flag || pSps->vcl_hrd_parameters_present_flag){
		fields.push_back((std::ostringstream() << "  cpb_removal_delay:" << cpb_removal_delay).str());
		fields.push_back((std::ostringstream() << "  dpb_output_delay:" << dpb_output_delay).str());
	}
	if(pSps->pic_struct_present_flag){
		fields.push_back((std::ostringstream() << "  pic_struct:" << (int)pic_struct).str());
		int NumClockTS = 0;
		switch(pic_struct){
			case 0: case 1: case 2: 
			NumClockTS = 1;
			break;
			case 3: case 4: case 7:
			NumClockTS = 2;
			break;
			case 5: case 6: case 8:
			NumClockTS = 3;
			break;
		}
		for(int i = 0;i < NumClockTS;++i){
			fields.push_back((std::ostringstream() << "    clock_timestamp_flag[" << i << "]:" << (int)clock_timestamp_flag[i]).str());
			if(clock_timestamp_flag[i]){
				fields.push_back((std::ostringstream() << "      ct_type[" << i << "]:" << (int)ct_type[i]).str());
				fields.push_back((std::ostringstream() << "      nuit_field_based_flag[" << i << "]:" << (int)nuit_field_based_flag[i]).str());
				fields.push_back((std::ostringstream() << "      counting_type[" << i << "]:" << (int)counting_type[i]).str());
				fields.push_back((std::ostringstream() << "      discontinuity_flag[" << i << "]:" << (int)discontinuity_flag[i]).str());
				fields.push_back((std::ostringstream() << "      cnt_dropped_flag[" << i << "]:" << (int)cnt_dropped_flag[i]).str());
				fields.push_back((std::ostringstream() << "      n_frames[" << i << "]:" << (int)n_frames[i]).str());
				fields.push_back((std::ostringstream() << "      full_timestamp_flag[" << i << "]:" << (int)full_timestamp_flag[i]).str());
				if(full_timestamp_flag[i] || seconds_flag[i]) fields.push_back((std::ostringstream() << "        seconds_value[" << i << "]:" << (int)seconds_value[i]).str());
				if(full_timestamp_flag[i] || minutes_flag[i]) fields.push_back((std::ostringstream() << "        minutes_value[" << i << "]:" << (int)minutes_value[i]).str());
				if(full_timestamp_flag[i] || hours_flag[i]) fields.push_back((std::ostringstream() << "        hours_value[" << i << "]:" << (int)hours_value[i]).str());
				if(pSps->nal_time_offset_length > 0 || pSps->vcl_time_offset_length > 0){
					fields.push_back((std::ostringstream() << "      full_timestamp_flag[" << i << "]:" << (int)full_timestamp_flag[i]).str());
				}
			}
		}
	}
	return fields;
}

std::vector<std::string> H264SEIFillerPayload::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Filler payload");
	return fields;
}

std::vector<std::string> H264SEIRecoveryPoint::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Recovery point");
	fields.push_back((std::ostringstream() << "  recovery_frame_cnt:" << recovery_frame_cnt).str());
	fields.push_back((std::ostringstream() << "  exact_match_flag:" << (int)exact_match_flag).str());
	fields.push_back((std::ostringstream() << "  broken_link_flag:" << (int)broken_link_flag).str());
	fields.push_back((std::ostringstream() << "  changing_slice_group_idc:" << (int)changing_slice_group_idc).str());
	return fields;
}

std::vector<std::string> H264SEIFullFrameFreeze::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Full frame freeze");
	fields.push_back((std::ostringstream() << "  full_frame_freeze_repetition_period:" << (int)full_frame_freeze_repetition_period).str());
	return fields;
}

void MvcdViewScalabilityInfo_dump_movi_fields(std::vector<std::string> fields, const std::string& prefix, H264SEIMvcdViewScalabilityInfo::movi movi, int i, int j){
	fields.push_back((std::ostringstream() << prefix << "view_info_depth_view_present_flag[" << i << "][ " << j << "]:" << (int)movi.view_info_depth_view_present_flag).str());
	if(movi.view_info_depth_view_present_flag)fields.push_back((std::ostringstream() << prefix <<"mcvd_depth_view_flag[" << i << "][ " << j << "]:" << (int)movi.mvcd_depth_view_flag).str());
	fields.push_back((std::ostringstream() << prefix << "view_info_texture_view_present_flag[" << i << "][ " << j << "]:" << (int)movi.view_info_texture_view_present_flag).str());
	if(movi.view_info_texture_view_present_flag)fields.push_back((std::ostringstream() << prefix <<"mvcd_texture_view_flag[" << i << "][ " << j << "]:" << (int)movi.mvcd_texture_view_flag).str());
}

H264SEIMvcdViewScalabilityInfo::movi::movi(){
	view_info_depth_view_present_flag = 0;
	mvcd_depth_view_flag = 0;
	view_info_texture_view_present_flag = 0;
	mvcd_texture_view_flag = 0;
}

H264SEIMvcdViewScalabilityInfo::H264SEIMvcdViewScalabilityInfo(){
	num_operation_points_minus1 = 0;
	for(int i = 0;i < 1024;++i){
		operation_point_id[i] = 0;
		priority_id[i] = 0;
		temporal_id[i] = 0;
		num_target_output_views_minus1[i] = 0;
		profile_level_info_present_flag[i] = 0;
		bitrate_info_present_flag[i] = 0;
		frm_rate_info_present_flag[i] = 0;
		view_dependency_info_present_flag[i] = 0;
		bitstream_restriction_info_present_flag[i] = 0;
		op_profile_level_idc[i] = 0;
		avg_bitrate[i] = 0;
		max_bitrate[i] = 0;
		max_bitrate_calc_window[i] = 0;
		constant_frm_rate_idc[i] = 0;
		avg_frm_rate[i] = 0;
		num_directly_dependant_views[i] = 0;
		view_dependency_info_src_op_id[i] = 0;
		num_seq_parameter_sets[i] = 0;
		num_subset_seq_parameter_set_minus1[i] = 0;
		num_pic_parameter_set_minus1[i] = 0;
		parameter_sets_info_src_op_id[i] = 0;
		motion_vector_over_pic_boundaries_flag[i] = 0;
		max_bytes_per_pic_denom[i] = 0;
		max_bits_per_mb_denom[i] = 0;
		log2_max_mv_length_horizontal[i] = 0;
		log2_max_mv_length_vertical[i] = 0;
		num_reorder_frames[i] = 0;
		max_dec_frame_buffering[i] = 0;
		parameters_sets_info_present_flag[i] = 0;
		for(int j = 0;j < 16;++j){
			directly_dependant_view_id[i][j] = 0;
			directely_dependant_view_movi[i][j] = movi();
		}
		for(int j = 0;j < 32;++j){
			seq_parameter_set_id_delta[i][j] = 0;
			subset_seq_parameter_set_id_delta[i][j] = 0;
		}
		for(int j = 0;j < 256;++j) pic_parameter_set_id_delta[i][j] = 0;
		for(int j = 0;j < 1024;++j){
			view_id[i][j] = 0;
			view_movi[i][j] = movi();
		}
	}
}

std::vector<std::string> H264SEIMvcdViewScalabilityInfo::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Mvcd view scalability info");
	fields.push_back((std::ostringstream() << "  num_operation_points_minus1:" << (int)num_operation_points_minus1).str());
	for(int i = 0;i <= num_operation_points_minus1;++i){
		fields.push_back((std::ostringstream() << "    operation_point_id[" << i << "]:" << (int)operation_point_id[i]).str());
		fields.push_back((std::ostringstream() << "    priority_id[" << i << "]:" << (int)priority_id[i]).str());
		fields.push_back((std::ostringstream() << "    temporal_id[" << i << "]:" << (int)temporal_id[i]).str());
		fields.push_back((std::ostringstream() << "    num_target_output_views_minus1[" << i << "]:" << (int)num_target_output_views_minus1[i]).str());
		for(int j = 0;j < num_target_output_views_minus1[i];++j){
			fields.push_back((std::ostringstream() << "      view_id[" << i << "][" << j << "]:" << (int)view_id[i][j]).str());
			MvcdViewScalabilityInfo_dump_movi_fields(fields, "      view_movi.", view_movi[i][j], i, j);
		}
		fields.push_back((std::ostringstream() << "    profile_level_info_present_flag[" << i << "]:" << (int)profile_level_info_present_flag[i]).str());
		if(profile_level_info_present_flag[i]) fields.push_back((std::ostringstream() << "      op_profile_level_idc[" << i << "]:" << op_profile_level_idc[i]).str());
		fields.push_back((std::ostringstream() << "    bitrate_info_present_flag[" << i << "]:" << (int)bitrate_info_present_flag[i]).str());
		if(bitrate_info_present_flag[i]) {
			fields.push_back((std::ostringstream() << "      avg_bitrate[" << i << "]:" << (int)avg_bitrate[i]).str());
			fields.push_back((std::ostringstream() << "      max_bitrate[" << i << "]:" << (int)max_bitrate[i]).str());
			fields.push_back((std::ostringstream() << "      max_bitrate_calc_window[" << i << "]:" << (int)max_bitrate_calc_window[i]).str());
			fields.push_back((std::ostringstream() << "      max_bitrate_calc_window[" << i << "]:" << (int)max_bitrate_calc_window[i]).str());
		}
		fields.push_back((std::ostringstream() << "    frm_rate_info_present_flag[" << i << "]:" << (int)frm_rate_info_present_flag[i]).str());
		if(frm_rate_info_present_flag[i]){
			fields.push_back((std::ostringstream() << "      constant_frm_rate_idc[" << i << "]:" << (int)constant_frm_rate_idc[i]).str());
			fields.push_back((std::ostringstream() << "      avg_frm_rate[" << i << "]:" << (int)avg_frm_rate[i]).str());
		}
		if(!num_target_output_views_minus1[i]) fields.push_back((std::ostringstream() << "    view_dependency_info_present_flag[" << i << "]:" << (int)view_dependency_info_present_flag[i]).str());
		if(view_dependency_info_present_flag[i]){
			fields.push_back((std::ostringstream() << "      num_directly_dependent_views[" << i << "]:" << (int)num_directly_dependant_views[i]).str());
			for(int j = 0;j < num_directly_dependant_views[i];++j){
				fields.push_back((std::ostringstream() << "        directly_dependant_view_id[" << i << "][" << j << "]:" << (int)directly_dependant_view_id[i][j]).str());
				MvcdViewScalabilityInfo_dump_movi_fields(fields, "        directly_dependant_view_movi.", directely_dependant_view_movi[i][j], i, j);
			}
		} else fields.push_back((std::ostringstream() << "      view_dependency_info_src_op_id[" << i << "]:" << (int)view_dependency_info_src_op_id[i]).str());
		fields.push_back((std::ostringstream() << "    parameter_sets_info_present_flag[" << i << "]:" << (int)parameters_sets_info_present_flag[i]).str());
		if(parameters_sets_info_present_flag[i]){
			fields.push_back((std::ostringstream() << "      num_seq_parameter_set_minus1[" << i << "]:" << (int)num_seq_parameter_sets[i]).str());
			for(int j = 0;j <= num_seq_parameter_sets[i];++j){
				fields.push_back((std::ostringstream() << "        seq_parameter_set_id_delta[" << i << "][" << j << "]:" << (int)seq_parameter_set_id_delta[i][j]).str());
			}
			fields.push_back((std::ostringstream() << "      num_subset_seq_parameter_set_minus1[" << i << "]:" << (int)num_subset_seq_parameter_set_minus1[i]).str());
			for(int j = 0;j <= num_subset_seq_parameter_set_minus1[i];++j){
				fields.push_back((std::ostringstream() << "        subset_seq_parameter_set_id_delta[" << i << "][" << j << "]:" << (int)subset_seq_parameter_set_id_delta[i][j]).str());
			}
			fields.push_back((std::ostringstream() << "      num_pic_parameter_set_minus1[" << i << "]:" << (int)num_pic_parameter_set_minus1[i]).str());
			for(int j = 0;j <= num_pic_parameter_set_minus1[i];++j){
				fields.push_back((std::ostringstream() << "        pic_parameter_set_id_delta[" << i << "][" << j << "]:" << (int)pic_parameter_set_id_delta[i][j]).str());
			}
		} else fields.push_back((std::ostringstream() << "      parameters_sets_info_src_op_id[" << i << "]:" << (int)parameter_sets_info_src_op_id[i]).str());
		fields.push_back((std::ostringstream() << "    bitrate_info_present_flag[" << i << "]:" << (int)bitrate_info_present_flag[i]).str());
		if(bitrate_info_present_flag[i]){
			fields.push_back((std::ostringstream() << "      num_seq_parameter_set_minus1[" << i << "]:" << (int)motion_vector_over_pic_boundaries_flag[i]).str());
			fields.push_back((std::ostringstream() << "      max_bytes_per_pic_denom[" << i << "]:" << (int)max_bytes_per_pic_denom[i]).str());
			fields.push_back((std::ostringstream() << "      max_bits_per_mb_denom[" << i << "]:" << (int)max_bits_per_mb_denom[i]).str());
			fields.push_back((std::ostringstream() << "      log2_max_mv_length_horizontal[" << i << "]:" << (int)log2_max_mv_length_horizontal[i]).str());
			fields.push_back((std::ostringstream() << "      log2_max_mv_length_vertical[" << i << "]:" << (int)log2_max_mv_length_vertical[i]).str());
			fields.push_back((std::ostringstream() << "      num_reorder_frames[" << i << "]:" << (int)num_reorder_frames[i]).str());
			fields.push_back((std::ostringstream() << "      max_dec_frame_buffering[" << i << "]:" << (int)max_dec_frame_buffering[i]).str());
		}
	}
	return fields;
}
