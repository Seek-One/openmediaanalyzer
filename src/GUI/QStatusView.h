#pragma once

#include <QStandardItemModel>
#include <QLabel>
#include <QCheckBox>

#include "../GUIModel/QDecoderModel.h"

class QStatusView : public QWidget
{
    Q_OBJECT
public:
    QStatusView(QWidget *parent = NULL);
    virtual ~QStatusView();

public slots:
    void sizeUpdated(uint64_t size);
    void frameValidityUpdated(uint32_t valid, uint32_t total);
    void streamStatusUpdated(QDecoderModel::StreamStatus status);

signals:
    void setLiveContent(bool val);

private:
    QLabel* m_pStreamSize;
    QLabel* m_pFrameValidity;
    QLabel* m_pStreamStatus;
    QCheckBox* m_pLiveContentCheckbox;
};