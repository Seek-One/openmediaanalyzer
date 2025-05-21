#include <iostream>
#include <cstring>
#include <fmt/core.h>
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
	if(!completelyParsed) return fields;
	for(H264SEIMessage* message : messages){
		std::vector<std::string> msgFields = message->dump_fields();
		fields.insert(fields.end(), msgFields.begin(), msgFields.end());
	}
	return fields;
}

void H264SEI::validate(){
	H264NAL::validate();
	if(!completelyParsed) return;
	for(H264SEIMessage* message : messages){
		message->validate();
		minorErrors.insert(minorErrors.end(), message->errors.begin(), message->errors.end());
		message->errors.clear();
	}
}

std::vector<std::string> H264SEIBufferingPeriod::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Buffering period");
	fields.push_back(fmt::format("  seq_parameter_set_id:{}", seq_parameter_set_id));
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) return fields;
	H264SPS* pSps = referencedSPS->second;
 	if(pSps->nal_hrd_parameters_present_flag){
		for(int i = 0;i <= pSps->nal_cpb_cnt_minus1;++i){
			fields.push_back(fmt::format("    nal_initial_cpb_removal_delay[{}]:{}", i, nal_initial_cpb_removal_delay[i]));
			fields.push_back(fmt::format("    nal_initial_cpb_removal_delay_offset[{}]:{}", i, nal_initial_cpb_removal_delay_offset[i]));
		}
	}
	if(pSps->vcl_hrd_parameters_present_flag){
		for(int i = 0;i <= pSps->vcl_cpb_cnt_minus1;++i){
			fields.push_back(fmt::format("    vcl_initial_cpb_removal_delay[{}]:{}", i, vcl_initial_cpb_removal_delay[i]));
			fields.push_back(fmt::format("    vcl_initial_cpb_removal_delay_offset[{}]:{}", i, vcl_initial_cpb_removal_delay_offset[i]));
		}
	}
	return fields;
}

void H264SEIBufferingPeriod::validate(){
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()){
		errors.push_back(fmt::format("[SEI Buffering period] unknown reference to a SPS unit ({})", seq_parameter_set_id));
		return;
	}
	H264SPS* pH264SPS = referencedSPS->second;
	if(seq_parameter_set_id > 31){
		errors.push_back(fmt::format("[SEI Buffering period] seq_parameter_set_id value ({}) not in valid range (0..31)", seq_parameter_set_id));
	}

	if(pH264SPS->nal_hrd_parameters_present_flag){
		for(int SchedSelIdx = 0;SchedSelIdx <= pH264SPS->nal_cpb_cnt_minus1;++SchedSelIdx){
			int CpbSize = (pH264SPS->nal_cpb_size_value_minus1[SchedSelIdx]+1)*(pow(2, 4+pH264SPS->nal_cpb_size_scale));
			int BitRate = (pH264SPS->nal_bit_rate_value_minus1[SchedSelIdx]+1)*(pow(2, 6+pH264SPS->nal_bit_rate_scale));
			int delay_limit = 90000 * (CpbSize/BitRate);
			if(nal_initial_cpb_removal_delay[SchedSelIdx] == 0 || nal_initial_cpb_removal_delay[SchedSelIdx] > delay_limit){
				errors.push_back(fmt::format("[SEI Buffering period] nal_initial_cpb_removal_delay[{}] value ({}) not in valid range (1..{})", SchedSelIdx, nal_initial_cpb_removal_delay[SchedSelIdx], delay_limit));
			}
		}
	}

	if(pH264SPS->vcl_hrd_parameters_present_flag){
		for(int SchedSelIdx = 0;SchedSelIdx <= pH264SPS->vcl_cpb_cnt_minus1;++SchedSelIdx){
			int CpbSize = (pH264SPS->vcl_cpb_size_value_minus1[SchedSelIdx]+1)*(pow(2, 4+pH264SPS->vcl_cpb_size_scale));
			int BitRate = (pH264SPS->vcl_bit_rate_value_minus1[SchedSelIdx]+1)*(pow(2, 6+pH264SPS->vcl_bit_rate_scale));
			int delay_limit = 90000 * (CpbSize/BitRate);
			if(vcl_initial_cpb_removal_delay[SchedSelIdx] == 0 || vcl_initial_cpb_removal_delay[SchedSelIdx] > delay_limit){
				errors.push_back(fmt::format("[SEI Buffering period] vcl_initial_cpb_removal_delay[{}] value ({}) not in valid range (1..{})", SchedSelIdx, vcl_initial_cpb_removal_delay[SchedSelIdx], delay_limit));
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
		fields.push_back(fmt::format("  cpb_removal_delay:{}", cpb_removal_delay));
		fields.push_back(fmt::format("  dpb_output_delay:{}", dpb_output_delay));
	}
	if(pSps->pic_struct_present_flag){
		fields.push_back(fmt::format("  pic_struct:{}", pic_struct));
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
			fields.push_back(fmt::format("    clock_timestamp_flag[{}]:{}", i, clock_timestamp_flag[i]));
			if(clock_timestamp_flag[i]){
				fields.push_back(fmt::format("      ct_type[{}]:{}", i, ct_type[i]));
				fields.push_back(fmt::format("      nuit_field_based_flag[{}]:{}", i, nuit_field_based_flag[i]));
				fields.push_back(fmt::format("      counting_type[{}]:{}", i, counting_type[i]));
				fields.push_back(fmt::format("      discontinuity_flag[{}]:{}", i, discontinuity_flag[i]));
				fields.push_back(fmt::format("      cnt_dropped_flag[{}]:{}", i, cnt_dropped_flag[i]));
				fields.push_back(fmt::format("      n_frames[{}]:{}", i, n_frames[i]));
				fields.push_back(fmt::format("      full_timestamp_flag[{}]:{}", i, full_timestamp_flag[i]));
				if(full_timestamp_flag[i] || seconds_flag[i]) fields.push_back(fmt::format("        seconds_value[{}]:{}", i, seconds_value[i]));
				if(full_timestamp_flag[i] || minutes_flag[i]) fields.push_back(fmt::format("        minutes_value[{}]:{}", i, minutes_value[i]));
				if(full_timestamp_flag[i] || hours_flag[i]) fields.push_back(fmt::format("        hours_value[{}]:{}", i, hours_value[i]));
				if(pSps->nal_time_offset_length > 0 || pSps->vcl_time_offset_length > 0){
					fields.push_back(fmt::format("      full_timestamp_flag[{}]:{}", i, full_timestamp_flag[i]));
				}
			}
		}
	}
	return fields;
}

