#include <QDebug>
#include <QPainter>

#include "QAccessUnitElement.h"

QAccessUnitElement::QAccessUnitElement(QWidget* parent)
    : QWidget(parent), m_selected(false), m_hovered(false)
{
    setFixedWidth(15);
    setMouseTracking(true);
}

QAccessUnitElement::~QAccessUnitElement(){}

void QAccessUnitElement::setAccessUnitElement(QSharedPointer<QAccessUnitModel> pAccessUnitModel){
    m_pAccessUnitModel = pAccessUnitModel;
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
    double size = m_pAccessUnitModel->size();
    double sizeRatio = size/m_maxSize;
    QRect barRect(width()/4, height()-(height()*sizeRatio), width()/2, height());
    painter.fillRect(barRect, m_barColor);
    setToolTip(m_text);
}

void QAccessUnitElement::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_selected = !m_selected;
        update();
        emit selectAccessUnit(m_selected ? m_pAccessUnitModel : nullptr);
    }
}

void QAccessUnitElement::enterEvent(QEvent* event) {
    m_hovered = true;
    update();
}

void QAccessUnitElement::leaveEvent(QEvent* event) {
    m_hovered = false;
    update();
}

void QAccessUnitElement::updateBar(){
    m_pAccessUnitModel->setDisplayedFrameNumber();
    m_pAccessUnitModel->setSliceType();
    m_pAccessUnitModel->setStatus();
    if(m_pAccessUnitModel->m_displayedFrameNum.has_value()) m_text = QString(tr("Frame #") + QString::number(m_pAccessUnitModel->m_displayedFrameNum.value()));
    else m_text = QString(tr("Frame #?"));
    switch(m_pAccessUnitModel->m_sliceType){
        case QAccessUnitModel::SliceType_B:
            m_barColor = QColor::fromRgb(0, 255, 255);
            m_text += "\nB";
            break;
        case QAccessUnitModel::SliceType_P:        
            m_barColor = QColor::fromRgb(246, 161, 146);
            m_text += "\nP";
            break;
        case QAccessUnitModel::SliceType_I:
            m_barColor = QColor::fromRgb(91, 215, 91);
            m_text += "\nI";
            break;
        default:
            m_barColor = QColor::fromRgb(255, 20, 20, 255);
    }   
    switch(m_pAccessUnitModel->m_status){
        case QAccessUnitModel::Status_INVALID_STRUCTURE:
            m_text += "\n" + tr("Invalid structure");
            break;
        case QAccessUnitModel::Status_REFERENCED_IFRAME_MISSING:
            m_text += "\n" + tr("Missing reference I-frame");
            break;
        case QAccessUnitModel::Status_REFERENCED_PPS_OR_SPS_MISSING:
            m_text += "\n" + tr("Missing parameter set");
            break;
        case QAccessUnitModel::Status_OUT_OF_ORDER:
            m_text += "\n" + tr("Out of order");
            break;
        case QAccessUnitModel::Status_NON_CONFORMING:
            m_text += "\n" + tr("Non-conforming");
            break;
        default:
            break;
    }
}

void QAccessUnitElement::updateBarColor(){
    switch(m_pAccessUnitModel->m_status){
        case QAccessUnitModel::Status_OK:
            break;
        case QAccessUnitModel::Status_INVALID_STRUCTURE:
        case QAccessUnitModel::Status_REFERENCED_IFRAME_MISSING:
        case QAccessUnitModel::Status_REFERENCED_PPS_OR_SPS_MISSING:
        case QAccessUnitModel::Status_OUT_OF_ORDER:
            m_barColor = QColor::fromRgb(255, 20, 20, 255);
            break;
        case QAccessUnitModel::Status_NON_CONFORMING:
            m_barColor = QColor::fromRgb(220, 220, 0, 255);
            break;
    }
}
