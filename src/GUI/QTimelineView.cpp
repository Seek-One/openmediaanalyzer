#include <QDebug>
#include <QScrollBar>

#include "../GUIModel/QAccessUnitModel.h"
#include "../Codec/H264/H264AccessUnit.h"
#include "QAccessUnitElement.h"

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
    m_pAccessUnitElements.clear();
    while(QLayoutItem* item = m_pHBoxLayout->takeAt(0)){
        if(QWidget* widget = item->widget()) widget->deleteLater();
        delete item;
    }
    QAccessUnitElement::m_maxSize = 0;
}

void QTimelineView::accessUnitsUpdated(){
    for(QSharedPointer<QAccessUnitElement> pTimelineAccessUnitElement : m_pAccessUnitElements){
        if(pTimelineAccessUnitElement->m_pAccessUnitModel->m_pAccessUnit && pTimelineAccessUnitElement->m_pAccessUnitModel->m_pAccessUnit->size() > QAccessUnitElement::m_maxSize){
            QAccessUnitElement::m_maxSize = pTimelineAccessUnitElement->m_pAccessUnitModel->m_pAccessUnit->size();
        }
    }
    for(QSharedPointer<QAccessUnitElement> pAccessUnitElement : m_pAccessUnitElements) pAccessUnitElement->update();
}

void QTimelineView::accessUnitsAdded(QVector<QSharedPointer<QAccessUnitModel>> pAccessUnitModels){
    QScrollBar* hScrollBar = m_pScrollArea->horizontalScrollBar();
    int prevHScrollBarMax = hScrollBar->maximum();
    int prevHScrollBarValue = hScrollBar->value();
    bool clampScrollRight = hScrollBar->value() >= SCROLL_CLAMP_RIGHT_THRESHOLD * hScrollBar->maximum();
    // check for new max size first
    // existing access units may have been updated
    for(QSharedPointer<QAccessUnitElement> pAccessUnitElement : m_pAccessUnitElements){
        if(pAccessUnitElement->m_pAccessUnitModel->m_pAccessUnit && pAccessUnitElement->m_pAccessUnitModel->m_pAccessUnit->size() > QAccessUnitElement::m_maxSize){
            QAccessUnitElement::m_maxSize = pAccessUnitElement->m_pAccessUnitModel->m_pAccessUnit->size();
        }
    }
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : pAccessUnitModels) {
        if(pAccessUnitModel->m_pAccessUnit && pAccessUnitModel->m_pAccessUnit->size() > QAccessUnitElement::m_maxSize){
            QAccessUnitElement::m_maxSize = pAccessUnitModel->m_pAccessUnit->size();
        }
    }
    // update existing accessUnits with the potential new max size
    for(QSharedPointer<QAccessUnitElement> pAccessUnitElement : m_pAccessUnitElements) pAccessUnitElement->update();
    // add the new access units afterwards to save on update calls
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : pAccessUnitModels) {
        QSharedPointer<QAccessUnitElement> pAccessUnitElement = QSharedPointer<QAccessUnitElement>(new QAccessUnitElement());
        m_pAccessUnitElements.push_back(pAccessUnitElement);
        pAccessUnitElement->setAccessUnitElement(pAccessUnitModel);
        m_pHBoxLayout->addWidget(pAccessUnitElement.get());
        connect(pAccessUnitElement.get(), &QAccessUnitElement::selectAccessUnit, this, &QTimelineView::accessUnitSelected);
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
        m_pAccessUnitElements.pop_front();
        delete item;
    }
    if(clampScrollRight) hScrollBar->setValue(hScrollBar->maximum());
    // else hScrollBar->setValue(hScrollBar->value() - (prevHScrollBarMax - hScrollBar->maximum()));
}

void QTimelineView::accessUnitSelected(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    for(QSharedPointer<QAccessUnitElement> pAccessUnitElement : m_pAccessUnitElements){
        pAccessUnitElement->m_selected = false;
        pAccessUnitElement->update();
    }
    emit selectAccessUnit(pAccessUnitModel);
}
