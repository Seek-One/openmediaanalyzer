#include <QDebug>
#include <QPainter>

#include "../Codec/H264/H264AccessUnit.h"
#include "../Codec/H264/H264Slice.h"
#include "../Codec/H265/H265AccessUnit.h"
#include "../Codec/H265/H265Slice.h"

#include "QAccessUnitElement.h"

QAccessUnitElement::QAccessUnitElement(QWidget* parent)
    : QWidget(parent), m_selected(false), m_hovered(false), m_interactable(true)
{
    setMinimumSize(4, 125);
    setMouseTracking(true);
}

QAccessUnitElement::~QAccessUnitElement(){}

void QAccessUnitElement::setAccessUnitElement(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    m_pAccessUnitModel = pAccessUnitModel;
    m_interactable = !!pAccessUnitModel;
    update();
}

void QAccessUnitElement::paintEvent(QPaintEvent* event) {
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
    double size = 0;
    if(m_pAccessUnitModel->isH264()) size = std::get<const H264AccessUnit*>(m_pAccessUnitModel->m_pAccessUnit)->size();
    else if(m_pAccessUnitModel->isH265()) size = std::get<const H265AccessUnit*>(m_pAccessUnitModel->m_pAccessUnit)->size();
    double sizeRatio = size/m_maxSize;
    QRect barRect(width()/4, height()-(height()*sizeRatio), width()/2, height());
    painter.fillRect(barRect, m_barColor);
    setToolTip(m_text);
}

void QAccessUnitElement::accessUnitSelected(){
    m_selected = false;
    update();
}

void QAccessUnitElement::mousePressEvent(QMouseEvent* event) {
    if(!m_interactable) return;
    if (event->button() == Qt::LeftButton) {
        emit selectAccessUnit(m_pAccessUnitModel);
        m_selected = !m_selected;
        update();
    }
}

void QAccessUnitElement::enterEvent(QEvent* event) {
    if(!m_interactable) return;
    m_hovered = true;
    update();
}

void QAccessUnitElement::leaveEvent(QEvent* event) {
    if(!m_interactable) return;
    m_hovered = false;
    update();
}

void QAccessUnitElement::updateBar(){
    m_pAccessUnitModel->setDisplayedFrameNumber();
    if(m_pAccessUnitModel->m_displayedFrameNum) m_text = QString("Frame #" + QString::number(m_pAccessUnitModel->m_displayedFrameNum.value()));
    else m_text = QString("Frame #?");
    if(m_pAccessUnitModel->isH264()){
        if(std::get<const H264AccessUnit*>(m_pAccessUnitModel->m_pAccessUnit)->slice()){
            switch(std::get<const H264AccessUnit*>(m_pAccessUnitModel->m_pAccessUnit)->slice()->slice_type % 5){
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
    } else if(m_pAccessUnitModel->isH265()){
        if(std::get<const H265AccessUnit*>(m_pAccessUnitModel->m_pAccessUnit)->slice()){
            switch(std::get<const H265AccessUnit*>(m_pAccessUnitModel->m_pAccessUnit)->slice()->slice_type % 5){
                case H265Slice::SliceType_B:
                    m_barColor = QColor::fromRgb(0, 255, 255);
                    m_text += "\nB";
                    break;
                case H265Slice::SliceType_P:        
                    m_barColor = QColor::fromRgb(246, 161, 146);
                    m_text += "\nP";
                    break;
                case H265Slice::SliceType_I:
                    m_barColor = QColor::fromRgb(91, 215, 91);
                    m_text += "\nI";
                    break;
                default:
                    m_barColor = QColor::fromRgb(255, 20, 20, m_interactable ? 255 : 128);
            }
        }
    }
    
}

void QAccessUnitElement::updateBarColor(){
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
