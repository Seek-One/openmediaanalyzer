#include <iostream>
#include <cstring>
#include <sstream>
#include <cmath>

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

H264SEI::~H264SEI(){
	for(H264SEIMessage* message : messages) delete message;
	messages.clear();
}

H264SEIMessage::H264SEIMessage(){
	payloadType = UINT8_MAX;
}

std::vector<std::string> H264SEIMessage::dump_fields(){
	return std::vector<std::string>();
}

void H264SEIMessage::validate(){}

std::vector<std::string> H264SEI::dump_fields(){
	std::vector<std::string> fields;
	for(H264SEIMessage* message : messages){
		std::vector<std::string> msgFields = message->dump_fields();
		fields.insert(fields.end(), msgFields.begin(), msgFields.end());
	}
	return fields;
}

void H264SEI::validate(){
	H264NAL::validate();
	for(H264SEIMessage* message : messages){
		message->validate();
		errors.insert(errors.end(), message->errors.begin(), message->errors.end());
		message->errors.clear();
	}
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

void H264SEIBufferingPeriod::validate(){
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()){
		errors.push_back((std::ostringstream() << "[SEI Buffering period] unknown reference to a SPS unit (" << (int)seq_parameter_set_id << ")").str());
		return;
	}
	H264SPS* pH264SPS = referencedSPS->second;
	if(seq_parameter_set_id > 31){
		errors.push_back((std::ostringstream() << "[SEI Buffering period] seq_parameter_set_id value (" << (int)seq_parameter_set_id << ") not in valid range (0..31)").str());
	}

	if(pH264SPS->nal_hrd_parameters_present_flag){
		for(int SchedSelIdx = 0;SchedSelIdx <= pH264SPS->nal_cpb_cnt_minus1;++SchedSelIdx){
			int CpbSize = (pH264SPS->nal_cpb_size_value_minus1[SchedSelIdx]+1)*(pow(2, 4+pH264SPS->nal_cpb_size_scale));
			int BitRate = (pH264SPS->nal_bit_rate_value_minus1[SchedSelIdx]+1)*(pow(2, 6+pH264SPS->nal_bit_rate_scale));
			int delay_limit = 90000 * (CpbSize/BitRate);
			if(nal_initial_cpb_removal_delay[SchedSelIdx] == 0 || nal_initial_cpb_removal_delay[SchedSelIdx] > delay_limit){
				errors.push_back((std::ostringstream() << "[SEI Buffering period] nal_initial_cpb_removal_delay[" << SchedSelIdx << "] value (" << nal_initial_cpb_removal_delay[SchedSelIdx] << ") not in valid range (1.." << delay_limit << ")").str());
			}
		}
	}

	if(pH264SPS->vcl_hrd_parameters_present_flag){
		for(int SchedSelIdx = 0;SchedSelIdx <= pH264SPS->vcl_cpb_cnt_minus1;++SchedSelIdx){
			int CpbSize = (pH264SPS->vcl_cpb_size_value_minus1[SchedSelIdx]+1)*(pow(2, 4+pH264SPS->vcl_cpb_size_scale));
			int BitRate = (pH264SPS->vcl_bit_rate_value_minus1[SchedSelIdx]+1)*(pow(2, 6+pH264SPS->vcl_bit_rate_scale));
			int delay_limit = 90000 * (CpbSize/BitRate);
			if(vcl_initial_cpb_removal_delay[SchedSelIdx] == 0 || vcl_initial_cpb_removal_delay[SchedSelIdx] > delay_limit){
				errors.push_back((std::ostringstream() << "[SEI Buffering period] vcl_initial_cpb_removal_delay[" << SchedSelIdx << "] value (" << vcl_initial_cpb_removal_delay[SchedSelIdx] << ") not in valid range (1.." << delay_limit << ")").str());
			}
		}
	}
}

std::vector<std::string> H264SEIPicTiming::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Pic timing");
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) return fields;
	H264SPS* pSps = referencedSPS->second;
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

