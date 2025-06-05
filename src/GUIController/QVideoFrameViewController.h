#pragma once

#include <QObject>

class QVideoFrameView;
class QDecoderModel; 
class QStreamModel; 

class QVideoFrameViewController : public QObject
{
    Q_OBJECT
public:
    QVideoFrameViewController(QVideoFrameView* pVideoFrameView, QDecoderModel* pDecoderModel, QStreamModel* pStreamModel);

    virtual ~QVideoFrameViewController();


private:
    QVideoFrameView* m_pVideoFrameView;
    QDecoderModel* m_pDecoderModel;
    QStreamModel* m_pStreamModel;
};