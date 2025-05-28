#pragma once

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <list>

#define SCROLL_CLAMP_RIGHT_THRESHOLD 0.925

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
    void accessUnitsUpdated();
    void accessUnitsAdded(QVector<QSharedPointer<QAccessUnitModel>> pAccessUnitss);
    void accessUnitsRemoved(uint count);
    void accessUnitSelected(QSharedPointer<QAccessUnitModel> pAccessUnits);
    void liveContentSet(bool val);

signals:
    void selectAccessUnit(QSharedPointer<QAccessUnitModel> pAccessUnits);
    void deleteFrame(QUuid id);

private:
    QHBoxLayout* m_pBarHBoxLayout;
    QHBoxLayout* m_pCounterHBoxLayout;
    std::list<QSharedPointer<QAccessUnitElement>> m_pAccessUnitElements;
    QScrollArea* m_pScrollArea;

    bool m_liveContent;
};