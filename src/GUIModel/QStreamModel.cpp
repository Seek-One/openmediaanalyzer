#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QMessageBox>
#include <curl/curl.h>
#include <fstream>

#include "../Codec/H26X/H26XUtils.h"
#include "../GUI/QFolderView.h"

#include "QStreamModel.h"

QStreamModel::QStreamModel():
    m_pThread(nullptr), m_pWorker(nullptr),
    m_pTimer(nullptr), m_videoBytes(0), m_audioBytes(0), m_globalBytes(0)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

QStreamModel::~QStreamModel(){
    emit stopProcessing();
    if(m_pTimer) m_pTimer->deleteLater();
}

size_t receiveHeader(char* contents, size_t size, size_t nmemb, QStreamWorker* inputData){
    size_t totalSize = size*nmemb;
    QString responseStr = QString((char*)contents);
    QRegularExpression responseRegEx = QRegularExpression("HTTP/\\d\\.\\d ((\\d+) [\\S ]+)");
    QRegularExpressionMatch responseMatch = responseRegEx.match(responseStr);

    QRegularExpression multipartRegEx = QRegularExpression("Content-Type: multipart");
    QRegularExpressionMatch multipartMatch = multipartRegEx.match(responseStr);
    if(!responseMatch.hasMatch() && !multipartMatch.hasMatch()) return totalSize;
    if(responseMatch.hasMatch()){
        if(responseMatch.lastCapturedIndex() != 2) {
            emit inputData->error(QStreamWorker::tr("Incomplete header response"));
            return 0;
        }
        int responseCode = responseMatch.captured(2).toInt();
        if(responseCode != 200){
            emit inputData->error(QStreamWorker::tr("Server replied with the following : ") + responseMatch.captured(1));
            return 0;
        }
    } else if(multipartMatch.hasMatch()){
        if(responseStr.contains("Content-Type:") && !multipartRegEx.match(responseStr).hasMatch()){
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
        switch(inputData->m_contentType){
            case ContentType_Video:
            case ContentType_Image:
                videoBytes += contentTypeMatch.capturedStart();
                break;
            case ContentType_Audio:
                audioBytes += contentTypeMatch.capturedStart();
                break;
        }
        QString type = contentTypeMatch.captured(1);
        if(type == "video"){
            inputData->m_contentType = ContentType_Video;
            QString codec = contentTypeMatch.captured(2);
            if(codec == "H264") inputData->m_codec = Codec_H264;
            else if(codec == "H265") inputData->m_codec = Codec_H265;
            else inputData->m_codec = Codec_Unsupported;
        } else if (type == "image"){
            inputData->m_contentType = ContentType_Image;
            QString imgType = contentTypeMatch.captured(2);
            if(imgType == "jpeg") inputData->m_codec = Codec_MJPEG;
            else inputData->m_codec = Codec_Unsupported;
        } else if (type == "audio") inputData->m_contentType = ContentType_Audio;
        else inputData->m_contentType = ContentType_Other;
        switch(inputData->m_contentType){
            case ContentType_Video:
            case ContentType_Image:
                videoBytes += (byteSize - contentTypeMatch.capturedStart());
                break;
            case ContentType_Audio:
                audioBytes += (byteSize - contentTypeMatch.capturedStart());
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
                    emit inputData->loadH264File(allocatedStream, inputData->m_buffer.size());
                    break;
                case Codec_H265:
                    emit inputData->loadH265File(allocatedStream, inputData->m_buffer.size());
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
    emit loadStreamStart();
    m_pThread = new QThread();
    m_pWorker = new QStreamWorker(URL, username, password);
    m_pWorker->moveToThread(m_pThread);
    connect(m_pThread, &QThread::started, m_pWorker, &QStreamWorker::process);
    connect(m_pWorker, &QStreamWorker::finished, m_pThread, &QThread::quit);
    connect(m_pWorker, &QStreamWorker::finished, m_pWorker, &QStreamWorker::deleteLater);
    connect(m_pThread, &QThread::finished, m_pThread, &QThread::deleteLater);
    connect(m_pWorker, &QStreamWorker::loadH264File, this, &QStreamModel::loadH264File);
    connect(m_pWorker, &QStreamWorker::loadH265File, this, &QStreamModel::loadH265File);
    connect(m_pWorker, &QStreamWorker::detectUnsupportedVideoCodec, this, &QStreamModel::detectUnsupportedVideoCodec);
    connect(this, &QStreamModel::stopProcessing, m_pWorker, &QStreamWorker::streamStopped);
    connect(m_pWorker, &QStreamWorker::error, this, &QStreamModel::stopProcessing);
    connect(m_pWorker, &QStreamWorker::error, this, [](const QString& errMsg){
        QMessageBox::critical(nullptr, tr("Error"), errMsg);
    });
    connect(m_pWorker, &QStreamWorker::receiveBytes, this, &QStreamModel::bytesReceived);
    m_pThread->start();

    m_videoBytes = 0;
    m_audioBytes = 0;
    m_globalBytes = 0;
    m_pTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, &QStreamModel::secondElapsed);
    m_pTimer->start(1000);
}

void QStreamModel::bytesReceived(uint64_t videoBytes, uint64_t audioBytes, uint64_t globalBytes){
    m_videoBytes += videoBytes;
    m_audioBytes += audioBytes;
    m_globalBytes += globalBytes;
}

void QStreamModel::streamStopped(){
    emit stopProcessing();
    if(m_pTimer){
        m_pTimer->stop();
        m_pTimer->deleteLater();
        m_pTimer = nullptr;
    }
}

void QStreamModel::secondElapsed(){
    emit updateStatusBitrates(m_videoBytes, m_audioBytes, m_globalBytes);
    m_videoBytes = 0;
    m_audioBytes = 0;
    m_globalBytes = 0;
}

QStreamWorker::QStreamWorker(const QString& URL, const QString& username, const QString& password):
    m_running(true), m_URL(URL), m_username(username), m_password(password), 
    m_codec(Codec_Unsupported), m_contentType(ContentType_Other) 
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
    if(m_URL.startsWith(httpsPrefix)) URL.remove(0, httpsPrefix.size());
    QString fullURL = QString("https://%1:%2@%3").arg(m_username, m_password, URL);
    curl_easy_setopt(curlE, CURLOPT_URL, fullURL.toStdString().c_str());
    curl_easy_setopt(curlE, CURLOPT_HEADERFUNCTION, receiveHeader);
    curl_easy_setopt(curlE, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(curlE, CURLOPT_WRITEFUNCTION, receiveResponse);
    curl_easy_setopt(curlE, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curlE, CURLOPT_CONNECTTIMEOUT, 10);
    curl_easy_setopt(curlE, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curlE, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLM* curlM = curl_multi_init();
    if(!curlM){
        qDebug() << "Couldn't initialize curl multi handle";
        return;
    }
    curl_multi_add_handle(curlM, curlE);
    int receivingData = 0;
    curl_multi_perform(curlM, &receivingData);
    while(receivingData && m_running){
        CURLMcode pollStatus = curl_multi_poll(curlM, nullptr, 0, 10*1000, nullptr);
        if(pollStatus != CURLM_OK){
            qDebug() << "Error while polling for activity on handles :" << curl_multi_strerror(pollStatus);
            break;
        }
        curl_multi_perform(curlM, &receivingData);
        QCoreApplication::processEvents();
    }

    curl_multi_cleanup(curlM);
    curl_easy_cleanup(curlE);
    curl_global_cleanup();
    emit finished();
}

void QStreamWorker::streamStopped(){
    m_running = false;
}