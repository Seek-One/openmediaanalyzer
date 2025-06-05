#pragma once

#include <QObject>

class QStatusView;
class QDecoderModel;
class QStreamModel;

class QStatusViewController : public QObject
{
    Q_OBJECT
public:
    QStatusViewController(QStatusView* pStatusView, QDecoderModel* pDecoderModel, QStreamModel* pStreamModel);

    virtual ~QStatusViewController();


private:
    QStatusView* m_pStatusView;
    QDecoderModel* m_pDecoderModel;
    QStreamModel* m_pStreamModel;
};