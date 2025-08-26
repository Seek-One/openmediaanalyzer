#include <iostream>
#include <cstring>
#include <sstream>
#include <cmath>

#include "H264SPS.h"
#include "../../StringHelpers/StringFormatter.h"

#include "H264SEI.h"

H264SEI::H264SEI():
	H264SEI(0, 0, 0, nullptr)
{}

H264SEI::H264SEI(uint8_t forbidden_zero_bit, uint8_t nal_ref_idc, uint32_t nal_size, const uint8_t* nal_data):
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

void H264SEIMessage::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("SEI Message");
	dumpObject.endUnitFieldList();
}

void H264SEIMessage::validate(){}

void H264SEI::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Supplemental Enhancement Information");
	H26X_BREAKABLE_SCOPE(H26XDumpScope) {
		H264NAL::dump(dumpObject);

		if (!completelyParsed) {
			break;
		}

		for (H264SEIMessage *message: messages) {
			message->dump(dumpObject);
		}
	}
	dumpObject.endUnitFieldList();
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

void H264SEIBufferingPeriod::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Buffering period");
	H26X_BREAKABLE_SCOPE(H26XDumpScope) {
		dumpObject.addUnitField("seq_parameter_set_id", seq_parameter_set_id);
		auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
		if (referencedSPS == H264SPS::SPSMap.end()) {
			break;
		}
		H264SPS *pSps = referencedSPS->second;
		if (pSps->nal_hrd_parameters_present_flag) {
			for (int i = 0; i <= pSps->nal_cpb_cnt_minus1; ++i) {
				dumpObject.addIdxUnitField("nal_initial_cpb_removal_delay", i, nal_initial_cpb_removal_delay[i]);
				dumpObject.addIdxUnitField("nal_initial_cpb_removal_delay_offset", i, nal_initial_cpb_removal_delay_offset[i]);
			}
		}
		if (pSps->vcl_hrd_parameters_present_flag) {
			for (int i = 0; i <= pSps->vcl_cpb_cnt_minus1; ++i) {
				dumpObject.addIdxUnitField("vcl_initial_cpb_removal_delay", i, vcl_initial_cpb_removal_delay[i]);
				dumpObject.addIdxUnitField("vcl_initial_cpb_removal_delay_offset", i, vcl_initial_cpb_removal_delay_offset[i]);
			}
		}
	}
	dumpObject.endUnitFieldList();
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

void H264SEIPicTiming::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Picture timing");
	H26X_BREAKABLE_SCOPE(H26XDumpScope) {
		auto referencedSPS = H264SPS::SPSMap.find(seq_parameter_set_id);
		if(referencedSPS == H264SPS::SPSMap.end()){
			break;
		}
		H264SPS* pSps = referencedSPS->second;
		if(pSps->nal_hrd_parameters_present_flag || pSps->vcl_hrd_parameters_present_flag){
			dumpObject.addUnitField("cpb_removal_delay", cpb_removal_delay);
			dumpObject.addUnitField("dpb_output_delay", dpb_output_delay);
		}
		if(pSps->pic_struct_present_flag) {
			dumpObject.startValueUnitFieldList("pic_struct", pic_struct);
			int NumClockTS = 0;
			switch (pic_struct) {
				case 0:
				case 1:
				case 2:
					NumClockTS = 1;
					break;
				case 3:
				case 4:
				case 7:
					NumClockTS = 2;
					break;
				case 5:
				case 6:
				case 8:
					NumClockTS = 3;
					break;
			}
			for (int i = 0; i < NumClockTS; ++i) {
				dumpObject.startIdxValueUnitFieldList("clock_timestamp_flag", i, clock_timestamp_flag[i]);
				if (clock_timestamp_flag[i]) {
					dumpObject.addIdxUnitField("ct_type", i, ct_type[i]);
					dumpObject.addIdxUnitField("nuit_field_based_flag", i, nuit_field_based_flag[i]);
					dumpObject.addIdxUnitField("counting_type", i, counting_type[i]);
					dumpObject.addIdxUnitField("discontinuity_flag", i, discontinuity_flag[i]);
					dumpObject.addIdxUnitField("cnt_dropped_flag", i, cnt_dropped_flag[i]);
					dumpObject.addIdxUnitField("n_frames", i, n_frames[i]);
					dumpObject.addIdxUnitField("full_timestamp_flag", i, full_timestamp_flag[i]);
					if (full_timestamp_flag[i] || seconds_flag[i]) {
						dumpObject.addIdxUnitField("seconds_value", i, seconds_value[i]);
					}
					if (full_timestamp_flag[i] || minutes_flag[i]) {
						dumpObject.addIdxUnitField("minutes_value", i, minutes_value[i]);
					}
					if (full_timestamp_flag[i] || hours_flag[i]) {
						dumpObject.addIdxUnitField("hours_value", i, hours_value[i]);
					}
					if (pSps->nal_time_offset_length > 0 || pSps->vcl_time_offset_length > 0) {
						dumpObject.addIdxUnitField("full_timestamp_flag", i, full_timestamp_flag[i]);
					}
				}
				dumpObject.endIdxValueUnitFieldList();
			}
			dumpObject.endValueUnitFieldList();
		}
	}
	dumpObject.endUnitFieldList();
}

void H264SEIPicTiming::validate()
{
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

void H264SEIFillerPayload::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Filler Payload");
	dumpObject.endUnitFieldList();
}

H264SEIUserDataUnregistered::~H264SEIUserDataUnregistered(){
}

void H264SEIUserDataUnregistered::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("User Data Unregistered");
	int index = 15;
	std::ostringstream uuidStringStream;
	for(int len : {8, 4, 4}){
		for(int i = 0;i < len;++i){
			uuidStringStream << std::hex << (int)((uuid_iso_iec_11578_msb >> index*4) & 0xFF);
			index--;
		}
		if(len != 12) uuidStringStream << "-";
	}
	index = 15;
	for(int len : {4, 12}){
		for(int i = 0;i < len;++i){
			uuidStringStream << std::hex << (int)((uuid_iso_iec_11578_lsb >> index*4) & 0xFF);
			index--;
		}
		if(len != 12) uuidStringStream << "-";
	}
	dumpObject.addStrUnitField("uuid_iso_iec_11578", uuidStringStream.str());
	for(uint32_t i = 0;i < user_data_payload_byte.size();++i){
		dumpObject.addIdxUnitField("user_data_payload_byte", i, user_data_payload_byte[i]);
	}
	dumpObject.endUnitFieldList();
}

void H264SEIRecoveryPoint::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Recovery Point");
	dumpObject.addUnitField("recovery_frame_cnt", recovery_frame_cnt);
	dumpObject.addUnitField("exact_match_flag", exact_match_flag);
	dumpObject.addUnitField("broken_link_flag", broken_link_flag);
	dumpObject.addUnitField("changing_slice_group_idc", changing_slice_group_idc);
	dumpObject.endUnitFieldList();
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

void H264SEIFullFrameFreeze::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Full Frame Freeze");
	dumpObject.addUnitField("full_frame_freeze_repetition_period", full_frame_freeze_repetition_period);
	dumpObject.endUnitFieldList();
}

void H264SEIFullFrameFreeze::validate(){
	if(full_frame_freeze_repetition_period > 16384){
		errors.push_back(StringFormatter::formatString("[SEI Full frame freeze] full_frame_freeze_repetition_period value (%ld) not in valid range (0..16384)", full_frame_freeze_repetition_period));
	}
}
