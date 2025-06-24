#include <sstream>
#include "StringFormatter.h"

#include "UnitFieldList.h"

UnitFieldListItem::UnitFieldListItem(const std::string name):
    field(name)
{}

std::string UnitFieldListItem::toStdString(uint16_t indent) const{
    return field;
}

QList<QStandardItem*>  UnitFieldListItem::toQtStandardItemRow() const{
    QList<QStandardItem*> row;
    row.append(new QStandardItem(field.c_str()));
    return row; ;
}

UnitField::UnitField(const std::string name, const int64_t val):
    UnitFieldListItem(name), value(val)
{}

std::string UnitField::toStdString(uint16_t indent) const{
    return StringFormatter::formatString("%s: %ld", field.c_str(), value);
}

QList<QStandardItem*> UnitField::toQtStandardItemRow() const{
    QList<QStandardItem*> row;
    row.append(new QStandardItem(field.c_str()));
    row.append(new QStandardItem(QString::number(value)));
    return row;
}

IdxUnitField::IdxUnitField(const std::string name, const int64_t val, uint16_t idx):
    UnitField(name, val), index(idx)
{}

std::string IdxUnitField::toStdString(uint16_t indent) const{
    return StringFormatter::formatString("%s[%hu]: %ld", field.c_str(), index, value);
}

QList<QStandardItem*> IdxUnitField::toQtStandardItemRow() const{
    QList<QStandardItem*> row;
    row.append(new QStandardItem(StringFormatter::formatString("%s[%hu]", field.c_str(), index).c_str()));
    row.append(new QStandardItem(QString::number(value)));
    return row;
}

DblIdxUnitField::DblIdxUnitField(const std::string name, const int64_t val, uint16_t idx1, uint16_t idx2):
    UnitField(name, val), index1(idx1), index2(idx2)
{}

std::string DblIdxUnitField::toStdString(uint16_t indent) const{
    return StringFormatter::formatString("%s[%hu][%hu]: %ld", field.c_str(), index1, index2, value);
}

QList<QStandardItem*> DblIdxUnitField::toQtStandardItemRow() const{
    QList<QStandardItem*> row;
    row.append(new QStandardItem(StringFormatter::formatString("%s[%hu][%hu]", field.c_str(), index1, index2).c_str()));
    row.append(new QStandardItem(QString::number(value)));
    return row;
}

StrUnitField::StrUnitField(const std::string name, const std::string val):
    UnitFieldListItem(name), value(val)
{
}

std::string StrUnitField::toStdString(uint16_t indent) const{
    return StringFormatter::formatString("%s: %s", field.c_str(), value.c_str());
}

QList<QStandardItem*> StrUnitField::toQtStandardItemRow() const{
    QList<QStandardItem*> row;
    row.append(new QStandardItem(field.c_str()));
    row.append(new QStandardItem(value.c_str()));
    return row;
}

void UnitFieldList::addItem(UnitField&& field){
    items.push_back(std::make_shared<UnitField>(field));
}

void UnitFieldList::addItem(UnitFieldList&& subFieldList){
    items.push_back(std::make_shared<UnitFieldList>(subFieldList));
}

void UnitFieldList::addItem(ValueUnitFieldList&& subFieldList){
    items.push_back(std::make_shared<ValueUnitFieldList>(subFieldList));
}

void UnitFieldList::addItem(IdxValueUnitFieldList&& subFieldList){
    items.push_back(std::make_shared<IdxValueUnitFieldList>(subFieldList));
}

void UnitFieldList::addItem(StrUnitField&& field){
    items.push_back(std::make_shared<StrUnitField>(field));
}

UnitFieldList::UnitFieldList(const std::string name):
    UnitFieldListItem(name)
{}

UnitFieldList::UnitFieldList(const std::string name, const UnitFieldList& fieldList):
    UnitFieldListItem(name)
{
    items.insert(items.end(), fieldList.items.begin(), fieldList.items.end());
}

UnitFieldList::UnitFieldList(const std::string name, UnitFieldList&& fieldList):
    UnitFieldListItem(name), items(std::move(fieldList.items))
{
}

std::string UnitFieldList::toStdString(uint16_t indent) const {
    std::ostringstream strBuilder;
    strBuilder << StringFormatter::formatString("%s%s:\n", std::string(indent, ' '), field);
    for(const std::shared_ptr<UnitFieldListItem>& item : items){
        strBuilder << item->toStdString(indent+1);
    }
    return strBuilder.str();
}

QList<QStandardItem*> UnitFieldList::toQtStandardItemRow() const{
    QList<QStandardItem*> row;
    QStandardItem* root = new QStandardItem(field.c_str());
    row.append(root);
    for(const std::shared_ptr<UnitFieldListItem>& item : items) root->appendRow(item->toQtStandardItemRow());
    return row;
}

ValueUnitFieldList::ValueUnitFieldList(const std::string name, int64_t val):
    UnitFieldList(name), value(val)
{}

std::string ValueUnitFieldList::toStdString(uint16_t indent) const {
    std::ostringstream strBuilder;
    strBuilder << StringFormatter::formatString("%s%s: %ld\n", std::string(indent, ' '), field.c_str(), value);
    for(const std::shared_ptr<UnitFieldListItem>& item : items){
        strBuilder << item->toStdString(indent+1);
    }
    return strBuilder.str();
}

QList<QStandardItem*> ValueUnitFieldList::toQtStandardItemRow() const{
    QList<QStandardItem*> row;
    QStandardItem* root = new QStandardItem(field.c_str());
    row.append(root);
    for(const std::shared_ptr<UnitFieldListItem>& item : items) root->appendRow(item->toQtStandardItemRow());
    row.append(new QStandardItem(QString::number(value)));
    return row;
}

IdxValueUnitFieldList::IdxValueUnitFieldList(const std::string name, int64_t val, uint16_t idx):
    ValueUnitFieldList(name, val), index(idx)
{}

std::string IdxValueUnitFieldList::toStdString(uint16_t indent) const {
    std::ostringstream strBuilder;
    strBuilder << StringFormatter::formatString("%s%s[%hu]: %ld\n", std::string(indent, ' '), field.c_str(), index, value);
    for(const std::shared_ptr<UnitFieldListItem>& item : items){
        strBuilder << item->toStdString(indent+1);
    }
    return strBuilder.str();
}

QList<QStandardItem*> IdxValueUnitFieldList::toQtStandardItemRow() const{
    QList<QStandardItem*> row;
    QStandardItem* root = new QStandardItem(StringFormatter::formatString("%s[%hu]", field.c_str(), index).c_str());
    row.append(root);
    for(const std::shared_ptr<UnitFieldListItem>& item : items) root->appendRow(item->toQtStandardItemRow());
    row.append(new QStandardItem(QString::number(value)));
    return row;
}