#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>

#include "QVideoInputView.h"

QVideoInputView::QVideoInputView(QWidget* parent)
    : QGroupBox(tr("No video data"), parent), m_pFolderView(new QTreeView(this)), m_pStreamInfoView(new QTableView(this)),
    m_pVideoContentBitrate(new QStandardItem()), m_pAudioContentBitrate(new QStandardItem()), m_pGlobalContentBitrate(new QStandardItem()),
    m_pGOVLength(new QStandardItem()), m_pContentType(new QStandardItem())
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QVBoxLayout* pVBoxLayout = new QVBoxLayout(this);
    setLayout(pVBoxLayout);
    pVBoxLayout->addWidget(m_pFolderView);
    pVBoxLayout->addWidget(m_pStreamInfoView);

    m_pFolderView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    m_pStreamInfoView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pStreamInfoView->verticalHeader()->setVisible(false);
    m_pStreamInfoView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_pStreamInfoView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_pStreamInfoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pStreamInfoView->setSelectionMode(QAbstractItemView::NoSelection);
    m_pStreamInfoView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    QStandardItemModel* pMetricsModel = new QStandardItemModel(m_pStreamInfoView);
    m_pStreamInfoView->setModel(pMetricsModel);
    pMetricsModel->setHorizontalHeaderLabels({tr("Stream metric"), tr("value")});
    pMetricsModel->setColumnCount(2);
    pMetricsModel->appendRow({new QStandardItem(tr("Video bitrate")), m_pVideoContentBitrate});
    pMetricsModel->appendRow({new QStandardItem(tr("Audio bitrate")), m_pAudioContentBitrate});
    pMetricsModel->appendRow({new QStandardItem(tr("Global bitrate")), m_pGlobalContentBitrate});
    pMetricsModel->appendRow({new QStandardItem(tr("Last packet data type")), m_pContentType});
    pMetricsModel->appendRow({new QStandardItem(tr("GOV length")), m_pGOVLength});
    m_pStreamInfoView->setColumnWidth(0, 3*width()*0.5);
    m_pStreamInfoView->setColumnWidth(1, 3*width()*0.51);
    
    bitratesUpdated(0, 0, 0);
    contentTypeUpdated("");
    GOVLengthUpdated(0);
    m_pStreamInfoView->hide();
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
