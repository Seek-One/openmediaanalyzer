#pragma once

#include <QObject>
#include <QFileSystemModel>

class QStatusView;
class QDecoderModel;

class QStatusViewController : public QObject
{
    Q_OBJECT
public:
    QStatusViewController(QStatusView* pStatusView, QDecoderModel* pDecoderModel);

    virtual ~QStatusViewController();

signals:
    void openFolder(QFileSystemModel* pModel);
    void openStream(const QString& URL);


private:
    QStatusView* m_pStatusView;
    QDecoderModel* m_pDecoderModel;
};