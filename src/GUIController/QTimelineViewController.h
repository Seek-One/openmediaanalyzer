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

signals:
    void startTimeline();
    void setLiveContent(bool val);

private:
    QTimelineView* m_pTimelineView;
    QDecoderModel* m_pDecoderModel;
};