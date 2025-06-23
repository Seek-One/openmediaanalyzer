#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QMessageBox>
#include <QSettings>
#include <curl/curl.h>
#include <fstream>

#include "../Codec/H26X/H26XUtils.h"
#include "../GUI/QVideoInputView.h"

#include "QStreamModel.h"

QStreamModel::QStreamModel():
    m_pThread(nullptr), m_pWorker(nullptr),
    m_pBitrateTimer(nullptr), m_videoBytes(0), m_audioBytes(0), m_globalBytes(0)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

QStreamModel::~QStreamModel(){
    emit stopProcessing();
    if(m_pBitrateTimer) m_pBitrateTimer->deleteLater();
}

size_t receiveHeader(char* contents, size_t size, size_t nmemb, QStreamWorker* inputData){
    size_t totalSize = size*nmemb;
    QString responseStr = QString((char*)contents);
    QRegularExpression responseRegEx = QRegularExpression("(HTTP/\\d+\\.\\d+) ((\\d+) [\\S ]+)");
    QRegularExpressionMatch responseMatch = responseRegEx.match(responseStr);

    QRegularExpression multipartRegEx = QRegularExpression("[Cc]ontent-[Tt]ype: (\\w+)");
    QRegularExpressionMatch multipartMatch = multipartRegEx.match(responseStr);
    if(!responseMatch.hasMatch() && !multipartMatch.hasMatch()) return totalSize;
    if(responseMatch.hasMatch()){
        if(responseMatch.lastCapturedIndex() != 3) {
            emit inputData->error(QStreamWorker::tr("Incomplete header response"));
            return 0;
        }
        int responseCode = responseMatch.captured(3).toInt();
        if(responseCode != 200){
            emit inputData->error(QStreamWorker::tr("Server replied with the following : ") + responseMatch.captured(2));
            return 0;
        }
        emit inputData->updateProtocol(responseMatch.captured(1));
    } else if(multipartMatch.hasMatch()){
        if(multipartMatch.captured(1) != "multipart"){
            emit inputData->error(QStreamWorker::tr("Multipart stream expected"));
            return 0;
        }
    }
    return totalSize;
}


