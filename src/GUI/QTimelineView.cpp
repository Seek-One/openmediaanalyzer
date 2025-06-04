#include <QDebug>
#include <QScrollBar>
#include <QLabel>

#include "../GUIModel/QAccessUnitModel.h"
#include "QAccessUnitElement.h"
#include "QTimelineCounterElement.h"

#include "QTimelineView.h"

QTimelineView::QTimelineView(QWidget* parent)
    : QGroupBox(tr("Video timeline"), parent), m_liveContent(false)
{
    const uint16_t HEIGHT = 180;
    setFixedHeight(HEIGHT);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QWidget* containerContainerWidget = new QWidget(this);
    QWidget* barContainerWidget = new QWidget(containerContainerWidget);
    QWidget* counterContainerWidget = new QWidget(containerContainerWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QVBoxLayout* pVBoxLayout = new QVBoxLayout(containerContainerWidget);
    m_pBarHBoxLayout = new QHBoxLayout(barContainerWidget);
    m_pCounterHBoxLayout = new QHBoxLayout(counterContainerWidget);
    
    counterContainerWidget->setLayout(m_pCounterHBoxLayout);
    containerContainerWidget->setLayout(pVBoxLayout);
    barContainerWidget->setLayout(m_pBarHBoxLayout);

    containerContainerWidget->setFixedHeight(HEIGHT-20);
    barContainerWidget->setFixedHeight(containerContainerWidget->height()*0.8);
    counterContainerWidget->setFixedHeight(containerContainerWidget->height()-barContainerWidget->height());
    barContainerWidget->setContentsMargins(0, 0, 0, 0);
    counterContainerWidget->setContentsMargins(0, 0, 0, 0);

    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pBarHBoxLayout->setSpacing(0);
    m_pBarHBoxLayout->setContentsMargins(0, 5, 0, 5);
    m_pCounterHBoxLayout->setSpacing(0);
    m_pCounterHBoxLayout->setContentsMargins(0, 0, 0, 0);

    pVBoxLayout->addWidget(barContainerWidget);
    pVBoxLayout->addWidget(counterContainerWidget);

    m_pBarHBoxLayout->addStretch(1);
    m_pBarHBoxLayout->addStretch(1);
    m_pCounterHBoxLayout->addStretch(1);
    m_pCounterHBoxLayout->addStretch(1);

    m_pScrollArea = new QScrollArea(this);
    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
    m_pScrollArea->setWidget(containerContainerWidget);

    mainLayout->addWidget(m_pScrollArea);
    setLayout(mainLayout);
}


QTimelineView::~QTimelineView(){}

void QTimelineView::resetTimeline(){
    m_pAccessUnitElements.clear();
    while(m_pBarHBoxLayout->count() > 2){
        QLayoutItem* item = m_pBarHBoxLayout->takeAt(1);
        if(item == nullptr) continue;
        if(QWidget* widget = item->widget()) widget->deleteLater();
        delete item;
    }
    QAccessUnitElement::m_maxSize = 0;
    while(m_pCounterHBoxLayout->count() > 2){
        QLayoutItem* item = m_pCounterHBoxLayout->takeAt(1);
        if(item == nullptr) continue;
        if(QWidget* widget = item->widget()) widget->deleteLater();
        delete item;
    }
}

void QTimelineView::accessUnitsUpdated(){
    for(QSharedPointer<QAccessUnitElement> pTimelineAccessUnitElement : m_pAccessUnitElements){
        if(pTimelineAccessUnitElement->m_pAccessUnitModel->size() > QAccessUnitElement::m_maxSize){
            QAccessUnitElement::m_maxSize = pTimelineAccessUnitElement->m_pAccessUnitModel->size();
        }
    }
    for(QSharedPointer<QAccessUnitElement> pAccessUnitElement : m_pAccessUnitElements) pAccessUnitElement->update();
}

void QTimelineView::accessUnitsAdded(QVector<QSharedPointer<QAccessUnitModel>> pAccessUnitModels){
    QScrollBar* hScrollBar = m_pScrollArea->horizontalScrollBar();
    int prevHScrollBarMax = hScrollBar->maximum();
    // bool clampScrollRight = hScrollBar->value() >= SCROLL_CLAMP_RIGHT_THRESHOLD * hScrollBar->maximum();

    // check for new max size first
    // existing access units may have been updated
    for(QSharedPointer<QAccessUnitElement> pAccessUnitElement : m_pAccessUnitElements){
        if(pAccessUnitElement->m_pAccessUnitModel->size() > QAccessUnitElement::m_maxSize){
            QAccessUnitElement::m_maxSize = pAccessUnitElement->m_pAccessUnitModel->size();
        }
    }
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : pAccessUnitModels) {
        if(pAccessUnitModel->size() > QAccessUnitElement::m_maxSize){
            QAccessUnitElement::m_maxSize = pAccessUnitModel->size();
        }
    }
    // update existing access units with the potential new max size
    for(QSharedPointer<QAccessUnitElement> pAccessUnitElement : m_pAccessUnitElements) pAccessUnitElement->update();
    // add the new access units afterwards to save on update calls
    for(QSharedPointer<QAccessUnitModel> pAccessUnitModel : pAccessUnitModels) {
        QSharedPointer<QAccessUnitElement> pAccessUnitElement = QSharedPointer<QAccessUnitElement>(new QAccessUnitElement());
        pAccessUnitElement->setAccessUnitElement(pAccessUnitModel);
        connect(pAccessUnitElement.get(), &QAccessUnitElement::selectAccessUnit, this, &QTimelineView::accessUnitSelected);

        int insertionIndex = m_pAccessUnitElements.size();
        if(pAccessUnitModel->m_sliceType == QAccessUnitModel::SliceType_B){
            auto accessUnitElementIt = m_pAccessUnitElements.rbegin();
            while(accessUnitElementIt != m_pAccessUnitElements.rend() && 
                ( (*accessUnitElementIt)->m_pAccessUnitModel->m_displayedFrameNum.has_value() && pAccessUnitElement->m_pAccessUnitModel->m_displayedFrameNum.has_value() &&
                    (*accessUnitElementIt)->m_pAccessUnitModel->m_displayedFrameNum.value() > pAccessUnitElement->m_pAccessUnitModel->m_displayedFrameNum.value())
            ){
                --insertionIndex;
                ++accessUnitElementIt;
            }
            m_pAccessUnitElements.insert(std::next(m_pAccessUnitElements.begin(), insertionIndex), pAccessUnitElement);
            m_pBarHBoxLayout->insertWidget(insertionIndex+1, pAccessUnitElement.get());
        } else {
            m_pAccessUnitElements.push_back(pAccessUnitElement);
            m_pBarHBoxLayout->insertWidget(m_pBarHBoxLayout->count()-1, pAccessUnitElement.get());
        }

        uint16_t accessUnitCount = m_pAccessUnitElements.size();
        m_pCounterHBoxLayout->insertWidget(m_pCounterHBoxLayout->count()-1, new QTimelineCounterElement(accessUnitCount, this));
    }

    if(m_liveContent) hScrollBar->setValue(hScrollBar->maximum());
    else hScrollBar->setValue(hScrollBar->value() - (hScrollBar->maximum() - prevHScrollBarMax));
}

