#include <QDebug>
#include <QThread>
#include <QSettings>
#include <QMessageBox>
#include <curl/curl.h>
#include <fstream>

#include "../Codec/H26X/H26XUtils.h"
#include "../GUI/QVideoInputView.h"
#include "../Stream/HTTPClient.h"
#include "../Stream/RTSPClient.h"

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
    m_URL(URL), m_username(username), m_password(password)
{}

QStreamWorker::~QStreamWorker(){}

void QStreamWorker::process(){
    QString fullURL = m_URL;
    fullURL.remove(QRegularExpression("\\w+://"));
    if(!m_username.isEmpty() || !m_password.isEmpty()) fullURL.prepend(QString("%1:%2@").arg(m_username, m_password));
    StreamClient* pStreamClient;
    if(m_URL.startsWith(HTTP_PREFIX)){
        pStreamClient = new HTTPClient(fullURL.prepend(HTTP_PREFIX));
    } else if (m_URL.startsWith(HTTPS_PREFIX)){
        pStreamClient = new HTTPClient(fullURL.prepend(HTTPS_PREFIX));
    } else if (m_URL.startsWith(RTSP_PREFIX)){
        pStreamClient = new RTSPClient(fullURL.prepend(RTSP_PREFIX));
    } else {
        emit error(tr("Invalid or unsupported protocol"));
        return;
    }
    connect(pStreamClient, &StreamClient::loadH264Packet, this, &QStreamWorker::loadH264Packet);
    connect(pStreamClient, &StreamClient::loadH265Packet, this, &QStreamWorker::loadH265Packet);
    connect(pStreamClient, &StreamClient::detectUnsupportedVideoCodec, this, &QStreamWorker::detectUnsupportedVideoCodec);
    connect(pStreamClient, &StreamClient::receiveBytes, this, &QStreamWorker::receiveBytes);
    connect(pStreamClient, &StreamClient::finished, this, [this, pStreamClient](){
        pStreamClient->deleteLater();
        emit finished();
    });
    connect(pStreamClient, &StreamClient::error, this, &QStreamWorker::error);
    connect(pStreamClient, &StreamClient::updateVideoCodec, this, &QStreamWorker::updateVideoCodec);
    connect(pStreamClient, &StreamClient::updateAudioCodec, this, &QStreamWorker::updateAudioCodec);
    connect(pStreamClient, &StreamClient::updateProtocol, this, &QStreamWorker::updateProtocol);
    connect(pStreamClient, &StreamClient::updateContentType, this, &QStreamWorker::updateContentType);
    connect(pStreamClient, &StreamClient::updateValidURLs, this, [this](const QString& URL){
        QSettings settings;
        QList<QString> registeredLinks = settings.value("validLinks").value<QList<QString>>();
        QString anonymizedURL = URL;
        anonymizedURL.remove(QRegularExpression("[\\w\\d_\\-]+:[\\w\\d_\\-]+@"));
        if(!registeredLinks.contains(anonymizedURL)) {
            registeredLinks.push_back(anonymizedURL);
            while(registeredLinks.size() > 7) registeredLinks.pop_front();
            settings.setValue("validLinks", QVariant::fromValue(registeredLinks));
            emit updateValidURLs(anonymizedURL);
        }
    });
    connect(this, &QStreamWorker::stopStream, pStreamClient, &StreamClient::streamStopped);
    
    pStreamClient->process();
}

void QStreamWorker::streamStopped(){
    emit stopStream();
}