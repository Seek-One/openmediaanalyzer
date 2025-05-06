#include <QDebug>
#include <QGridLayout>

#include "QStatusView.h"

QStatusView::QStatusView(QWidget* parent)
    : QWidget(parent), m_pStreamSize(new QLabel(this)), m_pFrameValidity(new QLabel(this)), m_pStreamStatus(new QLabel(this))
{
    QGridLayout* pGridLayout = new QGridLayout(this);
    setLayout(pGridLayout);
    pGridLayout->addWidget(m_pStreamStatus, 0, 0);
    pGridLayout->addWidget(m_pFrameValidity, 0, 1);
    pGridLayout->addWidget(m_pStreamSize, 0, 2);

    m_pStreamSize->setText(tr("Current stream size : 0B"));
    m_pFrameValidity->setText(tr("Valid frames : 0/0 (100%)"));
    m_pStreamStatus->setText(tr(""));
    show();
}

QStatusView::~QStatusView(){
    
}

void QStatusView::sizeUpdated(uint64_t size){
    float sizeGB = size/(float)1e9;
    float sizeMB = size/(float)1e6;
    float sizeKB = size/(float)1e3;
    if(sizeGB > 10) m_pStreamSize->setText(tr("Current stream size : ") + QString::number(sizeGB) + tr("GB"));
    else if(sizeMB > 10) m_pStreamSize->setText(tr("Current stream size : ") + QString::number(sizeMB) + tr("MB"));
    else m_pStreamSize->setText(tr("Current stream size : ") + QString::number(sizeKB) + tr("KB"));
}

void QStatusView::frameValidityUpdated(uint32_t valid, uint32_t total){
    if(total == 0) m_pFrameValidity->setText(tr("Valid frames : 0/0 (100%)"));
    else {
        float percentage = 100*valid/total;
        m_pFrameValidity->setText(tr("Valid frames : ") + QString::number(valid) + "/" + QString::number(total) + " (" + QString::number(percentage) + "%)");
    }
}

void QStatusView::streamStatusUpdated(QDecoderModel::StreamStatus status){
    switch(status){
        case QDecoderModel::StreamStatus_OK:
            m_pStreamStatus->setText(tr("Stream status : OK"));
            m_pStreamStatus->setStyleSheet("QLabel { color : green; }");
            break;
            case QDecoderModel::StreamStatus_NonConformant:
            m_pStreamStatus->setText(tr("Stream status : Non-conforming"));
            m_pStreamStatus->setStyleSheet("QLabel { color : orange; }");
            break;
            case QDecoderModel::StreamStatus_Damaged:
            m_pStreamStatus->setText(tr("Stream status : Damaged"));
            m_pStreamStatus->setStyleSheet("QLabel { color : red; }");
            break;
            case QDecoderModel::StreamStatus_NoStream:
            m_pStreamStatus->setText(tr(""));
            m_pStreamStatus->setStyleSheet("QLabel { color : black; }");
            break;
    }
}
