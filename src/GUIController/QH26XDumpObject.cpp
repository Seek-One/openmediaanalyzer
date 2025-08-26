//
// Created by ebeuque on 26/08/25.
//

#include <QStandardItem>

#include "../GUIModel/QDecoderModel.h"

#include "../StringHelpers/StringFormatter.h"

#include "QH26XDumpObject.h"

QH26XDumpObject::QH26XDumpObject()
{
	m_pModel = nullptr;
	m_pCurrentItem = nullptr;
}

QH26XDumpObject::~QH26XDumpObject()
{

}

void QH26XDumpObject::setModel(QStandardItemModel* pModel)
{
	m_pModel = pModel;
	m_pCurrentItem = m_pModel->invisibleRootItem();
}

void QH26XDumpObject::addUnitFieldListItem(const char* szFieldName)
{
	auto pItem = new QStandardItem(szFieldName);
	m_pCurrentItem->appendRow(pItem);

	m_stackItem.push(m_pCurrentItem);
	m_pCurrentItem = pItem;
}

void QH26XDumpObject::addUnitField(const char* szFieldName, int64_t value)
{
	QList<QStandardItem*> listItems;
	listItems.append(new QStandardItem(szFieldName));
	listItems.append(new QStandardItem(QString::number(value)));
	m_pCurrentItem->appendRow(listItems);
}

void QH26XDumpObject::addIdxUnitField(const char* szFieldName, int idx, int64_t value)
{
	QList<QStandardItem*> listItems;
	auto szFieldNameTmp = StringFormatter::formatString("%s[%hu]", szFieldName, idx);
	listItems.append(new QStandardItem(szFieldNameTmp.c_str()));
	listItems.append(new QStandardItem(QString::number(value)));
	m_pCurrentItem->appendRow(listItems);
}

void QH26XDumpObject::addDblIdxUnitField(const char* szFieldName, int idx1, int idx2, int64_t value)
{
	QList<QStandardItem*> listItems;
	auto szFieldNameTmp = StringFormatter::formatString("%s[%hu][%hu]", szFieldName, idx1, idx2);
	listItems.append(new QStandardItem(szFieldNameTmp.c_str()));
	listItems.append(new QStandardItem(QString::number(value)));
	m_pCurrentItem->appendRow(listItems);
}

void QH26XDumpObject::addStrUnitField(const char* szFieldName, const std::string& value)
{
	QList<QStandardItem*> listItems;
	listItems.append(new QStandardItem(szFieldName));
	listItems.append(new QStandardItem(value.c_str()));
	m_pCurrentItem->appendRow(listItems);
}

void QH26XDumpObject::startUnitFieldList(const char* szFieldName)
{
	auto pItem = new QStandardItem(szFieldName);
	m_pCurrentItem->appendRow(pItem);

	m_stackItem.push(m_pCurrentItem);
	m_pCurrentItem = pItem;
}

void QH26XDumpObject::endUnitFieldList()
{
	m_pCurrentItem = m_stackItem.pop();
}

void QH26XDumpObject::startValueUnitFieldList(const char* szFieldName, int64_t value)
{
	QList<QStandardItem*> listItems;
	auto pItem = new QStandardItem(szFieldName);
	listItems.append(pItem);
	listItems.append(new QStandardItem(QString::number(value)));

	m_pCurrentItem->appendRow(listItems);

	m_stackItem.push(m_pCurrentItem);
	m_pCurrentItem = pItem;
}

void QH26XDumpObject::endValueUnitFieldList()
{
	m_pCurrentItem = m_stackItem.pop();
}

void QH26XDumpObject::startIdxValueUnitFieldList(const char* szFieldName, int idx, int64_t value)
{
	QList<QStandardItem*> listItems;
	auto szFieldNameTmp = StringFormatter::formatString("%s[%hu]", szFieldName, idx);
	auto pItem = new QStandardItem(szFieldNameTmp.c_str());
	listItems.append(pItem);
	listItems.append(new QStandardItem(QString::number(value)));

	m_pCurrentItem->appendRow(listItems);

	m_stackItem.push(m_pCurrentItem);
	m_pCurrentItem = pItem;
}

void QH26XDumpObject::endIdxValueUnitFieldList()
{
	m_pCurrentItem = m_stackItem.pop();
}