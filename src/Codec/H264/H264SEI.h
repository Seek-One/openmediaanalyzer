#ifndef TOOLKIT_CODEC_UTILS_H264SEI_H_
#define TOOLKIT_CODEC_UTILS_H264SEI_H_

#include <cstdint>
#include <string>
#include <vector>

#include "H264NAL.h"

#define SEI_BUFFERING_PERIOD 0
#define SEI_PIC_TIMING 1
#define SEI_FILLER_PAYLOAD 3
#define SEI_USER_DATA_UNREGISTERED 5
#define SEI_RECOVERY_POINT 6
#define SEI_FULL_FRAME_FREEZE 13

struct H264SEIMessage {
	H264SEIMessage();
	virtual ~H264SEIMessage() = default;
	uint8_t payloadType;

	std::vector<std::string> errors;
	virtual UnitFieldList dump_fields();
	virtual void validate();
};

struct H264SEI : public H264NAL {
	H264SEI();
	H264SEI(uint8_t forbidden_zero_bit, uint8_t nal_ref_idc, uint32_t nal_size, uint8_t* nal_data);
	~H264SEI();

    std::vector<H264SEIMessage*> messages;
	
	UnitFieldList dump_fields() override;
	void validate() override;
};


struct H264SEIBufferingPeriod : public H264SEIMessage {
	~H264SEIBufferingPeriod() = default;
	uint8_t seq_parameter_set_id;
	uint32_t nal_initial_cpb_removal_delay[32];
	uint32_t nal_initial_cpb_removal_delay_offset[32];
	uint32_t vcl_initial_cpb_removal_delay[32];
	uint32_t vcl_initial_cpb_removal_delay_offset[32];
	UnitFieldList dump_fields() override;
	void validate() override;
};

struct H264SEIPicTiming : public H264SEIMessage {
	~H264SEIPicTiming() = default;
	uint32_t cpb_removal_delay;
	uint32_t dpb_output_delay;
	uint8_t pic_struct;
	uint8_t clock_timestamp_flag[3];
	uint8_t ct_type[3];
	uint8_t nuit_field_based_flag[3];
	uint8_t counting_type[3];
	uint8_t full_timestamp_flag[3];
	uint8_t discontinuity_flag[3];
	uint8_t cnt_dropped_flag[3];
	uint8_t n_frames[3];
	uint8_t seconds_value[3];
	uint8_t minutes_value[3];
	uint8_t hours_value[3];
	uint8_t seconds_flag[3];
	uint8_t minutes_flag[3];
	uint8_t hours_flag[3];
	uint32_t time_offset[3];

	uint8_t seq_parameter_set_id;
	UnitFieldList dump_fields() override;
	void validate() override;
};

struct H264SEIFillerPayload : public H264SEIMessage {
	UnitFieldList dump_fields() override;
};

struct H264SEIUserDataUnregistered : public H264SEIMessage {
	~H264SEIUserDataUnregistered();
	unsigned long long uuid_iso_iec_11578;
	std::vector<uint8_t> user_data_payload_byte;
	UnitFieldList dump_fields() override;
};

struct H264SEIRecoveryPoint : public H264SEIMessage {
	~H264SEIRecoveryPoint() = default;
	uint32_t recovery_frame_cnt;
	uint8_t exact_match_flag;
	uint8_t broken_link_flag;
	uint8_t changing_slice_group_idc;

	uint8_t seq_parameter_set_id;
	UnitFieldList dump_fields() override;
	void validate() override;
};


struct H264SEIFullFrameFreeze : public H264SEIMessage {
	~H264SEIFullFrameFreeze() = default;
	uint16_t full_frame_freeze_repetition_period;
	UnitFieldList dump_fields() override;
	void validate() override;
};

#endif // TOOLKIT_CODEC_UTILS_H264SEI_H_