void H264SEIPicTiming::validate(){
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) {
		errors.push_back(fmt::format("[SEI Pic timing] unknown reference to a SPS unit ({})", seq_parameter_set_id));
		return;
	}
	H264SPS* pSps = referencedSPS->second;
	if(pSps->pic_struct_present_flag){
		if(pic_struct > 8) errors.push_back(fmt::format("[SEI Pic timing] pic_struct value ({}) not in valid range (0..8)", pic_struct));
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
					errors.push_back(fmt::format("[SEI Pic timing] ct_type[{}] value ({}) not in valid range (0..2)", i, ct_type[i]));
				}
				if(counting_type[i] > 6){
					errors.push_back(fmt::format("[SEI Pic timing] counting_type[{}] value ({}) not in valid range (0..6)", i, counting_type[i]));
				}
				if(seconds_value[i] > 59){
					errors.push_back(fmt::format("[SEI Pic timing] seconds_value[{}] value ({}) not in valid range (0..59)", i, seconds_value[i]));
				}
				if(minutes_value[i] > 59){
					errors.push_back(fmt::format("[SEI Pic timing] minutes_value[{}] value ({}) not in valid range (0..59)", i, minutes_value[i]));
				}
				if(hours_value[i] > 23){
					errors.push_back(fmt::format("[SEI Pic timing] hours_value[{}] value ({}) not in valid range (0..23)", i, hours_value[i]));
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
	fields.push_back(fmt::format("  uuid_iso_iec_11578:{}", uuidStringStream.str()));
	for(int i = 0;i < user_data_payload_byte.size();++i){
		fields.push_back(fmt::format("    user_data_payload_byte[{}]:{}", i, user_data_payload_byte[i]));
	}
	return fields;
}

