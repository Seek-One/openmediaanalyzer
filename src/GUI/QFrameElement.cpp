#include <QDebug>
#include <QPainter>

#include "../Codec/H264/H264AccessUnit.h"
#include "../Codec/H264/H264Slice.h"

#include "QFrameElement.h"

QFrameElement::QFrameElement(QWidget* parent)
    : QWidget(parent), m_selected(false), m_hovered(false), m_interactable(true)
{
    setMinimumSize(4, 125);
    setMouseTracking(true);
}

QFrameElement::~QFrameElement(){}

void QFrameElement::setFrameElement(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    m_pAccessUnitModel = pAccessUnitModel;
    m_interactable = !!pAccessUnitModel;
    update();
}

void QFrameElement::paintEvent(QPaintEvent* event) {
    updateBar();
    updateBarColor();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect fullRect(0, 0, width(), height());
    if(m_selected){
        painter.fillRect(fullRect, QColor::fromRgb(173, 216, 230));
    } else if(m_hovered){
        painter.fillRect(fullRect, QColor::fromRgb(173, 216, 230, 120));
    }
    double size = (m_pAccessUnitModel->m_pAccessUnit != nullptr) ? m_pAccessUnitModel->m_pAccessUnit->size() : 0;
    double sizeRatio = size/m_maxSize;
    QRect barRect(width()/4, height()-(height()*sizeRatio), width()/2, height());
    painter.fillRect(barRect, m_barColor);
    setToolTip(m_text);
}

void QFrameElement::frameSelected(){
    m_selected = false;
    update();
}

void QFrameElement::mousePressEvent(QMouseEvent* event) {
    if(!m_interactable) return;
    if (event->button() == Qt::LeftButton) {
        emit selectFrame(m_pAccessUnitModel);
        m_selected = !m_selected;
        update();
    }
}

void QFrameElement::enterEvent(QEvent* event) {
    if(!m_interactable) return;
    m_hovered = true;
    update();
}

void QFrameElement::leaveEvent(QEvent* event) {
    if(!m_interactable) return;
    m_hovered = false;
    update();
}

void QFrameElement::updateBar(){
    m_pAccessUnitModel->m_displayedFrameNum = m_pAccessUnitModel->m_pAccessUnit->frameNumber();
    if(m_pAccessUnitModel->m_displayedFrameNum) m_text = QString("Frame " + QString::number(m_pAccessUnitModel->m_displayedFrameNum.value()));
    else m_text = QString("Frame ?");
    if(!m_pAccessUnitModel->m_pAccessUnit) m_barColor = QColor::fromRgb(128, 128, 128);
    else {
        if(m_pAccessUnitModel->m_pAccessUnit->slice()){
            switch(m_pAccessUnitModel->m_pAccessUnit->slice()->slice_type % 5){
                case H264Slice::SliceType_B:
                    m_barColor = QColor::fromRgb(0, 255, 255);
                    m_text += "\nB";
                    break;
                case H264Slice::SliceType_P:        
                    m_barColor = QColor::fromRgb(246, 161, 146);
                    m_text += "\nP";
                    break;
                case H264Slice::SliceType_I:
                    m_barColor = QColor::fromRgb(91, 215, 91);
                    m_text += "\nI";
                    break;
                default:
                    m_barColor = QColor::fromRgb(255, 20, 20, m_interactable ? 255 : 128);
            }
        }
    }
}

void QFrameElement::updateBarColor(){
    switch(m_pAccessUnitModel->m_status){
        case Status::OK:
            break;
        case Status::REFERENCED_IFRAME_MISSING:
        case Status::REFERENCED_PPS_OR_SPS_MISSING:
        case Status::OUT_OF_ORDER:
        case Status::MISSING_IFRAME:
            m_barColor = QColor::fromRgb(255, 20, 20, m_interactable ? 255 : 128);
            break;
    }
}
