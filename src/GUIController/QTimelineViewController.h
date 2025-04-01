#pragma once

#include <QObject>

class QTimelineView;
class QDecoderModel;

class QTimelineViewController : public QObject
{
    Q_OBJECT
public:
    QTimelineViewController(QTimelineView* pTimelineView, QDecoderModel* pDecoderModel);

    virtual ~QTimelineViewController();

public slots:
    void frameSelected(QSharedPointer<QAccessUnitModel> pAccessUnits);
    void timelineStarted();

signals:
    void selectFrame(QSharedPointer<QAccessUnitModel> pAccessUnits);
    void startTimeline();

private:
    QTimelineView* m_pTimelineView;
    QDecoderModel* m_pDecoderModel;
};