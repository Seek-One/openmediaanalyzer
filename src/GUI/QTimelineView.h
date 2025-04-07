#pragma once

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <list>

#define SCROLL_CLAMP_RIGHT_THRESHOLD 0.99

class QAccessUnitModel;
class QAccessUnitElement;

class QTimelineView : public QGroupBox
{
    Q_OBJECT
public:
    QTimelineView(QWidget *parent = NULL);
    virtual ~QTimelineView();

public slots:
    void resetTimeline();
    void unitsUpdated();
    void accessUnitsAdded(QVector<QSharedPointer<QAccessUnitModel>> pAccessUnitss);
    void accessUnitsRemoved(uint count);
    void accessUnitSelected(QSharedPointer<QAccessUnitModel> pAccessUnits);

signals:
    void selectAccessUnit(QSharedPointer<QAccessUnitModel> pAccessUnits);

private:
    QHBoxLayout* m_pHBoxLayout;
    std::list<QSharedPointer<QAccessUnitElement>> m_pAccessUnitElements;
    QScrollArea* m_pScrollArea;
};