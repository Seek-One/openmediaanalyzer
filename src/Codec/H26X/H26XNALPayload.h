//
// Created by ebeuque on 28/08/25.
//

#ifndef TOOLKIT_CODEC_UTILS_H26X_H26XNALPAYLOAD_H
#define TOOLKIT_CODEC_UTILS_H26X_H26XNALPAYLOAD_H

#include "../H26X/H26XDumpObject.h"
#include "../H26X/H26XErrors.h"

struct H26XNALPayload
{
	virtual void dump(H26XDumpObject& dumpObject) const;
	virtual void checkErrors(H26XErrors& errors) const;
};

#endif //TOOLKIT_CODEC_UTILS_H26X_H26XNALPAYLOAD_H