size_t receiveResponse(void* contents, size_t size, size_t nmemb, QStreamWorker* inputData){
    size_t byteSize = size*nmemb;
    uint8_t* stream = (uint8_t*)contents;
    uint64_t videoBytes = 0;
    uint64_t audioBytes = 0;
    
    QString responseStr = QString((char*)contents);
    QRegularExpression contentTypeRegEx = QRegularExpression("Content-type: (\\w+)/([\\w\\-\\+\\.]+)");
    QRegularExpressionMatch contentTypeMatch = contentTypeRegEx.match(responseStr);
    if(contentTypeMatch.hasMatch()){
        emit inputData->updateContentType(contentTypeMatch.captured(1) + "/" + contentTypeMatch.captured(2));
        switch(inputData->m_contentType){
            case ContentType_Video:
            case ContentType_Image:
                videoBytes += contentTypeMatch.capturedStart();
                break;
            case ContentType_Audio:
                audioBytes += contentTypeMatch.capturedStart();
                break;
            default:
                break;
        }
        QString type = contentTypeMatch.captured(1);
        if(type == "video"){
            inputData->m_contentType = ContentType_Video;
            QString codec = contentTypeMatch.captured(2);
            if(codec == "H264") inputData->m_codec = Codec_H264;
            else if(codec == "H265") inputData->m_codec = Codec_H265;
            else inputData->m_codec = Codec_Unsupported;
            emit inputData->updateVideoCodec(codec);
        } else if (type == "image"){
            inputData->m_contentType = ContentType_Image;
            QString imgType = contentTypeMatch.captured(2);
            if(imgType == "jpeg") inputData->m_codec = Codec_MJPEG;
            else inputData->m_codec = Codec_Unsupported;
        } else if (type == "audio") {
            inputData->m_contentType = ContentType_Audio;
            emit inputData->updateAudioCodec(contentTypeMatch.captured(2));
        }
        else inputData->m_contentType = ContentType_Other;
        switch(inputData->m_contentType){
            case ContentType_Video:
            case ContentType_Image:
                videoBytes += (byteSize - contentTypeMatch.capturedStart());
                break;
            case ContentType_Audio:
                audioBytes += (byteSize - contentTypeMatch.capturedStart());
                break;
            default:
                break;
        }
    } else {
         switch(inputData->m_contentType){
            case ContentType_Video:
            case ContentType_Image:
                videoBytes += byteSize;
                break;
            case ContentType_Audio:
                audioBytes += byteSize;
                break;
            default:   
                break;
        }
    }
    emit inputData->receiveBytes(videoBytes, audioBytes, byteSize);
    if(inputData->m_contentType != ContentType_Video && inputData->m_contentType != ContentType_Image) return byteSize;

    // locate potential start code in byte stream
    size_t startCodePosition = byteSize;
    for(size_t i = 0;i + 3 < byteSize;++i){
        if(stream[i] == 0x00 && stream[i+1] == 0x00){
            if(stream[i+2] == 0x01 || (stream[i+2] == 0x00 && stream[i+3] == 0x01)){
                startCodePosition = i;
                break;
            }
        }
    }

    // start code found : append all data preceding it and send it to decoding
    if(startCodePosition != byteSize){ 
        // non-empty buffer : previous batch getting completed
        if(!inputData->m_buffer.empty()){
            for(size_t i = 0; i < startCodePosition;++i) inputData->m_buffer.push_back(stream[i]);
            uint8_t* allocatedStream = new uint8_t[inputData->m_buffer.size()];
            memcpy(allocatedStream, inputData->m_buffer.data(), inputData->m_buffer.size());
            switch(inputData->m_codec){
                case Codec_H264:
                    emit inputData->loadH264Packet(allocatedStream, inputData->m_buffer.size());
                    break;
                case Codec_H265:
                    emit inputData->loadH265Packet(allocatedStream, inputData->m_buffer.size());
                    break;
                default:
                    emit inputData->detectUnsupportedVideoCodec();
            }
            inputData->m_buffer.clear();

            // data 
            for(size_t i = startCodePosition; i < byteSize;++i) inputData->m_buffer.push_back(stream[i]);
        } else { // empty buffer : first batch of the entire stream
            for(size_t i = startCodePosition; i < byteSize;++i) inputData->m_buffer.push_back(stream[i]);
        }
    } else { // no start code : append all data to the current batch if there is one
        if(!inputData->m_buffer.empty()) for(size_t i = 0; i < byteSize;++i) inputData->m_buffer.push_back(stream[i]);
    }
    return byteSize;
}

void QStreamModel::streamLoaded(const QString& URL, const QString& username, const QString& password){
    emit stopProcessing();
    emit loadStreamStart(true);
    m_pThread = new QThread();
    m_pWorker = new QStreamWorker(URL, username, password);
    m_pWorker->moveToThread(m_pThread);

    connect(m_pThread, &QThread::started, m_pWorker, &QStreamWorker::process);
    connect(m_pWorker, &QStreamWorker::finished, m_pThread, &QThread::quit);
    connect(m_pWorker, &QStreamWorker::finished, m_pWorker, &QStreamWorker::deleteLater);
    connect(m_pThread, &QThread::finished, m_pThread, &QThread::deleteLater);
    connect(this, &QStreamModel::stopProcessing, m_pWorker, &QStreamWorker::streamStopped);
    connect(m_pWorker, &QStreamWorker::error, this, &QStreamModel::stopProcessing);
    connect(m_pWorker, &QStreamWorker::error, this, [](const QString& errMsg){
        QMessageBox::critical(nullptr, tr("Error"), errMsg);
    });

    connect(m_pWorker, &QStreamWorker::updateValidURLs, this, &QStreamModel::updateValidURLs);
    
    connect(m_pWorker, &QStreamWorker::loadH264Packet, this, &QStreamModel::loadH264Packet);
    connect(m_pWorker, &QStreamWorker::loadH265Packet, this, &QStreamModel::loadH265Packet);
    connect(m_pWorker, &QStreamWorker::detectUnsupportedVideoCodec, this, &QStreamModel::detectUnsupportedVideoCodec);
    
    connect(m_pWorker, &QStreamWorker::receiveBytes, this, &QStreamModel::bytesReceived);
    connect(m_pWorker, &QStreamWorker::updateVideoCodec, this, &QStreamModel::updateVideoCodec);
    connect(m_pWorker, &QStreamWorker::updateAudioCodec, this, &QStreamModel::updateAudioCodec);
    connect(m_pWorker, &QStreamWorker::updateContentType, this, &QStreamModel::updateContentType);
    connect(m_pWorker, &QStreamWorker::updateProtocol, this, &QStreamModel::updateProtocol);

    

    m_pThread->start();

    m_videoBytes = 0;
    m_audioBytes = 0;
    m_globalBytes = 0;
    m_pBitrateTimer = new QTimer(this);
    connect(m_pBitrateTimer, &QTimer::timeout, this, &QStreamModel::secondElapsed);
    m_pBitrateTimer->start(1000);
}

