#include <iostream>
#include <cstring>
#include <sstream>
#include <cmath>

#include "H264SPS.h"
#include "../../StringHelpers/StringFormatter.h"
#include "../../StringHelpers/UnitFieldList.h"

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

UnitFieldList H264SEIMessage::dump_fields(){
	return UnitFieldList("SEI Message");
}

void H264SEIMessage::validate(){}

UnitFieldList H264SEI::dump_fields(){
	UnitFieldList fields = UnitFieldList("Supplemental Enhancement Information", H264NAL::dump_fields());
	if(!completelyParsed) return fields;
	for(H264SEIMessage* message : messages){
		fields.addItem(message->dump_fields());
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

UnitFieldList H264SEIBufferingPeriod::dump_fields(){
	UnitFieldList fields = UnitFieldList("Buffering period");
	fields.addItem(UnitField("seq_parameter_set_id", seq_parameter_set_id));
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) return fields;
	H264SPS* pSps = referencedSPS->second;
 	if(pSps->nal_hrd_parameters_present_flag){
		for(int i = 0;i <= pSps->nal_cpb_cnt_minus1;++i){
			fields.addItem(IdxUnitField("nal_initial_cpb_removal_delay", nal_initial_cpb_removal_delay[i], i));
			fields.addItem(IdxUnitField("nal_initial_cpb_removal_delay_offset", nal_initial_cpb_removal_delay_offset[i], i));
		}
	}
	if(pSps->vcl_hrd_parameters_present_flag){
		for(int i = 0;i <= pSps->vcl_cpb_cnt_minus1;++i){
			fields.addItem(IdxUnitField("vcl_initial_cpb_removal_delay", vcl_initial_cpb_removal_delay[i], i));
			fields.addItem(IdxUnitField("vcl_initial_cpb_removal_delay_offset", vcl_initial_cpb_removal_delay_offset[i], i));
		}
	}
	return fields;
}

void H264SEIBufferingPeriod::validate(){
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()){
		errors.push_back(StringFormatter::formatString("[SEI Buffering period] unknown reference to a SPS unit (%ld)", seq_parameter_set_id));
		return;
	}
	H264SPS* pH264SPS = referencedSPS->second;
	if(seq_parameter_set_id > 31){
		errors.push_back(StringFormatter::formatString("[SEI Buffering period] seq_parameter_set_id value (%ld) not in valid range (0..31)", seq_parameter_set_id));
	}

	if(pH264SPS->nal_hrd_parameters_present_flag){
		for(int SchedSelIdx = 0;SchedSelIdx <= pH264SPS->nal_cpb_cnt_minus1;++SchedSelIdx){
			int CpbSize = (pH264SPS->nal_cpb_size_value_minus1[SchedSelIdx]+1)*(pow(2, 4+pH264SPS->nal_cpb_size_scale));
			int BitRate = (pH264SPS->nal_bit_rate_value_minus1[SchedSelIdx]+1)*(pow(2, 6+pH264SPS->nal_bit_rate_scale));
			uint32_t delay_limit = 90000 * (CpbSize/BitRate);
			if(nal_initial_cpb_removal_delay[SchedSelIdx] == 0 || nal_initial_cpb_removal_delay[SchedSelIdx] > delay_limit){
				errors.push_back(StringFormatter::formatString("[SEI Buffering period] nal_initial_cpb_removal_delay[%d] value (%ld) not in valid range (1..{})", SchedSelIdx, nal_initial_cpb_removal_delay[SchedSelIdx], delay_limit));
			}
		}
	}

	if(pH264SPS->vcl_hrd_parameters_present_flag){
		for(int SchedSelIdx = 0;SchedSelIdx <= pH264SPS->vcl_cpb_cnt_minus1;++SchedSelIdx){
			int CpbSize = (pH264SPS->vcl_cpb_size_value_minus1[SchedSelIdx]+1)*(pow(2, 4+pH264SPS->vcl_cpb_size_scale));
			int BitRate = (pH264SPS->vcl_bit_rate_value_minus1[SchedSelIdx]+1)*(pow(2, 6+pH264SPS->vcl_bit_rate_scale));
			uint32_t delay_limit = 90000 * (CpbSize/BitRate);
			if(vcl_initial_cpb_removal_delay[SchedSelIdx] == 0 || vcl_initial_cpb_removal_delay[SchedSelIdx] > delay_limit){
				errors.push_back(StringFormatter::formatString("[SEI Buffering period] vcl_initial_cpb_removal_delay[%d] value (%ld) not in valid range (1..{})", SchedSelIdx, vcl_initial_cpb_removal_delay[SchedSelIdx], delay_limit));
			}
		}
	}
}

