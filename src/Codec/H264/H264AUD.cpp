#include "H264AUD.h"

H264AUD::H264AUD(H264NALHeader* pNALHeader, uint32_t nal_size, const uint8_t* nal_data):
	H264NAL(pNALHeader, nal_size, nal_data)
{
	primary_pic_type = 7; // All slices allowed
}

void H264AUD::dump(H26XDumpObject& dumpObject) const
{
	dumpObject.startUnitFieldList("Access Unit Delimiter");
	H26X_BREAKABLE_SCOPE(H26XDumpScope)
	{
		H264NAL::dump(dumpObject);

		if (!completelyParsed) {
			break;
		}

		dumpObject.addUnitField("primary_pic_type", primary_pic_type);
	}
	dumpObject.endUnitFieldList();
}

void H264AUD::validate()
{
	H264NAL::validate();
	if(!completelyParsed){
		return;
	}
}
