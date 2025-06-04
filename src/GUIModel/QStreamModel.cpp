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
    m_pThread(nullptr), m_pWorker(nullptr)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

QStreamModel::~QStreamModel(){
    emit stopProcessing();
}

size_t receiveHeader(char* contents, size_t size, size_t nmemb, QStreamWorker* inputData){
    size_t totalSize = size*nmemb;
    QString responseStr = QString((char*)contents);
    QRegularExpression responseRegEx = QRegularExpression("HTTP/\\d\\.\\d ((\\d+) [\\S ]+)");
    QRegularExpressionMatch responseMatch = responseRegEx.match(responseStr);
    if(!responseMatch.hasMatch()) return totalSize;
    if(responseMatch.lastCapturedIndex() != 2) {
        emit inputData->error(inputData->tr("Incomplete header response"));
        return 0;
    }
    int responseCode = responseMatch.captured(2).toInt();
    if(responseCode != 200){
        emit inputData->error(inputData->tr("Server replied with the following : ") + responseMatch.captured(1));
        return 0;
    }
    return totalSize;
}


size_t receiveResponse(void* contents, size_t size, size_t nmemb, QStreamWorker* inputData){
    size_t byteSize = size*nmemb;
    uint8_t* stream = (uint8_t*)contents;

    if(inputData->m_codec == Codec_Unspecified){
        QString responseStr = QString((char*)contents);
        QRegularExpression contentTypeRegEx = QRegularExpression("Content-type: (\\w+/\\w+)");
        QRegularExpressionMatch contentTypeMatch = contentTypeRegEx.match(responseStr);
        if(contentTypeMatch.hasMatch()){
            QString contentType = contentTypeMatch.captured(1); // 0 is whole regex
            if(contentType == "video/H264") inputData->m_codec = Codec_H264;
            else if (contentType == "video/H265") inputData->m_codec = Codec_H265;
        }
        if(inputData->m_codec == Codec_Unspecified){
            emit inputData->error(inputData->tr("Unsupported video codec"));
            return 0;
        }
    }

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
                    emit inputData->error(inputData->tr("Unable to decode video content : unspecified video codec"));
                    return 0;
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
    connect(this, &QStreamModel::stopProcessing, m_pWorker, &QStreamWorker::streamStopped);
    connect(m_pWorker, &QStreamWorker::error, this, &QStreamModel::stopProcessing);
    connect(m_pWorker, &QStreamWorker::error, this, [](const QString& errMsg){
        QMessageBox::critical(nullptr, tr("Error"), errMsg);
    });
    m_pThread->start();
}

void QStreamModel::streamStopped(){
    emit stopProcessing();
}

QStreamWorker::QStreamWorker(const QString& URL, const QString& username, const QString& password):
    m_running(true), m_URL(URL), m_username(username), m_password(password), m_codec(Codec_Unspecified)
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
    qDebug() << fullURL;
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
    curl_multi_perform(curlM, &receivingData);
    while(receivingData && m_running){
        CURLMcode pollStatus = curl_multi_poll(curlM, nullptr, 0, 0, nullptr);
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