std::vector<std::string> H264SEIRecoveryPoint::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Recovery point");
	fields.push_back(fmt::format("  recovery_frame_cnt:{}", recovery_frame_cnt));
	fields.push_back(fmt::format("  exact_match_flag:{}", exact_match_flag));
	fields.push_back(fmt::format("  broken_link_flag:{}", broken_link_flag));
	fields.push_back(fmt::format("  changing_slice_group_idc:{}", changing_slice_group_idc));
	return fields;
}

void H264SEIRecoveryPoint::validate(){
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) {
		errors.push_back(fmt::format("[SEI Recovery point] unknown reference to a SPS unit ({})", seq_parameter_set_id));
		return;
	}
	H264SPS* pSps = referencedSPS->second;
	int MaxNumFrames = pow(2, 4+pSps->log2_max_frame_num_minus4);
	if(recovery_frame_cnt > MaxNumFrames-1){
		errors.push_back(fmt::format("[SEI Recovery point] recovery_frame_cnt value ({}) not in valid range (0..{})", recovery_frame_cnt, MaxNumFrames-1));
	}
}

std::vector<std::string> H264SEIFullFrameFreeze::dump_fields(){
	std::vector<std::string> fields;
	fields.push_back("SEI Full frame freeze");
	fields.push_back(fmt::format("  full_frame_freeze_repetition_period:{}", full_frame_freeze_repetition_period));
	return fields;
}

void H264SEIFullFrameFreeze::validate(){
	if(full_frame_freeze_repetition_period > 16384){
		errors.push_back(fmt::format("[SEI Full frame freeze] full_frame_freeze_repetition_period value ({}) not in valid range (0..16384)", full_frame_freeze_repetition_period));
	}
}

