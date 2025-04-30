#pragma once

#include <QWidget>
#include <QMouseEvent>

class QTimelineCounterElement : public QWidget
{
    Q_OBJECT
public:
    QTimelineCounterElement(uint16_t value, QWidget *parent = NULL);
    ~QTimelineCounterElement();

protected:
    void paintEvent(QPaintEvent *event);

private:
    uint16_t m_value;
};