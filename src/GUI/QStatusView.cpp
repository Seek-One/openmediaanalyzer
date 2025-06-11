#include <QDebug>
#include <QGridLayout>

#include "QStatusView.h"

QStatusView::QStatusView(QWidget* parent):
    QWidget(parent), 
    m_pVideoStatus(new QLabel(this)), m_pFrameValidity(new QLabel(this)), m_pCodedVideoSize(new QLabel(this)), m_pDecodedPicturesSize(new QLabel(this))
{
    QGridLayout* pGridLayout = new QGridLayout(this);
    setLayout(pGridLayout);
    pGridLayout->addWidget(m_pVideoStatus, 0, 0, Qt::AlignCenter);
    pGridLayout->addWidget(m_pFrameValidity, 0, 1, Qt::AlignCenter);
    pGridLayout->addWidget(m_pCodedVideoSize, 0, 2, Qt::AlignCenter);
    pGridLayout->addWidget(m_pDecodedPicturesSize, 0, 3, Qt::AlignCenter);

    videoStatusUpdated(QDecoderModel::StreamStatus_NoStream);
    frameValidityUpdated(0, 0);
    codedSizeUpdated(0);
    decodedSizeUpdated(0);

    show();
}

QStatusView::~QStatusView(){
    
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

void QStatusView::frameValidityUpdated(uint32_t valid, uint32_t total){
    if(total == 0) m_pFrameValidity->setText(tr("Valid frames : 0/0 (100%)"));
    else {
        float percentage = 100*valid/total;
        m_pFrameValidity->setText(tr("Valid frames : ") + QString::number(valid) + "/" + QString::number(total) + " (" + QString::number(percentage) + "%)");
    }
}

void QStatusView::codedSizeUpdated(uint64_t size){
    float sizeGB = size/(float)1e9;
    float sizeMB = size/(float)1e6;
    float sizeKB = size/(float)1e3;
    if(sizeGB > 10) m_pCodedVideoSize->setText(tr("Current coded video size : ") + QString::number(sizeGB) + tr("GB"));
    else if(sizeMB > 10) m_pCodedVideoSize->setText(tr("Current coded video size : ") + QString::number(sizeMB) + tr("MB"));
    else m_pCodedVideoSize->setText(tr("Current coded video size : ") + QString::number(sizeKB) + tr("KB"));
}


void QStatusView::decodedSizeUpdated(uint64_t size){
    float sizeGB = size/(float)1e3;
    if(sizeGB > 10) m_pDecodedPicturesSize->setText(tr("Current decoded pictures size : ") + QString::number(sizeGB) + tr("GB"));
    else m_pDecodedPicturesSize->setText(tr("Current decoded pictures size : ") + QString::number(size) + tr("MB"));
}