void QTimelineView::accessUnitsRemoved(uint32_t count){
    QScrollBar* hScrollBar = m_pScrollArea->horizontalScrollBar();
    bool clampScrollRight = hScrollBar->value() >= SCROLL_CLAMP_RIGHT_THRESHOLD * hScrollBar->maximum();
    for(int i = 0;i < count;++i){
        QLayoutItem* item = m_pBarHBoxLayout->takeAt(1);
        if(item->widget()) item->widget()->deleteLater();
        item = m_pCounterHBoxLayout->takeAt(m_pBarHBoxLayout->count()-2);
        if(item->widget()) item->widget()->deleteLater();
        emit deleteFrame(m_pAccessUnitElements.front()->m_pAccessUnitModel->m_id);
        if(m_pAccessUnitElements.front()->m_selected) emit selectAccessUnit(nullptr);
        m_pAccessUnitElements.pop_front();
        delete item;
    }

    if(clampScrollRight) hScrollBar->setValue(hScrollBar->maximum());
}

void QTimelineView::accessUnitSelected(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    for(QSharedPointer<QAccessUnitElement> pAccessUnitElement : m_pAccessUnitElements){
        if(pAccessUnitModel && pAccessUnitModel->m_id == pAccessUnitElement->m_pAccessUnitModel->m_id) continue;
        pAccessUnitElement->m_selected = false;
        pAccessUnitElement->m_hovered = false;
        pAccessUnitElement->update();
    }
    emit selectAccessUnit(pAccessUnitModel);
}

void QTimelineView::liveContentSet(bool val){
    m_liveContent = val;
}
