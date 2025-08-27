//
// Created by ebeuque on 27/08/25.
//

#ifndef TOOLKIT_CODEC_UTILS_H26XNAL_H
#define TOOLKIT_CODEC_UTILS_H26XNAL_H

#include "H26XDumpObject.h"
#include "H26XErrors.h"

struct H26XNAL
{
	H26XNAL();

	bool completelyParsed;

	H26XErrors errors;

	virtual void dump(H26XDumpObject& dumpObject) const = 0;
	virtual void validate();
};

#endif //TOOLKIT_CODEC_UTILS_H26XNAL_H