void H264SEIPicTiming::validate(){
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) {
		errors.push_back((std::ostringstream() << "[SEI Pic timing] unknown reference to a SPS unit (" << (int)seq_parameter_set_id << ")").str());
		return;
	}
	H264SPS* pSps = referencedSPS->second;
	if(pSps->pic_struct_present_flag){
		if(pic_struct > 8) errors.push_back((std::ostringstream() << "[SEI Pic timing] pic_struct value (" << pic_struct << ") not in valid range (0..8)").str());
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
			if(clock_timestamp_flag[i]){
				if(ct_type[i] > 2){
					errors.push_back((std::ostringstream() << "[SEI Pic timing] ct_type[" << i << "] value (" << ct_type[i] << ") not in valid range (0..2)").str());
				}
				if(counting_type[i] > 6){
					errors.push_back((std::ostringstream() << "[SEI Pic timing] counting_type[" << i << "] value (" << counting_type[i] << ") not in valid range (0..6)").str());
				}
				if(seconds_value[i] > 59){
					errors.push_back((std::ostringstream() << "[SEI Pic timing] seconds_value[" << i << "] value (" << seconds_value[i] << ") not in valid range (0..59)").str());
				}
				if(minutes_value[i] > 59){
					errors.push_back((std::ostringstream() << "[SEI Pic timing] minutes_value[" << i << "] value (" << minutes_value[i] << ") not in valid range (0..59)").str());
				}
				if(hours_value[i] > 23){
					errors.push_back((std::ostringstream() << "[SEI Pic timing] hours_value[" << i << "] value (" << hours_value[i] << ") not in valid range (0..23)").str());
				}
			}
		}
	}
}

std::vector<std::string> H264SEIFillerPayload::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Filler payload");
	return fields;
}

H264SEIUserDataUnregistered::~H264SEIUserDataUnregistered(){
	user_data_payload_byte.clear();
}

std::vector<std::string> H264SEIUserDataUnregistered::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI User data unregistered");

	int index = 31;
	std::ostringstream uuidStringStream = std::ostringstream();
	for(int len : {8, 4, 4, 4, 12}){
		for(int i = 0;i < len;++i){
			uuidStringStream << std::hex << (int)((uuid_iso_iec_11578 >> index*4)&& 0xFF);
			index--;
		}
		if(len != 12) uuidStringStream << "-";
	}
	fields.push_back((std::ostringstream() << "  uuid_iso_iec_11578:" << uuidStringStream.str()).str());
	for(int i = 0;i < user_data_payload_byte.size();++i){
		fields.push_back((std::ostringstream() << "    user_data_payload_byte[" << i << "]:" << user_data_payload_byte[i]).str());
	}
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

void H264SEIRecoveryPoint::validate(){
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) {
		errors.push_back((std::ostringstream() << "[SEI Recovery point] unknown reference to a SPS unit (" << (int)seq_parameter_set_id << ")").str());
		return;
	}
	H264SPS* pSps = referencedSPS->second;
	int MaxNumFrames = pow(2, 4+pSps->log2_max_frame_num_minus4);
	if(recovery_frame_cnt > MaxNumFrames-1){
		errors.push_back((std::ostringstream() << "[SEI Recovery point] recovery_frame_cnt value (" << recovery_frame_cnt << ") not in valid range (0.." << MaxNumFrames-1 << ")").str());
	}
}

std::vector<std::string> H264SEIFullFrameFreeze::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Full frame freeze");
	fields.push_back((std::ostringstream() << "  full_frame_freeze_repetition_period:" << (int)full_frame_freeze_repetition_period).str());
	return fields;
}

