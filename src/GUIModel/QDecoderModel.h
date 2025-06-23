#pragma once

#include <vector>
#include <QObject>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QImage>
#include <QDateTime>
#include <QTimer>
#include <queue>
#include <QThread>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
}

#include "QAccessUnitModel.h"

class QVideoInputView;
class H264Stream;
class H265Stream;
class QPictureWorker;

class QDecoderModel : public QObject
{
    Q_OBJECT
public:
    enum StreamStatus {
        StreamStatus_OK,
        StreamStatus_NonConformant,
        StreamStatus_Damaged,
        StreamStatus_NoStream
    };

    QDecoderModel();

    virtual ~QDecoderModel();
    
    static QStandardItem* modelItemFromFields(std::vector<std::string> fields, QString header);
    QStringList m_minorStreamErrors;
    QStringList m_majorStreamErrors;

signals:
    void updateTimelineUnits();
    void addTimelineUnits(QVector<QSharedPointer<QAccessUnitModel>> accessUnits);
    void removeTimelineUnits(uint32_t count);

    void decodeH264Slice(QSharedPointer<QAccessUnitModel> pAccessUnitModel);
    void decodeH265Slice(QSharedPointer<QAccessUnitModel> pAccessUnitModel);

    void updateVPSInfoView(QStandardItemModel* pModel);
    void updateSPSInfoView(QStandardItemModel* pModel);
    void updatePPSInfoView(QStandardItemModel* pModel);
    void updateFrameInfoView(QStandardItemModel* pModel);
    void updateErrorView(QString title, QStringList minorErrors, QStringList majorErrors);

    void updateStatus(StreamStatus status);
    void updateResolution(int width, int height);
    void updateFrameRate(int frameRate);
    void updateValidity(uint32_t valid, uint32_t total);
    void updateCodedSize(uint64_t size);
    void updateDecodedSize(uint64_t size);
    void updateGOVLength(quint32 GOVLength);
    void updateVideoFrameViewImage(QSharedPointer<QImage> pImage);
    void updateVideoFrameViewText(const QString& text);

    void setLiveContent(bool activated);

public slots:
    void newVideoStream(bool isLiveStream);
    void startFrameRateTimer();
    void stopFrameRateTimer();
    void frameRateUpdater();

    void h264FileLoaded(uint8_t* fileContent, quint32 fileSize);
    void h264PacketLoaded(uint8_t* fileContent, quint32 fileSize);
    void h265FileLoaded(uint8_t* fileContent, quint32 fileSize);
    void h265PacketLoaded(uint8_t* fileContent, quint32 fileSize);

    void pictureReceived(QSharedPointer<QImage> pPicture);

    void frameSelected(QSharedPointer<QAccessUnitModel> pAccessUnits);
    void folderLoaded();
    void streamStopped();
    void framesTabOpened();
    void vpsTabOpened();
    void spsTabOpened();
    void ppsTabOpened();
    void frameDeleted(QUuid id);

    void liveContentSet(bool activated);
    void memoryLimitToggled(bool activated);
    void durationLimitToggled(bool activated);
    void GOPCountLimitToggled(bool activated);
    void memoryLimitUpdated(int val);
    void durationLimitUpdated(int val);
    void GOPCountLimitUpdated(int val);

private:
    void emitStreamErrors();
    void emitH264SPSErrors();
    void emitH264PPSErrors();
    void emitVPSErrors();
    void emitH265SPSErrors();
    void emitH265PPSErrors();

    void checkForNewGOP();
    void discardH264GOPs();
    void discardH265GOPs();
    void decodeH264GOP(QVector<QSharedPointer<QAccessUnitModel>> GOP);
    void decodeH265GOP(QVector<QSharedPointer<QAccessUnitModel>> GOP);
    void validateCurrentGOP();
    void validateH264GOPFrames();
    void validateH265GOPFrames();

    void updateH264StatusBarSize();
    void updateH264StatusBarValidity();
    void updateH264StatusBarStatus();
    void updateH264StatusBar();
    void updateH265StatusBarSize();
    void updateH265StatusBarValidity();
    void updateH265StatusBarStatus();
    void updateH265StatusBar();
    qsizetype pictureMemoryUsageMB();

    bool m_isLiveStream;

    H264Stream* m_pH264Stream;
    H265Stream* m_pH265Stream;
    QSharedPointer<QAccessUnitModel> m_pSelectedFrameModel;
    QVector<QSharedPointer<QAccessUnitModel>> m_currentGOPModel;

    QVector<QVector<QSharedPointer<QAccessUnitModel>>> m_previousGOPModels;
    QVector<QSharedPointer<QAccessUnitModel>> m_selectedDecodedGOPModel;

    uint m_frameCount;
    QTimer* m_pFrameRateTimer;
    
    int m_tabIndex;
    bool m_liveContent;

    bool m_memoryLimitSet;
    bool m_durationLimitSet;
    bool m_GOPCountLimitSet;
    uint32_t m_pictureMemoryLimit;
    uint32_t m_durationLimit;
    uint32_t m_GOPCountLimit;
    QMap<QUuid, QDateTime> m_firstGOPSliceTimestamp;

    QThread* m_pPictureThread;
    QPictureWorker* m_pPictureWorker;

    QMap<QUuid, QSharedPointer<QImage>> m_decodedFrames;
    std::queue<QSharedPointer<QAccessUnitModel>> m_requestedFrames;
};

class QPictureWorker : public QObject {
    Q_OBJECT
public:
    QPictureWorker();
    ~QPictureWorker();

signals:
    void pictureReady(QSharedPointer<QImage> pPicture);
    void updateResolution(int width, int height);

public slots:
    void decodeH264AccessUnit(QSharedPointer<QAccessUnitModel> pAccessUnitModel);
    void decodeH265AccessUnit(QSharedPointer<QAccessUnitModel> pAccessUnitModel);

private:
    QImage* getQImageFromH264Frame(const AVFrame* pFrame);
    QImage* getQImageFromH265Frame(const AVFrame* pFrame);

    const AVCodec* m_pH264Codec;
    AVCodecContext* m_pH264CodecCtx;
    SwsContext* m_pH264SwsCtx;
    const AVCodec* m_pH265Codec;
    AVCodecContext* m_pH265CodecCtx;
    SwsContext* m_pH265SwsCtx;
    int m_frameWidth;
    int m_frameHeight; 
    int m_pixelFormat;
};