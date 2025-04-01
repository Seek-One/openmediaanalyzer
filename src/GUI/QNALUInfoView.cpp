#include <QDebug>
#include <QVBoxLayout>

#include "QNALUInfoView.h"

QNALUInfoView::QNALUInfoView(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_pTreeView = new QTreeView(this);
    QVBoxLayout* pVBoxLayout = new QVBoxLayout(this);
    setLayout(pVBoxLayout);
    pVBoxLayout->addWidget(m_pTreeView);
    m_pTreeView->show();
    show();
}

QNALUInfoView::~QNALUInfoView(){
    
}

void QNALUInfoView::viewUpdated(QStandardItemModel* pModel){
    if(m_pTreeView->model()){
        delete m_pTreeView->model();
        delete m_pTreeView->selectionModel();
        m_pTreeView->setModel(nullptr);
    }
    m_pTreeView->setModel(pModel);
    m_pTreeView->setColumnWidth(0, 200);
    m_pTreeView->setColumnWidth(1, 50);
}
