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
    void videoStatusUpdated(QDecoderModel::StreamStatus status);
    void frameValidityUpdated(uint32_t valid, uint32_t total);
    void codedSizeUpdated(uint64_t size);
    void decodedSizeUpdated(uint64_t size);

private:
    QLabel* m_pVideoStatus;
    QLabel* m_pFrameValidity;
    QLabel* m_pCodedVideoSize;
    QLabel* m_pDecodedPicturesSize;
};