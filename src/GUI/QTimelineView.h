#pragma once

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <list>

#define SCROLL_CLAMP_RIGHT_THRESHOLD 0.975

class QAccessUnitModel;
class QTimelineAccessUnitElement;

class QTimelineView : public QGroupBox
{
    Q_OBJECT
public:
    QTimelineView(QWidget *parent = NULL);
    virtual ~QTimelineView();

public slots:
    void resetTimeline();
    void accessUnitsUpdated();
    void accessUnitsAdded(QVector<QSharedPointer<QAccessUnitModel>> pAccessUnitss);
    void accessUnitsRemoved(uint count);
    void frameSelected(QSharedPointer<QAccessUnitModel> pAccessUnits);

signals:
    void selectFrame(QSharedPointer<QAccessUnitModel> pAccessUnits);

private:
    QHBoxLayout* m_pHBoxLayout;
    std::list<QSharedPointer<QTimelineAccessUnitElement>> m_pTimelineAccessUnitElement;
    QScrollArea* m_pScrollArea;
};