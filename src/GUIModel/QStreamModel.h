#pragma once

#include <QObject>
#include <QFileSystemModel>
#include <QTimer>

#include "QAccessUnitModel.h"

Q_DECLARE_METATYPE(QList<QString>)

class QVideoInputView;
class QStreamWorker;

enum ContentType {
    ContentType_Other,
    ContentType_Video,
    ContentType_Image,
    ContentType_Audio
};

enum Codec {
    Codec_Unsupported,
    Codec_H264,
    Codec_H265,
    Codec_MJPEG
};

class QStreamModel : public QObject
{
    Q_OBJECT
public:
    QStreamModel();
    
    virtual ~QStreamModel();
    
signals:
    void loadH264File(uint8_t* fileContent, quint32 fileSize);
    void loadH265File(uint8_t* fileContent, quint32 fileSize);
    void loadStreamStart();
    void stopProcessing();
    void updateStatusBitrates(uint64_t videoBytesLastSecond, uint64_t audioBytesLastSecond, uint64_t globalBytesLastSecond);
    void updateContentType(const QString& contentType);
    void updateValidURLs(const QString& URL);
    void detectUnsupportedVideoCodec();
    
public slots:
    void streamLoaded(const QString& URL, const QString& username, const QString& password);
    void streamStopped();
    void bytesReceived(uint64_t videoBytes, uint64_t audioBytes, uint64_t globalBytes);
    void secondElapsed();
    
private:
    QThread* m_pThread;
    QStreamWorker* m_pWorker;
    
    QTimer* m_pTimer;
    uint64_t m_videoBytes;
    uint64_t m_audioBytes;
    uint64_t m_globalBytes;
};

class QStreamWorker : public QObject {
    Q_OBJECT
public:
    QStreamWorker(const QString& URL, const QString& username, const QString& password);
    ~QStreamWorker();
    
    QVector<uint8_t> m_buffer;
    Codec m_codec;
    ContentType m_contentType;
public slots:
    void process();
    void streamStopped();
signals:
    void loadH264File(uint8_t* fileContent, quint32 fileSize);
    void loadH265File(uint8_t* fileContent, quint32 fileSize);
    void detectUnsupportedVideoCodec();
    void receiveBytes(uint64_t videoBytes, uint64_t audioBytes, uint64_t globalBytes);
    void finished();
    void error(const QString& errMsg);
    void updateContentType(const QString& contentType);
    void updateValidURLs(const QString& URL);
private:
    bool m_running;
    const QString& m_URL;
    const QString& m_username;
    const QString& m_password;
};