void MvcdViewScalabilityInfo_dump_movi_fields(std::vector<std::string> fields, const std::string& prefix, H264SEIMvcdViewScalabilityInfo::movi movi, int i, int j){
	fields.push_back(fmt::format("{}view_info_depth_view_present_flag[{}][{}]:{}", prefix, i, j, movi.view_info_depth_view_present_flag));
	if(movi.view_info_depth_view_present_flag)fields.push_back(fmt::format("{}mcvd_depth_view_flag[{}][{}]:{}", prefix, i, j, movi.mvcd_depth_view_flag));
	fields.push_back(fmt::format("{}view_info_texture_view_present_flag[{}][{}]:{}", prefix, i, j, movi.view_info_texture_view_present_flag));
	if(movi.view_info_texture_view_present_flag)fields.push_back(fmt::format("{}mvcd_texture_view_flag[{}][{}]:{}", prefix, i, j, movi.mvcd_texture_view_flag));
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
	fields.push_back(fmt::format("  num_operation_points_minus1:{}", num_operation_points_minus1));
	for(int i = 0;i <= num_operation_points_minus1;++i){
		fields.push_back(fmt::format("    operation_point_id[{}]:{}", i, operation_point_id[i]));
		fields.push_back(fmt::format("    priority_id[{}]:{}", i, priority_id[i]));
		fields.push_back(fmt::format("    temporal_id[{}]:{}", i, temporal_id[i]));
		fields.push_back(fmt::format("    num_target_output_views_minus1[{}]:{}", i, num_target_output_views_minus1[i]));
		for(int j = 0;j < num_target_output_views_minus1[i];++j){
			fields.push_back(fmt::format("      view_id[{}][{}]:{}", i, j, view_id[i][j]));
			MvcdViewScalabilityInfo_dump_movi_fields(fields, "      view_movi.", view_movi[i][j], i, j);
		}
		fields.push_back(fmt::format("    profile_level_info_present_flag[{}]:{}", i, profile_level_info_present_flag[i]));
		if(profile_level_info_present_flag[i]) fields.push_back(fmt::format("      op_profile_level_idc[{}]:{}", i, op_profile_level_idc[i]));
		fields.push_back(fmt::format("    bitrate_info_present_flag[{}]:{}", i, bitrate_info_present_flag[i]));
		if(bitrate_info_present_flag[i]) {
			fields.push_back(fmt::format("      avg_bitrate[{}]:{}", i, avg_bitrate[i]));
			fields.push_back(fmt::format("      max_bitrate[{}]:{}", i, max_bitrate[i]));
			fields.push_back(fmt::format("      max_bitrate_calc_window[{}]:{}", i, max_bitrate_calc_window[i]));
			fields.push_back(fmt::format("      max_bitrate_calc_window[{}]:{}", i, max_bitrate_calc_window[i]));
		}
		fields.push_back(fmt::format("    frm_rate_info_present_flag[{}]:{}", i, frm_rate_info_present_flag[i]));
		if(frm_rate_info_present_flag[i]){
			fields.push_back(fmt::format("      constant_frm_rate_idc[{}]:{}", i, constant_frm_rate_idc[i]));
			fields.push_back(fmt::format("      avg_frm_rate[{}]:{}", i, avg_frm_rate[i]));
		}
		if(!num_target_output_views_minus1[i]) fields.push_back(fmt::format("    view_dependency_info_present_flag[{}]:{}", i, view_dependency_info_present_flag[i]));
		if(view_dependency_info_present_flag[i]){
			fields.push_back(fmt::format("      num_directly_dependent_views[{}]:{}", i, num_directly_dependant_views[i]));
			for(int j = 0;j < num_directly_dependant_views[i];++j){
				fields.push_back(fmt::format("        directly_dependant_view_id[{}][{}]:{}", i, j, directly_dependant_view_id[i][j]));
				MvcdViewScalabilityInfo_dump_movi_fields(fields, "        directly_dependant_view_movi.", directely_dependant_view_movi[i][j], i, j);
			}
		} else fields.push_back(fmt::format("      view_dependency_info_src_op_id[{}]:{}", i, view_dependency_info_src_op_id[i]));
		fields.push_back(fmt::format("    parameter_sets_info_present_flag[{}]:{}", i, parameters_sets_info_present_flag[i]));
		if(parameters_sets_info_present_flag[i]){
			fields.push_back(fmt::format("      num_seq_parameter_set_minus1[{}]:{}", i, num_seq_parameter_sets[i]));
			for(int j = 0;j <= num_seq_parameter_sets[i];++j){
				fields.push_back(fmt::format("        seq_parameter_set_id_delta[{}][{}]:{}", i, j, seq_parameter_set_id_delta[i][j]));
			}
			fields.push_back(fmt::format("      num_subset_seq_parameter_set_minus1[{}]:{}", i, num_subset_seq_parameter_set_minus1[i]));
			for(int j = 0;j <= num_subset_seq_parameter_set_minus1[i];++j){
				fields.push_back(fmt::format("        subset_seq_parameter_set_id_delta[{}][{}]:{}", i, j, subset_seq_parameter_set_id_delta[i][j]));
			}
			fields.push_back(fmt::format("      num_pic_parameter_set_minus1[{}]:{}", i, num_pic_parameter_set_minus1[i]));
			for(int j = 0;j <= num_pic_parameter_set_minus1[i];++j){
				fields.push_back(fmt::format("        pic_parameter_set_id_delta[{}][{}]:{}", i, j, pic_parameter_set_id_delta[i][j]));
			}
		} else fields.push_back(fmt::format("      parameters_sets_info_src_op_id[{}]:{}", i, parameter_sets_info_src_op_id[i]));
		fields.push_back(fmt::format("    bitrate_info_present_flag[{}]:{}", i, bitrate_info_present_flag[i]));
		if(bitrate_info_present_flag[i]){
			fields.push_back(fmt::format("      num_seq_parameter_set_minus1[{}]:{}", i, motion_vector_over_pic_boundaries_flag[i]));
			fields.push_back(fmt::format("      max_bytes_per_pic_denom[{}]:{}", i, max_bytes_per_pic_denom[i]));
			fields.push_back(fmt::format("      max_bits_per_mb_denom[{}]:{}", i, max_bits_per_mb_denom[i]));
			fields.push_back(fmt::format("      log2_max_mv_length_horizontal[{}]:{}", i, log2_max_mv_length_horizontal[i]));
			fields.push_back(fmt::format("      log2_max_mv_length_vertical[{}]:{}", i, log2_max_mv_length_vertical[i]));
			fields.push_back(fmt::format("      num_reorder_frames[{}]:{}", i, num_reorder_frames[i]));
			fields.push_back(fmt::format("      max_dec_frame_buffering[{}]:{}", i, max_dec_frame_buffering[i]));
		}
	}
	return fields;
}

