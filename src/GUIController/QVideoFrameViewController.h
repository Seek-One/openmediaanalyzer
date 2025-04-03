#pragma once

#include <QObject>

class QVideoFrameView;
class QDecoderModel;

class QVideoFrameViewController : public QObject
{
    Q_OBJECT
public:
    QVideoFrameViewController(QVideoFrameView* pVideoFrameView, QDecoderModel* pDecoderModel);

    virtual ~QVideoFrameViewController();


private:
    QVideoFrameView* m_pVideoFrameView;
    QDecoderModel* m_pDecoderModel;
};