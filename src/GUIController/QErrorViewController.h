#pragma once

#include <QObject>

class QErrorView;
class QDecoderModel;

class QErrorViewController : public QObject
{
    Q_OBJECT
public:
    QErrorViewController(QErrorView* pErrorView, QDecoderModel* pDecoderModel);

    virtual ~QErrorViewController();


private:
    QErrorView* m_pErrorView;
    QDecoderModel* m_pDecoderModel;
};