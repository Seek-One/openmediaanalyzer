#include <QDebug>
#include <QGridLayout>

#include "QStatusView.h"

QStatusView::QStatusView(QWidget* parent):
    QWidget(parent), 
    m_pVideoSize(new QLabel(this)), m_pFrameValidity(new QLabel(this)), m_pVideoStatus(new QLabel(this)),
    m_pVideoContentBitrate(new QLabel(this)), m_pAudioContentBitrate(new QLabel(this)), m_pGlobalContentBitrate(new QLabel(this))
{
    QGridLayout* pGridLayout = new QGridLayout(this);
    setLayout(pGridLayout);
    pGridLayout->addWidget(m_pVideoStatus, 0, 0);
    pGridLayout->addWidget(m_pFrameValidity, 0, 1);
    pGridLayout->addWidget(m_pVideoSize, 0, 2);
    pGridLayout->addWidget(m_pVideoContentBitrate, 0, 3);
    pGridLayout->addWidget(m_pAudioContentBitrate, 0, 4);
    pGridLayout->addWidget(m_pGlobalContentBitrate, 0, 5);

    m_pVideoSize->setText(tr("Current video size : 0B"));
    m_pFrameValidity->setText(tr("Valid frames : 0/0 (100%)"));
    m_pVideoStatus->setText("");
    m_pVideoContentBitrate->setText("");
    m_pAudioContentBitrate->setText("");
    m_pGlobalContentBitrate->setText("");

    pGridLayout->setAlignment(m_pVideoStatus, Qt::AlignCenter);
    pGridLayout->setAlignment(m_pFrameValidity, Qt::AlignCenter);
    pGridLayout->setAlignment(m_pVideoSize, Qt::AlignCenter);
    pGridLayout->setAlignment(m_pVideoContentBitrate, Qt::AlignCenter);
    pGridLayout->setAlignment(m_pAudioContentBitrate, Qt::AlignCenter);
    pGridLayout->setAlignment(m_pGlobalContentBitrate, Qt::AlignCenter);
    show();
}

QStatusView::~QStatusView(){
    
}

void QStatusView::sizeUpdated(uint64_t size){
    float sizeGB = size/(float)1e9;
    float sizeMB = size/(float)1e6;
    float sizeKB = size/(float)1e3;
    if(sizeGB > 10) m_pVideoSize->setText(tr("Current video size : ") + QString::number(sizeGB) + tr("GB"));
    else if(sizeMB > 10) m_pVideoSize->setText(tr("Current video size : ") + QString::number(sizeMB) + tr("MB"));
    else m_pVideoSize->setText(tr("Current video size : ") + QString::number(sizeKB) + tr("KB"));
}

void QStatusView::frameValidityUpdated(uint32_t valid, uint32_t total){
    if(total == 0) m_pFrameValidity->setText(tr("Valid frames : 0/0 (100%)"));
    else {
        float percentage = 100*valid/total;
        m_pFrameValidity->setText(tr("Valid frames : ") + QString::number(valid) + "/" + QString::number(total) + " (" + QString::number(percentage) + "%)");
    }
}

void QStatusView::videoStatusUpdated(QDecoderModel::StreamStatus status){
    switch(status){
        case QDecoderModel::StreamStatus_OK:
            m_pVideoStatus->setText(tr("Video status : OK"));
            m_pVideoStatus->setStyleSheet("QLabel { color : green; }");
            break;
        case QDecoderModel::StreamStatus_NonConformant:
            m_pVideoStatus->setText(tr("Video status : Non-conforming"));
            m_pVideoStatus->setStyleSheet("QLabel { color : rgb(128, 128, 0); }");
            break;
        case QDecoderModel::StreamStatus_Damaged:
            m_pVideoStatus->setText(tr("Video status : Damaged"));
            m_pVideoStatus->setStyleSheet("QLabel { color : red; }");
            break;
        case QDecoderModel::StreamStatus_NoStream:
            m_pVideoStatus->setText(tr(""));
            m_pVideoStatus->setStyleSheet("QLabel { color : black; }");
            break;
    }
}

void QStatusView::bitratesUpdated(uint64_t videoBitrate, uint64_t audioBitrate, uint64_t globalBitrate){
    QVector<QLabel*> pLabels = {m_pVideoContentBitrate, m_pAudioContentBitrate, m_pGlobalContentBitrate};
    QVector<QString> labelTexts = {tr("Video bitrate : "), tr("Audio bitrate : "), tr("Global bitrate : ")};
    QVector<uint64_t> bitrates = {videoBitrate, audioBitrate, globalBitrate};
    for(int i = 0;i < pLabels.size();++i){
        float bitrateMB = bitrates[i]/(float)1e6;
        float bitrateKB = bitrates[i]/(float)1e3;
        if(bitrateMB > 10) pLabels[i]->setText(labelTexts[i] + QString::number(bitrateMB) + tr("MB/s"));
        else pLabels[i]->setText(labelTexts[i] + QString::number(bitrateKB) + tr("KB/s"));
    }
}
