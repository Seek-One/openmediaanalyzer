#pragma once

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <list>

class QAccessUnitModel;
class QFrameElement;

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
    void frameSelected(QSharedPointer<QAccessUnitModel> pAccessUnits);

signals:
    void selectFrame(QSharedPointer<QAccessUnitModel> pAccessUnits);

private:
    QHBoxLayout* m_pHBoxLayout;
    std::list<QSharedPointer<QFrameElement>> m_pFrameElements;
    QScrollArea* m_pScrollArea;
};