UnitFieldList H264SEIPicTiming::dump_fields(){
	UnitFieldList fields = UnitFieldList("Picture timing");
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) return fields;
	H264SPS* pSps = referencedSPS->second;
	if(pSps->nal_hrd_parameters_present_flag || pSps->vcl_hrd_parameters_present_flag){
		fields.addItem(UnitField("cpb_removal_delay", cpb_removal_delay));
		fields.addItem(UnitField("dpb_output_delay", dpb_output_delay));
	}
	if(pSps->pic_struct_present_flag){
		ValueUnitFieldList pic_structField = ValueUnitFieldList("pic_struct", pic_struct);
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
			IdxValueUnitFieldList clock_timestamp_flagField = IdxValueUnitFieldList("clock_timestamp_flag", clock_timestamp_flag[i], i);
			if(clock_timestamp_flag[i]){
				clock_timestamp_flagField.addItem(IdxUnitField("ct_type", ct_type[i], i));
				clock_timestamp_flagField.addItem(IdxUnitField("nuit_field_based_flag", nuit_field_based_flag[i], i));
				clock_timestamp_flagField.addItem(IdxUnitField("counting_type", counting_type[i], i));
				clock_timestamp_flagField.addItem(IdxUnitField("discontinuity_flag", discontinuity_flag[i], i));
				clock_timestamp_flagField.addItem(IdxUnitField("cnt_dropped_flag", cnt_dropped_flag[i], i));
				clock_timestamp_flagField.addItem(IdxUnitField("n_frames", n_frames[i], i));
				clock_timestamp_flagField.addItem(IdxUnitField("full_timestamp_flag", full_timestamp_flag[i], i));
				if(full_timestamp_flag[i] || seconds_flag[i]) clock_timestamp_flagField.addItem(IdxUnitField("seconds_value", seconds_value[i], i));
				if(full_timestamp_flag[i] || minutes_flag[i]) clock_timestamp_flagField.addItem(IdxUnitField("minutes_value", minutes_value[i], i));
				if(full_timestamp_flag[i] || hours_flag[i]) clock_timestamp_flagField.addItem(IdxUnitField("hours_value", hours_value[i], i));
				if(pSps->nal_time_offset_length > 0 || pSps->vcl_time_offset_length > 0){
					clock_timestamp_flagField.addItem(IdxUnitField("full_timestamp_flag", full_timestamp_flag[i], i));
				}
			}
			pic_structField.addItem(std::move(clock_timestamp_flagField));
		}
		fields.addItem(std::move(pic_structField));
	}
	return fields;
}

