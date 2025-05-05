#include <QVBoxLayout>
#include <QDebug>

#include "QFolderView.h"

QFolderView::QFolderView(QWidget* parent)
    : QGroupBox(tr("Loaded folder : None"), parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_pTreeView = new QTreeView(this);
    QVBoxLayout* pVBoxLayout = new QVBoxLayout(this);
    setLayout(pVBoxLayout);
    pVBoxLayout->addWidget(m_pTreeView);
    m_pTreeView->show();
    show();
}

QFolderView::~QFolderView(){

}

void QFolderView::setFolder(QFileSystemModel* pModel){
    m_pTreeView->setModel(pModel);
    for (int col = 1; col < m_pTreeView->model()->columnCount(); ++col) m_pTreeView->setColumnHidden(col, true);
    const QString& path = pModel->rootPath();
    m_pTreeView->setRootIndex(pModel->index(path));
    setTitle(tr("Loaded folder : ") + path.split('/').last());
}
