#pragma once

#include <QGroupBox>
#include <QCheckBox>

#define MEMORY_LIMIT_MIN 512
#define MEMORY_LIMIT_MAX 8192
#define MEMORY_LIMIT_STEP 512
#define MEMORY_LIMIT_DEFAULT_VALUE 2048
#define MEMORY_LIMIT_SET_BY_DEFAULT true

#define DURATION_LIMIT_MIN 1
#define DURATION_LIMIT_MAX 5
#define DURATION_LIMIT_DEFAULT_VALUE 1
#define DURATION_LIMIT_SET_BY_DEFAULT false

#define GOP_COUNT_LIMIT_MIN 1
#define GOP_COUNT_LIMIT_MAX 256
#define GOP_COUNT_LIMIT_DEFAUT_VALUE 32
#define GOP_COUNT_LIMIT_SET_BY_DEFAULT false

class QStreamSettingsView : public QGroupBox
{
    Q_OBJECT
public:
    QStreamSettingsView(QWidget *parent = NULL);
    virtual ~QStreamSettingsView();

public slots:
    void liveContentSet(bool activated);
signals:
    void toggleMemoryLimit(bool activated);
    void toggleDurationLimit(bool activated);
    void toggleGOPCountLimit(bool activated);

    void updateMemoryLimit(int val);
    void updateDurationLimit(int val);
    void updateGOPCountLimit(int val);

    void setLiveContent(bool activated);
    void stopStreamClicked();

private:
    QCheckBox* m_pLiveContentCheckbox;
};