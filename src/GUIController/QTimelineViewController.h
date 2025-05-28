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
    void liveContentSet(bool val);

signals:
    void selectFrame(QSharedPointer<QAccessUnitModel> pAccessUnits);
    void startTimeline();
    void setLiveContent(bool val);

private:
    QTimelineView* m_pTimelineView;
    QDecoderModel* m_pDecoderModel;
};