void QStreamModel::bytesReceived(uint64_t videoBytes, uint64_t audioBytes, uint64_t globalBytes){
    m_videoBytes += videoBytes;
    m_audioBytes += audioBytes;
    m_globalBytes += globalBytes;
}

void QStreamModel::streamStopped(){
    emit stopProcessing();
    if(m_pBitrateTimer){
        m_pBitrateTimer->stop();
        m_pBitrateTimer->deleteLater();
        m_pBitrateTimer = nullptr;
    }
}

void QStreamModel::secondElapsed(){
    emit updateStreamBitrates(m_videoBytes, m_audioBytes, m_globalBytes);
    m_videoBytes = 0;
    m_audioBytes = 0;
    m_globalBytes = 0;
}

QStreamWorker::QStreamWorker(const QString& URL, const QString& username, const QString& password):
m_codec(Codec_Unsupported), m_contentType(ContentType_Other), 
    m_running(true), m_URL(URL), m_username(username), m_password(password)
{}

QStreamWorker::~QStreamWorker(){}

void QStreamWorker::process(){
    CURL* curlE = curl_easy_init();
    if(!curlE){
        qDebug() << "Couldn't initialize curl easy handle";
        return;
    }
    QString URL = m_URL;
    QString httpsPrefix = "https://";
    QString httpPrefix = "https://";
    if(m_URL.startsWith(httpsPrefix)) URL.remove(0, httpsPrefix.size());
    else if (m_URL.startsWith(httpPrefix)) URL.remove(0, httpPrefix.size());
    QString fullURL; 
    if(m_username.isEmpty() && m_password.isEmpty()) fullURL = QString("https://%1").arg(URL);
    else fullURL = QString("https://%1:%2@%3").arg(m_username, m_password, URL);
    curl_easy_setopt(curlE, CURLOPT_URL, fullURL.toStdString().c_str());
    curl_easy_setopt(curlE, CURLOPT_HEADERFUNCTION, receiveHeader);
    curl_easy_setopt(curlE, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(curlE, CURLOPT_WRITEFUNCTION, receiveResponse);
    curl_easy_setopt(curlE, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curlE, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curlE, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLM* curlM = curl_multi_init();
    if(!curlM){
        qDebug() << "Couldn't initialize curl multi handle";
        return;
    }
    curl_multi_add_handle(curlM, curlE);
    int receivingData = 0;
    int handleHasData = 1;
    CURLMcode curlMStatus = curl_multi_perform(curlM, &receivingData);
    if(curlMStatus != CURLM_OK) qDebug() << curl_multi_strerror(curlMStatus);
    while((receivingData || handleHasData) && m_running){
        CURLMcode pollStatus = curl_multi_poll(curlM, nullptr, 0, 3000, &handleHasData);
        if(pollStatus != CURLM_OK){
            qDebug() << "Error while polling for activity on handles :" << curl_multi_strerror(pollStatus);
            break;
        }

        curlMStatus = curl_multi_perform(curlM, &receivingData);
        if(curlMStatus != CURLM_OK) qDebug() << curl_multi_strerror(curlMStatus);
        QCoreApplication::processEvents();
    }
    if(handleHasData == 0) emit error(tr("No stream data found"));
    else if(receivingData == 1 && handleHasData == 1){
        QSettings settings;
        QList<QString> registeredLinks = settings.value("validLinks").value<QList<QString>>();
        if(!registeredLinks.contains(URL)) {
            registeredLinks.push_back(URL);
            if(registeredLinks.size() > 7) registeredLinks.pop_front();
            settings.setValue("validLinks", QVariant::fromValue(registeredLinks));
            emit updateValidURLs(URL);
        }
    }
    curl_multi_cleanup(curlM);
    curl_easy_cleanup(curlE);
    curl_global_cleanup();
    emit finished();
}

void QStreamWorker::streamStopped(){
    m_running = false;
}