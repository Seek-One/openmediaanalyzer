#pragma once

#include <QGroupBox>
#include <QFileSystemModel>
#include <QTreeView>
#include <QStandardItem>
#include <QLabel>

class QVideoInputView : public QGroupBox
{
    Q_OBJECT
public:
    QVideoInputView(QWidget *parent = NULL);
    virtual ~QVideoInputView();

public slots:
    void folderSet(QFileSystemModel* pModel);
    void streamSet(const QString& URL);

    void bitratesUpdated(uint64_t videoBitrate, uint64_t audioBitrate, uint64_t otherBitrate);

    void videoCodecUpdated(const QString& codec);
    void videoResolutionUpdated(int width, int height);
    void videoFrameRateUpdated(int frameRate);
    void GOVLengthUpdated(uint32_t GOVLength);
    void codedSizeUpdated(uint64_t size);
    void decodedSizeUpdated(uint64_t size);
    void frameValidityUpdated(uint32_t valid, uint32_t total);

    void audioCodecUpdated(const QString& codec);

    void protocolUpdated(const QString& protocol);
    void contentTypeUpdated(const QString& contentType);
    
private:
    void resetMetrics();

    QTreeView* m_pFolderView;
    QTreeView* m_pStreamInfoView;

    QStandardItem* m_pVideoCodec;
    QStandardItem* m_pVideoResolution;
    QStandardItem* m_pVideoFrameRate;
    QStandardItem* m_pGOVLength;
    QStandardItem* m_pVideoContentBitrate;
    QStandardItem* m_pCodedVideoSize;
    QStandardItem* m_pDecodedPicturesSize;
    QStandardItem* m_pFrameValidity;

    QStandardItem* m_pAudioCodec;
    QStandardItem* m_pAudioContentBitrate;

    QStandardItem* m_pContentType;
    QStandardItem* m_pProtocol;
    QStandardItem* m_pGlobalContentBitrate;
};