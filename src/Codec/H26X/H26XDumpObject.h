//
// Created by ebeuque on 26/08/25.
//

#ifndef TOOLKIT_CODEC_UTILS_H26X_H26XDUMPOBJECT_H
#define TOOLKIT_CODEC_UTILS_H26X_H26XDUMPOBJECT_H

#include <string>

#include "H26XTypes.h"

class H26XDumpObject
{
public:
	H26XDumpObject();
	virtual ~H26XDumpObject();

	virtual void addUnitFieldListItem(const char* szFieldName);
	virtual void addUnitField(const char* szFieldName, int64_t value);
	virtual void addIdxUnitField(const char* szFieldName, int idx, int64_t value);
	virtual void addDblIdxUnitField(const char* szFieldName, int idx1, int idx2, int64_t value);
	virtual void addStrUnitField(const char* szFieldName, const std::string& value);

	virtual void startUnitFieldList(const char* szFieldName);
	virtual void endUnitFieldList();
	virtual void startValueUnitFieldList(const char* szFieldName, int64_t value);
	virtual void endValueUnitFieldList();
	virtual void startIdxValueUnitFieldList(const char* szFieldName, int idx, int64_t value);
	virtual void endIdxValueUnitFieldList();
};

#endif //TOOLKIT_CODEC_UTILS_H26X_H26XDUMPOBJECT_H
