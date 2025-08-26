//
// Created by ebeuque on 26/08/25.
//

#ifndef OPENMEDIAANALYZER_QH26XDUMPOBJECT_H
#define OPENMEDIAANALYZER_QH26XDUMPOBJECT_H

#include <QStack>

#include "Codec/H26X/H26XDumpObject.h"

class QStandardItem;
class QStandardItemModel;

class QH26XDumpObject : public H26XDumpObject
{
public:
	QH26XDumpObject();
	virtual ~QH26XDumpObject();

	void setModel(QStandardItemModel* pModel);

	void addUnitFieldListItem(const char* szFieldName) override;
	void addUnitField(const char* szFieldName, int64_t value) override;
	void addIdxUnitField(const char* szFieldName, int idx, int64_t value) override;
	void addDblIdxUnitField(const char* szFieldName, int idx1, int idx2, int64_t value) override;
	void addStrUnitField(const char* szFieldName, const std::string& value) override;

	void startUnitFieldList(const char* szFieldName) override;
	void endUnitFieldList() override;

	void startValueUnitFieldList(const char* szFieldName, int64_t value) override;
	void endValueUnitFieldList() override;
	void startIdxValueUnitFieldList(const char* szFieldName, int idx, int64_t value) override;
	void endIdxValueUnitFieldList() override;

private:
	QStandardItemModel* m_pModel;

	QStandardItem* m_pCurrentItem;
	QStack<QStandardItem*> m_stackItem;
};

#endif //OPENMEDIAANALYZER_QH26XDUMPOBJECT_H
