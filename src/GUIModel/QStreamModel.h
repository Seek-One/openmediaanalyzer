#pragma once

#include <QObject>
#include <QFileSystemModel>
#include <QTimer>

#define HTTPS_PREFIX "https://"
#define HTTP_PREFIX "http://"
#define RTSP_PREFIX "rtsp://"

Q_DECLARE_METATYPE(QList<QString>)

class QStreamWorker;

class QStreamModel : public QObject
{
    Q_OBJECT
public:
    QStreamModel();
    
    virtual ~QStreamModel();
    
signals:
    void loadH264Packet(uint8_t* fileContent, quint32 fileSize);
    void loadH265Packet(uint8_t* fileContent, quint32 fileSize);
    void loadStreamStart(bool isLiveStream);
    void stopProcessing();
    void updateStreamBitrates(uint64_t videoBytesLastSecond, uint64_t audioBytesLastSecond, uint64_t globalBytesLastSecond);
    void updateVideoCodec(const QString& codec);
    void updateAudioCodec(const QString& codec);
    void updateContentType(const QString& contentType);
    void updateProtocol(const QString& protocol);
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
    
    QTimer* m_pBitrateTimer;
    uint64_t m_videoBytes;
    uint64_t m_audioBytes;
    uint64_t m_globalBytes;
};

class QStreamWorker : public QObject {
    Q_OBJECT
public:
    QStreamWorker(const QString& URL, const QString& username, const QString& password);
    ~QStreamWorker();

public slots:
    void process();
    void streamStopped();

signals:
    void loadH264Packet(uint8_t* fileContent, quint32 fileSize);
    void loadH265Packet(uint8_t* fileContent, quint32 fileSize);
    void detectUnsupportedVideoCodec();
    void receiveBytes(uint64_t videoBytes, uint64_t audioBytes, uint64_t globalBytes);
    void finished();
    void error(const QString& errMsg);
    void updateVideoCodec(const QString& codec);
    void updateAudioCodec(const QString& codec);
    void updateProtocol(const QString& protocol);
    void updateContentType(const QString& contentType);
    void updateValidURLs(const QString& URL);
    void stopStream();

private:
    const QString& m_URL;
    const QString& m_username;
    const QString& m_password;
};