void H264SEIFullFrameFreeze::validate(){
	if(full_frame_freeze_repetition_period > 16384){
		errors.push_back((std::ostringstream() << "[SEI Full frame freeze] full_frame_freeze_repetition_period value (" << full_frame_freeze_repetition_period << ") not in valid range (0..16384)").str());
	}
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

void H264SEIMvcdViewScalabilityInfo::validate(){
	if(num_operation_points_minus1 > 1023){
		errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] num_operation_points_minus1 value (" << (int)num_operation_points_minus1 << ") not in valid range (0..1023)").str());
	}
	for(int i = 0;i <= num_operation_points_minus1;++i){
		if(num_target_output_views_minus1[i] > 1023){
			errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] num_target_output_views_minus1[" << i <<"] value (" << num_target_output_views_minus1[i] << ") not in valid range (0..1023)").str());
		}
		for(int j = 0;j <= num_target_output_views_minus1[i];++j){
			if(view_id[i][j] > 1023){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] view_id[" << i <<"][" << j << "] value (" << view_id[i][j] << ") not in valid range (0..1023)").str());
			}
		}
		if(frm_rate_info_present_flag[i]){
			if(constant_frm_rate_idc[i] > 2){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] constant_frm_rate_idc[" << i <<"] value (" << (int)constant_frm_rate_idc[i] << ") not in valid range (0..2)").str());
			}
		}
		if(view_dependency_info_present_flag[i]){
			if(num_directly_dependant_views[i] > 16){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] num_directly_dependant_views[" << i <<"] value (" << (int)num_directly_dependant_views[i] << ") not in valid range (0..16)").str());
			}
			for(int j = 0;j < num_directly_dependant_views[i];++j){
				if(directly_dependant_view_id[i][j] > 1023){
					errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] directly_dependant_view_id[" << i <<"][" << j << "] value (" << (int)directly_dependant_view_id[i][j] << ") not in valid range (0..1023)").str());
				}
			}
		}
		if(parameters_sets_info_present_flag[i]) {
			if(num_seq_parameter_sets[i] > 32){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] num_seq_parameter_sets[" << i <<"] value (" << (int)num_seq_parameter_sets[i] << ") not in valid range (0..32)").str());
			}
			for(int j = 0;j <= num_seq_parameter_sets[i];++j){
				if(seq_parameter_set_id_delta[i][j] > 31){
					errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] seq_parameter_set_id_delta[" << i <<"][" << j << "] value (" << (int)seq_parameter_set_id_delta[i][j] << ") not in valid range (0..31)").str());
				}
			}
			if(num_subset_seq_parameter_set_minus1[i] > 32){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] num_subset_seq_parameter_set_minus1[" << i <<"] value (" << (int)num_subset_seq_parameter_set_minus1[i] << ") not in valid range (0..32)").str());
			}
			for(int j = 0;j <= num_subset_seq_parameter_set_minus1[i];++j){
				if(seq_parameter_set_id_delta[i][j] > 31){
					errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] seq_parameter_set_id_delta[" << i <<"][" << j << "] value (" << (int)seq_parameter_set_id_delta[i][j] << ") not in valid range (0..31)").str());
				}
			}
		}
		if(bitstream_restriction_info_present_flag[i]){
			if(max_bytes_per_pic_denom[i] > 16){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] max_bytes_per_pic_denom[" << i <<"] value (" << (int)max_bytes_per_pic_denom[i] << ") not in valid range (0..16)").str());
			}
			if(max_bits_per_mb_denom[i] > 16){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] max_bits_per_mb_denom[" << i <<"] value (" << (int)max_bits_per_mb_denom[i] << ") not in valid range (0..16)").str());
			}
			if(log2_max_mv_length_horizontal[i] > 16){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] log2_max_mv_length_horizontal[" << i <<"] value (" << (int)log2_max_mv_length_horizontal[i] << ") not in valid range (0..16)").str());
			}
			if(log2_max_mv_length_vertical[i] > 16){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] max_bits_per_mb_denom[" << i <<"] value (" << (int)log2_max_mv_length_vertical[i] << ") not in valid range (0..16)").str());
			}
			if(num_reorder_frames[i] > 16){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] num_reorder_frames[" << i <<"] value (" << (int)num_reorder_frames[i] << ") not in valid range (0..16)").str());
			}
			auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_seq_id);
			if(referencedSPS == H264SPS::SPSMap.end()) return;
			H264SPS* pSps = referencedSPS->second;
			if(num_reorder_frames[i] > pSps->MaxDpbFrames){
				errors.push_back((std::ostringstream() << "[SEI Mcvd view scalability info] num_reorder_frames[" << i <<"] value (" << num_reorder_frames[i] << ") not in valid range (0.." << (int)pSps->MaxDpbFrames << ")").str());
			}
		}
	}
}
