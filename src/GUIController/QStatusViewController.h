#pragma once

#include <QObject>

class QStatusView;
class QDecoderModel;

class QStatusViewController : public QObject
{
    Q_OBJECT
public:
    QStatusViewController(QStatusView* pStatusView, QDecoderModel* pDecoderModel);

    virtual ~QStatusViewController();


private:
    QStatusView* m_pStatusView;
    QDecoderModel* m_pDecoderModel;
};