#include <QDebug>
#include <QGridLayout>

#include "QStatusView.h"

QStatusView::QStatusView(QWidget* parent)
    : QWidget(parent), m_pStreamSize(new QLabel(this)), m_pFrameValidity(new QLabel(this))
{
    QGridLayout* pGridLayout = new QGridLayout(this);
    setLayout(pGridLayout);
    pGridLayout->addWidget(m_pFrameValidity, 0, 1);
    pGridLayout->addWidget(m_pStreamSize, 0, 2);

    m_pStreamSize->setText("Current stream size : 0B");
    m_pFrameValidity->setText("Valid frames : 0/0 (100%)");
    show();
}

QStatusView::~QStatusView(){
    
}

void QStatusView::sizeUpdated(uint64_t size){
    float sizeGB = size/(float)1e9;
    float sizeMB = size/(float)1e6;
    float sizeKB = size/(float)1e3;
    if(sizeGB > 10) m_pStreamSize->setText("Current stream size : " + QString::number(sizeGB) + "GB");
    else if(sizeMB > 10) m_pStreamSize->setText("Current stream size : " + QString::number(sizeMB) + "MB");
    else m_pStreamSize->setText("Current stream size : " + QString::number(sizeKB) + "KB");
}

void QStatusView::validityUpdated(uint32_t valid, uint32_t total){
    if(total == 0) m_pFrameValidity->setText("Valid frames : 0/0 (100%)");
    else {
        float percentage = 100*valid/total;
        m_pFrameValidity->setText("Valid frames : " + QString::number(valid) + "/" + QString::number(total) + "(" + QString::number(percentage) + "%)");
    }

}
