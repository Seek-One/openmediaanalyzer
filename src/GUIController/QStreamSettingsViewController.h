#pragma once

#include <QObject>

class QStreamSettingsView;
class QDecoderModel;
class QStreamModel;

class QStreamSettingsViewController : public QObject
{
    Q_OBJECT
public:
    QStreamSettingsViewController(QStreamSettingsView* pStreamSettingsView, QDecoderModel* pDecoderModel, QStreamModel* pStreamModel);

    virtual ~QStreamSettingsViewController();

signals:
    void setLiveContent(bool activated);

private:
    QStreamSettingsView* m_pStreamSettingsView;
    QDecoderModel* m_pDecoderModel;
    QStreamModel* m_pStreamModel;
};