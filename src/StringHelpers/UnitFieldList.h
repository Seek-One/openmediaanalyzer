#ifndef STRINGHELPERS_UNIT_FIELD_LIST_H_
#define STRINGHELPERS_UNIT_FIELD_LIST_H_

#include <vector>
#include <string>
#include <QStandardItemModel>

class ValueUnitFieldList;
class IdxValueUnitFieldList;

class UnitFieldListItem
{
public:
    UnitFieldListItem(const std::string name);
    virtual ~UnitFieldListItem() = default;

    virtual std::string toStdString(uint16_t indent) const;
    virtual QList<QStandardItem*> toQtStandardItemRow() const;

    std::string field;
};

class UnitField : public UnitFieldListItem
{
public:
    UnitField(const std::string name, int64_t val);
    virtual ~UnitField() = default;

    std::string toStdString(uint16_t indent) const override;
    QList<QStandardItem*> toQtStandardItemRow() const override;

    const int64_t value;
};

class IdxUnitField : public UnitField
{
public:
    IdxUnitField(const std::string name, int64_t val, uint16_t idx);
    ~IdxUnitField() = default;

    std::string toStdString(uint16_t indent) const override;
    QList<QStandardItem*> toQtStandardItemRow() const override;

    const uint16_t index;
};

class DblIdxUnitField : public UnitField
{
public:
    DblIdxUnitField(const std::string name, int64_t val, uint16_t idx1, uint16_t idx2);
    ~DblIdxUnitField() = default;

    std::string toStdString(uint16_t indent) const override;
    QList<QStandardItem*> toQtStandardItemRow() const override;

    const uint16_t index1;
    const uint16_t index2;
};

class StrUnitField : public UnitFieldListItem
{
public:
    StrUnitField(const std::string name, const std::string val);
    ~StrUnitField() = default;

    std::string toStdString(uint16_t indent) const override;
    QList<QStandardItem*> toQtStandardItemRow() const override;

    const std::string value;
};

class UnitFieldList : public UnitFieldListItem
{
public:
    UnitFieldList(const std::string name);
    UnitFieldList(const std::string name, const UnitFieldList& fieldList);
    UnitFieldList(const std::string name, UnitFieldList&& fieldList);
    virtual ~UnitFieldList() = default;


    virtual std::string toStdString(uint16_t indent) const override;
    virtual QList<QStandardItem*> toQtStandardItemRow() const override;

    std::vector<std::string> toStdStringVector() const;
    QStandardItemModel* toQtStandardItemModel(std::string header) const;
    void addItem(UnitField&& field);
    void addItem(StrUnitField&& field);
    void addItem(UnitFieldList&& subFieldList);
    void addItem(ValueUnitFieldList&& subFieldList);
    void addItem(IdxValueUnitFieldList&& subFieldList);
    
    std::vector<std::shared_ptr<UnitFieldListItem>> items;
};

class ValueUnitFieldList : public UnitFieldList
{
public:
    ValueUnitFieldList(const std::string name, int64_t val);
    virtual ~ValueUnitFieldList() = default;

    virtual std::string toStdString(uint16_t indent) const override;
    virtual QList<QStandardItem*> toQtStandardItemRow() const override;

    const int64_t value;
};

class IdxValueUnitFieldList : public ValueUnitFieldList
{
public:
    IdxValueUnitFieldList(const std::string name, int64_t val, uint16_t idx);
    ~IdxValueUnitFieldList() = default;

    std::string toStdString(uint16_t indent) const override;
    QList<QStandardItem*> toQtStandardItemRow() const override;

    const uint16_t index;
};

#endif // STRINGHELPERS_UNIT_FIELD_LIST_H_