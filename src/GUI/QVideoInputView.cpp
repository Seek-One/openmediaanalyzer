#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>

#include "QVideoInputView.h"

QVideoInputView::QVideoInputView(QWidget* parent)
    : QGroupBox(tr("No video data"), parent), m_pFolderView(new QTreeView(this)), m_pStreamInfoView(new QTreeView(this)),
    m_pVideoCodec(new QStandardItem()), m_pVideoResolution(new QStandardItem()), m_pVideoFrameRate(new QStandardItem()), 
    m_pGOVLength(new QStandardItem()), m_pVideoContentBitrate(new QStandardItem()), 
    m_pCodedVideoSize(new QStandardItem()), m_pDecodedPicturesSize(new QStandardItem()), m_pFrameValidity(new QStandardItem()),
    m_pAudioCodec(new QStandardItem()), m_pAudioContentBitrate(new QStandardItem()),
    m_pContentType(new QStandardItem()), m_pGlobalContentBitrate(new QStandardItem()), m_pProtocol(new QStandardItem())
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QVBoxLayout* pVBoxLayout = new QVBoxLayout(this);
    setLayout(pVBoxLayout);
    pVBoxLayout->addWidget(m_pFolderView);
    pVBoxLayout->addWidget(m_pStreamInfoView);

    m_pFolderView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    m_pStreamInfoView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pStreamInfoView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    QStandardItemModel* pMetricsModel = new QStandardItemModel(m_pStreamInfoView);
    m_pStreamInfoView->setModel(pMetricsModel);
    pMetricsModel->setHorizontalHeaderLabels({tr("Stream metric"), tr("value")});
    pMetricsModel->setColumnCount(2);

    QStandardItem* pVideoSection = new QStandardItem(tr("Video"));
    QStandardItem* pAudioSection = new QStandardItem(tr("Audio"));
    QStandardItem* pGeneralSection = new QStandardItem(tr("General"));
    pMetricsModel->appendRow(pVideoSection);
    pMetricsModel->appendRow(pAudioSection);
    pMetricsModel->appendRow(pGeneralSection);

    pVideoSection->appendRow({new QStandardItem(tr("Video codec")), m_pVideoCodec});
    pVideoSection->appendRow({new QStandardItem(tr("Video resolution")), m_pVideoResolution});
    pVideoSection->appendRow({new QStandardItem(tr("Video frame rate")), m_pVideoFrameRate});
    pVideoSection->appendRow({new QStandardItem(tr("GOV length")), m_pGOVLength});
    pVideoSection->appendRow({new QStandardItem(tr("Video bitrate")), m_pVideoContentBitrate});
    pVideoSection->appendRow({new QStandardItem(tr("Total encoded video size")), m_pCodedVideoSize});
    pVideoSection->appendRow({new QStandardItem(tr("Total decoded pictures size")), m_pDecodedPicturesSize});
    pVideoSection->appendRow({new QStandardItem(tr("Frame validity")), m_pFrameValidity});
    pAudioSection->appendRow({new QStandardItem(tr("Audio codec")), m_pAudioCodec});
    pAudioSection->appendRow({new QStandardItem(tr("Audio bitrate")), m_pAudioContentBitrate});
    pGeneralSection->appendRow({new QStandardItem(tr("Last packet data type")), m_pContentType});
    pGeneralSection->appendRow({new QStandardItem(tr("Global bitrate")), m_pGlobalContentBitrate});
    pGeneralSection->appendRow({new QStandardItem(tr("Stream protocol")), m_pProtocol});

    m_pStreamInfoView->setColumnWidth(0, 0.65*width()*3);
    
    for(QStandardItem* pItem : {m_pVideoCodec, m_pVideoResolution, m_pVideoFrameRate, m_pGOVLength, m_pVideoContentBitrate, m_pCodedVideoSize, m_pDecodedPicturesSize,
       m_pFrameValidity, m_pAudioCodec, m_pAudioContentBitrate, m_pContentType, m_pGlobalContentBitrate, m_pProtocol}) pItem->setText("-");
    m_pStreamInfoView->hide();
    m_pStreamInfoView->expandAll();
    show();
}

QVideoInputView::~QVideoInputView(){
    
}

void QVideoInputView::folderSet(QFileSystemModel* pModel){
    m_pFolderView->setModel(pModel);
    for (int col = 1; col < m_pFolderView->model()->columnCount(); ++col) m_pFolderView->setColumnHidden(col, true);
    const QString& path = pModel->rootPath();
    m_pFolderView->setRootIndex(pModel->index(path));
    setTitle(tr("Loaded folder : ") + path.split('/').last());
    m_pFolderView->show();
    m_pStreamInfoView->hide();
}

void QVideoInputView::streamSet(const QString& URL){
    QString trimmedURL = URL;
    const uint8_t MAX_VISIBLE_URL_LENGTH = 30;
    if(trimmedURL.startsWith("https://")) trimmedURL.remove(0, 8);
    if(trimmedURL.length() > MAX_VISIBLE_URL_LENGTH) {
        trimmedURL.resize(MAX_VISIBLE_URL_LENGTH);
        trimmedURL += "...";
    }
    setTitle(tr("Reading stream : ") + trimmedURL);
    m_pStreamInfoView->show();
    m_pFolderView->hide();
    m_pFolderView->setModel(nullptr);
}

void QVideoInputView::bitratesUpdated(uint64_t videoBitrate, uint64_t audioBitrate, uint64_t globalBitrate){
    QVector<QStandardItem*> pItems = {m_pVideoContentBitrate, m_pAudioContentBitrate, m_pGlobalContentBitrate};
    QVector<uint64_t> bitrates = {videoBitrate, audioBitrate, globalBitrate};
    for(int i = 0;i < pItems.size();++i){
        float bitrateMB = bitrates[i]/(float)1e6;
        float bitrateKB = bitrates[i]/(float)1e3;
        if(bitrateMB > 10) pItems[i]->setText(QString::number(bitrateMB) + tr("MB/s"));
        else pItems[i]->setText(QString::number(bitrateKB) + tr("KB/s"));
    }
}

void QVideoInputView::contentTypeUpdated(const QString& contentType){
    m_pContentType->setText(contentType);
}

void QVideoInputView::GOVLengthUpdated(uint32_t GOVLength){
    m_pGOVLength->setText(QString::number(GOVLength));
}

void QVideoInputView::frameValidityUpdated(uint32_t valid, uint32_t total){
    if(total == 0) m_pFrameValidity->setText("0/0 (100%)");
    else {
        float percentage = 100*valid/total;
        m_pFrameValidity->setText(QString::number(valid) + "/" + QString::number(total) + " (" + QString::number(percentage) + "%)");
    }
}

void QVideoInputView::codedSizeUpdated(uint64_t size){
    float sizeGB = size/(float)1e9;
    float sizeMB = size/(float)1e6;
    float sizeKB = size/(float)1e3;
    if(sizeGB > 10) m_pCodedVideoSize->setText(QString::number(sizeGB) + tr("GB"));
    else if(sizeMB > 10) m_pCodedVideoSize->setText(QString::number(sizeMB) + tr("MB"));
    else m_pCodedVideoSize->setText(QString::number(sizeKB) + tr("KB"));
}


void QVideoInputView::decodedSizeUpdated(uint64_t size){
    float sizeGB = size/(float)1e3;
    if(sizeGB > 10) m_pDecodedPicturesSize->setText( QString::number(sizeGB) + tr("GB"));
    else m_pDecodedPicturesSize->setText(QString::number(size) + tr("MB"));
}