void H264SEIMvcdViewScalabilityInfo::validate(){
	if(num_operation_points_minus1 > 1023){
		errors.push_back(fmt::format("[SEI Mcvd view scalability info] num_operation_points_minus1 value ({}) not in valid range (0..1023)", num_operation_points_minus1));
	}
	for(int i = 0;i <= num_operation_points_minus1;++i){
		if(num_target_output_views_minus1[i] > 1023){
			errors.push_back(fmt::format("[SEI Mcvd view scalability info] num_target_output_views_minus1[{}] value ({}) not in valid range (0..1023)", i, num_target_output_views_minus1[i]));
		}
		for(int j = 0;j <= num_target_output_views_minus1[i];++j){
			if(view_id[i][j] > 1023){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] view_id[{}][{}] value ({}) not in valid range (0..1023)", i, j, view_id[i][j]));
			}
		}
		if(frm_rate_info_present_flag[i]){
			if(constant_frm_rate_idc[i] > 2){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] constant_frm_rate_idc[{}] value ({}) not in valid range (0..2)", i, constant_frm_rate_idc[i]));
			}
		}
		if(view_dependency_info_present_flag[i]){
			if(num_directly_dependant_views[i] > 16){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] num_directly_dependant_views[{}] value ({}) not in valid range (0..16)", i, num_directly_dependant_views[i]));
			}
			for(int j = 0;j < num_directly_dependant_views[i];++j){
				if(directly_dependant_view_id[i][j] > 1023){
					errors.push_back(fmt::format("[SEI Mcvd view scalability info] directly_dependant_view_id[{}][{}] value ({}) not in valid range (0..1023)", i, j, directly_dependant_view_id[i][j]));
				}
			}
		}
		if(parameters_sets_info_present_flag[i]) {
			if(num_seq_parameter_sets[i] > 32){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] num_seq_parameter_sets[{}] value ({}) not in valid range (0..32)", i, num_seq_parameter_sets[i]));
			}
			for(int j = 0;j <= num_seq_parameter_sets[i];++j){
				if(seq_parameter_set_id_delta[i][j] > 31){
					errors.push_back(fmt::format("[SEI Mcvd view scalability info] seq_parameter_set_id_delta[{}][{}] value ({}) not in valid range (0..31)", i, j, seq_parameter_set_id_delta[i][j]));
				}
			}
			if(num_subset_seq_parameter_set_minus1[i] > 32){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] num_subset_seq_parameter_set_minus1[{}] value ({}) not in valid range (0..32)", i, num_subset_seq_parameter_set_minus1[i]));
			}
			for(int j = 0;j <= num_subset_seq_parameter_set_minus1[i];++j){
				if(seq_parameter_set_id_delta[i][j] > 31){
					errors.push_back(fmt::format("[SEI Mcvd view scalability info] seq_parameter_set_id_delta[{}][{}] value ({}) not in valid range (0..31)", i, j, seq_parameter_set_id_delta[i][j]));
				}
			}
		}
		if(bitstream_restriction_info_present_flag[i]){
			if(max_bytes_per_pic_denom[i] > 16){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] max_bytes_per_pic_denom[{}] value ({}) not in valid range (0..16)", i, max_bytes_per_pic_denom[i]));
			}
			if(max_bits_per_mb_denom[i] > 16){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] max_bits_per_mb_denom[{}] value ({}) not in valid range (0..16)", i, max_bits_per_mb_denom[i]));
			}
			if(log2_max_mv_length_horizontal[i] > 16){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] log2_max_mv_length_horizontal[{}] value ({}) not in valid range (0..16)", i, log2_max_mv_length_horizontal[i]));
			}
			if(log2_max_mv_length_vertical[i] > 16){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] max_bits_per_mb_denom[{}] value ({}) not in valid range (0..16)", i, log2_max_mv_length_vertical[i]));
			}
			if(num_reorder_frames[i] > 16){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] num_reorder_frames[{}] value ({}) not in valid range (0..16)", i, num_reorder_frames[i]));
			}
			auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_seq_id);
			if(referencedSPS == H264SPS::SPSMap.end()) return;
			H264SPS* pSps = referencedSPS->second;
			if(num_reorder_frames[i] > pSps->MaxDpbFrames){
				errors.push_back(fmt::format("[SEI Mcvd view scalability info] num_reorder_frames[{}] value ({}) not in valid range (0..{})", i, num_reorder_frames[i], pSps->MaxDpbFrames));
			}
		}
	}
}
