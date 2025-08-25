#ifndef TOOLKIT_CODEC_UTILS_H265SEI_H_
#define TOOLKIT_CODEC_UTILS_H265SEI_H_

#include <cstdint>
#include <string>
#include <vector>

#include "H265NAL.h"

struct H265SEIMessage {
	H265SEIMessage();
	virtual ~H265SEIMessage() = default;
	uint8_t payloadType;

	virtual UnitFieldList dump_fields();
};

struct H265SEI : public H265NAL {
	H265SEI();
	H265SEI(uint8_t forbidden_zero_bit, UnitType nal_unit_type, uint8_t nuh_layer_id, uint8_t nuh_temporal_id_plus1, uint32_t nal_size, const uint8_t* nal_data);
	~H265SEI();

    std::vector<H265SEIMessage*> messages;
	
	UnitFieldList dump_fields() override;
	void validate() override;
};

#endif // TOOLKIT_CODEC_UTILS_H265SEI_H_