#include <QDebug>
#include <QPainter>

#include "QTimelineCounterElement.h"

QTimelineCounterElement::QTimelineCounterElement(uint16_t value, QWidget* parent)
    : QWidget(parent), m_value(value)
{
    setFixedWidth(15);
    update();
}

QTimelineCounterElement::~QTimelineCounterElement(){}

void QTimelineCounterElement::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.scale(0.75, 0.75);
    painter.rotate(90);
    painter.drawText(0, -width()*(1/0.75)/2 + 5, (m_value == 1 || m_value%10 == 0) ? QString::number(m_value) : "-");
    
}