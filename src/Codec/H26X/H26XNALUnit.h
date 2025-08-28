//
// Created by ebeuque on 27/08/25.
//

#ifndef TOOLKIT_CODEC_UTILS_H26XNALUNIT_H
#define TOOLKIT_CODEC_UTILS_H26XNALUNIT_H

#include "H26XDumpObject.h"
#include "H26XErrors.h"
#include "H26XNALHeader.h"

struct H26XNALUnit
{
	H26XNALUnit(H26XNALHeader* pNALHeader);

	H26XNALHeader* m_pNALHeader;
	bool completelyParsed;

	H26XErrors errors;

	virtual void dump(H26XDumpObject& dumpObject) const;
	virtual void validate();
};

#endif //TOOLKIT_CODEC_UTILS_H26XNALUNIT_H
