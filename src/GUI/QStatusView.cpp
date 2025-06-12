#include <QDebug>
#include <QGridLayout>

#include "QStatusView.h"

QStatusView::QStatusView(QWidget* parent):
    QWidget(parent), 
    m_pVideoStatus(new QLabel(this)), m_pInputSource(new QLabel(this))
{
    QGridLayout* pGridLayout = new QGridLayout(this);
    setLayout(pGridLayout);
    pGridLayout->addWidget(m_pInputSource, 0, 0, Qt::AlignCenter);
    pGridLayout->addWidget(m_pVideoStatus, 0, 1, Qt::AlignCenter);

    videoStatusUpdated(QDecoderModel::StreamStatus_NoStream);

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

void QStatusView::folderPathSet(QFileSystemModel* pModel){
    m_pInputSource->setText(tr("Source : ") + pModel->rootPath());
}

void QStatusView::streamLinkSet(const QString& URL){
    QString trimmedURL = URL;
    if(trimmedURL.startsWith("https://")) trimmedURL.remove(0, 8);
    m_pInputSource->setText(tr("Source : ") + trimmedURL);
}


