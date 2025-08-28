#ifndef TOOLKIT_CODEC_UTILS_H265SEI_H_
#define TOOLKIT_CODEC_UTILS_H265SEI_H_

#include <cstdint>
#include <string>
#include <vector>

#include "H265NALUnit.h"

struct H265SEIMessage {
	H265SEIMessage();
	virtual ~H265SEIMessage() = default;
	uint8_t payloadType;

	void dump(H26XDumpObject& dumpObject) const;
};

struct H265SEI : public H265NALUnit {
	H265SEI(H265NALHeader* pNALHeader, uint32_t nal_size, const uint8_t* nal_data);
	~H265SEI();

    std::vector<H265SEIMessage*> messages;

	void dump(H26XDumpObject& dumpObject) const override;
	void validate() override;
};

#endif // TOOLKIT_CODEC_UTILS_H265SEI_H_