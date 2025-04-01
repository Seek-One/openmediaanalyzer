#include <QDebug>
#include <QScrollBar>

#include "../GUIModel/QAccessUnitModel.h"
#include "../Codec/H264/H264AccessUnit.h"
#include "QFrameElement.h"

#include "QTimelineView.h"

QTimelineView::QTimelineView(QWidget* parent)
    : QGroupBox("Video timeline", parent)
{
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_pHBoxLayout = new QHBoxLayout(this);
    m_pHBoxLayout->setSpacing(0);

    QWidget* containerWidget = new QWidget(this);
    containerWidget->setLayout(m_pHBoxLayout);

    m_pScrollArea = new QScrollArea(this);
    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); 
    m_pScrollArea->setWidget(containerWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pScrollArea);
    setLayout(mainLayout);
}


QTimelineView::~QTimelineView(){}

void QTimelineView::resetTimeline(){
    m_pFrameElements.clear();
    while(QLayoutItem* item = m_pHBoxLayout->takeAt(0)){
        if(QWidget* widget = item->widget()) widget->deleteLater();
        delete item;
    }
    QFrameElement::m_maxSize = 0;
}

void QTimelineView::unitsUpdated(){
    for(QSharedPointer<QFrameElement> pFrameElement : m_pFrameElements) pFrameElement->update();
}

void QTimelineView::accessUnitsAdded(QVector<QSharedPointer<QAccessUnitModel>> pAccessUnitModels){
    QScrollBar* hScrollBar = m_pScrollArea->horizontalScrollBar();
    int prevHScrollBarMax = hScrollBar->maximum();
    int prevHScrollBarValue = hScrollBar->value();
    bool clampScrollRight = hScrollBar->value() >= SCROLL_CLAMP_RIGHT_THRESHOLD * hScrollBar->maximum();
    // check for new max size first
    // existing access units may have been updated
    for(QSharedPointer<QFrameElement> pFrameElement : m_pFrameElements){
        if(pFrameElement->m_pAccessUnitModel->m_pAccessUnit && pFrameElement->m_pAccessUnitModel->m_pAccessUnit->size() > QFrameElement::m_maxSize){
            QFrameElement::m_maxSize = pFrameElement->m_pAccessUnitModel->m_pAccessUnit->size();
        }
    }
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : pAccessUnitModels) {
        if(pAccessUnitModel->m_pAccessUnit && pAccessUnitModel->m_pAccessUnit->size() > QFrameElement::m_maxSize){
            QFrameElement::m_maxSize = pAccessUnitModel->m_pAccessUnit->size();
        }
    }
    // update existing frames with the potential new max size
    for(QSharedPointer<QFrameElement> pFrameElement : m_pFrameElements) pFrameElement->update();
    // add the new access units afterwards to save on update calls
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : pAccessUnitModels) {
        QSharedPointer<QFrameElement> pFrameElement = QSharedPointer<QFrameElement>(new QFrameElement());
        m_pFrameElements.push_back(pFrameElement);
        pFrameElement->setFrameElement(pAccessUnitModel);
        m_pHBoxLayout->addWidget(pFrameElement.get());
        connect(pFrameElement.get(), &QFrameElement::selectFrame, this, &QTimelineView::frameSelected);
    }
    if(clampScrollRight) hScrollBar->setValue(hScrollBar->maximum());
    else hScrollBar->setValue(hScrollBar->value() - (hScrollBar->maximum() - prevHScrollBarMax));
}

void QTimelineView::accessUnitsRemoved(uint32_t count){
    QScrollBar* hScrollBar = m_pScrollArea->horizontalScrollBar();
    bool clampScrollRight = hScrollBar->value() >= SCROLL_CLAMP_RIGHT_THRESHOLD * hScrollBar->maximum();
    int prevHScrollBarMax = hScrollBar->maximum();
    int prevHScrollBarValue = hScrollBar->value();
    for(int i = 0;i < count;++i){
        QLayoutItem* item = m_pHBoxLayout->takeAt(0);
        if(item->widget()) item->widget()->deleteLater();
        m_pFrameElements.pop_front();
        delete item;
    }
    if(clampScrollRight) hScrollBar->setValue(hScrollBar->maximum());
    // else hScrollBar->setValue(hScrollBar->value() - (prevHScrollBarMax - hScrollBar->maximum()));
}

void QTimelineView::frameSelected(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    for(QSharedPointer<QFrameElement> pFrameElement : m_pFrameElements){
        pFrameElement->m_selected = false;
        pFrameElement->update();
    }
    emit selectFrame(pAccessUnitModel);
}
