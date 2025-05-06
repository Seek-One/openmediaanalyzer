#pragma once

#include <vector>
#include <QObject>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QImage>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
}

#include "QAccessUnitModel.h"

#define GOP_LIMIT 15

class QFolderView;
class H264Stream;
class H265Stream;

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
    void updateVPSInfoView(QStandardItemModel* pModel);
    void updateSPSInfoView(QStandardItemModel* pModel);
    void updatePPSInfoView(QStandardItemModel* pModel);
    void updateFrameInfoView(QStandardItemModel* pModel);
    void updateErrorView(QString title, QStringList minorErrors, QStringList majorErrors);
    void updateSize(uint64_t size);
    void updateValidity(uint32_t valid, uint32_t total);
    void updateStatus(StreamStatus status);
    void updateVideoFrameView(QSharedPointer<QImage> pImage);

public slots:
    void reset();
    void h264FileLoaded(uint8_t* fileContent, quint32 fileSize);
    void h265FileLoaded(uint8_t* fileContent, quint32 fileSize);
    void frameSelected(QSharedPointer<QAccessUnitModel> pAccessUnits);
    void folderLoaded();
    void framesTabOpened();
    void vpsTabOpened();
    void spsTabOpened();
    void ppsTabOpened();
    void frameDeleted(QUuid id);

private:
    void emitStreamErrors();
    void emitH264SPSErrors();
    void emitH264PPSErrors();
    void emitVPSErrors();
    void emitH265SPSErrors();
    void emitH265PPSErrors();
    void checkForNewGOP();
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
    QImage* decodeH264Slice(QSharedPointer<QAccessUnitModel> pAccessUnitModel);
    QImage* decodeH265Slice(QSharedPointer<QAccessUnitModel> pAccessUnitModel);
    QImage* getQImageFromH264Frame(const AVFrame* pFrame);
    QImage* getQImageFromH265Frame(const AVFrame* pFrame);

    QSharedPointer<QAccessUnitModel> m_pSelectedFrameModel;
    QVector<QSharedPointer<QAccessUnitModel>> m_currentGOPModel;
    int m_tabIndex;
    H264Stream* m_pH264Stream;
    H265Stream* m_pH265Stream;
    QMap<QUuid, QSharedPointer<QImage>> m_decodedFrames;

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