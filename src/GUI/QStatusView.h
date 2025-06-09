#pragma once

#include <QStandardItemModel>
#include <QLabel>

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
    void videoStatusUpdated(QDecoderModel::StreamStatus status);

private:
    QLabel* m_pVideoSize;
    QLabel* m_pFrameValidity;
    QLabel* m_pVideoStatus;
};