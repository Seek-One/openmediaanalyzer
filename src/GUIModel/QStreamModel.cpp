#include <QDebug>
#include <QCoreApplication>
#include <QThread>
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

size_t receiveResponse(void* contents, size_t size, size_t nmemb, QStreamWorker* data){
    size_t byteSize = size*nmemb;
    uint8_t* stream = (uint8_t*)contents;

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
        if(!data->m_buffer.empty()){
            for(size_t i = 0; i < startCodePosition;++i) data->m_buffer.push_back(stream[i]);
            uint8_t* allocatedStream = new uint8_t[data->m_buffer.size()];
            memcpy(allocatedStream, data->m_buffer.data(), data->m_buffer.size());
            emit data->loadH265File(allocatedStream, data->m_buffer.size());
            data->m_buffer.clear();

            // data 
            for(size_t i = startCodePosition; i < byteSize;++i) data->m_buffer.push_back(stream[i]);
        } else { // empty buffer : first batch of the entire stream
            for(size_t i = startCodePosition; i < byteSize;++i) data->m_buffer.push_back(stream[i]);
        }
    } else { // no start code : append all data to the current batch if there is one
        if(!data->m_buffer.empty()) for(size_t i = 0; i < byteSize;++i) data->m_buffer.push_back(stream[i]);
    }
    return byteSize;
}

void QStreamModel::streamLoaded(){
    emit stopProcessing();
    emit loadFolderStart();
    m_pThread = new QThread();
    m_pWorker = new QStreamWorker();
    m_pWorker->moveToThread(m_pThread);
    connect(m_pThread, &QThread::started, m_pWorker, &QStreamWorker::process);
    connect(m_pWorker, &QStreamWorker::finished, m_pThread, &QThread::quit);
    connect(m_pWorker, &QStreamWorker::finished, m_pWorker, &QStreamWorker::deleteLater);
    connect(m_pThread, &QThread::finished, m_pThread, &QThread::deleteLater);
    connect(m_pWorker, &QStreamWorker::loadH264File, this, [this](uint8_t* fileContent, quint32 fileSize){
        emit loadH264File(fileContent, fileSize);
    });
    connect(m_pWorker, &QStreamWorker::loadH265File, this, [this](uint8_t* fileContent, quint32 fileSize){
        emit loadH265File(fileContent, fileSize);
    });
    connect(this, &QStreamModel::stopProcessing, m_pWorker, &QStreamWorker::streamStopped);
    m_pThread->start();
}

void QStreamModel::streamStopped(){
    emit stopProcessing();
}

QStreamWorker::QStreamWorker():
    m_running(true)
{}

QStreamWorker::~QStreamWorker(){}

void QStreamWorker::process(){
    CURL* curlE = curl_easy_init();
    if(!curlE){
        qDebug() << "Couldn't initialize curl easy handle";
        return;
    }
    QString videoPath = "vms.seek-one.fr/stream/video?id=10&supported_video_format=H265";
    QString url = QString("https://%1:%2@%3").arg(getenv("VMS_USER"), getenv("VMS_PW"), videoPath);
    curl_easy_setopt(curlE, CURLOPT_URL, url.toStdString().c_str());
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