void H264SEIPicTiming::validate(){
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) {
		errors.push_back(StringFormatter::formatString("[SEI Pic timing] unknown reference to a SPS unit (%ld)", seq_parameter_set_id));
		return;
	}
	H264SPS* pSps = referencedSPS->second;
	if(pSps->pic_struct_present_flag){
		if(pic_struct > 8) errors.push_back(StringFormatter::formatString("[SEI Pic timing] pic_struct value (%ld) not in valid range (0..8)", pic_struct));
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
					errors.push_back(StringFormatter::formatString("[SEI Pic timing] ct_type[%d] value (%ld) not in valid range (0..2)", i, ct_type[i]));
				}
				if(counting_type[i] > 6){
					errors.push_back(StringFormatter::formatString("[SEI Pic timing] counting_type[%d] value (%ld) not in valid range (0..6)", i, counting_type[i]));
				}
				if(seconds_value[i] > 59){
					errors.push_back(StringFormatter::formatString("[SEI Pic timing] seconds_value[%d] value (%ld) not in valid range (0..59)", i, seconds_value[i]));
				}
				if(minutes_value[i] > 59){
					errors.push_back(StringFormatter::formatString("[SEI Pic timing] minutes_value[%d] value (%ld) not in valid range (0..59)", i, minutes_value[i]));
				}
				if(hours_value[i] > 23){
					errors.push_back(StringFormatter::formatString("[SEI Pic timing] hours_value[%d] value (%ld) not in valid range (0..23)", i, hours_value[i]));
				}
			}
		}
	}
}

UnitFieldList H264SEIFillerPayload::dump_fields(){
	UnitFieldList fields = UnitFieldList("Filler Payload");
	return fields;
}

H264SEIUserDataUnregistered::~H264SEIUserDataUnregistered(){
}

UnitFieldList H264SEIUserDataUnregistered::dump_fields(){
	UnitFieldList fields = UnitFieldList("User Data Unregistered");
	int index = 31;
	std::ostringstream uuidStringStream;
	for(int len : {8, 4, 4, 4, 12}){
		for(int i = 0;i < len;++i){
			uuidStringStream << std::hex << (int)((uuid_iso_iec_11578 >> index*4)&& 0xFF);
			index--;
		}
		if(len != 12) uuidStringStream << "-";
	}
	fields.addItem(StrUnitField("uuid_iso_iec_11578", uuidStringStream.str()));
	for(uint32_t i = 0;i < user_data_payload_byte.size();++i){
		fields.addItem(IdxUnitField("user_data_payload_byte", user_data_payload_byte[i], i));
	}
	return fields;
}

UnitFieldList H264SEIRecoveryPoint::dump_fields(){
	UnitFieldList fields = UnitFieldList("Recovery Point");
	fields.addItem(UnitField("recovery_frame_cnt", recovery_frame_cnt));
	fields.addItem(UnitField("exact_match_flag", exact_match_flag));
	fields.addItem(UnitField("broken_link_flag", broken_link_flag));
	fields.addItem(UnitField("changing_slice_group_idc", changing_slice_group_idc));
	return fields;
}

void H264SEIRecoveryPoint::validate(){
	auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
	if(referencedSPS == H264SPS::SPSMap.end()) {
		errors.push_back(StringFormatter::formatString("[SEI Recovery point] unknown reference to a SPS unit (%ld)", seq_parameter_set_id));
		return;
	}
	H264SPS* pSps = referencedSPS->second;
	uint32_t MaxNumFrames = pSps->MaxFrameNumber;
	if(recovery_frame_cnt > MaxNumFrames-1){
		errors.push_back(StringFormatter::formatString("[SEI Recovery point] recovery_frame_cnt value (%ld) not in valid range (0..{})", recovery_frame_cnt, MaxNumFrames-1));
	}
}

UnitFieldList H264SEIFullFrameFreeze::dump_fields(){
	UnitFieldList fields = UnitFieldList("Full Frame Freeze");
	fields.addItem(UnitField("full_frame_freeze_repetition_period", full_frame_freeze_repetition_period));
	return fields;
}

void H264SEIFullFrameFreeze::validate(){
	if(full_frame_freeze_repetition_period > 16384){
		errors.push_back(StringFormatter::formatString("[SEI Full frame freeze] full_frame_freeze_repetition_period value (%ld) not in valid range (0..16384)", full_frame_freeze_repetition_period));
	}
}
