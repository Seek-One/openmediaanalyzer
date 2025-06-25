#ifndef STREAM_STREAM_CLIENT_H_
#define STREAM_STREAM_CLIENT_H_


#include <QObject>

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

class StreamClient : public QObject {
    Q_OBJECT
public:
    StreamClient(const QString& URL);

    virtual void process();

    QVector<uint8_t> m_buffer;
    Codec m_codec;
    ContentType m_contentType;

public slots:
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

protected:
    const QString& m_URL;
    bool m_running;
};

#endif // STREAM_STREAM_CLIENT_H_