#include <QVBoxLayout>
#include <QDebug>

#include "QVideoInputView.h"

QVideoInputView::QVideoInputView(QWidget* parent)
    : QGroupBox(tr("No video data"), parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_pTreeView = new QTreeView(this);
    QVBoxLayout* pVBoxLayout = new QVBoxLayout(this);
    setLayout(pVBoxLayout);
    pVBoxLayout->addWidget(m_pTreeView);
    m_pTreeView->show();
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
    m_pTreeView->setModel(nullptr);
}
