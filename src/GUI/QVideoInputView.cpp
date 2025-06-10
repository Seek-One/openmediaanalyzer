#include <QVBoxLayout>
#include <QDebug>

#include "QVideoInputView.h"

QVideoInputView::QVideoInputView(QWidget* parent)
    : QGroupBox(tr("No video data"), parent), m_pTreeView(new QTreeView(this)), m_pStreamInfoView(new QWidget(this)),
    m_pVideoContentBitrate(new QLabel(m_pStreamInfoView)), m_pAudioContentBitrate(new QLabel(m_pStreamInfoView)), m_pGlobalContentBitrate(new QLabel(m_pStreamInfoView)),
    m_pGOVLength(new QLabel(m_pStreamInfoView)), m_pContentType(new QLabel(m_pStreamInfoView))
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QVBoxLayout* pVBoxLayout = new QVBoxLayout(this);
    setLayout(pVBoxLayout);
    pVBoxLayout->addWidget(m_pTreeView);
    pVBoxLayout->addWidget(m_pStreamInfoView);

    m_pTreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    QVBoxLayout* pMetricsLayout = new QVBoxLayout(m_pStreamInfoView);
    m_pStreamInfoView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pStreamInfoView->setLayout(pMetricsLayout);
    m_pStreamInfoView->setStyleSheet("QWidget { background-color: white;}");
    pMetricsLayout->setAlignment(Qt::AlignTop);
    pMetricsLayout->addWidget(m_pVideoContentBitrate);
    pMetricsLayout->addWidget(m_pAudioContentBitrate);
    pMetricsLayout->addWidget(m_pGlobalContentBitrate);
    pMetricsLayout->addWidget(m_pContentType);
    pMetricsLayout->addWidget(m_pGOVLength);

    m_pVideoContentBitrate->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pAudioContentBitrate->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pGlobalContentBitrate->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pContentType->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pGOVLength->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    bitratesUpdated(0, 0, 0);
    contentTypeUpdated("");
    GOVLengthUpdated(0);
    m_pStreamInfoView->hide();
    show();
}

QVideoInputView::~QVideoInputView(){
    
}

void QVideoInputView::folderSet(QFileSystemModel* pModel){
    m_pTreeView->setModel(pModel);
    for (int col = 1; col < m_pTreeView->model()->columnCount(); ++col) m_pTreeView->setColumnHidden(col, true);
    const QString& path = pModel->rootPath();
    m_pTreeView->setRootIndex(pModel->index(path));
    setTitle(tr("Loaded folder : ") + path.split('/').last());
    m_pTreeView->show();
    m_pStreamInfoView->hide();
}

void QVideoInputView::streamSet(const QString& URL){
    QString trimmedURL = URL;
    const uint8_t MAX_VISIBLE_URL_LENGTH = 22;
    if(trimmedURL.startsWith("https://")) trimmedURL.remove(0, 8);
    if(trimmedURL.length() > MAX_VISIBLE_URL_LENGTH) {
        trimmedURL.resize(MAX_VISIBLE_URL_LENGTH);
        trimmedURL += "...";
    }
    setTitle(tr("Reading stream : ") + trimmedURL);
    m_pStreamInfoView->show();
    m_pTreeView->hide();
    m_pTreeView->setModel(nullptr);
}

void QVideoInputView::bitratesUpdated(uint64_t videoBitrate, uint64_t audioBitrate, uint64_t globalBitrate){
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

void QVideoInputView::contentTypeUpdated(const QString& contentType){
    m_pContentType->setText(tr("Last packet data type : ") + contentType);
}

void QVideoInputView::GOVLengthUpdated(uint32_t GOVLength){
    m_pGOVLength->setText(tr("GOV length : ") + QString::number(GOVLength));
}
